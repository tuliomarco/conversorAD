#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

/* Desenvolvedor: Marco Túlio Macêdo Oliveira dos Santos */

// Definição dos pinos para LEDs RGB e botões
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BTN_JSTCK_PIN 22
#define BTN_A_PIN 5

// Configuração do barramento I2C para o display OLED
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C

// Definição de valores para PWM
#define WRAP 4096  // Resolução do PWM
#define VRX_PIN 27 // Entrada ADC do eixo X do joystick
#define VRY_PIN 26 // Entrada ADC do eixo Y do joystick

// Configuração do ADC e zona morta do joystick
#define ADC_MAX 4095
#define DEAD_ZONE 300 // Margem para evitar ativação involuntária do joystick

#define SQUARE_SIZE 8 // Tamanho do quadrado na tela
#define DEBOUNCE_DELAY_MS 200 // Tempo para evitar múltiplas leituras dos botões
volatile uint32_t last_interrupt_time = 0; // Armazena o tempo da última interrupção

// Inicializa um pino GPIO
void init_gpio(uint gpio, bool out, bool pullup) { 
    gpio_init(gpio);
    gpio_set_dir(gpio, out);
    if(pullup) gpio_pull_up(gpio);
}

void pwm_gpio_setup(uint gpio) { /* Configura um pino GPIO para funcionar como saída PWM */
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint pwm_slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(pwm_slice, WRAP);
    pwm_set_enabled(pwm_slice, true);
}

ssd1306_t ssd; // Estrutura do display

void init_display() { /* Inicializa o display OLED via I2C */
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    ssd1306_fill(&ssd, false); // Limpa a tela
    ssd1306_send_data(&ssd);

    // Desenha a borda inicial da tela
    ssd1306_rect(&ssd, 0, 0, 127, 63, true, false, 1);
    ssd1306_send_data(&ssd);
}

void init() { /* Inicializa todos os periféricos do sistema */
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);

    init_gpio(LED_G_PIN, true, false);
    init_gpio(BTN_JSTCK_PIN, false, true);
    init_gpio(BTN_A_PIN, false, true);

    pwm_gpio_setup(LED_R_PIN);
    pwm_gpio_setup(LED_B_PIN);

    init_display();
}

uint16_t jstck_center = ADC_MAX/2; // Valor central do ADC

uint16_t calculate_pwm(uint16_t adc_value) { /* Converte o valor do ADC para PWM, considerando a zona morta do joystick */ 
    if(adc_value > (jstck_center - DEAD_ZONE) && adc_value < (jstck_center + DEAD_ZONE)) {
        return 0; // Dentro da zona morta, LED apagado
    } else if (adc_value < (jstck_center - DEAD_ZONE)) {
        return (ADC_MAX - (adc_value * 2)); // Aumenta o valor do LED para valores ADC menores que o central
    } else {
        return ((adc_value - (jstck_center + DEAD_ZONE)) * 2); // Aumenta o valor do LED para valores ADC maiores que o central
    }
}

volatile bool green_led_on = false; // Estado do LED verde
volatile bool joystick_leds_on = true; // Estado dos LEDs controlados pelo joystick
volatile int border_thickness = 1; // Espessura da borda do display
volatile bool update_border = false; // Flag para atualizar borda

void button_callback(uint gpio, uint32_t events) { /* Callback das interrupções dos botões */
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if(now - last_interrupt_time > DEBOUNCE_DELAY_MS) { // Verifica debounce
        last_interrupt_time = now;
        if(gpio == BTN_JSTCK_PIN) {
            green_led_on = !green_led_on; // Alterna o LED verde
            gpio_put(LED_G_PIN, green_led_on);
            update_border = true; // Indica que a borda deve ser alterada
        } else if (gpio == BTN_A_PIN) {
            joystick_leds_on = !joystick_leds_on; // Liga/desliga os LEDs do joystick
        }
    }
}

// Posição inicial do quadrado, aproximada ao centro da tela (128x64)
uint16_t pos_x = 60;
uint16_t pos_y = 28;

void update_square_position(uint16_t vrx_value, uint16_t vry_value) { /* Atualiza a posição do quadrado na tela conforme o joystick */
    int old_x = pos_x;
    int old_y = pos_y;

    // Verifica zona morta do joystick
    if(vrx_value > (jstck_center - DEAD_ZONE) && vrx_value < (jstck_center + DEAD_ZONE)) pos_x = 60;
    else pos_x = (vrx_value * 128) / ADC_MAX;  

    if(vry_value > (jstck_center - DEAD_ZONE) && vry_value < (jstck_center + DEAD_ZONE)) pos_y = 28;
    else pos_y = 64 - (vry_value * 64) / ADC_MAX;  

    // Mantém o quadrado dentro da tela
    if (pos_x < border_thickness) pos_x = border_thickness;
    if (pos_x > (127 - SQUARE_SIZE - border_thickness)) pos_x = 127 - SQUARE_SIZE - border_thickness;
    if (pos_y < border_thickness) pos_y = border_thickness;
    if (pos_y > (63 - SQUARE_SIZE - border_thickness)) pos_y = 63 - SQUARE_SIZE - border_thickness;

    // Atualiza a posição na tela
    ssd1306_rect(&ssd, old_y, old_x, SQUARE_SIZE, SQUARE_SIZE, false, true, 1);
    ssd1306_rect(&ssd, pos_y, pos_x, SQUARE_SIZE, SQUARE_SIZE, true, true, 1);
    ssd1306_send_data(&ssd);
}

int main() {
    stdio_init_all();
    init();

    // Configura interrupções para os botões
    gpio_set_irq_enabled_with_callback(BTN_JSTCK_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    uint16_t vrx_value, vry_value;
    while (true) {
        // Lê os valores do joystick
        adc_select_input(0);
        vry_value = adc_read();
        adc_select_input(1);
        vrx_value = adc_read();

        if (update_border) { // Se necessário, altera a espessura da borda
            ssd1306_rect(&ssd, 0, 0, 127, 63, false, false, border_thickness);
            border_thickness = (border_thickness + 2) % 4;
            ssd1306_rect(&ssd, 0, 0, 127, 63, true, false, border_thickness);
            ssd1306_send_data(&ssd);
            update_border = false;
        }

        update_square_position(vrx_value, vry_value);

        // Controla LEDs com base no joystick
        if(joystick_leds_on) {
            vry_value = calculate_pwm(vry_value);
            vrx_value = calculate_pwm(vrx_value);
            pwm_set_gpio_level(LED_R_PIN, vrx_value);
            pwm_set_gpio_level(LED_B_PIN, vry_value);
        } else {
            pwm_set_gpio_level(LED_R_PIN, 0);
            pwm_set_gpio_level(LED_B_PIN, 0);
        }
        sleep_ms(100);
    }
}
