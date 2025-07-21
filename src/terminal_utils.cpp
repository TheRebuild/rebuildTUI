#include "terminal_utils.hpp"

#ifndef _WIN32
#include <sys/ioctl.h>
#endif

#include <unistd.h>

namespace tui {

// Static member definitions
#ifdef _WIN32
    HANDLE TerminalUtils::hConsole = INVALID_HANDLE_VALUE;
    CONSOLE_SCREEN_BUFFER_INFO TerminalUtils::csbi = {};
    DWORD TerminalUtils::originalConsoleMode = 0;
    bool TerminalUtils::is_wt = false;
#else
    termios TerminalUtils::original_termios = {};
    bool TerminalUtils::termios_saved = false;
#endif

    void TerminalUtils::init_terminal() {
        init_platform_terminal();
        clear_screen();
        hide_cursor();
    }

    void TerminalUtils::restore_terminal() {
        show_cursor();
        reset_formatting();
        restore_platform_terminal();
    }

    void TerminalUtils::clear_screen() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            COORD coord = {0, 0};
            DWORD written;
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            FillConsoleOutputCharacterA(hConsole, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &written);
            FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &written);
            SetConsoleCursorPosition(hConsole, coord);
        }
#else
        std::cout << "\033[2J\033[H";
        flush();
#endif
    }

    void TerminalUtils::move_cursor(int row, int col) {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            COORD coord = {static_cast<SHORT>(col - 1), static_cast<SHORT>(row - 1)};
            SetConsoleCursorPosition(hConsole, coord);
        }
#else
        std::cout << "\033[" << row << ";" << col << "H";
        flush();
#endif
    }

    void TerminalUtils::hide_cursor() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(hConsole, &cursorInfo);
            cursorInfo.bVisible = FALSE;
            SetConsoleCursorInfo(hConsole, &cursorInfo);
        }
#else
        std::cout << "\033[?25l";
        flush();
#endif
    }

    void TerminalUtils::show_cursor() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(hConsole, &cursorInfo);
            cursorInfo.bVisible = TRUE;
            SetConsoleCursorInfo(hConsole, &cursorInfo);
        }
#else
        std::cout << "\033[?25h";
        flush();
#endif
    }

    std::pair<int, int> TerminalUtils::get_terminal_size() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            return {height, width};
        }
        return {25, 80}; // Default fallback
#else
        winsize w{};
        // if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        //   return {w.ws_row, w.ws_col};
        // }
        // return {25, 80}; // Default fallback

        return ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0
            ? std::make_pair(static_cast<int>(w.ws_row), static_cast<int>(w.ws_col))
            : std::make_pair(25, 80);
#endif
    }

    void TerminalUtils::set_color(Color color) {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            WORD attributes = 0;
            switch (color) {
            case Color::BLACK:
                attributes = 0;
                break;
            case Color::RED:
                attributes = FOREGROUND_RED;
                break;
            case Color::GREEN:
                attributes = FOREGROUND_GREEN;
                break;
            case Color::YELLOW:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN;
                break;
            case Color::BLUE:
                attributes = FOREGROUND_BLUE;
                break;
            case Color::MAGENTA:
                attributes = FOREGROUND_RED | FOREGROUND_BLUE;
                break;
            case Color::CYAN:
                attributes = FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
            case Color::WHITE:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
            case Color::BRIGHT_BLACK:
                attributes = FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_RED:
                attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_GREEN:
                attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_YELLOW:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_BLUE:
                attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_MAGENTA:
                attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_CYAN:
                attributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_WHITE:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            default:
                attributes = csbi.wAttributes;
                break;
            }
            SetConsoleTextAttribute(hConsole, attributes);
        }
#else
        std::cout << std::format("\033[{}m", (color == Color::RESET) ? 0 : static_cast<int>(color));

        flush();
