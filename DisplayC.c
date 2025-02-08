#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "ws2812.pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

//  Definição das constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define SSD1306_ADDR 0x3C

#define LED_RGB_GREEN 11
#define LED_RGB_BLUE 12
#define BUTTON_A 5
#define BUTTON_B 6
#define WS2812_PIN 7
#define NUM_PIXELS 25
#define IS_RGBW false
#define DEBOUNCE_TIME 200000 //200 ms

// Variáveis globais
ssd1306_t ssd;
volatile bool led_green_state = false;
volatile bool led_blue_state = false;
static volatile uint32_t last_time_buttonA = 0;
static volatile uint32_t last_time_buttonB = 0;

// Matriz de LEDs 5 x 5
const bool number_patterns[10][NUM_PIXELS] = { 
    {1, 1, 1, 1, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 
        1, 0, 0, 0, 1, 
        1, 1, 1, 1, 1}, // 0
       {0, 1, 1, 1, 0, 
       0, 0, 1, 0, 0,
       0, 0, 1, 0, 0,
       0, 1, 1, 0, 0,
       0, 0, 1, 0, 0}, // 1
       {1, 1, 1, 1, 1,
       1, 0, 0, 0, 0,
       1, 1, 1, 1, 1, 
       0, 0, 0, 0, 1, 
       1, 1, 1, 1, 1}, // 2
       {1, 1, 1, 1, 1, 
       0, 0, 0, 0, 1, 
       1, 1, 1, 0, 0, 
       0, 0, 0, 0, 1, 
       1, 1, 1, 1, 1}, // 3
       {1, 1, 0, 0, 0, 
       0, 0, 0, 1, 1, 
       1, 1, 1, 1, 1, 
       1, 0, 0, 1, 1, 
       1, 1, 0, 0, 1}, // 4
       {1, 1, 1, 1, 1, 
       0, 0, 0, 0, 1, 
       1, 1, 1, 1, 1, 
       1, 0, 0, 0, 0, 
       1, 1, 1, 1, 1}, // 5
       {1, 1, 1, 1, 1, 
       1, 0, 0, 0, 1, 
       1, 1, 1, 1, 1, 
       1, 0, 0, 0, 0, 
       1, 1, 1, 1, 1}, // 6
       {0, 0, 1, 0, 0, 
       0, 0, 1, 0, 0, 
       0, 1, 0, 0, 0, 
       0, 0, 0, 0, 1, 
       1, 1, 1, 1, 1}, // 7
       {1, 1, 1, 1, 1, 
       1, 0, 0, 0, 1, 
       1, 1, 1, 1, 1, 
       1, 0, 0, 0, 1, 
       1, 1, 1, 1, 1}, // 8
       {1, 1, 1, 1, 1, 
       0, 0, 0, 0, 1, 
       1, 1, 1, 1, 1, 
       1, 0, 0, 0, 1, 
       1, 1, 1, 1, 1}  // 9
};

// Funções auxiliares

// Função para enviar um pixel para a matriz de LEDs
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

//Função para converter RGB para formato de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(g) << 16) | ((uint32_t)(r) << 8) | (uint32_t)(b);
}

// Atualizar o display
void update_display(const char *msg) {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, msg, 10, 10);
    ssd1306_send_data(&ssd);
}

// Função de interrupção e debounce
void button_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (gpio == BUTTON_A && (current_time - last_time_buttonA > DEBOUNCE_TIME)) {
        last_time_buttonA = current_time;
        led_green_state = !led_green_state;
        gpio_put(LED_RGB_GREEN, led_green_state);
        printf("Botão A pressionado. LED Verde: %s\n", led_green_state ? "Ligado" : "Desligado");
        update_display(led_green_state ? "LED Verde:     LIGADO" : "LED Verde:     DESLIGADO");
    } else if (gpio == BUTTON_B && (current_time - last_time_buttonB > DEBOUNCE_TIME)) {
        last_time_buttonB = current_time;
        led_blue_state = !led_blue_state;
        gpio_put(LED_RGB_BLUE, led_blue_state);
        printf("Botão B pressionado. LED Azul: %s\n", led_blue_state ? "Ligado" : "Desligado");
        update_display(led_blue_state ? "LED Azul:      LIGADO" : "LED Azul:      DESLIGADO");
    }
}

// Controle da matriz de LEDs
void set_led_matrix(uint8_t number, uint8_t r, uint8_t g, uint8_t b) {
    if (number >= 0 && number <= 9) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(number_patterns[number][i] ? urgb_u32(100, 0, 0) : 0);
        }
    }
}

void clear_led_matrix() {
    for (int i = 0; i < 25; i++) {
        put_pixel(0);  // Define todos os LEDs como apagados (R=0, G=0, B=0)
    }
}

// Função principal
int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Inicializando...\n");
   

 // Inicializa I2C para o display   
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

// Configuração dos LEDs e botões    
    gpio_init(LED_RGB_GREEN);
    gpio_set_dir(LED_RGB_GREEN, GPIO_OUT);
    gpio_put(LED_RGB_GREEN, 0);

    gpio_init(LED_RGB_BLUE);
    gpio_set_dir(LED_RGB_BLUE, GPIO_OUT);
    gpio_put(LED_RGB_BLUE, 0);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    printf("Aguardando entrada do usuário...\n");


// Loop infinito    
    while (1) {
        int c = getchar_timeout_us(1000000); // Timeout de 1 segundo
        if (c == PICO_ERROR_TIMEOUT) continue;

        printf("Recebido: %c\n", c);
        char str[2] = {c, '\0'};
        update_display(str);

        if (c >= '0' && c <= '9') {
            set_led_matrix(c - '0', 100, 0, 0);
        } else {
            clear_led_matrix();  // Apaga matriz se não for número
        }
    }
}