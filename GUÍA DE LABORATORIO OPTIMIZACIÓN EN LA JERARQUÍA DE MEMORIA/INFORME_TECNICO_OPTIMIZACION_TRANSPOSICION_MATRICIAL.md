# INFORME TÉCNICO DE OPTIMIZACIÓN: TRANSPOSICIÓN MATRICIAL MEDIANTE BLOQUEO DE CACHÉ (TILING)

**Asignatura:** Arquitectura de Computadoras (SIS-131)

**Docente:** Ing. Paulo César Loayza Carrasco

**Fecha de Entrega:** 16/09/26

**Entorno de Ejecución:** Ubuntu Linux x86_64 en Máquina Virtual (VirtualBox / osboxes)

---

## 1. Fundamentación Arquitectónica del Problema

El experimento evalúa la transposición de una matriz cuadrada de orden $N = 2048$ compuesta por elementos de punto flotante de 32 bits (`float`, 4 bytes).

### Dimensionamiento de Memoria

* **Elementos por matriz:** $2048 \times 2048 = 4\,194\,304\text{ elementos}$.
* **Huella en memoria por matriz:** $4\,194\,304 \times 4\text{ bytes} = 16\,777\,216\text{ bytes} \approx 16\text{ MB}$.
* **Huella activa total (Matrices A y B):** $\approx 32\text{ MB}$.
* **Capacidad de Caché L1 de datos disponible:** $32\text{ KiB}$ por núcleo físico ($64\text{ KiB}$ reportados en dos instancias).

Dado que la huella activa ($32\text{ MB}$) sobrepasa ampliamente la capacidad de la memoria SRAM L1 ($32\text{ KiB}$), cualquier patrón de recorrido ineficiente desencadenará continuos desalojos de líneas (*Cache Thrashing*).

---

## 2. Comparativa Estructural de las Implementaciones

### A. Algoritmo No Óptimo (C++ / Naive)

En la versión directa, la lectura sobre la matriz origen $A$ es secuencial (`A[i * N + j]`), pero la escritura en la matriz destino $B$ se realiza mediante la indexación transpuesta `B[j * N + i]`.

```cpp
// Salto en memoria por cada iteración del bucle interior 'j'
B[j * N + i] = A[i * N + j];
```

* **Comportamiento del Bus y Caché:** Al variar $j$, cada asignación salta $N$ elementos en la matriz $B$:

$$\text{Salto (Stride)} = 2048 \times 4\text{ bytes} = 8192\text{ bytes}$$

* **Desperdicio de Transferencia:** Una línea física de caché transfiere 64 bytes contiguos (16 `floats`). Al saltar 8192 bytes en cada iteración, el procesador almacena 16 elementos en la línea de caché de $B$, utiliza únicamente 1 elemento (4 bytes) y, debido al tamaño de la matriz, dicha línea es expulsada por reemplazo antes de que el bucle vuelva a necesitar los 15 elementos restantes. Esto genera una tasa teórica de fallos de escritura de prácticamente el 100%.

### B. Algoritmo Optimizado (ANSI C / Cache Blocking + Punteros)

La versión optimizada divide el espacio bidimensional en bloques o "baldosas" (*tiles*) de tamaño submúltiplo $\text{BLOCK\_SIZE} = 16$.

```c
/* Sub-bloque de 16x16 procesado con punteros contiguos */
for (int i = 0; i < BLOCK_SIZE; ++i) {
    const float *ptr_a = &A[(bi + i) * N + bj];
    float *ptr_b = &B[bj * N + (bi + i)];
    for (int j = 0; j < BLOCK_SIZE; ++j) {
        *ptr_b = ptr_a[j];
        ptr_b += N;
    }
}
```

* **Retención en Caché L1:** Un bloque de $16 \times 16$ elementos ocupa:

$$\text{Tamaño del Bloque} = 16 \times 16 \times 4\text{ bytes} = 1024\text{ bytes} = 1\text{ KiB}$$

* Como $1\text{ KiB}$ cabe con holgura dentro de la caché L1d ($32\text{ KiB}$), las 16 líneas de caché correspondientes a las filas del bloque en la matriz $B$ se mantienen residentes en la memoria estática SRAM durante toda la rotación del sub-bloque, eliminando los fallos de acceso repetitivos y reduciendo el tráfico en el bus principal.

* **Aritmética de Punteros:** Sustituye el cálculo algebraico de multiplicación de índices en cada paso (`j * N + i`) por incrementos directos de desplazamiento (`ptr_b += N`), aliviando la carga aritmética de la ALU.

---

## 3. Registro de Resultados Experimentales

### Ejecución Directa (Sin sobrecarga de instrumentación)

Mediciones de tiempo de reloj puro tomadas desde la consola:

