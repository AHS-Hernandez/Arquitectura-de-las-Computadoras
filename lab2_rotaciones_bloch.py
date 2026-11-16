import math
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

def simular_angulo(theta: float):
    # 1. Circuito de 1 cúbit y 1 bit clásico
    qc = QuantumCircuit(1, 1)
    # 2. Rotación en el eje Y por ángulo continuo theta
    qc.ry(theta, 0)
    # 3. Medición en la base computacional Z
    qc.measure(0, 0)
    # 4. Simulación de 10,000 mediciones
    backend = AerSimulator()
    counts = backend.run(qc, shots=10000).result().get_counts()
    return counts

# Evaluación en 4 posiciones geométricas clave de la Esfera de Bloch:
angulos = [
    (0.0, "Polo Norte (theta = 0)"),
    (math.pi / 4, "Ángulo 45° (theta = pi/4)"),
    (math.pi / 2, "Ecuador (theta = pi/2, Estado |+>)"),
    (math.pi, "Polo Sur (theta = pi, Estado |1>)")
]

for theta, desc in angulos:
    c = simular_angulo(theta)
    p0 = (c.get('0', 0) / 10000) * 100
    p1 = (c.get('1', 0) / 10000) * 100
    print(f"{desc:35s} -> P(|0>): {p0:5.1f}% | P(|1>): {p1:5.1f}%")
