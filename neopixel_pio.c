#include <stdio.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"  
#include "math.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"


// Definicao de notas musicais que serao uzadas nas musicas com buzzer 
#pragma region Notas musicais

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

#pragma endregion

void play_jogo_musica(uint PIN_BUZZER, int musica);
void play_musica(uint PIN_BUZZER, int musica);
void play_tone(uint pin, uint freq_nota);
void stop_tone(uint pin);
void setas_para_musicajoystick();
bool joystick_music();

void menu_principal();
void menu_musica();
void menu_matematica();

void mudar_jogo(int po);
void mudar_musica(int po);
void mudar_operacao_matematica(int po);


// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN_MATRIZ_5X5 7 // pino de informacao da matriz de leds 5x5
#define BUZZER_PIN 21        // pino do buzzer utilizado


const uint I2C_SDA = 14; // configuracao dos pinos de comunicacao 
const uint I2C_SCL = 15; // com o joystick

#define VRX_PIN 26       // Define o pino GP26 para o eixo X do joystick (Canal ADC0)
#define VRY_PIN 27       // Define o pino GP27 para o eixo Y do joystick (Canal ADC1)
#define SW_PIN 22        // Define o pino GP22 para o botão do joystick (entrada digital)

const uint BUTTON_PIN_1 = 5;  // Define o pino GPIO 5 para o botão.
const uint BUTTON_PIN_2 = 6;  // Define o pino GPIO 5 para o botão.


///////////////////////////////////////////////////////////////////////////
// configuracoes para o uso da matriz de leds 5x5 da bitdoglab

// Definição de pixel GRB
struct pixel_t {
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

// Inicializa a máquina PIO para controle da matriz de LEDs.
void npInit(uint pin) {
    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }

    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

// Atribui uma cor RGB a um LED.
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

// Limpa o buffer de pixels.
void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

// Escreve os dados do buffer nos LEDs.
void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Função para obter o índice de LED a partir das coordenadas (x, y)
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}



