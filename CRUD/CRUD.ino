#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definir os pinos dos sensores
int pinoSensorUmidade = 34; //sensor de umidade (HR202)
int pinoSensorLDR = 35;     //sensor LDR
int pinoSensorUV = 34;      //ensor UV
int pinoRef = 33;           //referência para calibração (não usado diretamente, mas pode ser útil)

// Constantes para o sensor MS5611
#define MS5611_ADDRESS_1 0x77  //endereço I2C padrão do sensor MS5611
#define MS5611_ADDRESS_2 0x76  //endereço alternativo
#define CMD_RESET 0x1E         //resetar o sensor
#define CMD_CONVERT_D1 0x40    //conversão de pressão
#define CMD_CONVERT_D2 0x50    //conversão de temperatura
#define CMD_READ_ADC 0x00      //ler o valor ADC

//armazenar dados do sensor MS5611
unsigned long D1, D2;
long TEMP, PRESSURE;
unsigned int C[6];

void setup() {
  Serial.begin(115200);  

  //isplay OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao inicializar o display OLED!"));
    while (true);  // Fica preso aqui se falhar
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  //sensor MS5611
  if (checkSensor(MS5611_ADDRESS_1)) {
    Serial.println("Sensor MS5611 encontrado no endereço 0x77");
    resetSensor(MS5611_ADDRESS_1);
    readCalibrationData(MS5611_ADDRESS_1);
  } else if (checkSensor(MS5611_ADDRESS_2)) {
    Serial.println("Sensor MS5611 encontrado no endereço 0x76");
    resetSensor(MS5611_ADDRESS_2);
    readCalibrationData(MS5611_ADDRESS_2);
  } else {
    Serial.println("Sensor MS5611 não encontrado!");
    while (1);  
  }
}

void loop() {
  //leitura dos sensores
  int umidade = readUmidade();
  int luz = readLuz();
  readData();  //lê a pressão e a temperatura
  String nivelUV = readUV();

  //leituras no Monitor Serial
  Serial.print("Umidade do solo (%): ");
  Serial.println(umidade);
  Serial.print("Luz (porcentagem): ");
  Serial.println(luz);
  Serial.print("Temperatura: ");
  Serial.print(TEMP / 100.0);
  Serial.println(" C");
  Serial.print("Pressão: ");
  Serial.print(PRESSURE / 100.0);
  Serial.println(" hPa");
  Serial.print("Nível UV: ");
  Serial.println(nivelUV);

  //leituras no Display OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Sensor Readings");

  display.setCursor(0, 10);
  display.print("Umidade: ");
  display.print(umidade);
  display.println(" %");

  display.setCursor(0, 20);
  display.print("Luz: ");
  display.print(luz);
  display.println(" %");

  display.setCursor(0, 30);
  display.print("Temp: ");
  display.print(TEMP / 100.0);
  display.println(" C");

  display.setCursor(0, 40);
  display.print("Pressao: ");
  display.print(PRESSURE / 100.0);
  display.println(" hPa");

  display.setCursor(0, 50);
  display.print("UV: ");
  display.print(nivelUV);

  display.display();  //atualiza o display OLED

  delay(2000);  
}

int readUmidade() {
  int valorUmidade = analogRead(pinoSensorUmidade); //leitura analógica do sensor de umidade
  return map(valorUmidade, 0, 4095, 0, 100); //conversão para porcentagem de umidade
}

int readLuz() {
  int valorLDR = analogRead(pinoSensorLDR); //leitura analógica do sensor LDR
  return map(valorLDR, 0, 4095, 0, 100); //conversão para porcentagem de luz
}

String readUV() {
  float valorSensorUV = analogRead(pinoSensorUV);  //lê o valor do sensor UV
  float tensaoConvertida = (valorSensorUV * (3.3 / 1023.0)) * 1000;

  if (tensaoConvertida <= 227) return "00";
  else if (tensaoConvertida > 227 && tensaoConvertida <= 318) return "01";
  else if (tensaoConvertida > 318 && tensaoConvertida <= 408) return "02";
  else if (tensaoConvertida > 408 && tensaoConvertida <= 503) return "03";
  else if (tensaoConvertida > 503 && tensaoConvertida <= 606) return "04";
  else if (tensaoConvertida > 606 && tensaoConvertida <= 696) return "05";
  else if (tensaoConvertida > 696 && tensaoConvertida <= 795) return "06";
  else if (tensaoConvertida > 795 && tensaoConvertida <= 881) return "07";
  else if (tensaoConvertida > 881 && tensaoConvertida <= 976) return "08";
  else if (tensaoConvertida > 976 && tensaoConvertida <= 1079) return "09";
  else if (tensaoConvertida > 1079 && tensaoConvertida <= 1170) return "10";
  else return "11";
}

bool checkSensor(byte address) {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();
  return error == 0;  
}

void resetSensor(byte address) {
  Wire.beginTransmission(address);
  Wire.write(CMD_RESET);
  Wire.endTransmission();
  delay(100);
}

void readCalibrationData(byte address) {
  Wire.beginTransmission(address);
  Wire.write(0xA2); 
  Wire.endTransmission();
  Wire.requestFrom(address, 12);  
  
  for (int i = 0; i < 6; i++) {
    C[i] = (Wire.read() << 8) | Wire.read();  
  }
}

void readData() {
  // Lê a temperatura
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_CONVERT_D2);  // Inicia a conversão de temperatura
  Wire.endTransmission();
  delay(10);  
  
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_READ_ADC);  //lê o valor de ADC da temperatura
  Wire.endTransmission();
  Wire.requestFrom(MS5611_ADDRESS_1, 3);
  D2 = ((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | Wire.read();  
  
  // Lê a pressão
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_CONVERT_D1);  //inicia a conversão de pressão
  Wire.endTransmission();
  delay(10);  
  
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_READ_ADC);  //lê o valor de ADC da pressão
  Wire.endTransmission();
  Wire.requestFrom(MS5611_ADDRESS_1, 3);
  D1 = ((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | Wire.read();  // Leitura de 3 bytes
  
  //calcula a temperatura
  long dT = D2 - (long)C[4] * 256;
  TEMP = 2000 + (dT * (long)C[5]) / 8388608;
  
  // Calcula a pressão
  long OFF = (long)C[1] * 65536 + ((long)C[3] * dT) / 128;
  long SENS = (long)C[0] * 32768 + ((long)C[2] * dT) / 256;
  
  PRESSURE = (D1 * SENS / 2097152 - OFF) / 32768;

  //corrige o valor da pressão e temperatura 
  if (TEMP < 2000) {
    long T2 = dT * dT / 2147483648;
    long OFF2 = 61 * ((TEMP - 2000) * (TEMP - 2000)) / 16;
    long SENS2 = 29 * ((TEMP - 2000) * (TEMP - 2000)) / 8;
    
    TEMP = TEMP - T2;
    PRESSURE = PRESSURE - OFF2 - SENS2;
  }
}