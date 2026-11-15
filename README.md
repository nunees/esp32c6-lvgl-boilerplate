# ESP32-C6 LCD Boilerplate



## Initial Steps

### 1. Create the project and install the ESP32-C6 toolchain

Create a new Project with
```
idf.py create-project -p <projectname> <entry-point-filename> && cd <projectname>
```
This installs the toolchain versions required by your ESP-IDF installation, including the RISC-V compiler.

```bash
cd ~/.espressif/v6.0.1/esp-idf

./install.sh esp32c6
```

### 2. Load the ESP-IDF environment

```bash
. ./export.sh
```

### 3. Confirm you have successfully installed the toolchain

```bash
which riscv32-esp-elf-gcc

riscv32-esp-elf-gcc --version
```

### 4. Now you can build with
```bash
cd ~/<myproject>

idf.py set-target esp32c6
idf.py fullclean
idf.py build
```

## LVGL

### 1. Install dependencies
```bash
idf.py add-dependency "espressif/esp_lvgl_port^2.3.0"
```

### 2. Create the `idf_component.yml` inside the `main` folder
```yml
dependencies:
  espressif/esp_lvgl_port: "^2.3.0"
  lvgl/lvgl:
    version: "^8"
    public: true
```