#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define RAM_SIZE 256
#define COLA_BASE_DIR  0x70
#define DIR_KB_HEAD    0x78
#define DIR_KB_TAIL    0x79
#define DIR_KB_COUNT   0x7A
#define COLA_CAPACIDAD 8

#define VRAM_INICIO    0xF0
#define VRAM_FIN       0xFF

typedef struct {
    uint8_t address_bus;
    uint8_t data_bus;
    bool io_read;
    bool io_write;
    bool mem_read;
    bool mem_write;
} SystemBuses;

typedef struct {
    uint8_t reg_status; /* 0x01: Bit 0 = Tecla lista */
    uint8_t reg_data;   /* 0x02: Codigo ASCII */
} TecladoHardware;

typedef struct {
    uint8_t ram[RAM_SIZE];
    uint8_t cursor_vram;
} Computadora;

static SystemBuses g_bus;
static TecladoHardware g_teclado;
static Computadora g_pc;

/* --- CONTROLADOR DE TECLADO POR PUERTO --- */
void tick_controlador_teclado(SystemBuses *bus, TecladoHardware *kb) {
    if (bus->io_read) {
        if (bus->address_bus == 0x01) {
            bus->data_bus = kb->reg_status;
        } else if (bus->address_bus == 0x02) {
            bus->data_bus = kb->reg_data;
            kb->reg_status = 0; /* Handshake automatico */
        }
    }
}

uint8_t cpu_instruccion_in(uint8_t puerto) {
    g_bus.address_bus = puerto;
    g_bus.io_read = true;
    g_bus.mem_read = false;
    tick_controlador_teclado(&g_bus, &g_teclado);
    uint8_t dato = g_bus.data_bus;
    g_bus.io_read = false;
    return dato;
}

/* --- GESTOR DE COLA CIRCULAR EN RAM --- */
bool cola_teclado_encolar(uint8_t ascii) {
    uint8_t tail = g_pc.ram[DIR_KB_TAIL];
    uint8_t count = g_pc.ram[DIR_KB_COUNT];
    if (count >= COLA_CAPACIDAD) return false; /* BUFFER OVERFLOW */
    g_pc.ram[COLA_BASE_DIR + tail] = ascii;
    g_pc.ram[DIR_KB_TAIL] = (tail + 1) % COLA_CAPACIDAD;
    g_pc.ram[DIR_KB_COUNT] = count + 1;
    return true;
}

bool cola_teclado_desencolar(uint8_t *salida_ascii) {
    uint8_t head = g_pc.ram[DIR_KB_HEAD];
    uint8_t count = g_pc.ram[DIR_KB_COUNT];
    if (count == 0) return false; /* COLA VACIA */
    *salida_ascii = g_pc.ram[COLA_BASE_DIR + head];
    g_pc.ram[DIR_KB_HEAD] = (head + 1) % COLA_CAPACIDAD;
    g_pc.ram[DIR_KB_COUNT] = count - 1;
    return true;
}

/* --- CONTROLADOR DE PANTALLA VRAM --- */
void monitor_actualizar_pantalla(void) {
    printf("\n+--- MONITOR VRAM (4x4) ---+\n");
    for (int f = 0; f < 4; f++) {
        printf("| ");
        for (int c = 0; c < 4; c++) {
            uint8_t dir = VRAM_INICIO + (f * 4) + c;
            char ch = (char)g_pc.ram[dir];
            if (dir == g_pc.cursor_vram) {
                printf("[%c] ", (ch >= 32 && ch <= 126) ? ch : '_');
            } else {
                printf(" %c  ", (ch >= 32 && ch <= 126) ? ch : ' ');
            }
        }
        printf("|\n");
    }
    printf("+--------------------------+\n");
    printf("Cursor VRAM: 0x%02X | Cola Count: %u\n", g_pc.cursor_vram, g_pc.ram[DIR_KB_COUNT]);
}

void vram_scroll_arriba(void) {
    for (int f = 0; f < 3; f++) {
        for (int c = 0; c < 4; c++) {
            g_pc.ram[VRAM_INICIO + (f * 4) + c] = g_pc.ram[VRAM_INICIO + ((f + 1) * 4) + c];
        }
    }
    for (int c = 0; c < 4; c++) {
        g_pc.ram[VRAM_INICIO + (3 * 4) + c] = 0x20;
    }
    g_pc.cursor_vram = VRAM_INICIO + 12;
}

/* --- LOGICA INTEGRADORA CON BACKSPACE --- */
bool terminal_procesar_siguiente_tecla(void) {
    uint8_t tecla;
    if (!cola_teclado_desencolar(&tecla)) return false;
    
    if (tecla == 0x08 || tecla == 0x7F) { /* BACKSPACE */
        if (g_pc.cursor_vram > VRAM_INICIO) {
            g_pc.cursor_vram--;
            g_pc.ram[g_pc.cursor_vram] = 0x20; /* Borra caracter */
        }
    } else if (tecla >= 32 && tecla <= 126) {
        if (g_pc.cursor_vram >= VRAM_FIN) {
            g_pc.ram[g_pc.cursor_vram] = tecla;
            vram_scroll_arriba();
        } else {
            g_pc.ram[g_pc.cursor_vram] = tecla;
            g_pc.cursor_vram++;
        }
    }
    monitor_actualizar_pantalla();
    return true;
}

/* Simulador de hardware de teclado */
void hardware_presionar_tecla(uint8_t ascii) {
    g_teclado.reg_data = ascii;
    g_teclado.reg_status = 0x01;

    uint8_t status = cpu_instruccion_in(0x01);
    if (status & 0x01) {
        uint8_t dato = cpu_instruccion_in(0x02);
        cola_teclado_encolar(dato);
    }
}

int main(void) {
    printf("=== RETO INTEGRADOR: TERMINAL CON FIFO, VRAM Y BACKSPACE ===\n");
    memset(g_pc.ram, 0, RAM_SIZE);
    for (int i = VRAM_INICIO; i <= VRAM_FIN; i++) g_pc.ram[i] = 0x20;
    g_pc.cursor_vram = VRAM_INICIO;

    monitor_actualizar_pantalla();

    /* 1. Escribir palabra "HOLA" */
    printf("\n>>> Simulando tipeo de 'H', 'O', 'L', 'A'...\n");
    hardware_presionar_tecla('H'); terminal_procesar_siguiente_tecla();
    hardware_presionar_tecla('O'); terminal_procesar_siguiente_tecla();
    hardware_presionar_tecla('L'); terminal_procesar_siguiente_tecla();
    hardware_presionar_tecla('A'); terminal_procesar_siguiente_tecla();

    /* 2. Error y correccion con BACKSPACE */
    printf("\n>>> Simulando error: tipeo de 'X' y correccion con BACKSPACE (0x08)...\n");
    hardware_presionar_tecla('X'); terminal_procesar_siguiente_tecla();
    hardware_presionar_tecla(0x08); terminal_procesar_siguiente_tecla();

    /* 3. Llenar segunda fila */
    printf("\n>>> Escribiendo '1', '2', '3', '4' en segunda fila...\n");
    hardware_presionar_tecla('1'); terminal_procesar_siguiente_tecla();
    hardware_presionar_tecla('2'); terminal_procesar_siguiente_tecla();
    hardware_presionar_tecla('3'); terminal_procesar_siguiente_tecla();
    hardware_presionar_tecla('4'); terminal_procesar_siguiente_tecla();

    return 0;
}
EOF
