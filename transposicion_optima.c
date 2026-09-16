#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 2048
#define BLOCK_SIZE 16 // 16 floats * 4 bytes = 64 bytes (1 linea de cache exacta)

static double medir_segundos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

/* Version con Bloqueo de Cache (Tiling) + Aritmetica de Punteros */
void transponer_tiling(const float *A, float *B) {
    for (int bi = 0; bi < N; bi += BLOCK_SIZE) {
        for (int bj = 0; bj < N; bj += BLOCK_SIZE) {
            
            /* Procesamiento del sub-bloque de 16x16 residente en L1 */
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                const float *ptr_a = &A[(bi + i) * N + bj];
                float *ptr_b = &B[bj * N + (bi + i)];
                
                for (int j = 0; j < BLOCK_SIZE; ++j) {
                    *ptr_b = ptr_a[j];
                    ptr_b += N; // Salto restringido solo a las lineas del bloque activo
                }
            }
        }
    }
}

int main(void) {
    size_t total = (size_t)N * N;
    size_t bytes = total * sizeof(float);

    float *A = (float*)malloc(bytes);
    float *B = (float*)malloc(bytes);

    for (size_t idx = 0; idx < total; ++idx) {
        A[idx] = (float)(idx % 100) * 0.05f;
        B[idx] = 0.0f;
    }

    double t0 = medir_segundos();
    transponer_tiling(A, B);
    double t_total = medir_segundos() - t0;

    printf("Tiempo Tiling Optimizado (ANSI C): %7.4f segundos\n", t_total);

    free(A);
    free(B);
    return 0;
}
