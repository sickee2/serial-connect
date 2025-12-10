# SSCOM - 串口通信工具

一个基于 C++ 和 FTXUI 的跨平台串口通信工具，提供命令行参数配置和交互式 TUI 界面。

## 功能特性

- **跨平台支持**: 支持 Linux、Windows (MinGW) 和 macOS
- **交互式 TUI 界面**: 使用 FTXUI 库提供美观的终端用户界面
- **完整的串口配置**:
  - 波特率 (50 - 4000000)
  - 数据位 (5, 6, 7, 8, 9)
  - 停止位 (1, 1.5, 2)
  - 校验位 (none, odd, even)
  - 流控制 (none, rtscts, xonxoff)
- **多种数据格式**:
  - 字符串 (str)
  - 十六进制 (hex)
  - 32位有符号整数 (i32)
  - 32位无符号整数 (u32)
- **实时消息显示**: 支持自动滚动和手动翻页
- **快捷键支持**:
  - `Enter`: 发送消息
  - `Ctrl+U`: 清空输入框
  - `Ctrl+W`: 删除最后一个单词
  - `PageUp/PageDown`: 消息翻页

## 依赖项

### 必需依赖
- **libserialport**: 跨平台串口通信库
- **FTXUI**: C++ 终端用户界面库
- **C++17 编译器**: g++ 或 clang++

### 安装依赖

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
1. 安装 MSYS2 和 MinGW-w64
2. 安装依赖:
```bash
pacman -S mingw-w64-x86_64-libserialport mingw-w64-x86_64-ftxui
```

## 编译安装

### 1. 克隆项目
```bash
git clone <repository-url>
cd sscom
```

### 2. 编译项目
```bash
make
```

### 3. 运行
```bash
# 显示可用串口
./sscom

# 显示帮助信息
./sscom --help

# 连接串口示例
./sscom --port /dev/ttyUSB0 --baudrate 115200 --read hex
```

### 4. 清理
```bash
make clean
```

## 使用方法

### 命令行参数

| 短参数 | 长参数 | 参数值 | 说明 |
|--------|--------|--------|------|
| `-s`, `-p` | `--serial`, `--port` | `SERIALPORT` | 串口设备路径 (必需) |
| `-b` | `--baudrate` | `RATE` | 波特率 (默认: 115200) |
| `-db` | `--databits` | `BITS` | 数据位 (5/6/7/8/9, 默认: 8) |
| `-sb` | `--stopbits` | `BITS` | 停止位 (1/1.5/2, 默认: 1) |
| `-prt` | `--parity` | `TYPE` | 校验位 (none/odd/even, 默认: none) |
| `-fc` | `--flowcontrol` | `TYPE` | 流控制 (none/rtscts/xonxoff, 默认: none) |
| `-r` | `--read` | `DATATYPE` | 读取格式 (str/hex/i32/u32, 默认: str) |
| `-h` | `--help` | | 显示帮助信息 |
| `-v` | `--version` | | 显示版本信息 |

### 使用示例

1. **列出可用串口**:
   ```bash
   ./sscom
   ```

2. **连接串口并显示十六进制数据**:
   ```bash
   ./sscom --port /dev/ttyUSB0 --baudrate 9600 --read hex
   ```

3. **完整配置示例**:
   ```bash
   ./sscom --port COM3 --baudrate 115200 --databits 8 --stopbits 1 \
           --parity none --flowcontrol none --read str
   ```

4. **使用短参数**:
   ```bash
   ./sscom -p /dev/ttyACM0 -b 57600 -r hex
   ```

### TUI 界面操作

进入 TUI 界面后:
1. **底部输入框**: 输入要发送的消息，按 `Enter` 发送
2. **消息区域**: 显示接收到的消息
3. **快捷键**:
   - `Enter`: 发送当前输入的消息
   - `Ctrl+U`: 清空输入框
   - `Ctrl+W`: 删除输入框中的最后一个单词
   - `PageUp`: 向上翻页查看历史消息
   - `PageDown`: 向下翻页查看历史消息
   - `Ctrl+C`: 退出程序

## 项目结构

```
sscom/
├── inc/                    # 头文件目录
│   ├── sscom.hpp          # 主头文件
│   └── ui.hpp             # UI 界面头文件
├── src/                   # 源文件目录
│   ├── main.cpp           # 主程序入口
│   └── sscom.cpp          # 串口功能实现
├── Makefile              # 构建配置
└── README.md            # 本文档
```

## 故障排除

### 权限问题 (Linux/macOS)
如果出现权限错误:
```bash
sudo chmod a+rw /dev/ttyUSB0  # 替换为你的设备
```

或者将用户添加到 dialout 组:
```bash
sudo usermod -a -G dialout $USER
# 需要重新登录生效
```

### Windows 权限问题
以管理员身份运行程序，或检查设备管理器中的串口权限设置。

### 编译错误
1. **找不到 FTXUI 库**:
   ```bash
   # 手动指定 FTXUI 路径
   make CXXFLAGS="-I/path/to/ftxui/include" LDFLAGS="-L/path/to/ftxui/lib -lftxui-component -lftxui-dom -lftxui-screen -lserialport -lpthread"
   ```

2. **找不到 libserialport**:
   确保已正确安装 libserialport 开发包。

## 许可证

MIT License

## 版本信息

- 当前版本: 0.1.0
- 构建日期: 2025-12-10
- 依赖库版本:
  - libserialport: [自动检测]
  - FTXUI: 6.1.9

## 贡献

欢迎提交 Issue 和 Pull Request 来改进这个项目。

## 致谢

- [libserialport](https://sigrok.org/wiki/Libserialport) - 跨平台串口库
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - C++ 终端用户界面库
