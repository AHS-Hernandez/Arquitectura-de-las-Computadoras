#include <iostream>
#include <fstream>
#include <cstring>   // memcpy, memset
#include <vector>

int main() {
    const int TOTAL = 1000;
    const int SUBSET = 100;
    const int OFFSET = 500;

    //1. Simulación/Carga desde Memoria Secundaria
    std::ofstream outFile("datos.bin", std::ios::binary);
    for (int i = 0; i < TOTAL; ++i) {
        int val = i + 1;
        outFile.write(reinterpret_cast<char*>(&val), sizeof(int));
    }
    outFile.close();

    //Leer desde archivo a memoria RAM
    int arregloOriginal[TOTAL];
    std::ifstream inFile("datos.bin", std::ios::binary);
    inFile.read(reinterpret_cast<char*>(arregloOriginal), sizeof(arregloOriginal));
    inFile.close();

    // 2. Transferencia de memoria (índices 500 a 599)
    int arregloDestino[SUBSET];
    std::memcpy(arregloDestino, &arregloOriginal[OFFSET], SUBSET * sizeof(int));

    //Limpiar a cero el rango transferido en el arreglo original
    std::memset(&arregloOriginal[OFFSET], 0, SUBSET * sizeof(int));

    // 3. Validaciones y salida por consola
    std::cout << "--- Primeros 5 elementos originales ---\n";
    for (int i = 0; i < 5; ++i) std::cout << arregloOriginal[i] << " ";

    std::cout << "\n\n--- Ultimos 5 elementos originales ---\n";
    for (int i = TOTAL - 5; i < TOTAL; ++i) std::cout << arregloOriginal[i] << " ";

    std::cout << "\n\n--- 100 elementos transferidos ---\n";
    for (int i = 0; i < SUBSET; ++i) std::cout << arregloDestino[i] << " ";

    //Validación de ceros
    bool sonCeros = true;
    for (int i = OFFSET; i < OFFSET + SUBSET; ++i) {
        if (arregloOriginal[i] != 0) {
            sonCeros = false;
            break;
        }
    }
    std::cout << "\n\nValidacion ceros en rango [500-599]: " 
              << (sonCeros ? "CORRECTO (todos en 0)" : "ERROR") << "\n";

    return 0;
}
