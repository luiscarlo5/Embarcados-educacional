#include <stdio.h>
#include <stdint.h>

#define FRAMES 4 // Número de matrizes
#define ROWS 5   // Número de linhas
#define COLS 5   // Número de colunas

static const uint32_t new_piskel_data[FRAMES][ROWS * COLS] = {
    {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0xff00ffea, 0x00000000, 0x00000000, 
    0x00000000, 0xff00ffea, 0xff00ffea, 0xff00ffea, 0x00000000, 
    0x00000000, 0x00000000, 0xff00ffea, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    },
    {
    0x00000000, 0x00000000, 0xffeeff00, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0xffeeff00, 0xffeeff00, 0xffeeff00, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0xffeeff00, 0x00000000, 0x00000000
    },
    {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0xff07f110, 0x00000000, 0xff07f110, 0x00000000, 
    0x00000000, 0x00000000, 0xff07f110, 0x00000000, 0x00000000, 
    0x00000000, 0xff07f110, 0x00000000, 0xff07f110, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    },
    {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    }
};
    
void hex_to_rgb(uint32_t hex, int *r, int *g, int *b) {
    *r = (hex >> 16) & 0xFF; // Extrai o componente vermelho
    *g = (hex >> 8) & 0xFF;  // Extrai o componente verde
    *b = hex & 0xFF;         // Extrai o componente azul
}

int main() {
    int r, g, b;
    for (int frame = 0; frame < FRAMES; frame++) {
        printf("matriz[5][5][3] = {\n");
        for (int i = 0; i < ROWS; i++) {
            printf("{");

            for (int j = 0; j < COLS; j++) {
                hex_to_rgb(new_piskel_data[frame][j * COLS + i], &b, &g, &r);
                printf("{%d, %d, %d}, ", r, g, b);
            }
            printf("},");

            printf("\n");
        }
        printf("};");

        printf("\n");
    }
    return 0;
}
