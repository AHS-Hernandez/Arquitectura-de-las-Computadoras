from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

# 1. Definición del circuito: 2 cúbits cuánticos y 2 bits clásicos
qc = QuantumCircuit(2, 2)

# 2. Creación de superposición equiprobable en el cúbit 0
qc.h(0)

# 3. Creación de entrelazamiento máximo mediante CNOT
qc.cx(0, 1)

# 4. Medición de los cúbits en los registros clásicos
qc.measure([0, 1], [0, 1])

# 5. Visualización del circuito en consola
print("CIRCUITO CUÁNTICO:")
print(qc.draw(output="text"))

# 6. Instanciación del simulador y ejecución de 10,000 disparos (shots)
backend = AerSimulator()
shots = 10000
job = backend.run(qc, shots=shots)
counts = job.result().get_counts(qc)

# 7. Presentación de los resultados estadísticos
print(f"\nRESULTADOS DE LA MEDICIÓN ({shots} disparos):")
for bitstring, count in sorted(counts.items()):
    porcentaje = (count / shots) * 100
    print(f"Estado |{bitstring}>: {count} ocurrencias ({porcentaje:.2f}%)")
