#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdbool.h>

// Bandera atómica de control para finalización limpia
static volatile bool ejecutando = true;

// Requerimiento 4: Rutina intensiva de cómputo (~100% de uso de CPU)
void* rutina_estres(void *arg) {
    int core_asignado = *(int*)arg;
    cpu_set_t cpuset;

    // Requerimiento 3: Fijar la afinidad del hilo al núcleo seleccionado
    CPU_ZERO(&cpuset);
    CPU_SET(core_asignado, &cpuset);

    pthread_t hilo_actual = pthread_self();
    if (pthread_setaffinity_np(hilo_actual, sizeof(cpu_set_t), &cpuset) != 0) {
        fprintf(stderr, "[-] Error al fijar afinidad en el núcleo %d\n", core_asignado);
        pthread_exit(NULL);
    }

    printf("   -> [Hilo Activo] Ejecutando al 100%% fijado en el Núcleo: %d\n", core_asignado);

    // Carga de trabajo en bucle cerrado sobre la ALU
    double carga = 1.0001;
    while (ejecutando) {
        carga = (carga * 1.000001) + 0.000001;
        if (carga > 1000000.0) {
            carga = 1.0001;
        }
    }

    pthread_exit(NULL);
}

int main(void) {
    // Requerimiento 1: Detección del número total de núcleos lógicos
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores < 1) {
        perror("Error al detectar núcleos");
        return 1;
    }

    printf("===================================================\n");
    printf("   CONTROL DE AFINIDAD DE CPU Y ESTRÉS MULTIHILO   \n");
    printf("===================================================\n");
    printf("[1] Núcleos lógicos detectados: %ld (IDs válidos: 0 a %ld)\n\n", num_cores, num_cores - 1);

    // Requerimiento 2: Configuración (cuántos y cuáles núcleos)
    int cantidad = 0;
    do {
        printf("[2] ¿Cuántos núcleos desea estresar? (1 a %ld): ", num_cores);
        if (scanf("%d", &cantidad) != 1 || cantidad < 1 || cantidad > num_cores) {
            printf("    Entrada inválida. Ingrese un número entre 1 y %ld.\n", num_cores);
            while (getchar() != '\n'); // Limpiar búfer
            cantidad = 0;
        }
    } while (cantidad == 0);

    int *nucleos_elegidos = (int*)malloc(cantidad * sizeof(int));
    pthread_t *hilos = (pthread_t*)malloc(cantidad * sizeof(pthread_t));

    printf("\nSeleccione los núcleos a utilizar:\n");
    for (int i = 0; i < cantidad; i++) {
        int core_id = -1;
        do {
            printf(" -> Ingrese el ID del núcleo #%d (0 a %ld): ", i + 1, num_cores - 1);
            if (scanf("%d", &core_id) != 1 || core_id < 0 || core_id >= num_cores) {
                printf("    ID fuera de rango. Debe ser entre 0 y %ld.\n", num_cores - 1);
                while (getchar() != '\n');
                core_id = -1;
            }
        } while (core_id == -1);
        nucleos_elegidos[i] = core_id;
    }

    // Requerimiento 3: Creación de un hilo por cada núcleo seleccionado
    printf("\n[3] Creando hilos y fijando máscaras de afinidad...\n");
    for (int i = 0; i < cantidad; i++) {
        if (pthread_create(&hilos[i], NULL, rutina_estres, &nucleos_elegidos[i]) != 0) {
            fprintf(stderr, "Error al crear el hilo %d\n", i);
        }
    }

    // Limpiar salto de línea residual de scanf
    while (getchar() != '\n');

    // Requerimiento 5: Control y parada limpia al presionar ENTER
    printf("\n[!] Estrés en ejecución al ~100%% en los núcleos elegidos.\n");
    printf("[!] Abra otra terminal con 'htop' para observar la telemetría.\n");
    printf("\n===> Presione [ENTER] para detener todos los hilos limpiamente <===\n");
    getchar();

    printf("\n[5] Deteniendo hilos...\n");
    ejecutando = false;

    for (int i = 0; i < cantidad; i++) {
        pthread_join(hilos[i], NULL);
    }

    printf("[OK] Todos los hilos finalizaron con éxito. Carga normalizada.\n");

    free(nucleos_elegidos);
    free(hilos);
    return 0;
}
