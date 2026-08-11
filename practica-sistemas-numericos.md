# Práctica: Sistemas Numericos y Aritmetica Binaria

## Nivel 1: Conversión de bases (fundamentos)

---

### 1.1 Conversión de decimal a binario
* **45:**
  * $ 2^7  2^7  2^7 2^7  2^7  2^7  2^7  2^7  2^7  2^7  $ 
  * $ 512  256  128 64  32  16   8   4   2   1  $ 
  * **Resultado:** `101101` (o en 8 bits: `00101101`)

* **128:**
  * $128 = 2^7$
  * **Resultado:** `10000000`

* **255:**
  * $255 = 2^8 - 1$
  * **Resultado:** `11111111`

* **1000:**
  * $1000 = 512 + 256 + 128 + 64 + 32 + 8$
  * **Resultado:** `1111101000`

---

### 1.2 Conversión de binario a decimal
* **1011:**
  * $1 \cdot 2^3 + 0 \cdot 2^2 + 1 \cdot 2^1 + 1 \cdot 2^0 = 8 + 0 + 2 + 1 = \mathbf{11}$

* **10110110:**
  * $128 + 32 + 16 + 4 + 2 = \mathbf{182}$

* **11111111:**
  * $2^8 - 1 = \mathbf{255}$

* **100000000:**
  * $2^8 = \mathbf{256}$

---

### 1.3 Conversión de binario a hexadecimal (agrupando en bloques de 4 bits)
* **10101100:**
  * `1010` = `A`
  * `1100` = `C`
  * **Resultado:** `0xAC`

* **111100001111:**
  * `1111` = `F`
  * `0000` = `0`
  * `1111` = `F`
  * **Resultado:** `0xF0F`

* **1010101010101010:**
  * `1010` `1010` `1010` `1010` = `A` `A` `A` `A`
  * **Resultado:** `0xAAAA`

---

### 1.4 Conversión de octal a decimal
* **17:**
  * $1 \cdot 8^1 + 7 \cdot 8^0 = 8 + 7 = \mathbf{15}$

* **254:**
  * $2 \cdot 8^2 + 5 \cdot 8^1 + 4 \cdot 8^0 = 2 \cdot 64 + 40 + 4 = 128 + 40 + 4 = \mathbf{172}$

* **777:**
  * $7 \cdot 8^2 + 7 \cdot 8^1 + 7 \cdot 8^0 = 7 \cdot 64 + 56 + 7 = 448 + 56 + 7 = \mathbf{511}$

---
## Nivel 2: Aritmética binaria básica

---

### 2.1 Sumas binarias (mostrando acarreos)

* **Operación:** `1011 + 01011`
  * Ajustando a 5 bits: `01011 + 01011`
  ```text
    Acarreos:  1 1 1 1 0
               0 1 0 1 1  (11)
             + 0 1 0 1 1  (11)
             -----------
               1 0 1 1 0  (22)
  ```
  * **Resultado:** `10110` (en decimal: 22)

* **Operación:** `011011101 + 01011`
  * Ajustando bits:
  ```text
    Acarreos:    1 1 1 1 1 1 0
               0 1 1 0 1 1 1 0 1  (221)
             + 0 0 0 0 0 1 0 1 1  (11)
             -------------------
               0 1 1 1 0 1 0 0 0  (232)
  ```
  * **Resultado:** `011101000` (en decimal: 232)

---

### 2.2 Restas binarias usando préstamo (borrow)

* **Operación:** `1100 - 0101`
  ```text
    Préstamos: 0 1 0 0
                 1 1 0 0  (12)
               - 0 1 0 1  (5)
               ---------
                 0 1 1 1  (7)
  ```
  * **Resultado:** `0111` (en decimal: 7)

* **Operación:** `10000 - 00111`
  ```text
    Préstamos: 1 1 1 1 0
                 1 0 0 0 0  (16)
               - 0 0 1 1 1  (7)
               -----------
                 0 1 0 0 1  (9)
  ```
  * **Resultado:** `01001` (en decimal: 9)

---

### 2.3 Multiplicación binaria: `1011 * 0101`

