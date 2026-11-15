# ESP32-C6 1.47" LCD Boilerplate (LVGL & ESP-IDF)

A streamlined ESP-IDF boilerplate configured to run **LVGL** on the **Waveshare ESP32-C6-LCD-1.47 (Non-Touch)** display module.

## Hardware & Pin Mapping

* **Display Controller:** ST7789 (IPS, 172×320 resolution)
* **Interface:** 4-wire SPI (`SPI2_HOST`)

| Pin Function | ESP32-C6 GPIO |
| :--- | :--- |
| **MOSI (SDA)** | GPIO 6 |
| **SCLK (SCL)** | GPIO 7 |
| **CS** | GPIO 14 |
| **DC** | GPIO 15 |
| **RST** | GPIO 21 |
| **Backlight (BL)** | GPIO 22 |

---

## Getting Started

### 1. Environment Setup

Initialize and export your ESP-IDF environment (adjust the path to match your installation version):

```bash
cd ~/.espressif/v6.0.1/esp-idf
./install.sh esp32c6
. ./export.sh
```

Confirm the RISC-V compiler toolchain is active:
```bash
which riscv32-esp-elf-gcc
riscv32-esp-elf-gcc --version
```

### 2. Project Creation

Create your project directory using ESP-IDF:
```bash
idf.py create-project <project-name>
cd <project-name>
```

### 3. Dependencies Configuration

Ensure your `main/idf_component.yml` includes the required LVGL components:

```yaml
dependencies:
  espressif/esp_lvgl_port: "^2.3.0"
  lvgl/lvgl:
    version: "^8"
    public: true
```

*(Alternatively, you can add the port via command line from your project root:)*
```bash
idf.py add-dependency "espressif/esp_lvgl_port^2.3.0"
```

---

## Building and Flashing

Target the ESP32-C6, clean build artifacts if changing configurations, and build/flash your project:

```bash
idf.py set-target esp32c6
idf.py fullclean
idf.py build
idf.py -p <PORT> flash monitor
```