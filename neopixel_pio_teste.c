#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "math.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

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

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
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

//
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

// Configuração do pino do buzzer
#define BUZZER_PIN 21

uint LED1 = 13;
uint slice;

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

int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED1, false);      // Desliga o LED.
       // Atualiza a variável indicando que o LED não está mais ativo.
    uint slice = pwm_gpio_to_slice_num(LED1);
    pwm_set_enabled(slice, false);
    printf("LED desligado\n"); // Exibe mensagem na saída serial indicando que o LED foi desligado.
    if(LED1 == 12){
      LED1 = 11;
    }else if(LED1 == 11){
      LED1 = 13;
    }else if (LED1 == 13){
      LED1 = 12;
    }
    gpio_set_function(LED1, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(LED1);
    pwm_set_clkdiv(slice, 500739.69);
    pwm_set_wrap(slice, 65535);
    pwm_set_gpio_level(LED1, 30000);
    pwm_set_enabled(slice, true);
    printf("LED ligado\n");  // Exibe mensagem na saída serial indicando que o LED foi ligado.

    return 0; // Retorna 0 para não repetir o alarme.
}


// Toca uma nota com a frequência e duração especificadas
void play_tone(uint pin, uint freq_nota, uint duration_ms) {
  
  gpio_set_function(pin, GPIO_FUNC_PWM);
   
    
  uint slice_num = pwm_gpio_to_slice_num(pin);

  float divisor_clock = 1907.34632 / freq_nota;
  pwm_set_clkdiv(slice_num, divisor_clock); // igual

  pwm_set_wrap(slice_num, 36000);
  pwm_set_gpio_level(pin, 36000 * 0.7); // 50% de duty cycle
  pwm_set_enabled(slice_num, true);

  sleep_ms(duration_ms);

  pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
  sleep_ms(50); // Pausa entre notas
}


// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / 120;


int tempo_auto = 100, tempo_harry = 144, tempo_corderosa = 120;
int divisor = 0, noteDuration = 0;

// Função principal para tocar a música
void play_musica(uint pin, int musica) {
// add_alarm_in_ms(50, turn_off_callback, NULL, false);
  if( musica == 0){
  int notes = sizeof(melodia_harrypotter) / sizeof(melodia_harrypotter[0]) / 2;
  int seminia = (60000 * 4) / tempo_harry;

  for (int i = 0; i < notes*2; i = i+2) {
    divisor = melodia_harrypotter[i+1];
    if (divisor > 0) {
      noteDuration = (seminia) / divisor;
    } else if (divisor < 0) {
      noteDuration = (seminia) / fabs(divisor);
      noteDuration *= 1.5; 
    }
    play_tone(pin,  melodia_harrypotter[i], noteDuration*0.9);  
  }
  }
  else  if(musica == 1){
  int notes = sizeof(melodia_corderosa) / sizeof(melodia_corderosa[0]) / 2;
  int seminia = (60000 * 4) / tempo_corderosa;

  for (int i = 0; i < notes*2; i = i+2) {
    divisor = melodia_corderosa[i+1];
    if (divisor > 0) {
      noteDuration = (seminia) / divisor;
    } else if (divisor < 0) {
      noteDuration = (seminia) / fabs(divisor);
      noteDuration *= 1.5; 
    }

    play_tone(pin,  melodia_corderosa[i], noteDuration*0.9);  
  }


  }
  else if(musica == 2){
    int notes = sizeof(melodia_auto) / sizeof(melodia_auto[0]) / 2;
    int seminia = (60000 * 4) / tempo_auto;

    for (int i = 0; i < notes*2; i = i+2) {
      divisor = melodia_auto[i+1];
      if (divisor > 0) {
        noteDuration = (seminia) / divisor;
      } else if (divisor < 0) {
        noteDuration = (seminia) / fabs(divisor);
        noteDuration *= 1.5; 
      }
      play_tone(pin,  melodia_auto[i], noteDuration*0.9);  
    }

    
  }
}



/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Matriz de LEDs 5x5 com valores RGB (em formato GRB)

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

#include <stdlib.h>
#include <stdint.h>

