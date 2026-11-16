# INFORME TÉCNICO Y CUESTIONARIO DE EVALUACIÓN: PROGRAMACIÓN CUÁNTICA CON QISKIT EN LINUX UBUNTU

**Asignatura:** Arquitectura de Computadoras (SIS-131)  
**Docente:** Ing. Paulo César Loayza Carrasco  
**Fecha de Entrega:** 16 de Noviembre de 2026  
**Entorno de Ejecución:** GNU/Linux Ubuntu x86_64 (`.venv` Python 3 + Qiskit 1.x + Qiskit Aer C++)  

---

## 1. Arquitectura Full-Stack de Computación Cuántica (Sustrato Físico vs. Simulación)

Para ejecutar algoritmos cuánticos como los desarrollados en este laboratorio, el software escrito en espacio de usuario a temperatura ambiente ($300\text{ K}$) atraviesa múltiples etapas de transpilación, síntesis de pulsos y aislamiento térmico hasta alcanzar el procesador cuántico superconductor enfriado a $15\text{ mK}$ ($-273.135^\circ\text{C}$):

```
====================================================================================================
                      ARQUITECTURA HETEROGÉNEA CLÁSICO-CUÁNTICA (FULL-STACK)
====================================================================================================

 [ CAPA 1: SOFTWARE Y COMPILACIÓN (300 K - Espacio de Usuario Linux .venv) ]
   ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
   │  Código Python (Qiskit) ──► Transpilador (Mapeo Topológico) ──► Simulador Aer (AVX2 C++)   │
   └──────────────────────────────────────────────┬──────────────────────────────────────────────┘
                                                  │ (Instrucciones OpenQASM / Pulsos)
                                                  ▼
 [ CAPA 2: ELECTRÓNICA DE CONTROL Y SÍNTESIS RF (300 K - Racks de Instrumentación) ]
   ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
   │  Controladores FPGA ──► Generadores de Ondas Arbitrarias (AWG/DAC) ──► Pulsos Microondas    │
   │                                                                           (4 GHz - 8 GHz)   │
   └──────────────────────────────────────────────┬──────────────────────────────────────────────┘
                                                  │ (Líneas Coaxiales Semirrígidas CuNi / NbTi)
                                                  ▼
 [ CAPA 3: REFRIGERADOR DE DILUCIÓN Y ATENUACIÓN CRIOGÉNICA ]
   ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
   │  Etapa 50 K ──► Etapa 4 K (Amplificador HEMT) ──► Etapa Still (700 mK) ──► Atenuadores dB   │
   │  * Objetivo: Suprimir ruido térmico infrarrojo para evitar decoherencia de fase/amplitud.   │
   └──────────────────────────────────────────────┬──────────────────────────────────────────────┘
                                                  │ (Acoplamiento Capacitivo a Resonadores)
                                                  ▼
 [ CAPA 4: UNIDAD DE PROCESAMIENTO CUÁNTICO - QPU (15 mK) ]
   ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
   │  Chip Superconductor con Cúbits Transmon (Uniones Josephson Al/AlOx/Al) + Lectura TWPA     │
   └─────────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Consolidación de Resultados Experimentales (Ejecución en Terminal)

### 2.1 Práctica 1: Creación del Estado de Bell $|\Phi^+\rangle$ (`lab1_hola_mundo.py`)

#### Esquema del Circuito Ejecutado
```text
        ┌───┐     ┌─┐   
   q_0: ┤ H ├──■──┤M├───
        └───┘┌─┴─┐└╥┘┌─┐
   q_1: ─────┤ X ├─╫─┤M├
             └───┘ ║ └╥┘
   c: 2/═══════════╩══╩═
                   0  1 
