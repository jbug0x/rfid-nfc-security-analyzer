#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RDM6300_RX_PIN 17
#define RDM6300_TX_PIN 18 // nao usado de fato, RDM6300 so transmite

HardwareSerial RDM6300Serial(1); // UART1 do ESP32-S3

void processarPacote(uint8_t *packet); 

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9); // SDA, SCL

  RDM6300Serial.begin(9600, SERIAL_8N1, RDM6300_RX_PIN, RDM6300_TX_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Falha ao iniciar o OLED - display nao encontrado no endereco 0x3C");
    while (true) { delay(1000); }
  }

  Serial.println("OLED iniciado com sucesso!");
  Serial.println("by jbug0x");
  Serial.println("Aguardando cartao LF (RDM6300)...");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RFID/NFC Analyzer");
  display.println("Aguardando LF...");
  display.display();
}

void loop() {
  static uint8_t packet[14];
  static uint8_t index = 0;
  static bool receiving = false;

  while (RDM6300Serial.available()) {
    uint8_t byteIn = RDM6300Serial.read();

    if (byteIn == 0x02) { // STX - inicio de um pacote novo
      receiving = true;
      index = 0;
      packet[index++] = byteIn;
      continue;
    }

    if (receiving) {
      packet[index++] = byteIn;

      if (byteIn == 0x03) { // ETX - fim do pacote
        receiving = false;

        if (index == 14) {
          processarPacote(packet);
        } else {
          Serial.println("Pacote com tamanho invalido, descartado.");
        }
      }

      if (index >= sizeof(packet)) {
        receiving = false; // protecao contra overflow
      }
    }
  }
}

void processarPacote(uint8_t *packet) {
  // packet[0] = STX, packet[13] = ETX
  // packet[1..10] = 5 bytes de UID em ASCII hex
  // packet[11..12] = checksum em ASCII hex

  char uidHex[11];
  memcpy(uidHex, &packet[1], 10);
  uidHex[10] = '\0';

  char checksumHex[3];
  memcpy(checksumHex, &packet[11], 2);
  checksumHex[2] = '\0';

  uint8_t uid[5];
  uint8_t checksumRecebido = (uint8_t)strtol(checksumHex, NULL, 16);
  uint8_t checksumCalculado = 0;

  for (int i = 0; i < 5; i++) {
    char byteStr[3] = { uidHex[i*2], uidHex[i*2+1], '\0' };
    uid[i] = (uint8_t)strtol(byteStr, NULL, 16);
    checksumCalculado ^= uid[i];
  }

  Serial.print("UID recebido: ");
  Serial.println(uidHex);

  if (checksumCalculado == checksumRecebido) {
    Serial.println("Checksum OK");
  } else {
    Serial.println("Checksum INVALIDO - pacote corrompido");
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Cartao LF detectado:");
  display.println(uidHex);
  display.println(checksumCalculado == checksumRecebido ? "Checksum OK" : "Checksum FALHOU");
  display.display();
}

