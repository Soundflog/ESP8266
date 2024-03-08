#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10
#define LED_PIN_OPEN 8 // Пин, к которому подключен светодиод для открытого состояния
#define LED_PIN_ERROR 7 // Пин, к которому подключен светодиод для ошибки

#define DATA_SIZE 16
#define READ_DATA_SIZE 18

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
int block_number = 5;
byte data[DATA_SIZE] = {'H', 'e', 'l', 'l', 'o', ',', 'R', 'F', 'I', 'D', 'I', '!'};
byte read_data[READ_DATA_SIZE];

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN_OPEN, OUTPUT); // Устанавливаем пин светодиода для открытого состояния как выход
  pinMode(LED_PIN_ERROR, OUTPUT); // Устанавливаем пин светодиода для ошибки как выход
  while (!Serial);
  SPI.begin();

  // Инициализация считывателя RFID
  mfrc522.PCD_Init();

  mfrc522.PCD_DumpVersionToSerial();
  Serial.println("Ready, Give me an RFID marker");
  key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.println("Card is ready to be written on");
      if (writeBlock(block_number, data) && readBlock(block_number, read_data)) {
        // Если запись и чтение прошли успешно, зажигаем светодиод для открытого состояния
        digitalWrite(LED_PIN_OPEN, HIGH);
      } else {
        // Если чтение или запись не удалось, зажигаем светодиод для ошибки
        digitalWrite(LED_PIN_ERROR, HIGH);
      }
      delay(1000); // Ждем 1 секунду
      digitalWrite(LED_PIN_OPEN, LOW); // Выключаем светодиод
      digitalWrite(LED_PIN_ERROR, LOW); // Выключаем светодиод
      Serial.print("Reading Block: ");
      for (int i = 0; i < DATA_SIZE; i++) {
        Serial.write(read_data[i]);
      }
      Serial.println();
      mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  } else {
    digitalWrite(LED_PIN_ERROR, HIGH); // Включаем светодиод ошибки, если нет метки
  }
}

int writeBlock(int block_number, byte data_buffer[]) {
  if (block_number > 288 && (block_number + 1) % 4 == 0) {
    Serial.print(block_number);
    Serial.println(" is a trailer block:");
    return 0;
  }

  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block_number, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD Authenticate() for write failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }

  status = mfrc522.MIFARE_Write(block_number, data_buffer, 16);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }
  Serial.println("Block was written");
  return 1;
}

int readBlock(int block_number, byte data_buffer[]) {
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block_number, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() for read failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }
  byte buffer_size = READ_DATA_SIZE;
  status = mfrc522.MIFARE_Read(block_number, data_buffer, &buffer_size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }
  Serial.println("Block was read");
  return 1;
}