```

#### Evolución del Vector de Estado ($|\psi\rangle$)
1. **Inicialización:** $|\psi_0\rangle = |00\rangle$
2. **Compuerta Hadamard en $q_0$ ($H \otimes I$):**
   $$|\psi_1\rangle = \left(\frac{|0\rangle + |1\rangle}{\sqrt{2}}\right) \otimes |0\rangle = \frac{|00\rangle + |10\rangle}{\sqrt{2}}$$
3. **Compuerta CNOT ($CX_{0,1}$):**
   $$|\psi_2\rangle = |\Phi^+\rangle = \frac{|00\rangle + |11\rangle}{\sqrt{2}}$$

#### Distribución Empírica Obtenida ($10\,000$ disparos)
```text
Estado |00>: 4994 ocurrencias (49.94%) █████████████████████████
Estado |01>:    0 ocurrencias ( 0.00%) 
Estado |10>:    0 ocurrencias ( 0.00%) 
Estado |11>: 5006 ocurrencias (50.06%) █████████████████████████
```

| Estado Base | Amplitud Teórica ($\alpha_i$) | Probabilidad Teórica ($|\alpha_i|^2$) | Conteo Experimental | Probabilidad Empírica | Desviación Estadística |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **$|00\rangle$** | $1/\sqrt{2}$ | $50.00\%$ | $4\,994$ | $49.94\%$ | $-0.06\%$ |
| **$|01\rangle$** | $0$ | $0.00\%$ | $0$ | $0.00\%$ | $0.00\%$ |
| **$|10\rangle$** | $0$ | $0.00\%$ | $0$ | $0.00\%$ | $0.00\%$ |
| **$|11\rangle$** | $1/\sqrt{2}$ | $50.00\%$ | $5\,006$ | $50.06\%$ | $+0.06\%$ |

---

### 2.2 Práctica 2: Rotaciones Continuas en la Esfera de Bloch (`lab2_rotaciones_bloch.py`)

#### Proyección Geométrica en el Plano $X$-$Z$ mediante $R_y(\theta)$
```text
                         |0> (Polo Norte: theta = 0)
                          |  P(|0>) = 100.0%
                          |
                        .-+-.  <-- theta = pi/4 (45°)
                      .'  |  '.    P(|0>) = 84.7% | P(|1>) = 15.3%
                     /    |    \
          |-⟩ ------+-----+-----+------ |+⟩ (Ecuador: theta = pi/2)
                     \    |    /             P(|0>) = 51.1% | P(|1>) = 48.9%
                      '.  |  .'
                        '-+-'
                          |
                         |1> (Polo Sur: theta = pi)
                             P(|1>) = 100.0%
```

#### Comparativa Analítica vs. Simulación Monte Carlo ($10\,000$ disparos)

| Posición Geométrica | Ángulo ($\theta$) | Fórmula Analítica $\cos^2(\theta/2)$ | $P(|0\rangle)$ Empírica | Fórmula Analítica $\sin^2(\theta/2)$ | $P(|1\rangle)$ Empírica |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Polo Norte** | $0$ | $100.00\%$ | **$100.0\%$** | $0.00\%$ | **$0.0\%$** |
| **Ángulo $45^\circ$** | $\pi/4$ | $85.36\%$ | **$84.7\%$** | $14.64\%$ | **$15.3\%$** |
| **Ecuador ($|+\rangle$)** | $\pi/2$ | $50.00\%$ | **$51.1\%$** | $50.00\%$ | **$48.9\%$** |
| **Polo Sur ($|1\rangle$)** | $\pi$ | $0.00\%$ | **$0.0\%$** | $100.00\%$ | **$100.0\%$** |

---

### 2.3 Práctica 3: Algoritmo de Deutsch y Ventaja Cuántica (`lab3_algoritmo_deutsch.py`)

#### Arquitectura del Circuito con Retroceso de Fase (*Phase Kickback*)
```text
          ┌───┐     ┌───────────┐┌───┐┌─┐
   q_0: ──┤ H ├─────┤0          ├┤ H ├┤M├───  (Cúbit de Consulta)
        ┌─┴───┴┐┌───┐│   U_f     │└───┘└╥┘
   q_1: ┤  X   ├┤ H ├┤1          ├──────╫───  (Cúbit Ancilla en |->)
        └──────┘└───┘└───────────┘      ║
   c: 1/════════════════════════════════╩═══
                                        0