/////////////////////////////////////////
// Funcao para exibir matriz em metiz de leds 5x5
void processar_matriz(int matriz_led[5][5][3]);
// Funcao para sorteiar numeros para o jogo de matematica
int sortear_numeros_matriz_leds() {
    // Inicializar o gerador de números aleatórios com o tempo atual
    srand(time(0));
    // Sorteia um número entre 1 e 10
    int sorteado = rand() % 10 + 1;
    // Exibe o número sorteado
    printf("Número sorteado: %d\n", sorteado);
    // Condições baseadas no número sorteado
    return sorteado;
}
// Funçao para verificar os numeros esxolhidos/sorteados na matriz de leds 5x5
int exibir_numeros_matriz_leds(int sorteado) {
  
    int um[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {232, 0, 0}, {0, 0, 0}, {0, 0, 0}, {232, 0, 0} },
        {{232, 0, 0}, {232, 0, 0}, {232, 0, 0}, {232, 0, 0}, {232, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {232, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int dois[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 255, 238} },
        {{0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238} },
        {{0, 255, 238}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int tres[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{234, 255, 0}, {0, 0, 0}, {234, 255, 0}, {0, 0, 0}, {234, 255, 0} },
        {{234, 255, 0}, {0, 0, 0}, {234, 255, 0}, {0, 0, 0}, {234, 255, 0} },
        {{234, 255, 0}, {234, 255, 0}, {234, 255, 0}, {234, 255, 0}, {234, 255, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int quatro[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{24, 250, 4}, {24, 250, 4}, {24, 250, 4}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {24, 250, 4}, {0, 0, 0}, {0, 0, 0} },
        {{24, 250, 4}, {24, 250, 4}, {24, 250, 4}, {24, 250, 4}, {24, 250, 4} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int cinco[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{255, 65, 0}, {255, 65, 0}, {255, 65, 0}, {0, 0, 0}, {255, 65, 0} },
        {{255, 65, 0}, {0, 0, 0}, {255, 65, 0}, {0, 0, 0}, {255, 65, 0} },
        {{255, 65, 0}, {0, 0, 0}, {255, 65, 0}, {255, 65, 0}, {255, 65, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int seis[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{255, 0, 219}, {255, 0, 219}, {255, 0, 219}, {255, 0, 219}, {255, 0, 219} },
        {{255, 0, 219}, {0, 0, 0}, {255, 0, 219}, {0, 0, 0}, {255, 0, 219} },
        {{255, 0, 219}, {0, 0, 0}, {255, 0, 219}, {255, 0, 219}, {255, 0, 219} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int sete[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{25, 27, 243}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{25, 27, 243}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{25, 27, 243}, {25, 27, 243}, {25, 27, 243}, {25, 27, 243}, {25, 27, 243} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int oito[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255} },
        {{255, 255, 255}, {0, 0, 0}, {255, 255, 255}, {0, 0, 0}, {255, 255, 255} },
        {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int nove[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{4, 102, 26}, {4, 102, 26}, {4, 102, 26}, {0, 0, 0}, {4, 102, 26} },
        {{4, 102, 26}, {0, 0, 0}, {4, 102, 26}, {0, 0, 0}, {4, 102, 26} },
        {{4, 102, 26}, {4, 102, 26}, {4, 102, 26}, {4, 102, 26}, {4, 102, 26} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int dez[5][5][3] = {
        {{144, 8, 181}, {144, 8, 181}, {144, 8, 181}, {144, 8, 181}, {144, 8, 181} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{144, 8, 181}, {144, 8, 181}, {144, 8, 181}, {144, 8, 181}, {144, 8, 181} },
        {{144, 8, 181}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {144, 8, 181} },
        {{144, 8, 181}, {144, 8, 181}, {144, 8, 181}, {144, 8, 181}, {144, 8, 181} }
    };
    
    
    if (sorteado == 1) {
        processar_matriz(um);
        printf("Operação 1: Ação para o número 1\n");
    } 
    else if (sorteado == 2) {
        processar_matriz(dois);
        printf("Operação 2: Ação para o número 2\n");
    } 
    else if (sorteado == 3) {
        processar_matriz(tres);
        printf("Operação 3: Ação para o número 3\n");
    } 
    else if (sorteado == 4) {
        processar_matriz(quatro);
        printf("Operação 4: Ação para o número 4\n");
    } 
    else if (sorteado == 5) {
        processar_matriz(cinco);
        printf("Operação 5: Ação para o número 5\n");
    } 
    else if (sorteado == 6) {
        processar_matriz(seis);
        printf("Operação 6: Ação para o número 6\n");
    } 
    else if (sorteado == 7) {
        processar_matriz(sete);
        printf("Operação 7: Ação para o número 7\n");
    } 
    else if (sorteado == 8) {
        processar_matriz(oito);
        printf("Operação 8: Ação para o número 8\n");
    } 
    else if (sorteado == 9) {
        processar_matriz(nove);
        printf("Operação 9: Ação para o número 9\n");
    } 
    else if (sorteado == 10) {
        processar_matriz(dez);
        printf("Operação 10: Ação para o número 10\n");
    }
    return sorteado;
}
// Funcao que processa a matriz e chama npSetLED
void processar_matriz(int matriz_led[5][5][3]) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int posicao = getIndex(i, j);  // Supondo que a função getIndex seja definida em algum lugar
            npSetLED(posicao, matriz_led[i][j][0], matriz_led[i][j][1], matriz_led[i][j][2]);
        }
    }
    // Escreve os dados nos LEDs inicialmente.
    npWrite();
}
// Funcao para exibir a operacao matematica
void operacao_matematica_matriz_led(int operacao){
    int somar[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {234, 255, 0}, {0, 0, 0}, {0, 0, 0} },        
        {{0, 0, 0}, {234, 255, 0}, {234, 255, 0}, {234, 255, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {234, 255, 0}, {0, 0, 0}, {0, 0, 0} },        
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
    };
    int dividir[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 0, 0} },        
        {{0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238} },
        {{0, 0, 0}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 0, 0} },        
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
    };
    int multiplicar[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0} },      
        {{0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0} },      
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
    };
    int subtrair[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },  
        {{0, 0, 0}, {0, 0, 0}, {255, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {255, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {255, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },  
        };

    if(operacao == 1){
        processar_matriz(somar);
        printf("Operação +\n");
    }else if(operacao == 2){
        processar_matriz(dividir);
        printf("Operação /\n");
    }else if(operacao == 3){
        processar_matriz(multiplicar);
        printf("Operação x\n");
    }else if(operacao == 4 ){
        processar_matriz(subtrair);
        printf("Operação -\n");
    }else {
        // 
        printf("ERRO\n");
        // retornar ao menu

    }
}
// Funcao principal para executar o jogo de matematica
void play_matematica(int op, uint PIN_1, uint PIN_2){
    if(op == 1 ){
        int rodadas =0;
        
        while(1){
        npClear();
        
        if(rodadas == 2){
            break;
        }
        int a = sortear_numeros_matriz_leds();
        int b = sortear_numeros_matriz_leds();
        exibir_numeros_matriz_leds(a);
        sleep_ms(4000);
        npClear();

        operacao_matematica_matriz_led(op);
        sleep_ms(2500);
        npClear();

        exibir_numeros_matriz_leds(b);

        int resposta = a+b;
        int botao_pressionado = 0;
        bool botao_pressionado_1 = !gpio_get(PIN_1); // O botão pressionado gera um nível baixo (0).
        bool botao_pressionado_2 = !gpio_get(PIN_2); // O botão pressionado gera um nível baixo (0).
        while(1){
          if(!gpio_get(PIN_1) || !gpio_get(PIN_2)){
            botao_pressionado++;
            if(botao_pressionado == resposta){
                printf("\nACERTOU\n");
                break;
            }
          }
        }
        rodadas++;
        sleep_ms(50);
        }    
    }
    if(op == 2 ){
        int rodadas =0;
        
        while(1){
        if(rodadas == 2){
            break;
        }
        int a = sortear_numeros_matriz_leds();
        int b = sortear_numeros_matriz_leds();
        while(1){
          if(b<a){
            break;
          }
          b = sortear_numeros_matriz_leds();
        }
        int resposta = a-b;
        int botao_pressionado = 0;
        bool botao_pressionado_1 = !gpio_get(PIN_1); // O botão pressionado gera um nível baixo (0).
        bool botao_pressionado_2 = !gpio_get(PIN_2); // O botão pressionado gera um nível baixo (0).

        exibir_numeros_matriz_leds(a);
        sleep_ms(4000);
        npClear();

        operacao_matematica_matriz_led(op);
        sleep_ms(2500);
        npClear();

        exibir_numeros_matriz_leds(b);
        while(1){
          if(!gpio_get(PIN_1) || !gpio_get(PIN_2)){
            botao_pressionado++;
            if(botao_pressionado == resposta){
                printf("\nACERTOU\n");
                break;
            }
          }
        }
        rodadas++;
        sleep_ms(50);
        }    
    }
    else if(op == 3 ){
        int rodadas =0;
        
        while(1){
        npClear();
        
        if(rodadas == 2){
            break;
        }
        int a = sortear_numeros_matriz_leds();
        int b = sortear_numeros_matriz_leds();
        while(1){
          if(a%b == 0 && a>b){
            break;
          }
          a = sortear_numeros_matriz_leds();
          b = sortear_numeros_matriz_leds();
        }
        int resposta = a/b;
        int botao_pressionado = 0;
        bool botao_pressionado_1 = !gpio_get(PIN_1); // O botão pressionado gera um nível baixo (0).
        bool botao_pressionado_2 = !gpio_get(PIN_2); // O botão pressionado gera um nível baixo (0).

        exibir_numeros_matriz_leds(a);
        sleep_ms(4000);
        npClear();

        operacao_matematica_matriz_led(op);
        sleep_ms(2500);
        npClear();

        exibir_numeros_matriz_leds(b);

        while(1){
          if(!gpio_get(PIN_1) || !gpio_get(PIN_2)){
            botao_pressionado++;
            if(botao_pressionado == resposta){
                printf("\nACERTOU\n");
                break;
            }
          }
        }
        rodadas++;
        sleep_ms(50);
        }    
    }
    else if(op == 4 ){
        int rodadas =0;
        
        while(1){
        npClear();
        
        if(rodadas == 2){
            break;
        }
        int a = sortear_numeros_matriz_leds();
        int b = sortear_numeros_matriz_leds();

        
        while(1){
          if( a*b <= 25){
            break;
          }
          a = sortear_numeros_matriz_leds();
          b = sortear_numeros_matriz_leds();
        }
        int resposta = a*b;
        int botao_pressionado = 0;
        bool botao_pressionado_1 = !gpio_get(PIN_1); // O botão pressionado gera um nível baixo (0).
        bool botao_pressionado_2 = !gpio_get(PIN_2); // O botão pressionado gera um nível baixo (0).
        exibir_numeros_matriz_leds(a);
        sleep_ms(4000);
        npClear();

        operacao_matematica_matriz_led(op);
        sleep_ms(2500);
        npClear();

        exibir_numeros_matriz_leds(b);


        while(1){
          if(!gpio_get(PIN_1) || !gpio_get(PIN_2)){
            botao_pressionado++;
            if(botao_pressionado == resposta){
                printf("\nACERTOU\n");
                break;
            }
          }
        }
        rodadas++;
        sleep_ms(50);
        }    
    }
}


void setas_para_musica(int op){
    npClear(); // limpa matriz de leds

    // definição das matrizes referentes a cada uma das direcoes de seta
    int cima_esquerdo[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {16, 241, 7} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0} },
        {{16, 241, 7}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0} },
        {{16, 241, 7}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{16, 241, 7}, {16, 241, 7}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0} }
    };
    int baixo_direito[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {16, 241, 7}, {16, 241, 7} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {16, 241, 7} },
        {{0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {16, 241, 7} },
        {{0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{16, 241, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }
    };
    int cima_direito[5][5][3] = {
        {{16, 241, 7}, {16, 241, 7}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0} },
        {{16, 241, 7}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{16, 241, 7}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {16, 241, 7} }
    };
    int baixo_esquerdo[5][5][3] = {
        {{16, 241, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
        {{0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {0, 0, 0}, {16, 241, 7} },
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {16, 241, 7} },
        {{0, 0, 0}, {0, 0, 0}, {16, 241, 7}, {16, 241, 7}, {16, 241, 7} }
    };
    sleep_ms(10);
    if(op == 1){
        processar_matriz(cima_esquerdo);
        printf("\n- cima esquerdo -");
    }
    else if(op == 2){
        processar_matriz(baixo_direito);
        printf("\n- baixo direito -");
    }
    else if(op == 3){
        processar_matriz(cima_direito);
        printf("\n- cima direito -");
    }
    else if(op == 4){
        processar_matriz(baixo_esquerdo);
        printf("\n- baixo esquerdo -");
    }
}






// Definicao d eparametros para musicas utilizadas com buzzer
int melodia_corderosa[] = {

  // Pink Panther theme
  // Score available at https://musescore.com/benedictsong/the-pink-panther
  // Theme by Masato Nakamura, arranged by Teddy Mason

  REST,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,2, REST,4, REST,8, NOTE_DS4,4,

  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_G4,8, NOTE_B4,-8, NOTE_E5,8,
  NOTE_DS5,1,   
  NOTE_D5,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,-4, REST,4,
  REST,4, NOTE_E5,-8, NOTE_D5,8, NOTE_B4,-8, NOTE_A4,8, NOTE_G4,-8, NOTE_E4,-8,
  NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8,   
  NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, NOTE_E4,16, NOTE_E4,16, NOTE_E4,2,
 
};
int melodia_auto[] = {
  
  // Pulo da gaita - Auto da Compadecida 
  // Score available at https://musescore.com/user/196039/scores/250206
  
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,2,
  
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_F4,8, NOTE_E4,8, NOTE_D4,8, NOTE_C4,8,
  NOTE_C4,2,

  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,2,

  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_F4,8, NOTE_E4,8, NOTE_D4,8, NOTE_C4,8,
  NOTE_C4,16, NOTE_D5,8, NOTE_D5,16, NOTE_D5,16, NOTE_D5,8, NOTE_D5,16,

  NOTE_D5,16, NOTE_D5,8, NOTE_D5,16, NOTE_C5,8, NOTE_E5,-8,
  NOTE_C5,8, NOTE_C5,16, NOTE_E5,16, NOTE_E5,8, NOTE_C5,16,
  NOTE_F5,8, NOTE_D5,8, NOTE_D5,8, NOTE_E5,-8,
  NOTE_C5,8, NOTE_D5,16, NOTE_E5,16, NOTE_D5,8, NOTE_C5,16,

  NOTE_F5,8, NOTE_F5,8, NOTE_A5,8, NOTE_G5,-8,//21
  NOTE_G5,8, NOTE_C5,16, NOTE_C5,16, NOTE_C5,8, NOTE_C5,16,
  NOTE_F5,-8, NOTE_E5,16, NOTE_D5,8, NOTE_C5,4,
  NOTE_C5,16, NOTE_C5,16, NOTE_C5,16, NOTE_C5,16,

  NOTE_F5,8, NOTE_F5,16, NOTE_A5,8, NOTE_G5,-8,//25
  NOTE_G5,8, NOTE_C5,16, NOTE_C5,16, NOTE_C5,8, NOTE_C5,16,
  NOTE_F5,16, NOTE_E5,8, NOTE_D5,16, NOTE_C5,8, NOTE_E5,-8,
  NOTE_C5,8, NOTE_D5,16, NOTE_E5,16, NOTE_D5,8, NOTE_C5,16,
 
  NOTE_F5,8, NOTE_F5,16, NOTE_A5,8, NOTE_G5,-8,//29
  NOTE_G5,8, NOTE_C5,16, NOTE_C5,16, NOTE_C5,8, NOTE_C5,16,
  NOTE_F5,8, NOTE_E5,16, NOTE_D5,8, NOTE_C5,8,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,

  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,2,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,

  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_F4,8, NOTE_E4,8, NOTE_D4,8, NOTE_C4,-2,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,

  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,
  NOTE_G4,2,
  NOTE_C5,4, NOTE_G4,8, NOTE_AS4,4, NOTE_A4,8,

  NOTE_G4,16, NOTE_C4,8, NOTE_C4,16, NOTE_G4,16, NOTE_G4,8, NOTE_G4,16,
  NOTE_F4,8, NOTE_E4,8, NOTE_D4,8, NOTE_C4,-2,
  NOTE_C4,16, NOTE_C4,8, NOTE_C4,16, NOTE_E4,16, NOTE_E4,8, NOTE_E4,16,
  NOTE_F4,16, NOTE_F4,8, NOTE_F4,16, NOTE_FS4,16, NOTE_FS4,8, NOTE_FS4,16,

  NOTE_G4,8, REST,8, NOTE_AS4,8, NOTE_C5,1,
  

};
int melodia_harrypotter[] = {


  // Hedwig's theme fromn the Harry Potter Movies
  // Socre from https://musescore.com/user/3811306/scores/4906610
  
  REST, 2, NOTE_D4, 4,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_C5, -2, 
  NOTE_A4, -2,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_F4, 2, NOTE_GS4, 4,
  NOTE_D4, -1, 
  NOTE_D4, 4,

  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4, //10
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_B4, 4,
  NOTE_G4, -1,
  NOTE_AS4, 4,
     
  NOTE_D5, 2, NOTE_AS4, 4,//18
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_DS5, 2, NOTE_D5, 4,
  NOTE_CS5, 2, NOTE_A4, 4,
  NOTE_AS4, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_D4, 4,
  NOTE_D5, -1, 
  REST,4, NOTE_AS4,4,  

  NOTE_D5, 2, NOTE_AS4, 4,//26
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_AS4, 4,
  NOTE_G4, -1, 
  
};


// Toca uma nota com a frequência e duração especificadas
void play_tone(uint PIN, uint freq_nota) {
  
  gpio_set_function(PIN, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(PIN);

  // Calcular o divisor e o TOP
  double pwm_div = 125000000.0 / (freq_nota * 65536.0); // Divisor ideal
  double top = (125000000.0 / pwm_div);
  top = top / freq_nota ; // Valor máximo (TOP)
  float level = top /2.0;

  // Configurar o divisor e o TOP do PWM
  pwm_set_clkdiv(slice_num, pwm_div);
  pwm_set_wrap(slice_num, (top/2)*1.2);
  pwm_set_gpio_level(PIN, (level/2)*1.2); // 50% de duty cycle
  pwm_set_enabled(slice_num, true);
}
// Funcao para parar saida em buzzer pwm com frequencia com base na nota musical selecionada
void stop_tone(uint PIN){
  pwm_set_gpio_level(PIN, 0); // Desliga o som após a duração
  sleep_ms(50); // Pausa entre notas
}


// Função principal para tocar a música
void play_jogo_musica(uint PIN_BUZZER, int musica) {
  int divisor = 0, duracao_da_Nota = 0;
  int tempo_auto = 100, tempo_harry = 144, tempo_corderosa = 120;

  if( musica == 1){
    play_musica(PIN_BUZZER, musica);

    int notes = sizeof(melodia_harrypotter) / sizeof(melodia_harrypotter[0]) / 2;
    int seminia = (60000 * 4) / tempo_harry;

    for (int i = 0; i < notes*2; i = i+2) {
      divisor = melodia_harrypotter[i+1];
      if (divisor > 0) {
        duracao_da_Nota = (seminia) / divisor;
      } else if (divisor < 0) {
        duracao_da_Nota = (seminia) / fabs(divisor);
        duracao_da_Nota *= 1.5; 
      }
      play_tone(PIN_BUZZER,  melodia_harrypotter[i]);  
      absolute_time_t proxima_parada = delayed_by_us(get_absolute_time(),  duracao_da_Nota  * 5 * 1000);

      // proxima_parada = delayed_by_us(proxima_parada, duracao_da_Nota * 2 * 1000);
      bool saida = false;
      setas_para_musicajoystick();
      sleep_ms(50);

      while(1){
      sleep_ms(50);

      saida = joystick_music();
      sleep_ms(50);

        if (time_reached(proxima_parada) || saida == true ) {
              // Imprime uma mensagem na saída serial indicando que 1 segundo se passou.
          printf("Proxima nota\n");
          break; // quebra o loop para que haja a proxima nota
        }
      }
      
      stop_tone(PIN_BUZZER);
      sleep_ms(50);
    }
  }
  else if( musica == 2){
    play_musica(PIN_BUZZER, musica);
    int notes = sizeof(melodia_corderosa) / sizeof(melodia_corderosa[0]) / 2;
    int seminia = (60000 * 4) / tempo_corderosa;

    for (int i = 0; i < notes*2; i = i+2) {
      divisor = melodia_corderosa[i+1];
      if (divisor > 0) {
        duracao_da_Nota = (seminia) / divisor;
      } else if (divisor < 0) {
        duracao_da_Nota = (seminia) / fabs(divisor);
        duracao_da_Nota *= 1.5; 
      }
      play_tone(PIN_BUZZER,  melodia_corderosa[i]);  
      absolute_time_t proxima_parada = delayed_by_us(get_absolute_time(),  duracao_da_Nota  * 5 * 1000);

      // proxima_parada = delayed_by_us(proxima_parada, duracao_da_Nota * 2 * 1000);
      bool saida = false;
      setas_para_musicajoystick();
      sleep_ms(50);

      while(1){
      sleep_ms(50);

      saida = joystick_music();
      sleep_ms(50);

        if (time_reached(proxima_parada) || saida == true ) {
              // Imprime uma mensagem na saída serial indicando que 1 segundo se passou.
          printf("Proxima nota\n");
          break; // quebra o loop para que haja a proxima nota
        }
      }
      
      stop_tone(PIN_BUZZER);
      sleep_ms(50);
    }
  }
  else if( musica == 3){
    play_musica(PIN_BUZZER, musica);
    int notes = sizeof(melodia_auto) / sizeof(melodia_auto[0]) / 2;
    int seminia = (60000 * 4) / tempo_corderosa;

    for (int i = 0; i < notes*2; i = i+2) {
      divisor = melodia_auto[i+1];
      if (divisor > 0) {
        duracao_da_Nota = (seminia) / divisor;
      } else if (divisor < 0) {
        duracao_da_Nota = (seminia) / fabs(divisor);
        duracao_da_Nota *= 1.5; 
      }
      play_tone(PIN_BUZZER,  melodia_auto[i]);  
      absolute_time_t proxima_parada = delayed_by_us(get_absolute_time(),  duracao_da_Nota  * 5 * 1000);

      // proxima_parada = delayed_by_us(proxima_parada, duracao_da_Nota * 2 * 1000);
      bool saida = false;
      setas_para_musicajoystick();
      sleep_ms(50);

      while(1){
      sleep_ms(50);

      saida = joystick_music();
      sleep_ms(50);

        if (time_reached(proxima_parada) || saida == true ) {
              // Imprime uma mensagem na saída serial indicando que 1 segundo se passou.
          printf("Proxima nota\n");
          break; // quebra o loop para que haja a proxima nota
        }
      }
      
      stop_tone(PIN_BUZZER);
      sleep_ms(50);
    }
  }
  printf("\n\nACABOU PARTIDA");
  sleep_ms(10000);

}
// Funcao para tocar musica antes de comecar partida
void play_musica(uint PIN_BUZZER, int musica) {
  int divisor = 0, duracao_da_Nota = 0;
  int tempo_auto = 100, tempo_harry = 144, tempo_corderosa = 120;

  if( musica == 1){
    int notes = sizeof(melodia_harrypotter) / sizeof(melodia_harrypotter[0]) / 2;
    int seminia = (60000 * 4) / tempo_harry;

    for (int i = 0; i < notes*2; i = i+2) {
      divisor = melodia_harrypotter[i+1];
      if (divisor > 0) {
        duracao_da_Nota = (seminia) / divisor;
      } else if (divisor < 0) {
        duracao_da_Nota = (seminia) / fabs(divisor);
        duracao_da_Nota *= 1.5; 
      }
      play_tone(PIN_BUZZER,  melodia_harrypotter[i]);   
      sleep_ms(duracao_da_Nota*0.9);
      stop_tone(PIN_BUZZER);
      sleep_ms(50);
    }
  }
  else if( musica == 2){
    int notes = sizeof(melodia_corderosa) / sizeof(melodia_corderosa[0]) / 2;
    int seminia = (60000 * 4) / tempo_corderosa;

    for (int i = 0; i < notes*2; i = i+2) {
      divisor = melodia_corderosa[i+1];
      if (divisor > 0) {
        duracao_da_Nota = (seminia) / divisor;
      } else if (divisor < 0) {
        duracao_da_Nota = (seminia) / fabs(divisor);
        duracao_da_Nota *= 1.5; 
      }
      play_tone(PIN_BUZZER,  melodia_corderosa[i]);  
      sleep_ms(duracao_da_Nota*0.9);
      stop_tone(PIN_BUZZER);
      sleep_ms(50);
    }
  }
  else if( musica == 3){
    int notes = sizeof(melodia_auto) / sizeof(melodia_auto[0]) / 2;
    int seminia = (60000 * 4) / tempo_corderosa;

    for (int i = 0; i < notes*2; i = i+2) {
      divisor = melodia_auto[i+1];
      if (divisor > 0) {
        duracao_da_Nota = (seminia) / divisor;
      } else if (divisor < 0) {
        duracao_da_Nota = (seminia) / fabs(divisor);
        duracao_da_Nota *= 1.5; 
      }
      play_tone(PIN_BUZZER,  melodia_auto[i]);  
      sleep_ms(duracao_da_Nota*0.9);
      stop_tone(PIN_BUZZER);
      sleep_ms(50);
    }
  }
  printf("\n\nACABOU PARTIDA");
  sleep_ms(10000);

}



////////////////////////////////////////////////////////////////////
// Obs: direita é zero, baixo é zero
int lim_x = 3200;
int lim_y = 3200;
uint8_t caso_jm = 0; // auxiliar para indicar a posicao da seta

// Funcao para escolher aleatoriamente qual a direcao das
// setas utilizadas no jogo de musica com joystick
void setas_para_musicajoystick() {
  int num_al1 = rand() % 11;  // Gera número entre 0 e 10 para eixo x 
  int num_al2 = rand() % 11;  // Gera número entre 0 e 10 para eixo y

  sleep_ms(100);

  printf("   %d", num_al1);
  printf(" - %d \n", num_al2);
  if(num_al1 > 5 && num_al2 > 5){
    // cima e esquerda
    int lim_x = 3200;
    int lim_y = 3200;
    caso_jm = 1;
    printf("\ncaso - 1");
    setas_para_musica(caso_jm);
  }
  
  else if(num_al1 < 5 && num_al2 < 5){
    // baixo e direita
    int lim_x = 1200;
    int lim_y = 1200;
    caso_jm = 2;
    printf("\ncaso - 2");
    setas_para_musica(caso_jm);

  }
  else if(num_al1 < 5 && num_al2 > 5){
    // cima e direita
    int lim_x = 1200;
    int lim_y = 3200;
    caso_jm = 3;
    printf("\ncaso - 3");
    setas_para_musica(caso_jm);

  }
  else if(num_al1 > 5 && num_al2 < 5){
    // baixo e esquerda
    int lim_x = 3200;
    int lim_y = 1200;
    caso_jm = 4;
    printf("\ncaso - 4");
    setas_para_musica(caso_jm);     
  }
  else {
    setas_para_musicajoystick();
    
  }
  sleep_ms(100);
}

// Funcao para verificar se o joystick esta na mesma direcao da
// seta artual inidicada pela matriz de leds 5x5 atual
bool joystick_music() {
  // Leitura do valor do ADC para VRY (Eixo X do joystick)
  adc_select_input(0); // Seleciona canal 0 (GP26 - VRX)
  uint16_t vrx_value = adc_read(); // Lê o valor do eixo X, de 0 a 4095

  // Leitura do valor do ADC para VRY (Eixo Y do joystick)
  adc_select_input(1); // Seleciona canal 1 (GP27 - VRY)
  uint16_t vry_value = adc_read(); // Lê o valor do eixo Y, de 0 a 4095
 
  sleep_ms(60);
  if(vrx_value > lim_x && vry_value > lim_y && caso_jm == 1 ){
    // cima e esquerda
    printf("ACERTOU");
    return true;
  }
  else if(vrx_value > lim_x && vry_value < lim_y && caso_jm == 2 ){
    // baixo e esquerda
    printf("ACERTOU");
    return true;
  }
  else if(vrx_value > lim_x && vry_value > lim_y && caso_jm == 3 ){
    // baixo e direita
    printf("ACERTOU");
    return true;
  }
  else if(vrx_value > lim_x && vry_value > lim_y && caso_jm == 4 ){
    // cima e direita
    printf("ACERTOU");
    return true;
  }else {
    return false;
  }
}

// Definicao das palavras da tela inicial oled
char *text[] = {
  "   ESCOLHA o  ",
  "  Jogo Abaixo  ",
  "",
  "x 1 Tabuada-LED",
  "  2 Beatstar-PI", 
  "  3 Ingles-pico",
  "   ",
  "click com botao"
};

// Funcao principal do sistemas embarcado
int main()
{
  stdio_init_all();   // Inicializa os tipos stdio padrão presentes ligados ao binário
  
  npInit(LED_PIN_MATRIZ_5X5); // inicializacao inical da matriz de leds 5x5
  
  // Inicializa e configura o pino do botão (GPIO 5) como entrada.
  gpio_init(BUTTON_PIN_1);
  gpio_set_dir(BUTTON_PIN_1, GPIO_IN);
  gpio_pull_up(BUTTON_PIN_1); // Habilita o resistor pull-up interno para evitar leituras incorretas.


  // Inicialização do i2c
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);


  ///////////  Inicializacao inicial do menu  ///////////////
  // Processo de inicialização completo do OLED SSD1306
  ssd1306_init();
  // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };
  calculate_render_area_buffer_length(&frame_area);

  // zera o display inteiro 
  uint8_t ssd[ssd1306_buffer_length];
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);

  restart:
  int y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += 8;
  }
  render_on_display(ssd, &frame_area);

  
  // Inicializa o módulo ADC do Raspberry Pi Pico para o joystick
  adc_init();
  adc_gpio_init(VRX_PIN); // Configura GP26 (ADC0) para o eixo X do joystick
  adc_gpio_init(VRY_PIN); // Configura GP27 (ADC1) para o eixo Y do joystick
  gpio_init(SW_PIN);
  gpio_set_dir(SW_PIN, GPIO_IN);
  gpio_pull_up(SW_PIN); // Habilita o pull-up interno para garantir leitura estável
 
  while(1){
    void menu_principal();

  }

  return 0;
}
int po= 1;

void menu_principal(){
  printf("mennu paai");
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };
  calculate_render_area_buffer_length(&frame_area);

  // zera o display inteiro 
  uint8_t ssd[ssd1306_buffer_length];
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);

  int y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += 8;
  }
  sleep_ms(10);
  render_on_display(ssd, &frame_area);
    while (true) {
    // Leitura do valor do ADC para VRX (Eixo X do joystick)
    
      adc_select_input(0); // Seleciona canal 0 (GP26 - VRX)
      uint16_t vrx_value = adc_read(); // Lê o valor do eixo X, de 0 a 4095

      // Leitura do valor do ADC para VRY (Eixo Y do joystick)
      adc_select_input(1); // Seleciona canal 1 (GP27 - VRY)
      uint16_t vry_value = adc_read(); // Lê o valor do eixo Y, de 0 a 4095
      sleep_ms(50);

      printf("-X: %i \n", vrx_value);
      printf("-Y: %d \n", vry_value);

      if(vry_value > 2900){
        po = po-1;
        po = (po == -1) ? 3 : po;
        mudar_jogo(po);
        y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
          ssd1306_draw_string(ssd, 5, y, text[i]);
          y += 8;
        }
        sleep_ms(10);
        render_on_display(ssd, &frame_area);


      }else if(vry_value < 1100){
        po = po+1;
        po = (po == 4) ? 0 : po;
        mudar_jogo(po);
        y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
          ssd1306_draw_string(ssd, 5, y, text[i]);
          y += 8;
        }
        sleep_ms(10);
        render_on_display(ssd, &frame_area);

      }

    sleep_ms(50);
    if(!gpio_get(BUTTON_PIN_1)){
      if(po == 0){
        printf("Jogo 1 escolhido");
        sleep_ms(2000);
      }
      else if(po == 1){
        printf("Jogo 2 escolhido");
        sleep_ms(2000);
        menu_musica();
      }
      else if(po == 2){
        printf("Jogo 3 escolhido");
        sleep_ms(2000);
      }
    }

    
  }
}

