#include <Wire.h>

#define MS5611_ADDRESS_1 0x77  // Endereço I2C padrão do sensor MS5611
#define MS5611_ADDRESS_2 0x76  // Endereço alternativo

#define CMD_RESET 0x1E
#define CMD_CONVERT_D1 0x40   // Comando para conversão de pressão
#define CMD_CONVERT_D2 0x50   // Comando para conversão de temperatura
#define CMD_READ_ADC 0x00     // Comando para ler o valor ADC

// Variáveis para armazenar dados do sensor
unsigned long D1, D2;
long TEMP, PRESSURE;
unsigned int C[6];

// Função para inicializar o sensor MS5611
void setup() {
  Serial.begin(115200);
  Wire.begin();  // Inicia a comunicação I2C
  
  // Testa o sensor na primeira e segunda opção de endereço
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
    while (1);  // Para o código aqui se o sensor não for encontrado
  }
  
  // Testa o sensor
  Serial.println("Sensor MS5611 iniciado. Lendo dados...");
}

// Função de loop contínuo
void loop() {
  readData();  // Lê a pressão e a temperatura
  delay(2000);  // Atraso de 2 segundos entre as leituras
}

// Função para verificar se o sensor está presente no endereço I2C
bool checkSensor(byte address) {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();
  return error == 0;  // Se não houver erro, o sensor está presente
}

// Função para resetar o sensor
void resetSensor(byte address) {
  Wire.beginTransmission(address);
  Wire.write(CMD_RESET);
  Wire.endTransmission();
  delay(100);
}

// Função para ler os dados de calibração do sensor
void readCalibrationData(byte address) {
  Wire.beginTransmission(address);
  Wire.write(0xA2);  // Endereço de leitura dos coeficientes de calibração
  Wire.endTransmission();
  Wire.requestFrom(address, 12);  // Solicita 12 bytes de dados de calibração
  
  for (int i = 0; i < 6; i++) {
    C[i] = (Wire.read() << 8) | Wire.read();  // Lê 2 bytes e armazena os coeficientes
  }
}

// Função para ler a pressão e a temperatura
void readData() {
  // Lê a temperatura
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_CONVERT_D2);  // Inicia a conversão de temperatura
  Wire.endTransmission();
  delay(10);  // Aguarda a conversão
  
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_READ_ADC);  // Lê o valor de ADC da temperatura
  Wire.endTransmission();
  Wire.requestFrom(MS5611_ADDRESS_1, 3);
  D2 = ((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | Wire.read();  // Leitura de 3 bytes
  
  // Lê a pressão
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_CONVERT_D1);  // Inicia a conversão de pressão
  Wire.endTransmission();
  delay(10);  // Aguarda a conversão
  
  Wire.beginTransmission(MS5611_ADDRESS_1);
  Wire.write(CMD_READ_ADC);  // Lê o valor de ADC da pressão
  Wire.endTransmission();
  Wire.requestFrom(MS5611_ADDRESS_1, 3);
  D1 = ((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | Wire.read();  // Leitura de 3 bytes
  
  // Calcula a temperatura
  long dT = D2 - (long)C[4] * 256;
  TEMP = 2000 + (dT * (long)C[5]) / 8388608;
  
  // Calcula a pressão
  long OFF = (long)C[1] * 65536 + ((long)C[3] * dT) / 128;
  long SENS = (long)C[0] * 32768 + ((long)C[2] * dT) / 256;
  
  PRESSURE = (D1 * SENS / 2097152 - OFF) / 32768;

  // Corrige o valor da pressão e temperatura se necessário
  if (TEMP < 2000) {
    long T2 = dT * dT / 2147483648;
    long OFF2 = 61 * ((TEMP - 2000) * (TEMP - 2000)) / 16;
    long SENS2 = 29 * ((TEMP - 2000) * (TEMP - 2000)) / 8;
    
    TEMP = TEMP - T2;
    PRESSURE = PRESSURE - OFF2 - SENS2;
  }
  
  // Exibe os resultados no Monitor Serial
  Serial.print("Temperatura: ");
  Serial.print(TEMP / 100.0);  // Exibe a temperatura em graus Celsius
  Serial.println(" C");
  
  Serial.print("Pressão: ");
  Serial.print(PRESSURE / 100.0);  // Exibe a pressão em hPa
  Serial.println(" hPa");
}
