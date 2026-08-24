#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9); // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Falha ao iniciar o OLED - display nao encontrado no endereco 0x3C");
    while (true) { delay(1000); }
  }

  Serial.println("OLED iniciado com sucesso!");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RFID/NFC Analyzer");
  display.println("SYH2 - teste OLED");
  display.println("by jbug0x");
  display.display();
}

void loop() {
  delay(1000);
}