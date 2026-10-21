# INFORME TÉCNICO: OPTIMIZACIÓN DE BUSES DE COMUNICACIÓN E IPC EN LINUX

**Asignatura:** Arquitectura de Computadoras (SIS-131)  
**Docente:** Ing. Paulo César Loayza Carrasco  
**Fecha de entrega:** 21/10/2026  
**Entorno de pruebas:** GNU/Linux Ubuntu x86_64 (osboxes)  
**Lenguaje:** C

---

## 1. Descripción del problema experimental

El experimento evalúa el rendimiento de dos mecanismos de comunicación entre procesos (IPC) en Linux, mediante el procesamiento de **10.000.000 de paquetes de 64 bytes**, comparando el tiempo de ejecución, la velocidad y la sobrecarga de comunicación.

Se implementaron dos modelos:

**A. Modelo no optimizado (FIFO / Named Pipes)**

Utiliza `mkfifo()`, `write()` y `read()` para transferir paquetes individualmente entre productor y consumidor.

- Programas: `Prod_NO.c` y `Cons_NO.c`.
- Cada paquete genera una operación de escritura y una de lectura.
- El kernel interviene en el transporte de los datos mediante el búfer FIFO.

**B. Modelo optimizado (POSIX Shared Memory + Batching)**

Utiliza `shm_open()`, `mmap()` y semáforos POSIX para compartir memoria entre procesos.

- Programas: `Prod_OPT.c` y `Cons_OPT.c`.
- Agrupa 500 paquetes por lote.
- El productor escribe directamente en memoria compartida.
- Los semáforos sincronizan el acceso al búfer.

**Parámetros experimentales:**

| Parámetro | Valor |
|---|---:|
| Paquetes procesados | 10.000.000 |
| Tamaño por paquete | 64 bytes |
| Volumen lógico | 610,35 MiB |
| Paquetes por lote | 500 |
| Tamaño de lote | 31,25 KiB |
| Lotes totales | 20.000 |

**Cálculo del volumen:**

Volumen = 10.000.000 × 64 = 640.000.000 bytes

Volumen = 640.000.000 ÷ 1.048.576 = **610,35 MiB**

---

## 2. Resultados experimentales

Los siguientes datos corresponden a las ejecuciones realizadas en Ubuntu.

### 2.1. Resultados del modelo no optimizado

Comando: `./Cons_NO & ./Prod_NO`

| Métrica | Productor | Consumidor |
|---|---:|---:|
| Paquetes | 10.000.000 | 10.000.000 |
| Tiempo total | 4,8754 s | 4,8741 s |
| Velocidad (PPS) | 2.051.134 | 2.051.668 |
| Rendimiento | 125,19 MiB/s | 125,22 MiB/s |

La transferencia mediante FIFO requirió aproximadamente **4,87 segundos**, debido a la ejecución individual de las operaciones `write()` y `read()`.

### 2.2. Resultados del modelo optimizado

Comando: `./Prod_OPT & ./Cons_OPT`

| Métrica | Productor | Consumidor |
|---|---:|---:|
| Paquetes contabilizados | 10.000.000 | 10.000.000 |
| Tiempo total | 0,1075 s | 0,1035 s |
| Velocidad (PPS) | 92.989.398 | 96.575.974 |
| Rendimiento nominal | 5.675,62 MiB/s | 5.894,53 MiB/s |

El productor terminó en aproximadamente **0,11 segundos**, mostrando una reducción considerable frente al modelo FIFO.

**Observación:** el consumidor optimizado contabiliza 500 paquetes por lote, pero no lee ni verifica individualmente su contenido. Por ello, sus métricas representan rendimiento nominal, no ancho de banda efectivo comprobado.

---

## 3. Comparación y cálculos de rendimiento

| Métrica | No optimizado | Optimizado | Mejora |
|---|---:|---:|---:|
| Tiempo productor | 4,8754 s | 0,1075 s | **45,35×** |
| Tiempo consumidor | 4,8741 s | 0,1035 s | **47,09×** |
| PPS consumidor | 2.051.668 | 96.575.974 | **47,07×** |
| Rendimiento nominal consumidor | 125,22 MiB/s | 5.894,53 MiB/s | **47,07×** |

### 3.1. Cálculo del Speedup

El factor de aceleración representa cuántas veces disminuyó el tiempo de ejecución.

**Fórmula:**

Speedup = Tiempo no optimizado ÷ Tiempo optimizado

**Productor:**

Speedup = 4,8754 ÷ 0,1075 = **45,35×**

**Consumidor:**

Speedup = 4,8741 ÷ 0,1035 = **47,09×**

### 3.2. Reducción porcentual del tiempo

**Fórmula:**

Reducción (%) = (1 − Tiempo optimizado ÷ Tiempo original) × 100

**Consumidor:**

Reducción = (1 − 0,1035 ÷ 4,8741) × 100

**Resultado: 97,88 % de reducción del tiempo de ejecución.**

### 3.3. Velocidad de procesamiento

