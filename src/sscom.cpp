#include <csignal>
#include <cstdarg>
#include <libserialport.h>
#include <memory>
#include <sscom.hpp>
// #include <map>
#ifdef _WIN32
#include <windows.h>
#endif

constexpr const char* VERSION = "0.1.0";
constexpr const char* BUILD_DATE = __DATE__ " " __TIME__;
constexpr const char* COPYRIGHT = "Copyright (c) 2025 SICKEE NO.2";
constexpr const char* FTXUI_VERSION = "6.1.9";

namespace sscom {
void print_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}
// valid baud rates
constexpr std::array<unsigned long, 30> standard_baud_rates = {
    50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800,
    9600, 19200, 38400, 57600, 115200, 230400, 460800, 500000,
    576000, 921600, 1000000, 1152000, 1500000, 2000000, 2500000,
    3000000, 3500000, 4000000
};

// valid data bits
constexpr std::array<unsigned int, 5> valid_data_bits = {5, 6, 7, 8, 9};

// valid stop bits: 1, 1.5, 2
constexpr std::array<float, 3> valid_stop_bits = {1.0f, 1.5f, 2.0f};

// map argumeng and aliases
const std::unordered_map<std::string, std::string> arg_aliases = {
    {"-s", "--serial"},       {"-p", "--port"},      {"-b", "--baudrate"},
    {"-db", "--databits"},    {"-sb", "--stopbits"}, {"-prt", "--parity"},
    {"-fc", "--flowcontrol"}, {"-r", "--read"},      {"-h", "--help"},
    {"-v", "--version"}};

// map valid arg and parameter
const std::unordered_map<std::string, sp_parity> parity_map = {
    {"none", SP_PARITY_NONE},
    {"odd", SP_PARITY_ODD},
    {"even", SP_PARITY_EVEN}};

const std::unordered_map<std::string, sp_flowcontrol> flowcontrol_map = {
    {"none", SP_FLOWCONTROL_NONE},
    {"rtscts", SP_FLOWCONTROL_RTSCTS},
    {"xonxoff", SP_FLOWCONTROL_XONXOFF}};

const std::unordered_map<std::string, data_type> read_type_map = {
    {"str", data_type::str},
    {"hex", data_type::hex},
    {"i32", data_type::int32},
    {"u32", data_type::uint32}};

std::shared_ptr<serial> instance = nullptr;

serial::serial(int argc, const char *argv[])
    : port(nullptr), keep_running(true), read_as_type(data_type::str),
      config() {
  arguments_ok = parse_arguments(argc, argv);
}

std::shared_ptr<serial> serial::create(int argc, const char* argv[]){
  if(!instance)
    instance = std::shared_ptr<serial>(new serial(argc, argv));
  return instance;
}

serial::~serial(){
  keep_running = false;
  if(nullptr != port){
    sp_close(port);
    sp_free_port(port);
  }
}

bool serial::parse_arguments(int argc, const char* argv[]){
  if(argc == 1){
    print_ports();
    return false;
  }
  for (int i = 1; i < argc;) {
    std::string arg = argv[i];

    // 处理参数别名
    if (arg_aliases.count(arg)) {
      arg = arg_aliases.at(arg);
    }

    if (arg == "--help" || arg == "-h") {
      print_help(argv[0]);
      return false;
    }

    if(arg == "--version"){
      print_version();
      return false;
    }

    // check next parameter
    if (i + 1 >= argc) {
      print_error("Missing value for parameter: %s", arg.c_str());
      return false;
    }

    const char *value = argv[++i];

    try {
      if (arg == "--serial" || arg == "--port") {
        config.port_name = value;
      } else if (arg == "--baudrate") {
        config.baudrate = std::stoi(value);
        if (!is_valid_baudrate()) {
          print_error("Invalid baudrate %d. Valid values:", config.baudrate);
          for (auto rate : standard_baud_rates) {
            fprintf(stderr, "%lu ", rate);
          }
          fprintf(stderr, "\n");
          return false;
        }
      } else if (arg == "--databits") {
        config.databits = std::stoi(value);
        if (!is_valid_data_bits()) {
          print_error("Invalid data bits %d. Valid: 5/6/7/8/9",
                      config.databits);
          return false;
        }
      } else if (arg == "--stopbits") {
        config.stopbits = std::stof(value);
        if (!is_valid_stop_bits()) {
          print_error("Invalid stop bits %.1f. Valid: 1/1.5/2",
                      config.stopbits);
          return false;
        }
      } else if (arg == "--parity") {
        if (!parity_map.count(value)) {
          print_error("Invalid parity '%s'. Valid: none/odd/even", value);
          return false;
        }
        config.parity = parity_map.at(value);
      } else if (arg == "--flowcontrol") {
        if (!flowcontrol_map.count(value)) {
          print_error("Invalid flowcontrol '%s'. Valid: none/rtscts/xonxoff",
                      value);
          return false;
        }
        config.flowcontrol = flowcontrol_map.at(value);
      } else if (arg == "--read") {
        if (!read_type_map.count(value)) {
          print_error("Invalid read type '%s'. Valid: str/hex/i32/u32",
                      value);
          return false;
        }
        read_as_type = read_type_map.at(value);
      } else {
        print_error("Unknown parameter: %s", arg.c_str());
        return false;
      }
      i++; // next argument
    } catch (const std::exception &e) {
      print_error("Invalid value '%s' for %s: %s", value, arg.c_str(),
                  e.what());
      return false;
    }
  }

  if (config.port_name.empty()) {
    print_error("Serial port name is required");
    print_help(argv[0]);
    return false;
  }

  return true;
}