#endif
    }

    void TerminalUtils::set_color(tui_extras::AccentColor color) {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            WORD attributes = 0;
            switch (color) {
            case Color::BLACK:
                attributes = 0;
                break;
            case Color::RED:
                attributes = FOREGROUND_RED;
                break;
            case Color::GREEN:
                attributes = FOREGROUND_GREEN;
                break;
            case Color::YELLOW:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN;
                break;
            case Color::BLUE:
                attributes = FOREGROUND_BLUE;
                break;
            case Color::MAGENTA:
                attributes = FOREGROUND_RED | FOREGROUND_BLUE;
                break;
            case Color::CYAN:
                attributes = FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
            case Color::WHITE:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
            case Color::BRIGHT_BLACK:
                attributes = FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_RED:
                attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_GREEN:
                attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_YELLOW:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_BLUE:
                attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_MAGENTA:
                attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_CYAN:
                attributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            case Color::BRIGHT_WHITE:
                attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            default:
                attributes = csbi.wAttributes;
                break;
            }
            SetConsoleTextAttribute(hConsole, attributes);
        }
#else
        std::cout << std::format("\033[{}m", (color == tui_extras::AccentColor::RESET) ? 0 : static_cast<int>(color));

        flush();
#endif
    }

    void TerminalUtils::set_color_rgb(uint8_t r, uint8_t g, uint8_t b) {
        // #ifdef _WIN32
        //         printf("\033[38;2;%d;%d;%dm", r, g, b);
        // #else
        std::cout << std::format("\033[38;2;{};{};{}m", static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
        // #endif
        flush();
    }

    void TerminalUtils::set_color_rgb(const tui_extras::GradientColor color) {
#ifdef _WIN32
        if (!is_wt) {
            return;
        }
#endif

        auto [r, g, b] = color.get_color();
        set_color_rgb(r, g, b);
    }

    void TerminalUtils::set_style(Style style) {
#ifdef _WIN32
        // Windows console doesn't support all styles, so we'll do our best
        if (hConsole != INVALID_HANDLE_VALUE) {
            switch (style) {
            case Style::BOLD:
                SetConsoleTextAttribute(hConsole, csbi.wAttributes | FOREGROUND_INTENSITY);
                break;
            case Style::REVERSE:
                SetConsoleTextAttribute(hConsole, ((csbi.wAttributes & 0xF0) >> 4) | ((csbi.wAttributes & 0x0F) << 4));
                break;
            case Style::RESET:
                SetConsoleTextAttribute(hConsole, csbi.wAttributes);
                break;
            default:
                break; // Other styles not supported on Windows
            }
        }
#else
        std::cout << std::format("\033[{}m", static_cast<int>(style));
        // std::cout << "\033[" << static_cast<int>(style) << "m";
        flush();
#endif
    }

    void TerminalUtils::reset_formatting() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, csbi.wAttributes);
        }
#else
        std::cout << "\033[0m";
        flush();
#endif
    }

    void TerminalUtils::print_colored(const std::string &text, Color color) {
        set_color(color);
        std::cout << text;
        reset_formatting();
        flush();
    }

    void TerminalUtils::print_styled(const std::string &text, Style style) {
        set_style(style);
        std::cout << text;
        reset_formatting();
        flush();
    }

    void TerminalUtils::print_formatted(const std::string &text, Color color, Style style) {
        set_color(color);
        set_style(style);
        std::cout << text;
        reset_formatting();
        flush();
    }

    int TerminalUtils::get_key() {
#ifdef _WIN32
        return _getch();
#else
        return getchar();
#endif
    }

    bool TerminalUtils::key_available() {
#ifdef _WIN32
        return _kbhit();
#else
        fd_set readfds;
        timeval timeout{};

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        const int result = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);
        return result > 0;
