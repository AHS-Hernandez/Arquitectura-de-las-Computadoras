#include <iostream>
#include <vector>
#include <chrono>

const int N = 2048; // Matriz de 2048x2048 floats (16 MB por matriz)

void transponer_naive(const std::vector<float>& A, std::vector<float>& B) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            // Salto de 8192 bytes en cada iteracion sobre B (Cache Thrashing severo)
            B[j * N + i] = A[i * N + j];
        }
    }
}

int main() {
    size_t total_elementos = static_cast<size_t>(N) * N;
    std::vector<float> A(total_elementos, 1.5f);
    std::vector<float> B(total_elementos, 0.0f);

    auto t_inicio = std::chrono::high_resolution_clock::now();
    transponer_naive(A, B);
    auto t_fin = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duracion = t_fin - t_inicio;
    std::cout << "Tiempo Naive (C++): " << duracion.count() << " segundos" << std::endl;

    return 0;
}