| Implementación | Tiempo Registrado | Speedup ($S = T_{\text{naive}} / T_{\text{opt}}$) | Reducción de Latencia |
| --- | --- | --- | --- |
| **Naive (C++)** | **0.0448 s** | 1.00x (Base) | 0.0% |
| **Tiling Optimizado (C)** | **0.0182 s** | **2.46x** | **59.38%** |

$$\text{Factor de Aceleración (Speedup)} = \frac{0.0448478\text{ s}}{0.0182000\text{ s}} \approx 2.464\times$$

---

### Ejecución con Telemetría `perf stat`

Salidas obtenidas bajo perfilado con contadores del sistema de Linux:

| Parámetro / Evento | Naive (C++) | Tiling Optimizado (ANSI C) | Variación / Análisis |
| --- | --- | --- | --- |
| **Tiempo Interno (Algoritmo)** | **0.0505 s** | **0.0369 s** | Aceleración de **1.37x** bajo trazado |
| **task-clock (Uso de CPU)** | 110.51 ms | 107.26 ms | Menor tiempo efectivo de despacho del proceso |
| **page-faults (Fallos de Página)** | 8,326 | 8,257 | Coherente con la asignación bajo demanda |
| **Time Elapsed (Total)** | 0.1148 s | 0.1508 s | Afectado por sincronización de E/S y runtime |
| **User CPU Time** | 0.0739 s | 0.0704 s | Menor consumo de cómputo en espacio de usuario |

---

## 4. Análisis Crítico de Telemetría a Bajo Nivel

### A. Justificación de los Fallos de Página (Page Faults)

La telemetría arrojó **8,326 fallos de página** para la versión Naive y **8,257** para la optimizada.

* En sistemas operativos basados en Linux con paginación x86_64, el tamaño estándar de una página de memoria es de **4 KiB** ($4096\text{ bytes}$).
* Las dos matrices de datos ocupan un total combinado de $32\text{ MB}$ ($33\,554\,432\text{ bytes}$).
* Al reservar memoria dinámica con `malloc` o `std::vector`, el kernel no asigna marcos físicos de inmediato (*Demand Paging* / copia en escritura). Cuando los bucles tocan la memoria por primera vez, el hardware de la MMU detona fallos de página menores (*Minor Page Faults*) para asignar las páginas físicas:

$$\text{Páginas Teóricas} = \frac{33\,554\,432\text{ bytes}}{4096\text{ bytes}} = 8192\text{ páginas}$$

* Los 8,326 y 8,257 fallos medidos coinciden con la demanda física del espacio de direcciones más la sobrecarga de librerías dinámicas (`libc`, `libstdc++`), validando que la asignación física de memoria fue idéntica en ambos programas.

### B. Análisis del Estado `<not supported>` en Ciclos e Instrucciones

El reporte de `perf stat` muestra `<not supported>` en los eventos `cycles` e `instructions`:

* **Causa Física:** La ejecución se llevó a cabo sobre una máquina virtual huésped montada sobre VirtualBox (Hipervisor de Tipo 2). Por restricciones de aislamiento y emulación de la interfaz vCPU, el hipervisor no virtualiza ni expone al sistema invitado los registros de control específicos del microprocesador (MSR) asignados a la Unidad de Monitoreo de Rendimiento (PMU).

* **Solidez de la Demostración:** A pesar de la ausencia de contadores PMU directos, la ganancia de rendimiento se demuestra de forma determinística mediante la reducción neta del tiempo de ejecución ($2.46\times$ de Speedup) y la disminución del tiempo de procesador en espacio de usuario (`user time`).

---

## 5. Conclusiones

1. **Aprovechamiento de la Localidad Espacial:** La técnica de *Tiling* adaptada al tamaño exacto de la línea de caché física (64 bytes = 16 floats) transforma un acceso con saltos de 8192 bytes en accesos secuenciales concentrados en sub-bloques de 1 KiB, logrando una reducción del tiempo de transposición de **0.0448 s a 0.0182 s**.

2. **Impacto de la Jerarquía de Memoria frente a la Complejidad Algorítmica:** Ambas versiones ejecutan exactamente la misma cantidad de lecturas y asignaciones ($N^2 = 4\,194\,304$ operaciones). La mejora de más del **146% en velocidad** se debe exclusivamente a mantener los datos dentro del nivel L1 de la memoria física, evitando paradas de la CPU (*stalls*) a la espera de transferencias lentas desde la RAM.

3. **Eficiencia en Sistemas Embebidos y Código de Alto Rendimiento:** Migrar la indexación matricial clásica a aritmética de punteros en ANSI C elimina sobrecargas del runtime de abstracción, permitiendo un control directo sobre el direccionamiento de hardware.