#endif
    }

    // std::pair<TerminalUtils::Key, char> TerminalUtils::get_input() {
    //   unsigned char buf[3] = {0};
    //   int n = read(STDIN_FILENO, buf, 1);
    //   if (n <= 0)
    //     return {Key::UNKNOWN, 0};

    //   if (buf[0] == 27) {
    //     n = read(STDIN_FILENO, buf + 1, 2);
    // #ifdef DEBUG_BUILD
    //     std::cout << "n: " << n << std::endl;
    // #endif
    //     if (n == 1)
    //       return {Key::ESCAPE, 0};
    //     else if (buf[1] == '[') {
    //       switch (buf[2]) {
    //       case 'A':
    //         return {Key::ARROW_UP, 0};
    //       case 'B':
    //         return {Key::ARROW_DOWN, 0};
    //       case 'C':
    //         return {Key::ARROW_RIGHT, 0};
    //       case 'D':
    //         return {Key::ARROW_LEFT, 0};
    //       case 'H':
    //         return {Key::HOME, 0};
    //       case 'F':
    //         return {Key::END, 0};
    //       case '5':
    //         read(STDIN_FILENO, buf, 1);
    //         return {Key::PAGE_UP, 0};
    //       case '6':
    //         read(STDIN_FILENO, buf, 1);
    //         return {Key::PAGE_DOWN, 0};
    //       case '3':
    //         read(STDIN_FILENO, buf, 1);
    //         return {Key::DELETE, 0};
    //       default:
    //         return {Key::UNKNOWN, 0};
    //       }
    //     }
    //     return {Key::UNKNOWN, 0};
    //   }

    //   switch (buf[0]) {
    //   case '\n':
    //   case '\r':
    //     return {Key::ENTER, 0};
    //   case ' ':
    //     return {Key::SPACE, 0};
    //   case '\t':
    //     return {Key::TAB, 0};
    //   case 8:
    //   case 127:
    //     return {Key::BACKSPACE, 0};
    //   case 3:
    //     return {Key::ESCAPE, 0};
    //   default:
    //     return {Key::UNKNOWN,
    //             (buf[0] >= 32 && buf[0] <= 126) ? static_cast<char>(buf[0]) : 0};
    //   }
    // }

    std::pair<TerminalUtils::Key, char> TerminalUtils::get_input() {
        int ch = get_key();
        if (ch == 27) {
            const int ch1 = get_key();
            if (ch1 == 27) {
                return {Key::ESCAPE, 0};
            }

            if (ch1 == '[' || ch1 == 'O') {
                switch (get_key()) {
                case 'A':
                    return {Key::ARROW_UP, 0};
                case 'B':
                    return {Key::ARROW_DOWN, 0};
                case 'C':
                    return {Key::ARROW_RIGHT, 0};
                case 'D':
                    return {Key::ARROW_LEFT, 0};
                case 'H':
                    return {Key::HOME, 0};
                case 'F':
                    return {Key::END, 0};
                case '5':
                    get_key();
                    return {Key::PAGE_UP, 0};
                case '6':
                    get_key();
                    return {Key::PAGE_DOWN, 0};
                case '3':
                    get_key();
                    return {Key::KEY_DELETE, 0};
                default:
                    return {Key::UNKNOWN, 0};
                }
            }
            return {Key::UNKNOWN, 0};
        }

        switch (ch) {
        case '\n':
        case '\r':
            return {Key::ENTER, 0};
        case ' ':
            return {Key::SPACE, 0};
        case '\t':
            return {Key::TAB, 0};
        case 8:
        case 127:
            return {Key::BACKSPACE, 0};
        case 3:
            return {Key::ESCAPE, 0};
#ifdef _WIN32
        case 224:
            ch = get_key();
            switch (ch) {
            case 72:
                return {Key::ARROW_UP, 0};
            case 80:
                return {Key::ARROW_DOWN, 0};
            case 75:
                return {Key::ARROW_LEFT, 0};
            case 77:
                return {Key::ARROW_RIGHT, 0};
            case 71:
                return {Key::HOME, 0};
            case 79:
                return {Key::END, 0};
            case 73:
                return {Key::PAGE_UP, 0};
            case 81:
                return {Key::PAGE_DOWN, 0};
            case 83:
                return {Key::KEY_DELETE, 0};
            default:
                return {Key::UNKNOWN, 0};
            }
#endif
        default:
            // if (ch >= 32 && ch <= 126)
            //   return {Key::UNKNOWN, static_cast<char>(ch)};
            // return {Key::UNKNOWN, 0};
            return {Key::UNKNOWN, (ch >= 32 && ch <= 126) ? static_cast<char>(ch) : 0};
        }
    }

    TerminalUtils::Key TerminalUtils::parse_escape_sequence() {
#ifndef _WIN32
        if (const int ch1 = get_key(); ch1 == '[') {
            switch (get_key()) {
            case 'A':
                return Key::ARROW_UP;
            case 'B':
                return Key::ARROW_DOWN;
            case 'C':
                return Key::ARROW_RIGHT;
            case 'D':
                return Key::ARROW_LEFT;
            case 'H':
                return Key::HOME;
            case 'F':
                return Key::END;
            default:
                return Key::UNKNOWN;
            }
        }
#endif
        return Key::UNKNOWN;
    }

    void TerminalUtils::draw_horizontal_line(const int row, const int start_col, const int length, const char ch) {
        move_cursor(row, start_col);
        for (auto i = 0; i < length; ++i) {
            std::cout << ch;
        }

        flush();
    }

    void TerminalUtils::draw_vertical_line(int start_row, int col, int length, char ch) {
        for (int i = 0; i < length; ++i) {
            move_cursor(start_row + i, col);
            std::cout << ch;
        }

        flush();
    }

    void TerminalUtils::draw_box(int top_row, int left_col, int width, int height) {
        // Top border
        move_cursor(top_row, left_col);
        std::cout << '+';
        for (int i = 1; i < width - 1; ++i) {
            std::cout << '-';
        }

        std::cout << '+';

        // Side borders
        for (int i = 1; i < height - 1; ++i) {
            move_cursor(top_row + i, left_col);
            std::cout << '|';
            move_cursor(top_row + i, left_col + width - 1);
            std::cout << '|';
        }

        // Bottom border
        move_cursor(top_row + height - 1, left_col);
        std::cout << '+';
        for (int i = 1; i < width - 1; ++i) {
            std::cout << '-';
        }
        std::cout << '+';

        flush();
    }

    void TerminalUtils::print_centered(const std::string &text, int width, int row) {
        int padding = (width - static_cast<int>(text.length())) / 2;
        std::string padded_text = std::string(std::max(0, padding), ' ') + text;

        if (row >= 0) {
            move_cursor(row, 1);
        }

        std::cout << padded_text;
        flush();
    }

    void TerminalUtils::print_at(int row, int col, const std::string &text) {
        move_cursor(row, col);
        std::cout << text;
        flush();
    }

    void TerminalUtils::save_cursor_position() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            GetConsoleScreenBufferInfo(hConsole, &csbi);
        }