```

#### Resultados de Clasificación Global en 1 Consulta (*Single Query*)

| Tipo de Oráculo $f(x)$ | Definición Lógica | Fase Inyectada en $q_0$ | Estado Pre-Hadamard | Medición $q_0$ | Clasificación Global |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **`constante_0`** | $f(0)=0, f(1)=0$ | $(-1)^0 = +1$ | $+\frac{|0\rangle + |1\rangle}{\sqrt{2}} = |+\rangle$ | **`'0'`** | **CONSTANTE** |
| **`constante_1`** | $f(0)=1, f(1)=1$ | $(-1)^1 = -1$ | $-\frac{|0\rangle + |1\rangle}{\sqrt{2}} = -|+\rangle$ | **`'0'`** | **CONSTANTE** |
| **`balanceada_identidad`** | $f(0)=0, f(1)=1$ | $+1$ y $-1$ | $+\frac{|0\rangle - |1\rangle}{\sqrt{2}} = |-\rangle$ | **`'1'`** | **BALANCEADA** |
| **`balanceada_inversa`** | $f(0)=1, f(1)=0$ | $-1$ y $+1$ | $-\frac{|0\rangle - |1\rangle}{\sqrt{2}} = -|-\rangle$ | **`'1'`** | **BALANCEADA** |

---

### 2.4 Práctica 4: Protocolo de Teletransportación Cuántica (`lab4_teletransportacion.py`)

#### Topología del Canal Cuántico y Clásico
```text
        ┌──────────┐      ┌───┐┌───┐ ░ ┌─┐            ░               ░    
   q_0: ┤ Ry(pi/3) ├──────┤ X ├┤ H ├─░─┤M├────────────░────────■──────░──── (Estado Alice)
        └──────────┘      └─┬─┘└───┘ ░ └╥┘┌─┐         ░        │      ░    
        ┌───┐       ┌───┐   │        ░  ║ └╥┘         ░        │      ░    
   q_1: ┤ H ├───────┤ X ├───■────────░──╫──╫──────────░───■────┼──────░──── (Par Bell Alice)
        └───┘       └─┬─┘            ░  ║  ║          ░   │    │      ░    
                    ┌─┴─┐            ░  ║  ║          ░ ┌─┴─┐┌─┴─┐    ░ ┌─┐
   q_2: ────────────┤ X ├────────────░──╫──╫──────────░─┤ X ├┤ Z ├────░─┤M├ (Par Bell Bob)
                    └───┘            ░  ║  ║          ░ └───┘└───┘    ░ └╥┘
   c: 3/════════════════════════════════╩══╩══════════════╪════╪═════════╩═
                                        0  1              │    │         2 
                                     [Canal Clásico de 2 Bits]
