# INFORME TÉCNICO: CONTROLADORES DE E/S POR PUERTO, COLA CIRCULAR EN RAM Y VRAM

**Asignatura:** Arquitectura de Computadoras (SIS-131)  
**Docente:** Ing. Paulo César Loayza Carrasco  
**Fecha:** 28 de Octubre de 2026  
**Entorno de Ejecución:** GNU/Linux Ubuntu x86_64 (GCC -std=c99 -Wall -Wextra -pedantic)  

---

## 1. Objetivos del Laboratorio y Arquitectura del Sistema

El propósito de la práctica es implementar y verificar en lenguaje ANSI C a bajo nivel los componentes fundamentales del subsistema de Entrada/Salida (E/S), almacenamiento intermedio en memoria principal y renderizado gráfico directo en memoria de video (VRAM):

1. **E/S Aislada y Handshake:** Modelar el ciclo de bus de control (`IO_READ`), el bus de direcciones (`address_bus`) y el bus de datos (`data_bus`), verificando la limpieza automática del registro de estado tras la lectura de datos.
2. **Buffer Circular FIFO en RAM:** Gestionar una estructura de cola secuencial de 8 bytes directamente sobre las posiciones físicas de memoria RAM (`0x70` a `0x77`) mediante punteros modulados (`KB_HEAD`, `KB_TAIL`, `KB_COUNT`) y control estricto de desbordamiento (*Buffer Overflow*).
3. **Memoria de Video VRAM:** Manipular una matriz de pantalla de $4 \times 4$ caracteres mapeada en el rango `0xF0` a `0xFF`, implementando traslación vertical de memoria (*Scroll*).
4. **Terminal Integradora (Opción B):** Unificar la captura interactiva desde teclado físico, la transferencia por puerto simulado con instrucción `IN`, la inserción/extracción en cola y el renderizado con soporte de retroceso (*Backspace* `0x08` / `0x7F`).

---

## 2. Mapa de Direccionamiento de Memoria y Puertos

El diseño arquitectónico del sistema distribuye la memoria y los puertos de la siguiente forma:

| Rango / Registro | Recurso de Hardware | Capacidad / Función |
| :--- | :--- | :--- |
| `0x00` – `0x5F` | RAM de Código | Instrucciones ejecutables del programa de la CPU. |
| `0x70` – `0x77` | Buffer Cola Teclado | Búfer circular FIFO en memoria RAM física (8 bytes). |
| `0x78` | Registro `KB_HEAD` | Puntero de extracción en RAM ($0 \le \text{HEAD} < 8$). |
| `0x79` | Registro `KB_TAIL` | Puntero de inserción en RAM ($0 \le \text{TAIL} < 8$). |
| `0x7A` | Registro `KB_COUNT` | Contador de bytes ocupados ($0 \le \text{COUNT} \le 8$). |
| `0xF0` – `0xFF` | VRAM (Monitor) | Matriz de $4 \times 4$ celdas de pantalla (16 bytes con Latch). |
| **Puerto 0x01** | Puerto de Estado E/S | Registro de Estado (`reg_status`): Bit 0 = Tecla lista. |
| **Puerto 0x02** | Puerto de Datos E/S | Registro de Datos (`reg_data`): Código ASCII de la tecla. |

---

## 3. Experimento 1: E/S Aislada por Puerto y Handshake (`exp1`)

### Registro de Ejecución en Consola
```text
osboxes@osboxes:~$ ./exp1
=== EXPERIMENTO 1: E/S POR PUERTO Y HANDSHAKE ===
[1] Estado inicial del puerto 0x01 (Status): 0x00
[2] Pulsacion de hardware simulada: Tecla 'Z' (0x5A)
[3] Sondeo CPU puerto 0x01 (Status): 0x01 (Tecla lista)
[4] CPU lee puerto 0x02 (Data): 'Z' (0x5A)
[5] Verificacion de Handshake post-lectura (Status): 0x00 (OK: Limpieza exitosa)
```

