#include <Wire.h>
#include <Adafruit_SSD1306.h>  // Biblioteca para o display OLED

// Definir os pinos dos sensores
int pinoSensorUmidade = 34; // Pino para o sensor de umidade (HR202)
int pinoSensorLDR = 35;     // Pino para o sensor LDR

// Configuração do display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao inicializar o display OLED!"));
    while (true);  // Fica preso aqui se falhar
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
}

void loop() {
  // Leitura dos sensores
  int valorUmidade = analogRead(pinoSensorUmidade); // Leitura analógica do sensor de umidade
  int valorLDR = analogRead(pinoSensorLDR); // Leitura analógica do sensor LDR

  // Converte a leitura para um valor mais compreensível (0-100%)
  int umidade = map(valorUmidade, 0, 4095, 0, 100); // Conversão para porcentagem de umidade
  int luz = map(valorLDR, 0, 4095, 0, 100); // Conversão para porcentagem de luz

  // Exibe as leituras no Monitor Serial
  Serial.print("Umidade do solo (%): ");
  Serial.println(umidade);
  Serial.print("Luz (porcentagem): ");
  Serial.println(luz);

  // Exibe as leituras no Display OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Sensor HR202 e LDR");

  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("Umidade: ");
  display.print(umidade);
  display.println(" %");

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("Luz: ");
  display.print(luz);
  display.println(" %");

  display.display();  // Atualiza o display OLED

  delay(2000);  // Aguarda 2 segundos antes de fazer nova leitura
}
