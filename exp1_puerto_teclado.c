#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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

static SystemBuses g_bus;
static TecladoHardware g_teclado;

void tick_controlador_teclado(SystemBuses *bus, TecladoHardware *kb) {
    if (bus->io_read) {
        if (bus->address_bus == 0x01) {
            bus->data_bus = kb->reg_status;
        } else if (bus->address_bus == 0x02) {
            bus->data_bus = kb->reg_data;
            kb->reg_status = 0; /* Handshake automatico: limpia estado */
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

int main(void) {
    printf("=== EXPERIMENTO 1: E/S POR PUERTO Y HANDSHAKE ===\n");

    /* 1. Estado inicial sin pulsacion */
    printf("[1] Estado inicial del puerto 0x01 (Status): 0x%02X\n", cpu_instruccion_in(0x01));

    /* 2. Simular pulsacion de tecla en hardware */
    g_teclado.reg_data = 'Z';
    g_teclado.reg_status = 0x01;
    printf("[2] Pulsacion de hardware simulada: Tecla '%c' (0x%02X)\n", g_teclado.reg_data, g_teclado.reg_data);

    /* 3. CPU sondea puerto 0x01 */
    uint8_t status = cpu_instruccion_in(0x01);
    printf("[3] Sondeo CPU puerto 0x01 (Status): 0x%02X (%s)\n", 
           status, (status & 0x01) ? "Tecla lista" : "No lista");

    /* 4. CPU lee dato por puerto 0x02 */
    if (status & 0x01) {
        uint8_t dato = cpu_instruccion_in(0x02);
        printf("[4] CPU lee puerto 0x02 (Data): '%c' (0x%02X)\n", dato, dato);
    }

    /* 5. Verificar Handshake automatico: puerto 0x01 debe estar limpio */
    uint8_t status_post = cpu_instruccion_in(0x01);
    printf("[5] Verificacion de Handshake post-lectura (Status): 0x%02X (%s)\n",
           status_post, (status_post == 0) ? "OK: Limpieza exitosa" : "ERROR");

    return 0;
}
EOF
