#include <stdio.h>

#define TAMANO 5

int main(void) {
    int arreglo[TAMANO] = {10, 20, 30, 40, 50}; // Arreglo almacenado en RAM
    int suma_total = 0;

    __asm__ (
        "xorl %%eax, %%eax\n\t"        // %%eax = 0 (acumulador de la suma)
        "xorl %%ecx, %%ecx\n\t"        // %%ecx = 0 (índice / contador i)
        "bucle_suma:\n\t"              // Inicio del bucle
        "cmpl %2, %%ecx\n\t"           // Compara el índice con TAMANO (%2)
        "jge fin_suma\n\t"             // Si i >= TAMANO, salir del bucle
        // Leer de RAM y acumular: eax += arreglo[i]
        "addl (%1, %%rcx, 4), %%eax\n\t" 
        "incl %%ecx\n\t"               // i++
        "jmp bucle_suma\n\t"           // Repetir bucle
        "fin_suma:\n\t"
        "movl %%eax, %0\n\t"           // Guardar el total en la variable de C
        : "=r" (suma_total)            // %0: Salida del resultado
        : "r" (arreglo), "r" (TAMANO)  // %1: Dirección base en RAM, %2: Límite
        : "%rax", "%rcx", "cc"         // Registros y banderas modificados
    );

    printf("--- Suma de Arreglo en RAM ---\n");
    printf("Resultado total acumulado: %d\n", suma_total);

    return 0;
}
