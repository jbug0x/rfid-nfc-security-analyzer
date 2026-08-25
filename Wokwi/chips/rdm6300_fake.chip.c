#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Protocolo do RDM6300:
// STX (0x02) + 10 chars ASCII hex (UID de 5 bytes) + 2 chars ASCII hex (checksum) + ETX (0x03)
// Checksum = XOR dos 5 bytes do UID

typedef struct {
  uart_dev_t uart;
  timer_t timer;
} chip_state_t;

static void send_fake_card(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;

  // UID fake fixo pra teste: 12 34 56 78 9A
  uint8_t uid[5] = {0x12, 0x34, 0x56, 0x78, 0x9A};
  uint8_t checksum = uid[0] ^ uid[1] ^ uid[2] ^ uid[3] ^ uid[4];

  uint8_t packet[14];
  char hex[13];
  snprintf(hex, sizeof(hex), "%02X%02X%02X%02X%02X%02X",
           uid[0], uid[1], uid[2], uid[3], uid[4], checksum);

  packet[0] = 0x02; // STX
  for (int i = 0; i < 12; i++) {
    packet[1 + i] = (uint8_t)hex[i];
  }
  packet[13] = 0x03; // ETX

  uart_write(chip->uart, packet, 14);
}

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  uart_config_t uart_config = {
    .tx = pin_init("TX", OUTPUT),
    .rx = pin_init("RX", INPUT),
    .baud_rate = 9600,
    .user_data = chip,
  };
  chip->uart = uart_init(&uart_config);

  timer_config_t timer_config = {
    .callback = send_fake_card,
    .user_data = chip,
  };
  chip->timer = timer_init(&timer_config);
  timer_start(chip->timer, 2000000, true); // dispara a cada 2s, repetindo
}