#else
        std::cout << "\033[s";
        flush();
#endif
    }

    void TerminalUtils::restore_cursor_position() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
        }
#else
        std::cout << "\033[u";
        flush();
#endif
    }

    void TerminalUtils::set_echo(bool enable) {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            DWORD mode;
            GetConsoleMode(hConsole, &mode);
            mode = enable ? mode | ENABLE_ECHO_INPUT : mode & ~ENABLE_ECHO_INPUT;
            SetConsoleMode(hConsole, mode);
        }
#else
        if (termios_saved) {
            struct termios new_termios = original_termios;
            new_termios.c_lflag = enable ? new_termios.c_lflag | ECHO : new_termios.c_lflag & ~ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        }
#endif
    }

    void TerminalUtils::set_canonical_mode(bool enable) {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            DWORD mode;
            GetConsoleMode(hConsole, &mode);
            mode = enable ? mode | ENABLE_LINE_INPUT : mode & ~ENABLE_LINE_INPUT;
            SetConsoleMode(hConsole, mode);
        }
#else
        if (termios_saved) {
            struct termios new_termios = original_termios;
            new_termios.c_lflag = enable ? new_termios.c_lflag | ICANON : new_termios.c_lflag & ~ICANON;
            tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        }
#endif
    }

    void TerminalUtils::flush() { std::cout.flush(); }

    int TerminalUtils::get_centered_col(int content_width) {
        auto [height, width] = get_terminal_size();
        return std::max(1, (width - content_width) / 2 + 1);
    }

    int TerminalUtils::get_centered_row(int content_height) {
        auto [height, width] = get_terminal_size();
        return std::max(1, (height - content_height) / 2 + 1);
    }

    std::pair<int, int> TerminalUtils::get_centered_position(int content_width, int content_height) {
        return {get_centered_row(content_height), get_centered_col(content_width)};
    }

    void TerminalUtils::print_centered_at_row(int row, const std::string &text) {
        // auto [height, width] = get_terminal_size();
        int col = get_centered_col(static_cast<int>(text.length()));
        print_at(row, col, text);
    }

    void TerminalUtils::print_centered_screen(const std::string &text) {
        // auto [height, width] = get_terminal_size();
        int row = get_centered_row(1);
        int col = get_centered_col(static_cast<int>(text.length()));
        print_at(row, col, text);
    }

    void TerminalUtils::draw_centered_box(int box_width, int box_height) {
        auto [row, col] = get_centered_position(box_width, box_height);
        draw_box(row, col, box_width, box_height);
    }

    std::pair<int, int> TerminalUtils::get_centering_margins(int content_width, int content_height) {
        auto [height, width] = get_terminal_size();
        int horizontal_margin = std::max(0, (width - content_width) / 2);
        int vertical_margin = std::max(0, (height - content_height) / 2);
        return {horizontal_margin, vertical_margin};
    }

    void TerminalUtils::init_platform_terminal() {
#ifdef _WIN32
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
            if (hInput != INVALID_HANDLE_VALUE) {
                GetConsoleMode(hInput, &originalConsoleMode);
                SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT);
            }
        }
        is_wt = std::getenv("WT_SESSION") ? true : false;
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#else
        if (tcgetattr(STDIN_FILENO, &original_termios) == 0) {
            termios_saved = true;
            struct termios new_termios = original_termios;
            new_termios.c_lflag &= ~(ICANON | ECHO);
            new_termios.c_iflag &= ~ICRNL;
            new_termios.c_cc[VMIN] = 1;
            new_termios.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        }