### Análisis Técnico
* **Ciclo de Bus:** La función `cpu_instruccion_in(puerto)` activa la línea de control `io_read = true` manteniendo `mem_read = false`, lo que caracteriza a una arquitectura de **E/S Aislada (Port-Mapped I/O)**.
* **Handshake Automático:** Cuando la CPU realiza la lectura sobre el puerto `0x02`, el controlador de teclado no solo deposita el byte `0x5A` ('Z') en el bus de datos, sino que conmuta inmediatamente su registro interno `reg_status = 0x00`. Como se evidencia en el paso `[5]`, un nuevo sondeo al puerto `0x01` arroja `0x00`, impidiendo lecturas redundantes o duplicadas por parte del procesador.

---

## 4. Experimento 2: Cola Circular FIFO en Memoria RAM (`exp2`)

### Registro de Ejecución en Consola
```text
osboxes@osboxes:~$ ./exp2
=== EXPERIMENTO 2: COLA CIRCULAR FIFO EN RAM ===

[1] Encolando 8 caracteres ('A'..'H')...
 -> Encolado 'A' | RAM[HEAD]=0, RAM[TAIL]=1, RAM[COUNT]=1 | Buffer: [ 'A' 0x00 0x00 0x00 0x00 0x00 0x00 0x00 ]
 -> Encolado 'B' | RAM[HEAD]=0, RAM[TAIL]=2, RAM[COUNT]=2 | Buffer: [ 'A' 'B' 0x00 0x00 0x00 0x00 0x00 0x00 ]
 -> Encolado 'C' | RAM[HEAD]=0, RAM[TAIL]=3, RAM[COUNT]=3 | Buffer: [ 'A' 'B' 'C' 0x00 0x00 0x00 0x00 0x00 ]
 -> Encolado 'D' | RAM[HEAD]=0, RAM[TAIL]=4, RAM[COUNT]=4 | Buffer: [ 'A' 'B' 'C' 'D' 0x00 0x00 0x00 0x00 ]
 -> Encolado 'E' | RAM[HEAD]=0, RAM[TAIL]=5, RAM[COUNT]=5 | Buffer: [ 'A' 'B' 'C' 'D' 'E' 0x00 0x00 0x00 ]
 -> Encolado 'F' | RAM[HEAD]=0, RAM[TAIL]=6, RAM[COUNT]=6 | Buffer: [ 'A' 'B' 'C' 'D' 'E' 'F' 0x00 0x00 ]
 -> Encolado 'G' | RAM[HEAD]=0, RAM[TAIL]=7, RAM[COUNT]=7 | Buffer: [ 'A' 'B' 'C' 'D' 'E' 'F' 'G' 0x00 ]
 -> Encolado 'H' | RAM[HEAD]=0, RAM[TAIL]=0, RAM[COUNT]=8 | Buffer: [ 'A' 'B' 'C' 'D' 'E' 'F' 'G' 'H' ]

[2] Intentando encolar 'I' con cola llena (Capacidad=8):
 -> [OK] Deteccion de BUFFER OVERFLOW correcta (rechazado).

[3] Desencolando 3 elementos:
 -> Desencolado 'A' | RAM[HEAD]=1, RAM[TAIL]=0, RAM[COUNT]=7 | Buffer: [ 'A' 'B' 'C' 'D' 'E' 'F' 'G' 'H' ]
 -> Desencolado 'B' | RAM[HEAD]=2, RAM[TAIL]=0, RAM[COUNT]=6 | Buffer: [ 'A' 'B' 'C' 'D' 'E' 'F' 'G' 'H' ]
 -> Desencolado 'C' | RAM[HEAD]=3, RAM[TAIL]=0, RAM[COUNT]=5 | Buffer: [ 'A' 'B' 'C' 'D' 'E' 'F' 'G' 'H' ]

[4] Encolando 'X' e 'Y' (punteros circulares):
 -> Encolado 'X' | RAM[HEAD]=3, RAM[TAIL]=1, RAM[COUNT]=6 | Buffer: [ 'X' 'B' 'C' 'D' 'E' 'F' 'G' 'H' ]
 -> Encolado 'Y' | RAM[HEAD]=3, RAM[TAIL]=2, RAM[COUNT]=7 | Buffer: [ 'X' 'Y' 'C' 'D' 'E' 'F' 'G' 'H' ]

[5] Vaciando la cola completa:
 -> Extraido: 'D' (quedan 6)
 -> Extraido: 'E' (quedan 5)
 -> Extraido: 'F' (quedan 4)
 -> Extraido: 'G' (quedan 3)
 -> Extraido: 'H' (quedan 2)
 -> Extraido: 'X' (quedan 1)
 -> Extraido: 'Y' (quedan 0)
 -> [OK] Deteccion de COLA VACIA correcta.
```

