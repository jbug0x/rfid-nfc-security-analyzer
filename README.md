# SYH2 RFID/NFC Security Analyzer

Ferramenta portátil de análise de credenciais RFID/NFC, voltada a auditorias
físicas de controle de acesso.

Não é um clonador de crachá — o objetivo é reconhecimento e análise: identificar
tecnologia (HF/LF), tipo de tag, UID, estrutura NDEF (quando aplicável) e permitir
comparação entre credenciais.
## Hardware

- **ESP32-S3** (DevKitC-1, N8R2 — 8MB Flash / 2MB PSRAM) — controlador principal
- **PN532** — leitura HF/NFC (13,56 MHz)
- **RDM6300** — leitura LF (125 kHz, EM4100/EM4102)
- **OLED SSD1306** 0,96" I²C — interface local

## Status atual

- [x] ESP32-S3 + OLED validado (I²C, SDA=GPIO8, SCL=GPIO9)
- [x] Parsing do protocolo RDM6300 (LF) validado via simulação
- [ ] Leitura PN532 (HF)
- [ ] Auto Scan HF/LF
- [ ] Interface completa no OLED

## Como rodar (simulação)

Pré-requisitos:
- [PlatformIO Core](https://platformio.org/) (`pip install platformio --break-system-packages`)
- [VS Code](https://code.visualstudio.com/) ou [VSCodium](https://vscodium.com/) com a extensão [Wokwi for VS Code](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode) (licença gratuita)
- Docker (apenas se for recompilar o chip simulado do RDM6300)

Compilar o firmware:

```bash
pio run
```

Simular no VS Code/VSCodium:

1. `F1` → `Wokwi: Request a new License` (uma vez só, se ainda não tiver)
2. Compile o firmware com `pio run`
3. `F1` → `Wokwi: Start Simulator`

O circuito simulado inclui um **Custom Chip** (`Wokwi/chips/rdm6300_fake.chip.wasm`)
que emula o protocolo serial do RDM6300, permitindo testar o parsing de pacotes LF
sem o módulo físico. Fonte e instruções de rebuild em `Wokwi/chips/rdm6300-build/`.

## Estrutura

```
├── platformio.ini
├── src/
│   └── main.cpp              # firmware
└── Wokwi/
    ├── diagram.json          # circuito simulado
    ├── wokwi.toml
    └── chips/
        ├── rdm6300_fake.chip.wasm/.json  # chip fake compilado (versionado)
        └── rdm6300-build/                # fonte C do chip fake
```

## Licença

TBD
