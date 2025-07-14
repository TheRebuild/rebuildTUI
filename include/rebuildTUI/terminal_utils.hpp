#pragma once

#include <iostream>
#include <optional>
#include <string>

#include "styles.hpp"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#endif

namespace tui {

    /**
     * @brief Terminal control utilities for cross-platform TUI applications
     */
    class TerminalUtils {
    public:
        /**
         * @brief Special key codes for navigation
         */
        enum class Key {
            UNKNOWN = 0,
            ARROW_UP = 1,
            ARROW_DOWN = 2,
            ARROW_LEFT = 3,
            ARROW_RIGHT = 4,
            ENTER = 5,
            SPACE = 6,
            ESCAPE = 7,
            BACKSPACE = 8,
            TAB = 9,
            HOME = 10,
            END = 11,
            PAGE_UP = 12,
            PAGE_DOWN = 13,
            DELETE = 14,
            KEY_J = 15,
            KEY_K = 16,
            KEY_H = 17,
            KEY_L = 18,
            NORMAL = 19,
            F1 = 20,
            F2 = 21,
            F3 = 22,
            F4 = 23,
            F5 = 24,
            F6 = 25,
            F7 = 26,
            F8 = 27,
            F9 = 28,
            F10 = 29,
            F11 = 30,
            F12 = 31
        };

        /**
         * @brief Color codes for terminal output
         */
        enum class Color {
            RESET = 0,
            BLACK = 30,
            RED = 31,
            GREEN = 32,
            YELLOW = 33,
            BLUE = 34,
            MAGENTA = 35,
            CYAN = 36,
            WHITE = 37,
            BRIGHT_BLACK = 90,
            BRIGHT_RED = 91,
            BRIGHT_GREEN = 92,
            BRIGHT_YELLOW = 93,
            BRIGHT_BLUE = 94,
            BRIGHT_MAGENTA = 95,
            BRIGHT_CYAN = 96,
            BRIGHT_WHITE = 97
        };

        /**
         * @brief Text style codes
         */
        enum class Style {
            RESET = 0,
            BOLD = 1,
            DIM = 2,
            ITALIC = 3,
            UNDERLINE = 4,
            BLINK = 5,
            REVERSE = 7,
            STRIKETHROUGH = 9
        };

        static void init_terminal();
        static void restore_terminal();
        static void clear_screen();
        static void move_cursor(int row, int col);
        static void hide_cursor();
        static void show_cursor();
        static std::pair<int, int> get_terminal_size();
        static void set_color(Color color);

        // Gradient
        static void set_color_rgb(uint8_t r, uint8_t g, uint8_t b);
        static void set_color_rgb(tui_extras::GradientColor &color);
        static void set_color_rgb(tui_extras::GradientColor color);

        static void set_style(Style style);
        static void reset_formatting();

        /**
         * @brief Key event structure
         */
        struct KeyEvent {
            Key key;
            char character;

            explicit KeyEvent(const Key k = Key::UNKNOWN, const char c = '\0') : key(k), character(c) {}
        };

        static void print_colored(const std::string &text, Color color);
        static void print_styled(const std::string &text, Style style);
        static void print_formatted(const std::string &text, Color color, Style style);
        static int get_key();
        static bool key_available();
        static std::pair<Key, char> get_input();
        static void draw_horizontal_line(int row, int start_col, int length, char ch = '-');
        static void draw_vertical_line(int start_row, int col, int length, char ch = '|');
        static void draw_box(int top_row, int left_col, int width, int height);
        static void print_centered(const std::string &text, int width, int row = -1);
        static void print_at(int row, int col, const std::string &text);
        static int get_centered_col(int content_width);
        static int get_centered_row(int content_height);
        static std::pair<int, int> get_centered_position(int content_width, int content_height);
        static void print_centered_at_row(int row, const std::string &text);
        static void print_centered_screen(const std::string &text);
        static void draw_centered_box(int width, int height);
        static std::pair<int, int> get_centering_margins(int content_width, int content_height);
        static void save_cursor_position();
        static void restore_cursor_position();
        static void set_echo(bool enable);
        static void set_canonical_mode(bool enable);
        static void flush();

    private:
#ifdef _WIN32
        static HANDLE hConsole;
        static CONSOLE_SCREEN_BUFFER_INFO csbi;
        static DWORD originalConsoleMode;
#else
        static struct termios original_termios;
        static bool termios_saved;
#endif
        static Key parse_escape_sequence();
        static void init_platform_terminal();
        static void restore_platform_terminal();
    };

    /**
     * @brief Terminal manager class for instance-based terminal control
     */
    class TerminalManager {
    private:
        bool terminal_initialized_;

    public:
        TerminalManager() : terminal_initialized_(false) {}
        ~TerminalManager() {
            if (terminal_initialized_) {
                restore_terminal();
            }
        }

        void setup_terminal() {
            if (!terminal_initialized_) {
                TerminalUtils::init_terminal();
                terminal_initialized_ = true;
            }
        }

        void restore_terminal() {
            if (terminal_initialized_) {
                TerminalUtils::restore_terminal();
                terminal_initialized_ = false;
            }
        }

        static void clear_screen() { TerminalUtils::clear_screen(); }
        static void flush_output() { std::cout.flush(); }

        static std::optional<TerminalUtils::KeyEvent> get_key_input();
        static bool key_available();
        static std::pair<int, int> get_terminal_size() { return TerminalUtils::get_terminal_size(); }
    };

} // namespace tui