void menu_musica(){
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };
  calculate_render_area_buffer_length(&frame_area);

  // zera o display inteiro 
  uint8_t ssd[ssd1306_buffer_length];
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);

  
  text[0] = "   Escolha a   ";
  text[1] = "  Musica abaixo";
  text[2] = "               ";
  text[3] = "x 1 Harrypotter";
  text[4] = "  2 AutoCompad."; 
  text[5] = "  3 PanteraRosa";
  text[6] = "  voltar menu";



  int y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += 8;
  }
  sleep_ms(10);
  render_on_display(ssd, &frame_area);
    while (true) {
    // Leitura do valor do ADC para VRX (Eixo X do joystick)
    
      adc_select_input(0); // Seleciona canal 0 (GP26 - VRX)
      uint16_t vrx_value = adc_read(); // Lê o valor do eixo X, de 0 a 4095

      // Leitura do valor do ADC para VRY (Eixo Y do joystick)
      adc_select_input(1); // Seleciona canal 1 (GP27 - VRY)
      uint16_t vry_value = adc_read(); // Lê o valor do eixo Y, de 0 a 4095
      sleep_ms(50);

      printf("-X: %i \n", vrx_value);
      printf("-Y: %d \n", vry_value);

      if(vry_value > 2900){
        po = po-1;
        po = (po == -1) ? 4 : po;
        mudar_musica(po);
        y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
          ssd1306_draw_string(ssd, 5, y, text[i]);
          y += 8;
        }
        sleep_ms(10);
        render_on_display(ssd, &frame_area);


      }else if(vry_value < 1100){
        po = po+1;
        po = (po == 5) ? 0 : po;
        mudar_musica(po);
        y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
          ssd1306_draw_string(ssd, 5, y, text[i]);
          y += 8;
        }
        sleep_ms(10);
        render_on_display(ssd, &frame_area);

      }

    sleep_ms(50);
    if(!gpio_get(BUTTON_PIN_1)){
      if(po == 0){
        printf("musica 1 escolhida");
        sleep_ms(3000);
      }
      else if(po == 1){
        printf("musica 2 escolhida");
        sleep_ms(3000);
        play_jogo_musica(BUZZER_PIN, 1);
      }
      else if(po == 2){
        printf("musica 3 escolhida");
        sleep_ms(3000);
      }
    }

    
  }
}

