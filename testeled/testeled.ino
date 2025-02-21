#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//dimensões do display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_ADDRESS 0x3C 

// Cria uma instância do display OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setup() {
  Serial.begin(115200);
  Serial.println("Teste do display OLED iniciado.");

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("Falha ao inicializar o display OLED. Verifique as conexões!");
    while (true); 
  }

  // Limpa o buffer do display
  display.clearDisplay();

  // Exibe uma mensagem no display
  display.setTextSize(1);                //tamanho do texto
  display.setTextColor(SSD1306_WHITE);   //cor do texto
  display.setCursor(0, 0);               //posição inicial do texto
  display.println("NICITE");         //exibe texto no display
  display.println("Teste OLED");
  display.display();                   
  delay(2000);                           //pausa por 2 segundos

  // Mostra formas geométricas
  //display.clearDisplay();
  //display.drawRect(10, 10, 50, 30, SSD1306_WHITE); // Desenha um retângulo
  //display.fillCircle(90, 32, 15, SSD1306_WHITE);   // Desenha um círculo preenchido
  //display.display();                               // Atualiza o display
  //delay(2000);

  // Animação de rolagem
  display.startscrollleft(0x00, 0x0F); 
  delay(2000);
  display.stopscroll();               
}

void loop() {
}