**Fórmula:**

PPS = Número de paquetes ÷ Tiempo total

- No optimizado: 10.000.000 ÷ 4,8741 ≈ **2,05 millones PPS**.
- Optimizado: 10.000.000 ÷ 0,1035 ≈ **96,62 millones PPS**.

Los valores aproximados pueden diferir ligeramente de la terminal porque esta imprime los tiempos redondeados.

---

## 4. Justificación técnica de la optimización

### A. Reducción de llamadas al sistema

En el modelo FIFO, cada paquete requiere una operación de escritura y una de lectura mediante el kernel.

**Operaciones esperadas de transferencia:**

- 10.000.000 llamadas `write()`.
- 10.000.000 llamadas `read()`.
- **Total: 20.000.000 operaciones de lectura/escritura.**

Estas llamadas generan transiciones entre modo usuario y modo kernel, aumentando el costo de comunicación.

La memoria compartida elimina la necesidad de realizar estas llamadas por cada paquete. Sin embargo, una llamada al sistema no equivale necesariamente a un cambio de contexto entre procesos.

### B. Memoria compartida y Zero-Copy

**Modelo FIFO:**

Productor → Kernel (búfer FIFO) → Consumidor

Los datos se trasladan mediante búferes administrados por el kernel.

**Modelo optimizado:**

Productor → Memoria compartida ← Consumidor

Con `mmap()`, ambos procesos acceden a una región física de memoria compartida, evitando las copias intermedias asociadas al transporte FIFO.

Esto reduce el trabajo necesario para comunicar los datos, aunque no elimina los accesos físicos a memoria ni la sincronización.

### C. Batching y jerarquía de memoria

Cada lote contiene:

500 × 64 = **32.000 bytes = 31,25 KiB**

Cantidad de lotes:

10.000.000 ÷ 500 = **20.000 lotes**

La agrupación permite sincronizar por lote en lugar de hacerlo por paquete.

Este tamaño es comparable al de algunas cachés L1 de datos de 32 KiB, aunque no garantiza que todos los paquetes permanezcan en dicha caché.

### D. Sincronización mediante semáforos

El productor y consumidor utilizan `sem_wait()` y `sem_post()` para coordinar el acceso al búfer.

Por cada lote se ejecutan:

- 2 operaciones de semáforos en el productor.
- 2 operaciones de semáforos en el consumidor.

**Total:**

20.000 lotes × 4 operaciones = **80.000 operaciones de semáforos**.

Estas operaciones no equivalen necesariamente a 80.000 syscalls, porque algunas pueden resolverse en espacio de usuario.

---

## 5. Análisis crítico de los resultados

La optimización presenta una mejora considerable en los tiempos registrados. No obstante, existen aspectos que deben considerarse:

1. **Lectura no verificada:** `Cons_OPT.c` incrementa `total_received` en 500 por lote, sin recorrer realmente los paquetes. Esto limita la comparación del rendimiento de transferencia.
2. **Métricas nominales:** el valor de 5.894,53 MiB/s representa una tasa lógica calculada, no una medición directa del ancho de banda físico de RAM.
3. **Medición de syscalls:** las operaciones de semáforos y los cambios de contexto no fueron medidos mediante herramientas como `strace` o `perf`.
4. **Validez estadística:** los resultados corresponden a una ejecución por modelo. Varias repeticiones permitirían calcular promedios y reducir la influencia de fluctuaciones del sistema.

Por tanto, la aceleración observada es válida para las implementaciones probadas, pero no demuestra por sí sola una mejora equivalente en lectura efectiva de datos.

---

## 6. Conclusiones

1. **Mejora significativa del rendimiento:** la implementación optimizada redujo el tiempo medido del consumidor de 4,8741 a 0,1035 segundos, equivalente a una aceleración de aproximadamente **47,09×** y una reducción temporal del **97,88 %**.

2. **Eficiencia de memoria compartida:** utilizar `mmap()` permite evitar las transferencias individuales mediante búferes FIFO del kernel, disminuyendo la sobrecarga de comunicación entre procesos.

3. **Impacto del batching:** agrupar 500 paquetes por lote reduce la frecuencia de sincronización y permite procesar grandes cantidades de datos con menos operaciones de coordinación por paquete.

4. **Relación con la arquitectura de computadoras:** los resultados evidencian la importancia de las transiciones entre usuario y kernel, la jerarquía de memoria y la sincronización para el rendimiento de aplicaciones concurrentes.

5. **Limitación experimental:** el consumidor optimizado no realiza lectura ni validación individual de los paquetes. Se requiere completar esta operación y repetir las pruebas para confirmar el rendimiento efectivo de extremo a extremo.

**Conclusión general:** el experimento evidencia que combinar memoria compartida POSIX y procesamiento por lotes puede reducir significativamente la sobrecarga IPC. Los resultados obtenidos demuestran una aceleración temporal importante, aunque la validación completa de la transferencia de datos requiere mejorar la implementación del consumidor optimizado.