void menu_matematica(){
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };
  calculate_render_area_buffer_length(&frame_area);

  // zera o display inteiro 
  uint8_t ssd[ssd1306_buffer_length];
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);

  
  text[0] = "   Escolha a   ";
  text[1] = "Operacao abaixo";
  text[2] = "x Adicao";
  text[3] = "  Subtracao"; 
  text[4] = "  Divisao";
  text[5] = "  Multiplicacao";
  text[6] = "  voltar menu";

  int y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += 8;
  }
  sleep_ms(10);
  render_on_display(ssd, &frame_area);
    while (true) {
    // Leitura do valor do ADC para VRX (Eixo X do joystick)
    
      adc_select_input(0); // Seleciona canal 0 (GP26 - VRX)
      uint16_t vrx_value = adc_read(); // Lê o valor do eixo X, de 0 a 4095

      // Leitura do valor do ADC para VRY (Eixo Y do joystick)
      adc_select_input(1); // Seleciona canal 1 (GP27 - VRY)
      uint16_t vry_value = adc_read(); // Lê o valor do eixo Y, de 0 a 4095
      sleep_ms(50);

      printf("-X: %i \n", vrx_value);
      printf("-Y: %d \n", vry_value);

      if(vry_value > 2900){
        po = po-1;
        po = (po == -1) ? 4 : po;
        mudar_musica(po);
        y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
          ssd1306_draw_string(ssd, 5, y, text[i]);
          y += 8;
        }
        sleep_ms(10);
        render_on_display(ssd, &frame_area);


      }else if(vry_value < 1100){
        po = po+1;
        po = (po == 5) ? 0 : po;
        mudar_musica(po);
        y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
          ssd1306_draw_string(ssd, 5, y, text[i]);
          y += 8;
        }
        sleep_ms(10);
        render_on_display(ssd, &frame_area);

      }

    sleep_ms(50);
    if(!gpio_get(BUTTON_PIN_1)){
      if(po == 0){
        printf("Adicao escolhido");
        sleep_ms(3000);
        play_matematica(1, BUTTON_PIN_1, BUTTON_PIN_2);
      }
      else if(po == 1){
        printf("Subtracao escolhido");
        sleep_ms(3000);
        play_matematica(2, BUTTON_PIN_1, BUTTON_PIN_2);

      }
      else if(po == 2){
        printf("Multiplicacao escolhido");
        sleep_ms(3000);
        play_matematica(3, BUTTON_PIN_1, BUTTON_PIN_2);

      }
      else if(po == 3){
        printf("Multiplicacao escolhido");
        sleep_ms(3000);
        play_matematica(4, BUTTON_PIN_1, BUTTON_PIN_2);

      }
    }

    
  }
}

