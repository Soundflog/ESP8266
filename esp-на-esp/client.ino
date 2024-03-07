#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "wifiSSID";
const char* password = "passwordWIFI";
const char* serverIP = "192.168.180.207";

WiFiClient client;

const int buttonPlusPin = D5; // D3
const int buttonMinusPin = D4; // D4

void setup() {
  Serial.begin(115200);
  pinMode(buttonPlusPin, INPUT_PULLUP);
  pinMode(buttonMinusPin, INPUT_PULLUP);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  if (digitalRead(buttonPlusPin) == LOW) {
    sendRequest("PLUS");
    delay(1000);  // Задержка для избежания многократной отправки при удержании кнопки
  }

  if (digitalRead(buttonMinusPin) == LOW) {
    sendRequest("MINUS");
    delay(1000);  // Задержка для избежания многократной отправки при удержании кнопки
  }
}

void sendRequest(String action) {
  HTTPClient http;
  String url = "http://" + String(serverIP) + "/action?value=" + action;

  Serial.print("Sending request to: ");
  Serial.println(url);

  http.begin(client, url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Request sent successfully");
    } else {
      Serial.println("Failed to send request");
    }
  } else {
    Serial.println("Error on sending request");
  }

  http.end();
}
