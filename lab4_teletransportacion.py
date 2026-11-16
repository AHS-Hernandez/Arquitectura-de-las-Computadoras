import math
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

# Circuito con 3 cúbits y 3 bits clásicos:
# q0 = Cúbit a teletransportar (Alice)
# q1 = Cúbit del par de Bell de Alice
# q2 = Cúbit del par de Bell de Bob
qc = QuantumCircuit(3, 3)

# 1. Preparar un estado cuántico arbitrario en q0 mediante Ry(pi/3)
# Estado teórico: (sqrt(3)/2)|0> + (1/2)|1> -> P(|0>) = 75%, P(|1>) = 25%
qc.ry(math.pi / 3, 0)
qc.barrier()

# 2. Generar el par entrelazado de Bell entre Alice (q1) y Bob (q2)
qc.h(1)
qc.cx(1, 2)
qc.barrier()

# 3. Operaciones de Alice: Medición en la base de Bell
qc.cx(0, 1)
qc.h(0)
qc.barrier()
qc.measure(0, 0)
qc.measure(1, 1)

# 4. Operaciones de Corrección en Bob condicionadas por los bits recibidos
qc.barrier()
qc.cx(1, 2)  # Si bit 1 es 1 -> aplicar Pauli-X
qc.cz(0, 2)  # Si bit 0 es 1 -> aplicar Pauli-Z

# 5. Medición final del cúbit q2 en Bob para verificar coincidencia
qc.barrier()
qc.measure(2, 2)

counts = AerSimulator().run(qc, shots=10000).result().get_counts()

# Contabilizar el resultado medido en el bit más significativo (cúbit de Bob q2):
bob = {'0': 0, '1': 0}
for bitstring, n in counts.items():
    bob[bitstring[0]] += n

print("VERIFICACIÓN EN EL CÚBIT DE BOB (q2):")
print(f"P(0): {bob['0']/100:.2f}% (Teórico: 75.00%)")
print(f"P(1): {bob['1']/100:.2f}% (Teórico: 25.00%)")
