# Tarefa Aula 10/02: Conversores A/D no RP2040

## Descrição do Projeto
Este projeto utiliza o microcontrolador **RP2040** para capturar valores analógicos de um **joystick** e controlar a intensidade de LEDs RGB via **PWM**. Além disso, exibe a posição do joystick em um **display SSD1306** e permite interação via botões.

### Componentes Utilizados
- **Raspberry Pi Pico (RP2040)**
- **Joystick analógico** (ADC - GPIO 26 e 27, Botão - GPIO 22)
- **LED RGB** (GPIOs 11, 12 e 13)
- **Botão A** - GPIO 5
- **Display OLED SSD1306** (I2C - GPIO 14 e 15)

## Funcionalidades do Projeto

### 1. Controle da Intensidade dos LEDs via Joystick
- O **LED Azul** é controlado pelo **eixo Y** do joystick:
  - **Centro**: Apagado.
  - **Para cima ou para baixo**: Aumenta o brilho gradualmente.
  - **Extremos**: Intensidade máxima.

- O **LED Vermelho** é controlado pelo **eixo X** do joystick:
  - **Centro**: Apagado.
  - **Para a esquerda ou direita**: Aumenta o brilho gradualmente.
  - **Extremos**: Intensidade máxima.

- Os LEDs são controlados via **PWM** para variação suave da intensidade.

### 2. Exibição da Posição do Joystick no Display
- Um **quadrado de 8x8 pixels** se move na tela conforme os valores capturados pelo joystick.
- Inicialmente centralizado, ele se desloca proporcionalmente ao movimento do joystick.

### 3. Função do Botão do Joystick
- **Alterna o estado do LED Verde** a cada acionamento.
- **Modifica a borda do display**, alternando entre diferentes estilos.

### 4. Função do Botão A
- **Ativa ou desativa os LEDs PWM** a cada acionamento.

## Como Executar o Projeto
1. Compile o código e grave no **Raspberry Pi Pico**.
2. Conecte o dispositivo ao PC via **USB**.
3. Movimente o joystick para alterar o brilho dos LEDs e a posição do quadrado na tela.
4. Pressione o **botão do joystick** para mudar a borda do display e alternar o LED Verde.
5. Pressione o **botão A** para ativar ou desativar os LEDs PWM.

## Demonstração
Assista ao funcionamento do projeto neste [vídeo demonstrativo](https://drive.google.com/file/d/1Wv7_wCuhm-4tu1OdbS7g2kKX4MzRpphw/view?usp=sharing).
