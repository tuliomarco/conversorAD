#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definição dos pinos para conexão com os LEDs RGB
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

// Definição dos pinos dos para conexão com os botões (Joystick e A)
#define BTN_JSTCK_PIN 22
#define BTN_A_PIN 5

// Valores utilizados para comunicação I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x30

// Valores utilizados na modulação por largura de pulso (PWM)
#define WRAP 4096
#define VRX_PIN 16
#define VRY_PIN 17

void init_gpio(uint gpio, bool out, bool pullup) { /* Função para inicialização das GPIOs */
    gpio_init(gpio);
    gpio_set_dir(gpio, out);
    if(pullup) (gpio);
}

void pwm_gpio_setup(uint gpio) { /* Configura um pino GPIO para funcionar como saída PWM */
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint pwm_slice = pwm_gpio_to_slice_num(VRY_PIN);
    pwm_set_wrap(pwm_slice, WRAP);
    pwm_set_enabled(pwm_slice, true);
}


ssd1306_t ssd; 
void init_display() { /* Inicializa a estrutura do display */
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void init() { /* Chama todas as inicializações */
    init_gpio(LED_R_PIN, true, false);
    init_gpio(LED_R_PIN, true, false);
    init_gpio(LED_R_PIN, true, false);
    init_gpio(BTN_JSTCK_PIN, false, true);
    init_gpio(BTN_A_PIN, false, true);

    pwm_gpio_setup(VRX_PIN);
    pwm_gpio_setup(VRY_PIN);
    
    init_display();
}

int main() {
    stdio_init_all();
    init();

    while (true) {
        printf("Conversor Analógico Digital!\n");
        sleep_ms(1000);
    }
}
