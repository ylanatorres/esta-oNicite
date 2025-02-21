#include <Wire.h>
#include <Adafruit_SSD1306.h>  // Biblioteca para o display OLED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int pinoSensorUV = 34;  // Pino do sensor UV
int pinoRef = 33;       // Pino de referência para calibração (não usado diretamente, mas pode ser útil)
float valorSensorUV, tensaoConvertida, valorSensorRef;
String nivelUV = "00";

void setup() {
  Serial.begin(115200);  // Inicializa o Monitor Serial
  pinMode(pinoSensorUV, INPUT);
  pinMode(pinoRef, INPUT);

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
  Nivel_UV();  // Função para calcular o nível de UV

  // Exibe no Monitor Serial
  Serial.print("Valor Sensor UV (raw): ");
  Serial.println(valorSensorUV);  // Exibe o valor bruto do sensor
  Serial.print("Tensão UV (mV): ");
  Serial.println(tensaoConvertida);  // Exibe a tensão convertida em milivolts
  Serial.print("Nível UV: ");
  Serial.println(nivelUV);  // Exibe o nível de UV baseado na tensão

  // Exibe no Display OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Medidor UV");
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("UV: ");
  display.print(nivelUV);
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("Tensao: ");
  display.print(tensaoConvertida);
  display.print(" mV");

  display.display();  // Atualiza o display OLED

  delay(1000);  // Aguarda 1 segundo antes de fazer nova leitura
}

void Nivel_UV() {
  valorSensorUV = analogRead(pinoSensorUV);  // Lê o valor do sensor UV
  valorSensorRef = analogRead(pinoRef);      // Lê o valor do pino de referência (não utilizado diretamente)

  // Converte o valor do sensor UV para uma tensão em milivolts
  tensaoConvertida = (valorSensorUV * (3.3 / 1023.0)) * 1000;

  // Compara a tensão calculada com valores de uma tabela de níveis de UV
  if (tensaoConvertida <= 227) nivelUV = "00";  // Nível baixo
  else if (tensaoConvertida > 227 && tensaoConvertida <= 318) nivelUV = "01";
  else if (tensaoConvertida > 318 && tensaoConvertida <= 408) nivelUV = "02";
  else if (tensaoConvertida > 408 && tensaoConvertida <= 503) nivelUV = "03";
  else if (tensaoConvertida > 503 && tensaoConvertida <= 606) nivelUV = "04";
  else if (tensaoConvertida > 606 && tensaoConvertida <= 696) nivelUV = "05";
  else if (tensaoConvertida > 696 && tensaoConvertida <= 795) nivelUV = "06";
  else if (tensaoConvertida > 795 && tensaoConvertida <= 881) nivelUV = "07";
  else if (tensaoConvertida > 881 && tensaoConvertida <= 976) nivelUV = "08";
  else if (tensaoConvertida > 976 && tensaoConvertida <= 1079) nivelUV = "09";
  else if (tensaoConvertida > 1079 && tensaoConvertida <= 1170) nivelUV = "10";
  else nivelUV = "11";  // Nível muito alto
}
