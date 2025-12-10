# SSCOM - Serial Communication Tool

A cross-platform serial communication tool based on C++ and FTXUI, providing command-line parameter configuration and an interactive TUI interface.

## Features

- **Cross-platform Support**: Works on Linux, Windows (MinGW), and macOS
- **Interactive TUI Interface**: Beautiful terminal user interface using the FTXUI library
- **Complete Serial Port Configuration**:
  - Baud rate (50 - 4000000)
  - Data bits (5, 6, 7, 8, 9)
  - Stop bits (1, 1.5, 2)
  - Parity (none, odd, even)
  - Flow control (none, rtscts, xonxoff)
- **Multiple Data Formats**:
  - String (str)
  - Hexadecimal (hex)
  - 32-bit signed integer (i32)
  - 32-bit unsigned integer (u32)
- **Real-time Message Display**: Supports auto-scroll and manual paging
- **Keyboard Shortcuts**:
  - `Enter`: Send message
  - `Ctrl+U`: Clear input field
  - `Ctrl+W`: Delete last word
  - `PageUp/PageDown`: Page through messages

## Dependencies

### Required Dependencies
- **libserialport**: Cross-platform serial port communication library
- **FTXUI**: C++ Terminal User Interface library
- **C++17 Compiler**: g++ or clang++

### Installing Dependencies

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install libserialport-dev libftxui-dev build-essential
```

#### Arch Linux
```bash
sudo pacman -S libserialport ftxui
```

#### macOS (Homebrew)
```bash
brew install libserialport ftxui
```

#### Windows (MinGW)
1. Install MSYS2 and MinGW-w64
2. Install dependencies:
```bash
pacman -S mingw-w64-x86_64-libserialport mingw-w64-x86_64-ftxui
```

## Building and Installation

### 1. Clone the Project
```bash
git clone <repository-url>
cd sscom
```

### 2. Build the Project
```bash
make
```

### 3. Run
```bash
# List available serial ports
./sscom

# Show help information
./sscom --help

# Connect to serial port example
./sscom --port /dev/ttyUSB0 --baudrate 115200 --read hex
```

### 4. Clean
```bash
make clean
```

## Usage

### Command-line Arguments

| Short | Long | Value | Description |
|-------|------|-------|-------------|
| `-s`, `-p` | `--serial`, `--port` | `SERIALPORT` | Serial port device path (required) |
| `-b` | `--baudrate` | `RATE` | Baud rate (default: 115200) |
| `-db` | `--databits` | `BITS` | Data bits (5/6/7/8/9, default: 8) |
| `-sb` | `--stopbits` | `BITS` | Stop bits (1/1.5/2, default: 1) |
| `-prt` | `--parity` | `TYPE` | Parity (none/odd/even, default: none) |
| `-fc` | `--flowcontrol` | `TYPE` | Flow control (none/rtscts/xonxoff, default: none) |
| `-r` | `--read` | `DATATYPE` | Read format (str/hex/i32/u32, default: str) |
| `-h` | `--help` | | Show help message |
| `-v` | `--version` | | Show version information |

### Usage Examples

1. **List available serial ports**:
   ```bash
   ./sscom
   ```

2. **Connect to serial port and display hexadecimal data**:
   ```bash
   ./sscom --port /dev/ttyUSB0 --baudrate 9600 --read hex
   ```

3. **Complete configuration example**:
   ```bash
   ./sscom --port COM3 --baudrate 115200 --databits 8 --stopbits 1 \
           --parity none --flowcontrol none --read str
   ```

4. **Using short arguments**:
   ```bash
   ./sscom -p /dev/ttyACM0 -b 57600 -r hex
   ```

### TUI Interface Operations

Once in the TUI interface:
1. **Bottom input field**: Type messages to send, press `Enter` to send
2. **Message area**: Displays received messages
3. **Keyboard shortcuts**:
   - `Enter`: Send the current input message
   - `Ctrl+U`: Clear the input field
   - `Ctrl+W`: Delete the last word in the input field
   - `PageUp`: Page up to view historical messages
   - `PageDown`: Page down to view historical messages
   - `Ctrl+C`: Exit the program

## Project Structure

```
sscom/
├── inc/                    # Header files directory
│   ├── sscom.hpp          # Main header file
│   └── ui.hpp             # UI interface header file
├── src/                   # Source files directory
│   ├── main.cpp           # Main program entry
│   └── sscom.cpp          # Serial port functionality implementation
├── Makefile              # Build configuration
└── README.md            # This document
```

## Troubleshooting

### Permission Issues (Linux/macOS)
If you encounter permission errors:
```bash
sudo chmod a+rw /dev/ttyUSB0  # Replace with your device
```

Or add your user to the dialout group:
```bash
sudo usermod -a -G dialout $USER
# Log out and log back in for changes to take effect
```

### Windows Permission Issues
Run the program as Administrator, or check serial port permissions in Device Manager.

### Compilation Errors
1. **FTXUI library not found**:
   ```bash
   # Manually specify FTXUI path
   make CXXFLAGS="-I/path/to/ftxui/include" LDFLAGS="-L/path/to/ftxui/lib -lftxui-component -lftxui-dom -lftxui-screen -lserialport -lpthread"
   ```

2. **libserialport not found**:
   Ensure the libserialport development package is properly installed.

## License

MIT License

## Version Information

- Current Version: 0.1.0
- Build Date: 2025-12-10
- Library Versions:
  - libserialport: [Auto-detected]
  - FTXUI: 6.1.9

## Contributing

Issues and Pull Requests are welcome to improve this project.

## Acknowledgments

- [libserialport](https://sigrok.org/wiki/Libserialport) - Cross-platform serial port library
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - C++ Terminal User Interface library
