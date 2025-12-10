#include "ui.hpp"
#include <atomic>
#include <thread>
#include <memory>
#include <libserialport.h>

namespace sscom {
enum class data_type { str, hex, int32, uint32 };
enum class app_state { opening, opened, disconnected, connecting, connected, error };
class serial {
  struct serial_config_pack {
    std::string port_name;
    int baudrate = 115200;
    int databits = 8;
    float stopbits = 1;
    sp_parity parity = SP_PARITY_NONE;
    sp_flowcontrol flowcontrol = SP_FLOWCONTROL_NONE;
    bool is_inited = false;
  };

  sp_port *port = nullptr;
  std::atomic<bool> keep_running;

  data_type read_as_type = data_type::str;
  std::mutex port_mutex;

  serial_config_pack config;
  bool arguments_ok = false;
  sscom_ui ui;
  std::thread receiver;

  bool parse_arguments(int argc, const char *argv[]);
  bool is_valid_baudrate() const;
  bool is_valid_stop_bits() const ;
  bool is_valid_data_bits() const;

  app_state current_state = app_state::disconnected;

  app_state config_port();
  app_state open();
  void receive_message_thread();
  void print_help(const char* app);
  serial(int argc, const char *argv[]);
public:
  static std::shared_ptr<serial> create(int argc, const char* argv[]);
  ~serial();
  void print_version();
  void print_ports();
  app_state get_port_state() const;
  void connect();
  void disconnect();
};

} // namespace sscom