app_state serial::config_port(){

  auto check = [](sp_return result, const char* msg) {
    if (result != SP_OK) {
      fprintf(stderr, "%s failed\n", msg);
      return false;
    }
    return true;
  };

  if(nullptr == port){
    current_state = app_state::error;
    print_error("config port must after open port");
    return current_state;
  }
 
  current_state = app_state::connecting;
  bool state = check(sp_set_baudrate(port, config.baudrate), "Baudrate") &&
        check(sp_set_bits(port, config.databits), "Data bits") &&
        check(sp_set_parity(port, config.parity), "Parity") &&
        check(sp_set_stopbits(port, config.stopbits), "Stop bits") &&
        check(sp_set_flowcontrol(port, config.flowcontrol), "Flow control");

  if(!state){
    current_state = app_state::error;
    sp_close(port);
    sp_free_port(port);
    port = nullptr;
  }
  current_state = app_state::connected;
  return current_state;
}

app_state serial::open(){

  current_state = app_state::opening;

  if (sp_get_port_by_name(config.port_name.c_str(), &port) != SP_OK) {
    current_state = app_state::error;
    print_error("Cannot find port %s\n", config.port_name.c_str());
    return current_state;
  }

  if (sp_open(port, SP_MODE_READ_WRITE) != SP_OK) {
    current_state = app_state::error;
#ifdef _WIN32
    if (GetLastError() == ERROR_ACCESS_DENIED) {
      print_error("Run as Administrator or check port permissions\n");
    }
#else
    if (errno == EACCES) {
      print_error("Permission denied. Try:\nsudo chmod a+rw /dev/xxx\n");
    }
#endif
    print_error("Failed to open port %s", config.port_name.c_str());
    sp_free_port(port);
    port = nullptr;
    return current_state;
  }
  current_state = app_state::opened;
  return current_state;
}

app_state serial::get_port_state() const {
  return current_state;
}

void signal_handler(int signal) {
  if (signal == SIGINT || signal == SIGTERM) {
    instance->disconnect();
  }
}

void serial::disconnect(){
  this->keep_running = false;
}

void serial::connect(){
  if(!arguments_ok) return;
  if(open() != app_state::opened) return;
  if(config_port() != app_state::connected) return;

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  // add helper message
  ui.add_message("Connected to " + config.port_name +
                  " at " + std::to_string(config.baudrate) + " baud");
  ui.add_message("ctrl+c to exit");

  // callback on add message in UI
  ui.set_on_send([&](const std::string& msg) {
    {
      std::lock_guard<std::mutex> lock(port_mutex);
      if (port) {
        sp_blocking_write(port, msg.c_str(), msg.size(), 100);
      }
    }
    ui.post_message("<- " + msg);
  });

  auto recicer_func = [this](){this->receive_message_thread();};
  receiver = std::thread(recicer_func);

  ui.run();

  keep_running = false;
  receiver.join();

}