### Análisis Técnico
* **Aritmética Modular:** El puntero de inserción avanza bajo la relación $\text{TAIL}_{\text{nuevo}} = (\text{TAIL} + 1) \pmod 8$. En el paso `[1]`, al insertar el 8vo byte ('H'), `TAIL` se modula volviendo a `0`, dejando el búfer completamente saturado (`COUNT = 8`).
* **Protección ante Desbordamiento:** En el paso `[2]`, la rutina valida `count >= COLA_CAPACIDAD` y rechaza la inserción de `'I'` retornando `false`, impidiendo la corrupción de celdas vecinas de memoria.
* **Comportamiento Circular:** En el paso `[4]`, tras liberar espacio extrayendo `'A'`, `'B'` y `'C'`, los nuevos elementos `'X'` e `'Y'` se escriben en los índices físicos 0 y 1 de la RAM, demostrando la reutilización estática del búfer sin reubicar memoria.

---

## 5. Experimento 3: Memoria de Video VRAM y Scroll Vertical (`exp3`)

### Registro de Ejecución en Consola
```text
osboxes@osboxes:~$ ./exp3
=== EXPERIMENTO 3: CONTROLADOR DE VRAM Y SCROLL VERTICAL ===

--- ESTADO INICIAL VRAM (4x4) ---
+------+------+------+------+
|  A   |  B   |  C   |  D   | (0xF0..0xF3)
+------+------+------+------+
|  E   |  F   |  G   |  H   | (0xF4..0xF7)
+------+------+------+------+
|  I   |  J   |  K   |  L   | (0xF8..0xFB)
+------+------+------+------+
|  M   |  N   |  O   |  P   | (0xFC..0xFF)
+------+------+------+------+

>>> Ejecutando vram_scroll_arriba()...

--- POST 1er SCROLL (Fila 0 eliminada, Fila 3 limpia) ---
+------+------+------+------+
|  E   |  F   |  G   |  H   | (0xF0..0xF3)
+------+------+------+------+
|  I   |  J   |  K   |  L   | (0xF4..0xF7)
+------+------+------+------+
|  M   |  N   |  O   |  P   | (0xF8..0xFB)
+------+------+------+------+
|      |      |      |      | (0xFC..0xFF)
+------+------+------+------+

>>> Escribiendo 'W', 'X', 'Y', 'Z' en Fila 3...

--- POST ESCRITURA EN ULTIMA FILA ---
+------+------+------+------+
|  E   |  F   |  G   |  H   | (0xF0..0xF3)
+------+------+------+------+
|  I   |  J   |  K   |  L   | (0xF4..0xF7)
+------+------+------+------+
|  M   |  N   |  O   |  P   | (0xF8..0xFB)
+------+------+------+------+
|  W   |  X   |  Y   |  Z   | (0xFC..0xFF)
+------+------+------+------+
```

### Análisis Técnico
* **Direccionamiento Lineal de Pantalla:** La dirección física dentro del arreglo de memoria se calcula mediante:
  $$\text{Dir}_{\text{VRAM}} = 0\text{xF0} + (\text{fila} \times 4) + \text{columna}$$
* **Mecanismo de Scroll:** La rutina `vram_scroll_arriba()` traslada el bloque de datos de la fila $f+1$ hacia la fila $f$ para $f \in \{0, 1, 2\}$, descartando la fila 0 original (`'A'..'D'`). Finalmente, limpia la fila 3 (`0xFC`–`0xFF`) llenándola con el código ASCII de espacio (`0x20`), dejándola disponible para la inserción de nuevos caracteres (`'W'..'Z'`).