```

#### Verificación de Fidelidad de Transferencia en Bob ($q_2$)
* **Estado inicial preparado en $q_0$:** $|\psi\rangle = \cos(\pi/6)|0\rangle + \sin(\pi/6)|1\rangle = \frac{\sqrt{3}}{2}|0\rangle + \frac{1}{2}|1\rangle$
* **Probabilidad Teórica Esperada:** $P(|0\rangle) = 75.00\%$ | $P(|1\rangle) = 25.00\%$
* **Medición Experimental en $q_2$ ($10\,000$ shots):**
  * **$P(|0\rangle) = 75.67\%$** (Desviación: $+0.67\%$)
  * **$P(|1\rangle) = 24.33\%$** (Desviación: $-0.67\%$)

---

## 3. Resolución Detallada del Cuestionario de Evaluación y Retos Prácticos

### Pregunta 1
> **¿Por qué al aplicar una compuerta Hadamard sobre un cúbit en estado $|0\rangle$ se genera una probabilidad del $50\%$ de medir $0$ o $1$, pero su estado sigue siendo puro?**

#### Respuesta Técnica:
Un **estado puro** es aquel que puede describirse completamente mediante un único vector de estado $|\psi\rangle$ en el espacio de Hilbert $\mathcal{H}$, lo que implica ausencia de incertidumbre clásica sobre la preparación del sistema (su matriz de densidad cumple $\text{Tr}(\rho^2) = 1$).

1. **Transformación Unitaria Determinística:** La compuerta Hadamard es un operador lineal unitario ($H^\dagger H = I$):
   $$H|0\rangle = \frac{1}{\sqrt{2}}\begin{pmatrix} 1 & 1 \\ 1 & -1 \end{pmatrix}\begin{pmatrix} 1 \\ 0 \end{pmatrix} = \frac{1}{\sqrt{2}}\begin{pmatrix} 1 \\ 1 \end{pmatrix} = |+\rangle$$
   El vector resultante $|+\rangle$ apunta exactamente hacia el eje $+X$ en la superficie de la Esfera de Bloch (radio $r = 1$, pureza máxima). No existe ninguna mezcla estadística ni pérdida de coherencia de fase entre las componentes $|0\rangle$ y $|1\rangle$.
2. **Origen de la Aleatoriedad:** La probabilidad del $50\%$ no reside en una ignorancia previa del estado, sino en la **regla de Born** durante el proceso irreversible de medición proyectiva sobre la base computacional $\{|0\rangle, |1\rangle\}$ (eje $Z$), el cual es ortogonal al eje $X$ donde se encuentra el vector $|+\rangle$:
   $$P(0) = |\langle 0 | + \rangle|^2 = \left|\frac{1}{\sqrt{2}}\right|^2 = 0.50, \qquad P(1) = |\langle 1 | + \rangle|^2 = \left|\frac{1}{\sqrt{2}}\right|^2 = 0.50$$

---

### Pregunta 2
> **En el Estado de Bell $\frac{|00\rangle + |11\rangle}{\sqrt{2}}$, si medimos el primer cúbit y obtenemos `'1'`, ¿cuál es la probabilidad exacta de que el segundo cúbit colapse en `'0'`?**

#### Respuesta Técnica:
La probabilidad exacta es **$0\%$** ($0.00$).

#### Demostración Matemática por Proyección de Estado:
El estado compuesto es $|\Phi^+\rangle = \frac{1}{\sqrt{2}}|0\rangle_A \otimes |0\rangle_B + \frac{1}{\sqrt{2}}|1\rangle_A \otimes |1\rangle_B$.  
Al aplicar el operador de proyección $M_1 = |1\rangle\langle 1|_A \otimes I_B$ correspondiente a obtener `'1'` en el primer cúbit ($q_0$):

$$|\psi_{\text{post}}\rangle = \frac{M_1 |\Phi^+\rangle}{\sqrt{\langle \Phi^+ | M_1^\dagger M_1 | \Phi^+ \rangle}} = \frac{\frac{1}{\sqrt{2}}|1\rangle_A \otimes |1\rangle_B}{\sqrt{1/2}} = |11\rangle$$

Tras el colapso de la función de onda, el segundo cúbit ($q_1$) queda determinísticamente en el estado $|1\rangle$. Por lo tanto, la probabilidad de observar $|0\rangle$ en el segundo cúbit es $|\langle 0 | 1 \rangle|^2 = 0$.

---

### Pregunta 3 (Reto Práctico 1)
> **Modifique el script `lab1_hola_mundo.py` para generar el Estado de Bell $|\Psi^+\rangle = \frac{|01\rangle + |10\rangle}{\sqrt{2}}$. ¿Qué compuerta Pauli debe intercalar y sobre qué cúbit?**

#### Respuesta Técnica:
Se debe intercalar una compuerta **Pauli-X** (`qc.x(1)`) sobre el **segundo cúbit ($q_1$)** (ya sea antes o después de aplicar `qc.cx(0, 1)`).

#### Trazado Algebraico del Circuito Modificado:
1. Estado inicial: $|00\rangle$
2. Aplicar `qc.h(0)`: $\frac{|00\rangle + |10\rangle}{\sqrt{2}}$
3. Aplicar `qc.x(1)` (invierte $q_1$): $\frac{|01\rangle + |11\rangle}{\sqrt{2}}$
4. Aplicar `qc.cx(0, 1)` (invierte $q_1$ cuando $q_0 = 1$):
   $$CX_{0,1}\left(\frac{|01\rangle + |11\rangle}{\sqrt{2}}\right) = \frac{|01\rangle + |10\rangle}{\sqrt{2}} = |\Psi^+\rangle$$

#### Código Implementado (`reto1_bell_psi_plus.py`):
```python
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

qc = QuantumCircuit(2, 2)
qc.h(0)
qc.x(1)       # Compuerta Pauli-X intercalada sobre el cúbit 1
qc.cx(0, 1)
qc.measure([0, 1], [0, 1])