```text
         1 0 1 1   (11)
       * 0 1 0 1   (5)
       ---------
         1 0 1 1   (1011 * 1)
       0 0 0 0     (1011 * 0)
     1 0 1 1       (1011 * 1)
   0 0 0 0         (1011 * 0)
   -------------
   0 1 1 0 1 1 1   (55)
```
* **Resultado:** `0110111` (en decimal: 55)

---

## Nivel 3: Complemento a dos

---

### 3.1 Representación en complemento a dos (8 bits)

* **-45:**
  1. $45$ en binario: `00101101`
  2. Invertir bits: `11010010`
  3. Sumar 1: `11010011`
  * **Resultado:** `11010011`

* **-1:**
  1. $1$ en binario: `00000001`
  2. Invertir bits: `11111110`
  3. Sumar 1: `11111111`
  * **Resultado:** `11111111`

* **-128:**
  * Límite inferior exacto en 8 bits con signo.
  * **Resultado:** `10000000`

* **27:**
  * Al ser positivo, el bit más significativo (MSB) es 0:
  * **Resultado:** `00011011`

---

### 3.2 Conversión a decimal del valor `11010110` (8 bits)
* Como el primer bit es `1`, el número es negativo:
  1. Invertir bits: `00101001`
  2. Sumar 1: `00101010`
  3. Convertir a decimal: $32 + 8 + 2 = 42$
  4. Agregar signo negativo.
* **Resultado:** `-42`

---

### 3.3 Resta $45 - 60$ usando complemento a dos (8 bits)
* Operación equivalente: $45 + (-60)$
  * $+45 = \text{00101101}$
  * $+60 = \text{00111100} \implies -60 = \text{11000011} + 1 = \text{11000100}$

```text
    Acarreos:  0 0 0 0 0 0 0 0
               0 0 1 0 1 1 0 1  (+45)
             + 1 1 0 0 0 1 0 0  (-60)
             -----------------
               1 1 1 1 0 0 0 1
```

* **Verificación del resultado `11110001`:**
  * Invertir: `00001110`
  * Sumar 1: `00001111` ($8 + 4 + 2 + 1 = 15$)
  * Por tanto, `11110001` equivale a **$-15$**, lo cual verifica que $45 - 60 = -15$.

---

## Nivel 4: Desbordamiento (overflow) y banderas

---

### 4.1 Análisis de overflow en 8 bits con signo (Rango: $[-128, 127]$)

* **Operación 1:** `01111111 + 00000001`
  * En decimal: $(+127) + (+1) = +128$ (fuera de rango).
  * Suma binaria: `01111111 + 00000001 = 10000000`
  * **Análisis:** Se sumaron dos números positivos y se obtuvo un resultado con bit de signo `1` (negativo, $-128$).
  * **Resultado:** **Sí ocurre Overflow** ($V=1$, $C_{in} \neq C_{out}$ en MSB).

* **Operación 2:** `10000000 + 11111111`
  * En decimal: $(-128) + (-1) = -129$ (fuera de rango).
  * Suma binaria: `10000000 + 11111111 = 01111111` (descartando acarreo saliente).
  * **Análisis:** Se sumaron dos números negativos y se obtuvo un resultado con bit de signo `0` (positivo, $+127$).
  * **Resultado:** **Sí ocurre Overflow** ($V=1$).

---

### 4.2 Diferencia entre Carry y Overflow (registros de 4 bits)

* **Carry (Acarreo):** Ocurre en aritmética sin signo cuando el resultado no cabe en los bits disponibles (se genera un bit extra a la izquierda).
* **Overflow (Desbordamiento):** Ocurre en aritmética con signo (complemento a dos) cuando el resultado excede el rango representable (cambia el bit de signo incorrectamente).

#### Ejemplos en 4 bits:
* **Caso Carry sin Overflow:** `1111 + 0001 = 0000` (con carry = 1).
  * Sin signo: $15 + 1 = 16$ (no cabe, genera Carry).
  * Con signo: $(-1) + (+1) = 0$ (es correcto, no hay Overflow).