void serial::print_help(const char* app){
  printf("Print serial ports if no options to added.\n"
          "Usage: %s [options]\n"
          "Options:\n"
          "  -s   --serial  -p  --port\n"
          "                                   SERIALPORT  Specify serial port (required)\n"
          "  -b   --baudrate      RATE        Set baud rate (default: 115200)\n"
          "  -db  --databits      BITS        Set data bits (5/6/7/8, default: 8)\n"
          "  -sb  --stopbits      BITS        Set stop bits (1/1.5/2, default: 1)\n"
          "  -prt --parity        TYPE        Set parity (none/odd/even, default: none)\n"
          "  -fc  --flowcontrol   TYPE        Set flow control (none/rtscts/xonxoff, default: none)\n"
          "  -r,  --read          DATATYPE    Read data as string or hex (str/hex/i32/u32, default: str)\n"
          "  -h,  --help          Show        this help message\n",
          app);
}

void serial::print_ports(){
  struct sp_port **port_list;

  // get ports list
  enum sp_return result = sp_list_ports(&port_list);
  if (result != SP_OK) {
    printf("Failed to get serial port list: %d\n", result);
    return;
  }

  if(nullptr != port_list[0]) {
    printf("found serials:\n");
    for (int i = 0; port_list[i]; i++) {
      printf("%d : %s => %s\n", i, sp_get_port_name(port_list[i]),
            sp_get_port_description(port_list[i]));
    }
  }else{
    printf("No supported serial ports found.\n");
  }

  // free
  sp_free_port_list(port_list);
}


void serial::print_version(){
  printf("Serial Tool %s\n", VERSION);
  printf("Build date: %s\n", BUILD_DATE);
  printf("%s\n", COPYRIGHT);
  printf("License: MIT\n");
  printf("\n");
  printf("Library versions:\n");
  printf("  - libserialport: %s\n", sp_get_package_version_string());
  printf("  - FTXUI: %s\n", FTXUI_VERSION);
}

bool serial::is_valid_baudrate() const{
  return std::find(standard_baud_rates.begin(), 
                  standard_baud_rates.end(), 
                  config.baudrate) != standard_baud_rates.end();
}

bool serial::is_valid_stop_bits() const{
  // allow compare float
  auto approx_equal = [](float a, float b) {
      return std::abs(a - b) < 0.01f;
  };

  // check valid
  return std::any_of(valid_stop_bits.begin(), valid_stop_bits.end(),
      [&](float valid) { return approx_equal(config.stopbits, valid); });
}

bool serial::is_valid_data_bits() const{
  // check valid
  return std::find(valid_data_bits.begin(), 
                  valid_data_bits.end(), 
                  config.databits) != valid_data_bits.end();
}

void serial::receive_message_thread(){
  std::string message;
  message.reserve(1024);
  char hex[4];

  char buf[512];
  while (keep_running) {
    message.clear();
    int bytes_read = sp_nonblocking_read(port, buf, sizeof(buf) - 1);
    if (bytes_read > 0) {
      switch(read_as_type){
        case data_type::hex:
        {
          message.reserve(bytes_read*3 + 4);
          message = "->: ";
          for(int i = 0; i < bytes_read; i++){
            snprintf(hex, sizeof(hex), "%02X ", (unsigned char)buf[i]);
            message += hex;
          }
        }
        break;
        case data_type::int32:
        {
          message = "->: ";
          int32_t *data = (int32_t*)(buf);
          unsigned count = bytes_read/sizeof(uint32_t);
          for(unsigned i = 0; i < count; i++){
            message += std::to_string(data[i]) + " ";
          }
        }
        break;
        case data_type::uint32:
        {
          message = "->: ";
          uint32_t *data = (uint32_t*)(buf);
          unsigned count = bytes_read/sizeof(int32_t);
          for(unsigned i = 0; i < count; i++){
            message += std::to_string(data[i]) + " ";
          }
        }
        break;
        default:
        {
          buf[bytes_read] = '\0';
          if(bytes_read > 2 && buf[bytes_read - 1] == '\n')
            buf[bytes_read - 1] = '\0';
          message = "->: " + std::string(buf);
        }
      }
      ui.post_message(message);
    } else if (bytes_read == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } else {
      if (keep_running) { // 仅在未主动退出时报错
        ui.post_message("[ERROR] Read error");
      }
      break;
    }
  }
}
} // namespace sscom
