#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHUNK_SIZE_MB 50 // Bloques de 50 MB por iteracion

int main(void) {
    size_t chunk_bytes = (size_t)CHUNK_SIZE_MB * 1024 * 1024;
    size_t memoria_total_mb = 0;

    printf("[+] Iniciando consumo continuo de memoria...\n");
    printf("[+] Presiona Ctrl + C en cualquier momento para abortar.\n\n");

    while (1) {
        char *ptr = (char*)malloc(chunk_bytes);
        if (ptr == NULL) {
            printf("[!] Fallo malloc: el sistema denego mas asignaciones.\n");
            break;
        }

        // Escritura forzada para obligar al kernel a mapear paginas fisicas reales
        memset(ptr, 0xAA, chunk_bytes);

        memoria_total_mb += CHUNK_SIZE_MB;
        printf("-> Asignados y escritos: %zu MB acumulados\n", memoria_total_mb);

        // Breve pausa para observar el incremento en el monitor del sistema
        usleep(50000); // 50 milisegundos
    }

    printf("[+] Proceso finalizado. Esperando 30 segundos antes de liberar...\n");
    sleep(30);

    return 0;
}