#endif
    }

    void TerminalUtils::restore_platform_terminal() {
#ifdef _WIN32
        if (hConsole != INVALID_HANDLE_VALUE) {
            HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
            if (hInput != INVALID_HANDLE_VALUE) {
                SetConsoleMode(hInput, originalConsoleMode);
            }
        }
#else
        if (termios_saved) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
            termios_saved = false;
        }
#endif
    }

    // TerminalManager implementations
    std::optional<tui::TerminalUtils::KeyEvent> TerminalManager::get_key_input() {
        if (!TerminalUtils::key_available()) {
            return std::nullopt;
        }

        auto [key, character] = TerminalUtils::get_input();

        TerminalUtils::Key converted_key;

        switch (key) {
        case TerminalUtils::Key::ARROW_UP:
            converted_key = TerminalUtils::Key::ARROW_UP;
            break;
        case TerminalUtils::Key::ARROW_DOWN:
            converted_key = TerminalUtils::Key::ARROW_DOWN;
            break;
        case TerminalUtils::Key::ARROW_LEFT:
            converted_key = TerminalUtils::Key::ARROW_LEFT;
            break;
        case TerminalUtils::Key::ARROW_RIGHT:
            converted_key = TerminalUtils::Key::ARROW_RIGHT;
            break;
        case TerminalUtils::Key::ENTER:
            converted_key = TerminalUtils::Key::ENTER;
            break;
        case TerminalUtils::Key::SPACE:
            converted_key = TerminalUtils::Key::SPACE;
            break;
        case TerminalUtils::Key::ESCAPE:
            converted_key = TerminalUtils::Key::ESCAPE;
            break;
        default:
            if (character >= 'a' && character <= 'z') {
                switch (character) {
                case 'j':
                    converted_key = TerminalUtils::Key::KEY_J;
                    break;
                case 'k':
                    converted_key = TerminalUtils::Key::KEY_K;
                    break;
                case 'h':
                    converted_key = TerminalUtils::Key::KEY_H;
                    break;
                case 'l':
                    converted_key = TerminalUtils::Key::KEY_L;
                    break;
                default:
                    converted_key = TerminalUtils::Key::NORMAL;
                    break;
                }
            } else {
                converted_key = TerminalUtils::Key::NORMAL;
            }
            break;
        }

        return TerminalUtils::KeyEvent(converted_key, character);
    }

    bool TerminalManager::key_available() { return TerminalUtils::key_available(); }

} // namespace tui
