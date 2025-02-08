# Tarefa - Unidade 04 - Capítulo 06 - Introdução às Interfaces de Comunicação Serial com RP2040 - UART, SPI e I2C

Este projeto implementa um sistema de controle para uma matriz de LEDs WS2812 (5x5) utilizando a placa Raspberry Pi Pico. Ele inclui suporte para exibição no display  SSD1306 via I2C, botões com interrupções para controle de LEDs RGB e comunicação via UART para interação com o usuário.  

# Componentes necessários   
• Matriz 5x5 de LEDs (endereçáveis) WS2812  
• LED RGB  
• Botão A  
• Botão B   
• Display SSD1306 conectado via I2C  

# Ligações necessárias
Matriz WS2812:  
• Pino de dados: GPIO 7    
  
Display SSD1306:  
• SDA: GPIO 14    
• SCL: GPIO 15    
• Endereço I2C: 0x3C    
    
LEDs RGB:  
• Verde: GPIO 11    
• Azul: GPIO 12    
    
Botões:  
• Botão A: GPIO 5    
• Botão B: GPIO 6    

# Funcionamento
1) O sistema inicia exibindo "Inicializando..." e "Aguardando entrada do usuário..." no monitor serial.  
2) O display é configurado e pronto para exibir caracteres recebidos via UART.  
3) A matriz de LEDs exibe números (0-9) conforme entrada do usuário no monitor serial. O número que foi dado entrada (0-9) também é exibido no display.  
4) Se um caractere recebido não for um número, a matriz é apagada.  O caractere que foi dado entrada (A-Z ou a-z) é exibido no display e no monitor serial. 
5) Os botões A e B controlam os LEDs RGB verde e azul, respectivamente.  
6) O estado dos LEDs RGB é mostrado no display e também no monitor serial.

# Link do vídeo
https://drive.google.com/file/d/1_C2FsJ0vbJUKdMKLXnlNnsbWs9HBUDh9/view?usp=sharing  

# Desenvolvido por
Priscila Pereira Suzart de Carvalho
