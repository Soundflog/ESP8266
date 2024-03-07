#include <Wire.h>
#include <TroykaLedMatrix.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Martin";
const char* password = "987651234";

ESP8266WebServer server(80);

TroykaLedMatrix matrix;

int currentX = 0;
int currentY = 0;

void handleRoot() {
  server.send(200, "text/plain", "Hello, ESP!");
}

void handleAction() {
  String value = server.arg("value");
  Serial.print("Received action: ");
  Serial.println(value);

  if (value == "PLUS") {
    matrix.drawPixel(currentX, currentY);

    // Переходим к следующей позиции
    currentX++;
    if (currentX >= 8) {
      currentX = 0;
      currentY++;
      if (currentY >= 8) {
        currentY = 0;
      }
    }
  }
  if (value == "MINUS") {
    currentX--;
    if (currentX < 0) {
      currentX = 7;
      currentY--;
      if (currentY < 0) {
        currentY = 7;
      }
    }

    matrix.clearPixel(currentX, currentY);
  }

  server.send(200, "text/plain", "Action received: " + value);
}

void setup() {
  Serial.begin(115200);
  delay(100);


  matrix.begin();
  matrix.clear();



  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  IPAddress localIP = WiFi.localIP();
  Serial.print("Local IP: ");
  Serial.println(localIP);

  // Установка маршрутов на сервере
  server.on("/", HTTP_GET, handleRoot);
  server.on("/action", HTTP_GET, handleAction);

  // Запуск сервера
  server.begin();
}

void loop() {
  server.handleClient();
}