* **Caso Overflow sin Carry:** `0100 + 0100 = 1000` (Rango con signo: $[-8, 7]$).
  * Con signo: $(+4) + (+4) = -8$ (Signo erróneo $\implies$ Overflow).
  * Sin signo: $4 + 4 = 8$ (cabe en 4 bits $\implies$ Carry = 0).

---

### 4.3 Programa práctico en C (`overflow.c`)

```c
#include <stdio.h>
#include <stdint.h>

int main() {
    int8_t a = 120;
    int8_t b = 20;
    int8_t resultado = a + b;

    printf("a = %d\n", a);
    printf("b = %d\n", b);
    printf("Resultado de a + b = %d\n", resultado);

    return 0;
}
```

* **Comandos de compilación y ejecución:**
```bash
gcc overflow.c -o overflow
./overflow
```

* **Salida de terminal:**
```text
a = 120
b = 20
Resultado de a + b = -116
```

* **Explicación:**
  * El tipo `int8_t` tiene un rango de $[-128, 127]$. La suma $120 + 20 = 140$ sobrepasa el límite positivo ($127$).
  * A nivel de bits, $140$ es `10001100`. Al interpretarse en complemento a dos con signo, el bit más significativo (`1`) representa un número negativo: $-128 + 12 = -116$.

---

## Nivel 5: Punto flotante (IEEE 754 - 32 bits)

---

### 5.1 Representación de $10.25$

1. **Signo:** Positivo $\implies S = 0$
2. **Conversión a binario:**
   * $10 = 1010_2$
   * $0.25 = 0.01_2$
   * $10.25_{10} = 1010.01_2$
3. **Normalización:**
   * $1.01001 \times 2^3$
4. **Exponente polarizado:**
   * $E = 3 + 127 = 130_{10} = 10000010_2$
5. **Mantisa (23 bits tras el 1 implícito):**
   * $M = 01001000000000000000000$

* **Resultado Desglosado:**
  * **Signo (1 bit):** `0`
  * **Exponente (8 bits):** `10000010`
  * **Mantisa (23 bits):** `01001000000000000000000`
  * **Trama completa:** `0 10000010 01001000000000000000000` (`0x41240000`)

---

### 5.2 Decodificación de `1 10000010 01100000000000000000000`

1. **Signo:** $S = 1 \implies$ Negativo.
2. **Exponente:** $10000010_2 = 130_{10}$. Exponente real $= 130 - 127 = 3$.
3. **Mantisa:** $0110..._2 = \frac{0}{2} + \frac{1}{4} + \frac{1}{8} = 0.25 + 0.125 = 0.375$.
4. **Cálculo:**
   $$(-1)^1 \times (1 + 0.375) \times 2^3 = -1 \times 1.375 \times 8 = -11.0$$
* **Resultado:** `-11.0`

---

### 5.3 Pérdida de precisión en punto flotante
En base 2, fracciones decimales como $0.1$ y $0.2$ son números periódicos infinitos ($0.1_{10} = 0.0001100110011..._2$). Al almacenarse en una mantisa finita (23 bits en simple precisión o 52 bits en doble), el valor se trunca o redondea.

* **Ejemplo numérico:**
  * En precisión simple o doble, $0.1 + 0.2$ produce `0.30000000000000004` en lugar de `0.3` exacto. La acumulación de estos errores de redondeo puede generar fallas de concordancia en comparaciones lógicas directas (`if (a + b == 0.3)` evaluará a falso).

---

## Nivel 6: Integración: script de verificación

---

### 6.1 Script en Python (`verificacion.py`)

