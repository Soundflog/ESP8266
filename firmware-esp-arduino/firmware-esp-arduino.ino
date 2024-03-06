#include <ESPWiFi.h>

const char *ssid = "Martin";
const char *password = "987651234";

const int redPin = 2; // Красный пин
const int greenPin = 5; // Зеленый пин
const int bluePin = 4; // Синий пин

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Настройка сервера
  server.begin();

  // Настройка пинов RGB светодиода
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.println("Ready to control RGB LED over WiFi");
}

void loop() {
  WiFiClient client = server.available(); // Прослушивание подключений

  if (client) {
    Serial.println("New client connected");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          // Обработка команды от клиента
          if (currentLine.startsWith("GET /red_on")) {
            digitalWrite(redPin, HIGH);
          } else if (currentLine.startsWith("GET /red_off")) {
            digitalWrite(redPin, LOW);
          } else if (currentLine.startsWith("GET /green_on")) {
            digitalWrite(greenPin, HIGH);
          } else if (currentLine.startsWith("GET /green_off")) {
            digitalWrite(greenPin, LOW);
          } else if (currentLine.startsWith("GET /blue_on")) {
            digitalWrite(bluePin, HIGH);
          } else if (currentLine.startsWith("GET /blue_off")) {
            digitalWrite(bluePin, LOW);
          }

          // Отправка ответа клиенту
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          // HTML-страница с кнопками управления
          client.println("<html><body>");
          client.println("<h1>ESP32 RGB LED Control</h1>");
          client.println("<p><a href='/red_on'>Turn Red Off</a> | <a href='/red_off'>Turn Red On</a></p>");
          client.println("<p><a href='/green_on'>Turn Green Off</a> | <a href='/green_off'>Turn Green On</a></p>");
          client.println("<p><a href='/blue_on'>Turn Blue Off</a> | <a href='/blue_off'>Turn Blue On</a></p>");
          client.println("</body></html>");

          break;
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    delay(10);
    client.stop();
    Serial.println("Client disconnected");
  }
}
