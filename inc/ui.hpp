#include "ftxui/component/component_options.hpp"
#include "ftxui/screen/color.hpp"
#include <deque>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <functional>
#include <mutex>
#include <string>


class sscom_ui {
public:
  sscom_ui(size_t max_messages = 100)
      : MAX_MESSAGES(max_messages),
        screen_(ftxui::ScreenInteractive::TerminalOutput()) {
    setup_components();
  }

  void run() { screen_.Loop(renderer); }

  void add_message(const std::string &msg) {
    std::lock_guard<std::mutex> lock(messages_mutex_);
    messages_.push_back(msg);
    if (messages_.size() > MAX_MESSAGES) {
      messages_.pop_front();
    }
    screen_.PostEvent(ftxui::Event::Custom);
  }

  void set_on_send(std::function<void(const std::string &)> callback) {
    on_send_callback = callback;
  }

  void post_message(const std::string& msg) {
    screen_.Post([this, msg] {
      this->add_message(msg);
    });
  }

  // ftxui::ScreenInteractive& screen() { return screen_; }
private:
  const size_t MAX_MESSAGES;
  ftxui::ScreenInteractive screen_;
  std::deque<std::string> messages_;
  std::mutex messages_mutex_;
  std::string input_;
  std::mutex input_mutex_;
  ftxui::Component input_component_;
  ftxui::Component renderer;
  std::function<void(const std::string &)> on_send_callback;

  void setup_components() {
    auto input_option = ftxui::InputOption();
    input_option.transform = [](ftxui::InputState state) {
      state.element |= color(ftxui::Color::Chartreuse1);
      return state.element;
    };

    input_component_ = Input(&input_, "Type your message...", input_option);

    input_component_ |= ftxui::CatchEvent([this](ftxui::Event event) {
      if (event == ftxui::Event::Return && !input_.empty()) {
        {
          std::lock_guard<std::mutex> lock(messages_mutex_);
          std::lock_guard<std::mutex> input_lock(input_mutex_);
          if (on_send_callback) {
            on_send_callback(input_);
          }
          // messages.push_back("send: " + input);
          if (messages_.size() > MAX_MESSAGES) {
            messages_.pop_front();
          }
          input_.clear();
        }
        screen_.PostEvent(ftxui::Event::Custom);
        return true;
      }
      if (event == ftxui::Event::CtrlU) {
        std::lock_guard<std::mutex> lock(input_mutex_);
        input_.clear();
        return true;
      }
      if (event == ftxui::Event::CtrlW) {
        std::lock_guard<std::mutex> lock(input_mutex_);
        size_t pos = input_.find_last_of(" ");
        if (pos == std::string::npos) {
          input_.clear();
        } else {
          input_.erase(pos);
        }
        return true;
      }
      return false;
    });

    static int scroll_offset = 0;
    static bool auto_scroll = true;
    auto component = ftxui::Container::Vertical({input_component_});
    component |= ftxui::CatchEvent([this](ftxui::Event event) {
      if (event == ftxui::Event::PageUp) {
        auto_scroll = false;
        std::lock_guard<std::mutex> lock(messages_mutex_);
        scroll_offset = std::min(scroll_offset + 1, (int)messages_.size() - 1);
        screen_.PostEvent(ftxui::Event::Custom);
        return true;
      }
      if (event == ftxui::Event::PageDown) {
        std::lock_guard<std::mutex> lock(messages_mutex_);
        scroll_offset = std::max(scroll_offset - 1, 0);
        if (scroll_offset == 0)
          auto_scroll = true;
        screen_.PostEvent(ftxui::Event::Custom);
        return true;
      }
      if (event == ftxui::Event::Custom) {
        if (auto_scroll)
          scroll_offset = 0;
        return false;
      }
      return false;
    });

    renderer = Renderer(component, [this] {
      std::lock_guard<std::mutex> lock(messages_mutex_);
      ftxui::Elements visible_messages;
      int visible_height = screen_.dimy() - 3 - 2;

      int start_idx = 0;
      if (messages_.size() > (size_t)visible_height) {
        if (auto_scroll) {
          start_idx = messages_.size() - visible_height;
        } else {
          start_idx = std::max(0, (int)messages_.size() - visible_height +
                                      scroll_offset);
        }
      }

      for (int i = start_idx; i < (int)messages_.size(); ++i) {
        visible_messages.push_back(ftxui::text(messages_[i]));
      }

      auto message_box = vbox(visible_messages);
      if (messages_.size() > (size_t)visible_height) {
        message_box = message_box | ftxui::vscroll_indicator | ftxui::frame | ftxui::yframe;
      }

      return ftxui::vbox(
          {window(ftxui::text("Messages"),
                  message_box | size(ftxui::HEIGHT, ftxui::EQUAL, visible_height + 2)) |
               ftxui::flex,
           window(ftxui::text("Send"), input_component_->Render()) |
               size(ftxui::HEIGHT, ftxui::EQUAL, 3)});
    });
  }
};