```python
import sys

def complemento_a_dos(n, bits):
    # Calcula la máscara para n bits
    mascara = (1 << bits) - 1
    val = n & mascara
    return format(val, f'0{bits}b')

def verificar_numero(n):
    print("=" * 60)
    print(f"Número analizado: {n}")
    print("=" * 60)
    
    # Representaciones estándar (magnitud absoluta para bin/oct/hex estándar)
    if n >= 0:
        print(f"Binario:      {bin(n)}")
        print(f"Octal:        {oct(n)}")
        print(f"Hexadecimal:  {hex(n)}")
    else:
        print(f"Binario:     -{bin(abs(n))}")
        print(f"Octal:       -{oct(abs(n))}")
        print(f"Hexadecimal: -{hex(abs(n))}")

    # Complemento a dos
    print("\n--- Complemento a dos ---")
    print(f" 8 bits: {complemento_a_dos(n, 8)}")
    print(f"16 bits: {complemento_a_dos(n, 16)}")
    print(f"32 bits: {complemento_a_dos(n, 32)}")

    # Detección de overflow para 8 bits con signo [-128, 127]
    print("\n--- Validación de Rango (8 bits con signo) ---")
    if -128 <= n <= 127:
        print("Estado: RANGO VÁLIDO (No produce overflow en 8 bits con signo)")
    else:
        print("Estado: ¡OVERFLOW DETECTADO! (Excede el rango [-128, 127])")
    print("\n")

if __name__ == "__main__":
    casos_prueba = [127, 128, -128, -129, 0]
    for caso in casos_prueba:
        verificar_numero(caso)
```

#### Salida de ejecución del script:
```text
============================================================
Número analizado: 127
============================================================
Binario:      0b1111111
Octal:        0o177
Hexadecimal:  0x7f

--- Complemento a dos ---
 8 bits: 01111111
16 bits: 0000000001111111
32 bits: 00000000000000000000000001111111

--- Validación de Rango (8 bits con signo) ---
Estado: RANGO VÁLIDO (No produce overflow en 8 bits con signo)


============================================================
Número analizado: 128
============================================================
Binario:      0b10000000
Octal:        0o200
Hexadecimal:  0x80

--- Complemento a dos ---
 8 bits: 10000000
16 bits: 0000000010000000
32 bits: 00000000000000000000000010000000

--- Validación de Rango (8 bits con signo) ---
Estado: ¡OVERFLOW DETECTADO! (Excede el rango [-128, 127])


============================================================
Número analizado: -128
============================================================
Binario:     -0b10000000
Octal:       -0o200
Hexadecimal: -0x80

--- Complemento a dos ---
 8 bits: 10000000
16 bits: 1111111110000000
32 bits: 11111111111111111111111110000000

--- Validación de Rango (8 bits con signo) ---
Estado: RANGO VÁLIDO (No produce overflow en 8 bits con signo)


============================================================
Número analizado: -129
============================================================
Binario:     -0b10000001
Octal:       -0o201
Hexadecimal: -0x81

--- Complemento a dos ---
 8 bits: 01111111
16 bits: 1111111101111111
32 bits: 11111111111111111111111101111111

--- Validación de Rango (8 bits con signo) ---
Estado: ¡OVERFLOW DETECTADO! (Excede el rango [-128, 127])


============================================================
Número analizado: 0
============================================================
Binario:      0b0
Octal:        0o0
Hexadecimal:  0x0

--- Complemento a dos ---
 8 bits: 00000000
16 bits: 0000000000000000
32 bits: 00000000000000000000000000000000

--- Validación de Rango (8 bits con signo) ---
Estado: RANGO VÁLIDO (No produce overflow en 8 bits con signo)
```

---

### 6.2 Análisis crítico: Desbordamiento silencioso en sensores de hardware

Si un sensor de temperatura reporta lecturas como enteros de 8 bits con signo (`int8_t`), su rango operativo está acotado entre $-128$ y $+127$. Si la temperatura real de un reactor o motor se incrementa de $+127$ a $+128$, los bits pasan de `01111111` a `10000000`, provocando un desbordamiento silencioso que el microcontrolador interpreta en complemento a dos como $-128$. Este comportamiento es crítico en sistemas embebidos industriales y automotrices, ya que el software de control detectará falsamente un enfriamiento extremo en lugar de un sobrecalentamiento crítico[cite: 1]. En consecuencia, los actuadores de refrigeración se apagarán o se activarán calentadores, derivando potencialmente en fallas catastróficas, daños estructurales irreversibles o incendios al no existir una interrupción por excepción a nivel de hardware.