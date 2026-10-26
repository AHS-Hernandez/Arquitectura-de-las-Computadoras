#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static uint8_t g_ram[256];

bool vram_escribir(uint8_t fila, uint8_t col, char c) {
    if (fila >= 4 || col >= 4) return false;
    uint8_t dir = 0xF0 + (fila * 4) + col;
    g_ram[dir] = (uint8_t)c;
    return true;
}

void vram_scroll_arriba(void) {
    for (int f = 0; f < 3; f++) {
        for (int c = 0; c < 4; c++) {
            g_ram[0xF0 + (f * 4) + c] = g_ram[0xF0 + ((f + 1) * 4) + c];
        }
    }
    for (int c = 0; c < 4; c++) {
        g_ram[0xF0 + (3 * 4) + c] = 0x20; /* Blanquear fila 3 */
    }
}

void vram_inicializar(void) {
    for (int i = 0xF0; i <= 0xFF; i++) {
        g_ram[i] = 0x20;
    }
}

void vram_imprimir_monitor(const char *titulo) {
    printf("\n--- %s ---\n", titulo);
    printf("+------+------+------+------+\n");
    for (int f = 0; f < 4; f++) {
        printf("|");
        for (int c = 0; c < 4; c++) {
            uint8_t dir = 0xF0 + (f * 4) + c;
            char val = (char)g_ram[dir];
            printf("  %c   |", (val >= 32 && val <= 126) ? val : ' ');
        }
        printf(" (0x%02X..0x%02X)\n", 0xF0 + (f * 4), 0xF0 + (f * 4) + 3);
        printf("+------+------+------+------+\n");
    }
}

int main(void) {
    printf("=== EXPERIMENTO 3: CONTROLADOR DE VRAM Y SCROLL VERTICAL ===\n");
    vram_inicializar();

    /* 1. Llenar la pantalla de 4x4 */
    char letras[16] = {'A','B','C','D', 'E','F','G','H', 'I','J','K','L', 'M','N','O','P'};
    for (int i = 0; i < 16; i++) {
        vram_escribir(i / 4, i % 4, letras[i]);
    }
    vram_imprimir_monitor("ESTADO INICIAL VRAM (4x4)");

    /* 2. Scroll hacia arriba */
    printf("\n>>> Ejecutando vram_scroll_arriba()...\n");
    vram_scroll_arriba();
    vram_imprimir_monitor("POST 1er SCROLL (Fila 0 eliminada, Fila 3 limpia)");

    /* 3. Escribir en la nueva fila inferior */
    printf("\n>>> Escribiendo 'W', 'X', 'Y', 'Z' en Fila 3...\n");
    vram_escribir(3, 0, 'W');
    vram_escribir(3, 1, 'X');
    vram_escribir(3, 2, 'Y');
    vram_escribir(3, 3, 'Z');
    vram_imprimir_monitor("POST ESCRITURA EN ULTIMA FILA");

    return 0;
}
EOF
