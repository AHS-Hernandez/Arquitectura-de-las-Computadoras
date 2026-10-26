#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define COLA_BASE_DIR  0x70
#define DIR_KB_HEAD    0x78
#define DIR_KB_TAIL    0x79
#define DIR_KB_COUNT   0x7A
#define COLA_CAPACIDAD 8

static uint8_t g_ram[256];

bool cola_teclado_encolar(uint8_t ascii) {
    uint8_t tail = g_ram[DIR_KB_TAIL];
    uint8_t count = g_ram[DIR_KB_COUNT];
    if (count >= COLA_CAPACIDAD) return false; /* BUFFER OVERFLOW */
    g_ram[COLA_BASE_DIR + tail] = ascii;
    g_ram[DIR_KB_TAIL] = (tail + 1) % COLA_CAPACIDAD;
    g_ram[DIR_KB_COUNT] = count + 1;
    return true;
}

bool cola_teclado_desencolar(uint8_t *salida_ascii) {
    uint8_t head = g_ram[DIR_KB_HEAD];
    uint8_t count = g_ram[DIR_KB_COUNT];
    if (count == 0) return false; /* COLA VACIA */
    *salida_ascii = g_ram[COLA_BASE_DIR + head];
    g_ram[DIR_KB_HEAD] = (head + 1) % COLA_CAPACIDAD;
    g_ram[DIR_KB_COUNT] = count - 1;
    return true;
}

void cola_teclado_imprimir_estado(void) {
    printf("RAM[HEAD]=%u, RAM[TAIL]=%u, RAM[COUNT]=%u | Buffer: [",
           g_ram[DIR_KB_HEAD], g_ram[DIR_KB_TAIL], g_ram[DIR_KB_COUNT]);
    for (int i = 0; i < COLA_CAPACIDAD; i++) {
        uint8_t val = g_ram[COLA_BASE_DIR + i];
        if (val >= 32 && val <= 126) printf(" '%c'", val);
        else printf(" 0x%02X", val);
    }
    printf(" ]\n");
}

int main(void) {
    printf("=== EXPERIMENTO 2: COLA CIRCULAR FIFO EN RAM ===\n");
    g_ram[DIR_KB_HEAD] = 0;
    g_ram[DIR_KB_TAIL] = 0;
    g_ram[DIR_KB_COUNT] = 0;

    /* 1. Encolar caracteres hasta llenar capacidad (8 elementos) */
    printf("\n[1] Encolando 8 caracteres ('A'..'H')...\n");
    for (char c = 'A'; c <= 'H'; c++) {
        if (cola_teclado_encolar((uint8_t)c)) {
            printf(" -> Encolado '%c' | ", c);
            cola_teclado_imprimir_estado();
        }
    }

    /* 2. Provocar Buffer Overflow con el 9no elemento */
    printf("\n[2] Intentando encolar 'I' con cola llena (Capacidad=8):\n");
    if (!cola_teclado_encolar('I')) {
        printf(" -> [OK] Deteccion de BUFFER OVERFLOW correcta (rechazado).\n");
    }

    /* 3. Desencolar 3 elementos */
    printf("\n[3] Desencolando 3 elementos:\n");
    for (int i = 0; i < 3; i++) {
        uint8_t ch;
        if (cola_teclado_desencolar(&ch)) {
            printf(" -> Desencolado '%c' | ", ch);
            cola_teclado_imprimir_estado();
        }
    }

    /* 4. Encolar 2 nuevos elementos para demostrar comportamiento circular */
    printf("\n[4] Encolando 'X' e 'Y' (punteros circulares):\n");
    cola_teclado_encolar('X');
    printf(" -> Encolado 'X' | "); cola_teclado_imprimir_estado();
    cola_teclado_encolar('Y');
    printf(" -> Encolado 'Y' | "); cola_teclado_imprimir_estado();

    /* 5. Vaciar completamente la cola */
    printf("\n[5] Vaciando la cola completa:\n");
    uint8_t ch;
    while (cola_teclado_desencolar(&ch)) {
        printf(" -> Extraido: '%c' (quedan %u)\n", ch, g_ram[DIR_KB_COUNT]);
    }
    if (!cola_teclado_desencolar(&ch)) {
        printf(" -> [OK] Deteccion de COLA VACIA correcta.\n");
    }

    return 0;
}
EOF