void mudar_jogo(int po){
  if(po == 0 ){
    text[3] = "x 1 Tabuada-LED";
    text[4] = "  2 Beatstar-PI"; 
    text[5] = "  3 Ingles-pico";
  }
  else if(po == 1){
    text[3] = "  1 Tabuada-LED";
    text[4] = "x 2 Beatstar-PI"; 
    text[5] = "  3 Ingles-pico";
  }
  else if(po == 2){
    text[3] = "  1 Tabuada-LED";
    text[4] = "  2 Beatstar-PI"; 
    text[5] = "x 3 Ingles-pico";
  }
  else{
    text[3] = "x 1 Tabuada-LED";
    text[4] = "  2 Beatstar-PI"; 
    text[5] = "  3 Ingles-pico";
  }
}

void mudar_operacao_matematica(int po){
  if(po == 0 ){
    text[2] = "x Adicao";
    text[3] = "  Subtracao"; 
    text[4] = "  Divisao";
    text[5] = "  Multiplicacao";
  }
  else if(po == 1){
    text[2] = "  Adicao";
    text[3] = "x Subtracao"; 
    text[4] = "  Divisao";
    text[5] = "  Multiplicacao";
  }
  else if(po == 2){
    text[2] = "  Adicao";
    text[3] = "  Subtracao"; 
    text[4] = "x Divisao";
    text[5] = "  Multiplicacao";
  }
  else if(po == 3){
    text[2] = "  Adicao";
    text[3] = "  Subtracao"; 
    text[4] = "  Divisao";
    text[5] = "x Multiplicacao";
  }
  else{
    text[2] = "x Adicao";
    text[3] = "  Subtracao"; 
    text[4] = "  Divisao";
    text[5] = "  Multiplicacao";
  }
}

void mudar_musica(int po){
  if(po == 0 ){
    text[3] = "x 1 Harrypotter";
    text[4] = "  2 AutoCompad."; 
    text[5] = "  3 PanteraRosa";
    text[6] = " voltar menu";

  }
  else if(po == 1){
    text[3] = "  1 Harrypotter";
    text[4] = "x 2 AutoCompad."; 
    text[5] = "  3 PanteraRosa";
    text[6] = "  voltar menu";
  
  }
  else if(po == 2){
    text[3] = "  1 Harrypotter";
    text[4] = "  2 AutoCompad."; 
    text[5] = "x 3 PanteraRosa";
    text[6] = "  voltar menu";
  
  }
  else if(po == 3){
    text[3] = "  1 Harrypotter";
    text[4] = "  2 AutoCompad."; 
    text[5] = "  3 PanteraRosa";
    text[6] = "x voltar menu";
  
  }else {
    text[3] = "x 1 Harrypotter";
    text[4] = "  2 AutoCompad."; 
    text[5] = "  3 PanteraRosa";
    text[6] = "  voltar menu";
  
  }
}