print(qc.draw(output="text"))
counts = AerSimulator().run(qc, shots=10000).result().get_counts()
print("Resultados Estado |Psi+>:", counts)
# Salida esperada: ~50% '01' y ~50% '10' (0% en '00' y '11')
```

---

### Pregunta 4 (Reto Práctico 2)
> **¿Qué ocurre en el Algoritmo de Deutsch si olvidamos aplicar la compuerta $X$ sobre el cúbit ancilla $q_1$ antes de aplicar Hadamard? Justifique qué pasaría con el *Phase Kickback*.**

#### Respuesta Técnica:
Si se omite `qc.x(1)`, el circuito fallará por completo en distinguir funciones constantes de balanceadas, retornando siempre `'0'` en $q_0$ ($100\%$ de las veces), diagnosticando falsamente cualquier función como **CONSTANTE**.

#### Justificación Física y Matemática (Anulación del *Phase Kickback*):
1. **Estado erróneo de la Ancilla:** Al partir de $|0\rangle$ y aplicar únicamente `qc.h(1)`, el cúbit $q_1$ queda preparado en el estado $|+\rangle = \frac{|0\rangle + |1\rangle}{\sqrt{2}}$ en lugar de $|-\rangle = \frac{|0\rangle - |1\rangle}{\sqrt{2}}$.
2. **Invarianza ante el Oráculo:** El oráculo unitario actúa según $U_f|x\rangle|y\rangle = |x\rangle|y \oplus f(x)\rangle$. Cuando el registro objetivo está en $|+\rangle$:
   * Si $f(x) = 0$: $U_f|x\rangle|+\rangle = |x\rangle (I|+\rangle) = +|x\rangle|+\rangle$
   * Si $f(x) = 1$: $U_f|x\rangle|+\rangle = |x\rangle (X|+\rangle) = |x\rangle\left(\frac{|1\rangle + |0\rangle}{\sqrt{2}}\right) = +|x\rangle|+\rangle$
3. **Desaparición de la Fase Relativa:** Debido a que $|+\rangle$ es autoestado de la compuerta $X$ con autovalor $+1$, el factor $(-1)^{f(x)}$ nunca se genera. El estado compuesto tras el oráculo permanece factorizado como $|+\rangle_0 \otimes |+\rangle_1$ sin importar si $f(x)$ es constante o balanceada. Al aplicar el segundo Hadamard en $q_0$, $H|+\rangle = |0\rangle$ determinísticamente, destruyendo la interferencia cuántica necesaria para la ventaja computacional.

---

### Pregunta 5
> **¿Por qué la teletransportación cuántica no permite transmitir información más rápido que la luz (superlumínica), a pesar de que el entrelazamiento es instantáneo?**

#### Respuesta Técnica:
El protocolo de teletransportación cuántica respeta estrictamente el **Teorema de No-Comunicación** (*No-Communication Theorem*) de la mecánica cuántica y la causalidad relativista por dos razones físicas fundamentales:

1. **Estado Local Indistinguible en Bob (Matriz de Densidad Reducida):**
   Antes de recibir los resultados de las mediciones de Alice, el estado del cúbit de Bob ($q_2$) descrito mediante la traza parcial sobre los cúbits de Alice ($A$) es:
   $$\rho_B = \text{Tr}_A(|\Psi_{\text{total}}\rangle\langle\Psi_{\text{total}}|) = \frac{1}{2}|0\rangle\langle 0| + \frac{1}{2}|1\rangle\langle 1| = \frac{I}{2}$$
   Esto representa un estado **máximamente mezclado** ($50\%$ de probabilidad de medir $0$ y $50\%$ de medir $1$, equivalente a ruido blanco aleatorio) completamente independiente de los parámetros $(\alpha, \beta)$ del estado $|\psi\rangle$ que Alice desea enviar. Bob no puede extraer ninguna información útil midiendo su cúbit de forma aislada.
2. **Dependencia Obligatoria del Canal Clásico de 2 Bits:**
   El colapso instantáneo proyecta el cúbit de Bob en uno de cuatro estados rotados posibles: $|\psi\rangle$, $X|\psi\rangle$, $Z|\psi\rangle$ o $XZ|\psi\rangle$. Para que Bob pueda aplicar las compuertas unitarias inversas ($CX$ y $CZ$) y reconstruir $|\psi\rangle$, debe recibir los **2 bits clásicos** resultantes de las mediciones de Alice. Dichos bits deben viajar a través de un canal físico clásico (fibra óptica, ondas de radio), cuya velocidad de propagación está acotada superiormente por la velocidad de la luz en el vacío ($c \approx 3 \times 10^8\text{ m/s}$).

---

## 4. Conclusiones Generales del Laboratorio

1. **Aislamiento de Entornos de Desarrollo:** El uso de entornos virtuales aislados (`.venv`) en Ubuntu garantiza la integridad del sistema operativo frente a la directiva PEP 668, permitiendo compilar y ejecutar simulaciones vectorizadas de alto rendimiento con `qiskit-aer` sin conflictos de dependencias.
2. **Validación Empírica del Entrelazamiento y Superposición:** Las simulaciones de $10\,000$ disparos demostraron una convergencia estadística superior al $99.3\%$ respecto a las predicciones analíticas de la mecánica cuántica, evidenciando tanto la ley trigonométrica de Malus/Born en la Esfera de Bloch como la correlación no local en los Estados de Bell.
3. **Eficiencia Algorítmica e Interferencia:** El Algoritmo de Deutsch comprobó que la superposición y el *Phase Kickback* permiten extraer propiedades globales de una función en una sola evaluación, reduciendo la complejidad de consulta en un $50\%$ respecto al paradigma clásico determinístico.
