#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define SSD1306_ADDR 0x3C

#define LED_RGB_GREEN 11  // Defina o pino do LED RGB Verde
#define LED_RGB_BLUE 12   // Defina o pino do LED RGB Azul
#define BUTTON_A 5        // Defina o pino do botão A
#define BUTTON_B 6        // Defina o pino do botão B

ssd1306_t ssd;
volatile bool led_green_state = false; // Estado do LED Verde
volatile bool led_blue_state = false;  // Estado do LED Azul

void button_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A) {
        led_green_state = !led_green_state;
        gpio_put(LED_RGB_GREEN, led_green_state);

        // Exibir no Serial Monitor
        printf ("Botão A pressionado. LED Verde: %s\n", led_green_state ? "Ligado" : "Desligado");

        // Atualizar o display
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "LED Verde:", 10, 10);
        ssd1306_draw_string(&ssd, led_green_state ? "LIGADO" : "DESLIGADO", 10, 30);
        ssd1306_send_data(&ssd);
    }
    else if (gpio == BUTTON_B) {
        led_blue_state = !led_blue_state;
        gpio_put(LED_RGB_BLUE, led_blue_state);

        // Exibir no Serial Monitor
        printf ("Botão B pressionado. LED Azul: %s\n", led_blue_state ? "Ligado" : "Desligado");

        // Atualizar o display
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, "LED Azul:", 10, 10);
        ssd1306_draw_string(&ssd, led_blue_state ? "LIGADO" : "DESLIGADO", 10, 30);
        ssd1306_send_data(&ssd);
    }
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial

    // Configuração do I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Configuração do LED RGB Verde
    gpio_init(LED_RGB_GREEN);
    gpio_set_dir(LED_RGB_GREEN, GPIO_OUT);
    gpio_put(LED_RGB_GREEN, led_green_state);

    // Configuração do LED RGB Azul
    gpio_init(LED_RGB_BLUE);
    gpio_set_dir(LED_RGB_BLUE, GPIO_OUT);
    gpio_put(LED_RGB_BLUE, led_blue_state);

    // Configuração do botão A
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // Configuração do botão B
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while (1) {
        sleep_ms(100); // Pequena espera para reduzir uso da CPU
    }
}