// Função para obter a matriz com as cores RGB, dependendo de "aux"
int*** matrizABC(int aux) {
    int ***mat;
    
    // Alocando memória para a matriz 5x5x3
    mat = (int ***)malloc(5 * sizeof(int **));
    for (int i = 0; i < 5; i++) {
        mat[i] = (int **)malloc(5 * sizeof(int *));
        for (int j = 0; j < 5; j++) {
            mat[i][j] = (int *)malloc(3 * sizeof(int)); // Para as 3 cores RGB
        }
    }

    // Preenchendo a matriz com as cores RGB dependendo de "aux"
    if (aux == 1) {
        int temp[5][5][3] = {
            {{0, 0, 0}, {167, 255, 0}, {167, 255, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {0, 0, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {167, 255, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {0, 0, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {167, 255, 0}, {167, 255, 0}, {0, 0, 0}}
        };
        // Copiando os valores para a matriz alocada dinamicamente
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 3; k++) {
                    mat[i][j][k] = temp[i][j][k];
                }
            }
        }
    } else if (aux == 2) {
        int temp[5][5][3] = {
            {{0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 0, 0}, {0, 255, 238}},
            {{0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 0, 0}, {0, 255, 238}},
            {{0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}}
        };
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 3; k++) {
                    mat[i][j][k] = temp[i][j][k];
                }
            }
        }
    } else if (aux == 3) {
        int temp[5][5][3] = {
            {{0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}}
        };
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 3; k++) {
                    mat[i][j][k] = temp[i][j][k];
                }
            }
        }
    } else if (aux == 4) {
        int temp[5][5][3] = {
            {{0, 0, 0}, {167, 255, 0}, {167, 255, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {0, 0, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {167, 255, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {0, 0, 0}, {167, 255, 0}, {0, 0, 0}},
            {{0, 0, 0}, {167, 255, 0}, {167, 255, 0}, {167, 255, 0}, {0, 0, 0}}
        };
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 3; k++) {
                    mat[i][j][k] = temp[i][j][k];
                }
            }
        }
    } else if (aux == 5) {
        int temp[5][5][3] = {
            {{0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 0, 0}, {0, 255, 238}, {0, 0, 0}},
            {{0, 0, 0}, {0, 255, 238}, {0, 255, 238}, {0, 0, 0}, {0, 0, 0}}
        };
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 3; k++) {
                    mat[i][j][k] = temp[i][j][k];
                }
            }
        }
    }

    return mat; // Retorna o ponteiro para a matriz alocada dinamicamente
}

// Função para liberar a memória alocada
void liberarMatriz(int ***mat) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            free(mat[i][j]); // Liberando cada linha de 3 elementos
        }
        free(mat[i]); // Liberando cada linha 5
    }
    free(mat); // Liberando a matriz 5x5
}

int main() {
    // Inicializa entradas e saídas.
    stdio_init_all();
    gpio_init(LED1);
    gpio_set_dir(LED1, GPIO_OUT);
    gpio_put(LED1, 0);
    
    gpio_init(12);
    gpio_set_dir(12, GPIO_OUT);
    gpio_put(12, 0);

    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_put(11, 0);
  
    // Configura o temporizador repetitivo para verificar o estado do botão a cada 100 ms.
    struct repeating_timer timer;
    //add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);

    // Inicializa matriz de LEDs NeoPixel.
    npInit(LED_PIN);
    npClear();

    // Escreve os dados nos LEDs inicialmente.
    npWrite();

    while(1) {
      play_musica(BUZZER_PIN, 0);
      play_musica(BUZZER_PIN, 1);
      play_musica(BUZZER_PIN, 2);

    int aux = 0;
        aux = aux + 1;
        

        int*** matriz = matrizABC(aux);

        // Atribui as cores aos LEDs da matriz
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                int posicao = getIndex(i, j);
                npSetLED(posicao, matriz[i][j][0], matriz[i][j][1], matriz[i][j][2]);
            }
        }
        if (aux > 5) {
            aux = 1; // Reseta o contador para 1 após 5
        }
        // Atualiza os LEDs
        npWrite();

        sleep_ms(2000); // Espera 2 segundos antes de atualizar novamente
    }

    return 0;
}