---

## 6. Reto Integrador: Terminal Interactiva (Opción B)

El programa integrador fue ejecutado bajo la modalidad **Opción B** avalada por el docente: captura de pulsaciones en vivo desde el teclado físico de la terminal en modo no canónico (`termios.h`), canalización hacia el controlador de puerto mediante la instrucción `IN`, encolado intermedio en RAM y renderizado en VRAM.

### Evidencias de Ejecución Dinámica

#### 1. Escritura inicial y avance de cursor
Al presionar secuencialmente `h`, `o`, `l`:
```text
+--- PANTALLA VRAM (4x4) ---+
|  h   o   l  [ ] |
|                 |
|                 |
|                 |
+---------------------------+
Registros de Hardware:
 -> Cursor VRAM       : 0xF3
 -> Encolados en RAM  : 0 / 8
 -> Ultima Instruccion: IN (Puerto 0x01 y 0x02)
```
* El cursor se ubica en `0xF3`, listo para recibir el cuarto carácter.

#### 2. Detección de error tipográfico
El usuario escribe un carácter erróneo `'x'`, avanzando a la segunda fila:
```text
+--- PANTALLA VRAM (4x4) ---+
|  h   o   l   x  |
| [ ]             |
|                 |
|                 |
+---------------------------+
Registros de Hardware:
 -> Cursor VRAM       : 0xF4
 -> Encolados en RAM  : 0 / 8
 -> Ultima Instruccion: IN (Puerto 0x01 y 0x02)
```

#### 3. Procesamiento de Tecla Backspace (Borrado)
Al presionar la tecla física `Backspace` (`0x08` / `0x7F`), el procesador no inserta un símbolo visual; ejecuta la lógica de retroceso:
$$\text{cursor\_vram} \leftarrow \text{cursor\_vram} - 1 = 0\text{xF3}$$
$$\text{RAM}[0\text{xF3}] \leftarrow 0\text{x20}\quad (\text{Espacio en blanco})$$
El carácter erróneo `'x'` queda eliminado y el cursor retrocede a su posición anterior:
```text
+--- PANTALLA VRAM (4x4) ---+
|  h   o   l  [ ] |
|                 |
|                 |
|                 |
+---------------------------+
Registros de Hardware:
 -> Cursor VRAM       : 0xF3
 -> Encolados en RAM  : 0 / 8
```

#### 4. Corrección y continuación de escritura
El usuario escribe `'a'`, corrige la palabra a `"hola"` y continúa con `'a'` y `'a'` en la siguiente línea:
```text
+--- PANTALLA VRAM (4x4) ---+
|  h   o   l   a  |
|  a   a  [ ]     |
|                 |
|                 |
+---------------------------+
Registros de Hardware:
 -> Cursor VRAM       : 0xF6
 -> Encolados en RAM  : 0 / 8
 -> Ultima Instruccion: IN (Puerto 0x01 y 0x02)
```

---

## 7. Conclusiones

1. **Aislamiento de Periféricos y Desacoplamiento Temporal:** La cola FIFO en RAM (`0x70`–`0x77`) actúa como un amortiguador de velocidad esencial. Permite que las pulsaciones del usuario registradas a frecuencias asíncronas no se pierdan si la CPU o el controlador de video se encuentran ejecutando rutinas de refresco o scroll.
2. **Determinismo del Handshake:** El borrado automático de `reg_status` en el puerto `0x01` ante la lectura de `0x02` garantiza la atomicidad del ciclo de E/S, evitando que una misma pulsación física genere múltiples caracteres en pantalla.
3. **Mapeo Directo en VRAM:** El control de video mediante arreglos directos en RAM evidencia cómo operaban los adaptadores de texto clásicos (CGA/MDA), donde modificar un carácter en pantalla requería únicamente una instrucción `MOV` o puntero a la dirección de memoria de video correspondiente.
