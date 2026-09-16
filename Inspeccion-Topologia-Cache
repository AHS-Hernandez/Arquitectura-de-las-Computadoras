# TAREA 1: INSPECCIÓN DE TOPOLOGÍA DE PROCESADOR Y JERARQUÍA DE MEMORIA CACHÉ EN GNU/LINUX

**Materia:** Arquitectura de Computadoras (SIS-131)

**Docente:** Ing. Paulo César Loayza Carrasco

**Fecha:** 16/09/26

**Entorno de Ejecución:** Ubuntu Linux x86_64 (Entorno Virtualizado / VirtualBox)

---

## 1. Comandos Ejecutados y Salidas de Terminal Obtenidas

A continuación se registran los comandos ejecutados en consola para la inspección de hardware y sus correspondientes salidas reales del sistema:

### A. Jerarquía y capacidades de memoria caché

```bash
osboxes@osboxes:~$ lscpu | grep -E "L1|L2|L3|Model name"
L1d cache:               64 KiB (2 instances)
L1i cache:               64 KiB (2 instances)
L2 cache:                512 KiB (2 instances)
Vulnerability L1tf:      Not affected

```

### B. Tamaño de la línea de caché L1 de datos

```bash
osboxes@osboxes:~$ getconf LEVEL1_DCACHE_LINESIZE
64

```

### C. Inspección del Sysfs del Kernel (Bloque de Coherencia)

```bash
osboxes@osboxes:~$ cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size
64

```

---

## 2. Resumen Técnico de Hardware Detectado

| Nivel de Hardware | Valor Detectado | Interpretación Arquitectónica |
| --- | --- | --- |
| **Caché L1 de Datos (L1d)** | 64 KiB (2 instancias) | 32 KiB dedicados por núcleo para operandos y datos en ejecución. |
| **Caché L1 de Instrucciones (L1i)** | 64 KiB (2 instancias) | 32 KiB dedicados por núcleo para código máquina decodificado. |
| **Caché L2 Unificada** | 512 KiB (2 instancias) | 256 KiB dedicados por núcleo para respaldo intermedio de SRAM. |
| **Línea de Caché L1 (Linesize)** | **64 Bytes** | Tamaño del bloque indivisible de transferencia bus $\leftrightarrow$ CPU. |
| **Coherency Line Size (Kernel)** | **64 Bytes** | Unidad mínima de sincronización de coherencia de caché multiprocesador. |

---

## 3. Justificación Técnica: "El Porqué" de Esta Inspección

### 1. El Principio Físico de Transferencia por Bloques

Los procesadores modernos no leen variables aisladas de la memoria RAM. Cada vez que la CPU requiere un dato ausente en caché (*Cache Miss*), el controlador del bus de memoria transfiere una línea completa de **64 bytes contiguos** desde la memoria principal.

### 2. Capacidad de Datos por Línea (Punto Flotante)

Dado que el tipo de dato `float` en arquitectura x86_64 estándar IEEE 754 ocupa 4 bytes (`sizeof(float) = 4`), una sola línea física de 64 bytes almacena exactamente:

$$\text{Elementos por línea} = \frac{64\text{ bytes}}{4\text{ bytes}} = 16\text{ números flotantes de 32 bits}$$

### 3. Relevancia Previa a la Compilación y Benchmarks

Verificar que el tamaño de línea sea de 64 bytes es el prerrequisito para diseñar y justificar algoritmos optimizados:

* **Orden de Almacenamiento (Row-Major Order):** En C/C++, las matrices se disponen contiguamente por filas. Si recorremos una fila secuencialmente, el primer acceso trae 16 elementos; 1 provoca fallo y los 15 restantes se resuelven en 1 ciclo de reloj (*Cache Hit*).

* **Impacto del Algoritmo Naive:** Si el código itera saltando columnas (brincando de fila en fila), cada salto avanza $N \times 4\text{ bytes} = 1024 \times 4 = 4096\text{ bytes}$. Esto significa que de los 64 bytes transferidos solo se aprovechan 4 bytes, desechando el 93.75% de la transferencia de datos y saturando el bus de memoria.

---

## 4. Respuestas Técnicas Asociadas a la Inspección

* **¿Por qué la consulta se hace tanto con `getconf` como mediante `/sys`?**
`getconf` consulta las variables de configuración POSIX del entorno de usuario a través de la librería estándar `glibc`. En contraste, la ruta `/sys/devices/system/cpu/...` consulta directamente el sistema de archivos virtual *sysfs*, el cual expone en tiempo real la información nativa que el Kernel Linux leyó de los registros MSR del microprocesador al arrancar el sistema. La coincidencia en el valor `64` confirma de forma determinística la consistencia hardware-software.

* **¿Qué implican las "2 instances" reportadas por `lscpu`?**
Indica que el entorno virtualizado dispone de 2 núcleos lógicos independientes asignados. Cada núcleo cuenta con su propia unidad de caché L1d de 32 KiB física/virtual privada, sumando un espacio de 64 KiB en el encapsulado general.
