#include "navigation_tui.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>
#include "terminal_utils.hpp"

namespace tui {
    NavigationTUI::NavigationTUI() :
        current_state_(NavigationState::SECTION_SELECTION), current_section_index_(0), current_selection_index_(0),
        current_page_(0), running_(false), needs_redraw_(true) {
        config_ = Config{};
        terminal_manager_ = std::make_unique<TerminalManager>();
    }

    NavigationTUI::NavigationTUI(Config config) :
        current_state_(NavigationState::SECTION_SELECTION), current_section_index_(0), current_selection_index_(0),
        current_page_(0), config_(std::move(config)), running_(false), needs_redraw_(true) {
        terminal_manager_ = std::make_unique<TerminalManager>();
    }

    void NavigationTUI::add_section(const Section &section) { sections_.push_back(section); }

    void NavigationTUI::add_section(Section &&section) { sections_.push_back(std::move(section)); }

    void NavigationTUI::add_sections(const std::vector<Section> &sections) {
        sections_.insert(sections_.end(), sections.begin(), sections.end());
    }

    void NavigationTUI::add_sections(std::vector<Section> &&sections) {
        sections_.insert(sections_.end(), std::make_move_iterator(sections.begin()),
                         std::make_move_iterator(sections.end()));
    }

    Section *NavigationTUI::get_section(size_t index) {
        return (index < sections_.size()) ? &sections_[index] : nullptr;
    }

    const Section *NavigationTUI::get_section(size_t index) const {
        return (index < sections_.size()) ? &sections_[index] : nullptr;
    }

    Section *NavigationTUI::get_section_by_name(const std::string &name) {
        const auto it =
            std::ranges::find_if(sections_, [&name](const Section &section) { return section.name == name; });
        return (it != sections_.end()) ? &(*it) : nullptr;
    }

    const Section *NavigationTUI::get_section_by_name(const std::string &name) const {
        const auto it =
            std::ranges::find_if(sections_, [&name](const Section &section) { return section.name == name; });
        return (it != sections_.end()) ? &(*it) : nullptr;
    }

    size_t NavigationTUI::get_section_count() const { return sections_.size(); }

    bool NavigationTUI::remove_section(const size_t index) {
        if (index < sections_.size()) {
            using diff_t = typename decltype(sections_)::difference_type;

            sections_.erase(sections_.begin() + static_cast<diff_t>(index));
            validate_indices();
            return true;
        }
        return false;
    }

    bool NavigationTUI::remove_section_by_name(const std::string &name) {
        const auto it =
            std::ranges::find_if(sections_, [&name](const Section &section) { return section.name == name; });
        if (it != sections_.end()) {
            sections_.erase(it);
            validate_indices();
            return true;
        }
        return false;
    }

    void NavigationTUI::clear_sections() {
        sections_.clear();
        current_section_index_ = 0;
        current_selection_index_ = 0;
        current_page_ = 0;
        current_state_ = NavigationState::SECTION_SELECTION;
    }

    void NavigationTUI::set_section_selected_callback(SectionSelectedCallback callback) {
        on_section_selected_ = std::move(callback);
    }

    void NavigationTUI::set_item_toggled_callback(ItemToggledCallback callback) {
        on_item_toggled_ = std::move(callback);
    }

    void NavigationTUI::set_page_changed_callback(PageChangedCallback callback) {
        on_page_changed_ = std::move(callback);
    }

    void NavigationTUI::set_state_changed_callback(StateChangedCallback callback) {
        on_state_changed_ = std::move(callback);
    }

    void NavigationTUI::set_exit_callback(ExitCallback callback) { on_exit_ = std::move(callback); }

    void NavigationTUI::set_custom_command_callback(CustomCommandCallback callback) {
        on_custom_command_ = std::move(callback);
    }

    void NavigationTUI::run() {
        if (sections_.empty()) {
            std::cout << "No sections available. Please add sections before running." << std::endl;
            return;
        }

        initialize();
        running_ = true;

        while (running_) {
            if (needs_redraw_) {
                render();
                needs_redraw_ = false;
            }
            process_events();
        }

        terminal_manager_->restore_terminal();

        if (on_exit_) {
            on_exit_(sections_);
        }
    }

    void NavigationTUI::exit() { running_ = false; }

    NavigationTUI::NavigationState NavigationTUI::get_current_state() const { return current_state_; }

    size_t NavigationTUI::get_current_section_index() const { return current_section_index_; }

    int NavigationTUI::get_current_page() const { return current_page_; }

    size_t NavigationTUI::get_current_selection_index() const { return current_selection_index_; }

    void NavigationTUI::return_to_sections() {
        if (current_state_ != NavigationState::SECTION_SELECTION) {
            change_state(NavigationState::SECTION_SELECTION);
            current_selection_index_ = current_section_index_;
            current_page_ = 0;
            needs_redraw_ = true;
        }
    }

    void NavigationTUI::enter_section(size_t section_index) {
        if (section_index < sections_.size()) {
            current_section_index_ = section_index;
            current_selection_index_ = 0;
            current_page_ = 0;
            change_state(NavigationState::ITEM_SELECTION);

            auto &section = sections_[section_index];
            section.trigger_enter();

            if (on_section_selected_) {
                on_section_selected_(section_index, section);
            }

            needs_redraw_ = true;
        }
    }

    void NavigationTUI::go_to_page(int page) {
        int total_pages = calculate_total_pages();
        if (page >= 0 && page < total_pages && page != current_page_) {
            current_page_ = page;
            current_selection_index_ = 0;

            if (on_page_changed_) {
                on_page_changed_(page, total_pages);
            }

            needs_redraw_ = true;
        }
    }

    void NavigationTUI::next_page() { go_to_page(current_page_ + 1); }

    void NavigationTUI::previous_page() { go_to_page(current_page_ - 1); }

    std::map<std::string, std::vector<std::string>> NavigationTUI::get_all_selections() const {
        std::map<std::string, std::vector<std::string>> selections;

        for (const auto &section : sections_) {
            auto selected_items = section.get_selected_names();
            if (!selected_items.empty()) {
                selections[section.name] = selected_items;
            }
        }

        return selections;
    }

    std::vector<std::string> NavigationTUI::get_section_selections(size_t section_index) const {
        if (section_index < sections_.size()) {
            return sections_[section_index].get_selected_names();
        }
        return {};
    }

    void NavigationTUI::clear_all_selections() {
        for (auto &section : sections_) {
            section.clear_selections();
        }
        needs_redraw_ = true;
    }

    void NavigationTUI::clear_section_selections(size_t section_index) {
        if (section_index < sections_.size()) {
            sections_[section_index].clear_selections();
            needs_redraw_ = true;
        }
    }

    void NavigationTUI::update_config(const Config &new_config) {
        config_ = new_config;
        needs_redraw_ = true;
    }

    void NavigationTUI::update_theme(const Theme &new_theme) {
        config_.theme = new_theme;
        needs_redraw_ = true;
    }

    void NavigationTUI::update_layout(const Layout &new_layout) {
        config_.layout = new_layout;
        needs_redraw_ = true;
    }

    void NavigationTUI::update_text_config(const TextConfig &new_text_config) {
        config_.text = new_text_config;
        needs_redraw_ = true;
    }

    const NavigationTUI::Config &NavigationTUI::get_config() const { return config_; }

    void NavigationTUI::initialize() {
        terminal_manager_->setup_terminal();
        validate_indices();
        needs_redraw_ = true;
    }

    void NavigationTUI::process_events() {
        if (auto key_event = TerminalManager::get_key_input(); key_event.has_value()) {
            handle_input(key_event->key, key_event->character);
        }
    }

    void NavigationTUI::handle_input(const TerminalUtils::Key key, const char character) {
        // Handle global commands first
        if (std::tolower(character) == 'q') {
            exit();
            return;
        }

        // Custom keybindings
        if (on_custom_command_ && on_custom_command_(character, current_state_)) {
            return;
        }

        // Handle state-specific input
        handle_item_input(key, character);
    }

    void NavigationTUI::handle_item_input(const TerminalUtils::Key key, const char character) {
        switch (key) {
        case TerminalUtils::Key::ESCAPE:
            return_to_sections();
            break;

        case TerminalUtils::Key::ARROW_UP:
            move_selection_up();
            break;

        case TerminalUtils::Key::ARROW_DOWN:
            move_selection_down();
            break;

        case TerminalUtils::Key::ARROW_LEFT:
            previous_page();
            break;

        case TerminalUtils::Key::ARROW_RIGHT:
            next_page();
            break;

        case TerminalUtils::Key::SPACE:
            toggle_current_item();
            break;

        case TerminalUtils::Key::ENTER:
            if (current_state_ == NavigationState::ITEM_SELECTION) {
                return_to_sections();
            } else if (current_state_ == NavigationState::SECTION_SELECTION) {
                select_current_item();
            }
            break;

        case TerminalUtils::Key::NORMAL:
            if (current_state_ == NavigationState::ITEM_SELECTION) {
                if (character == 'b') {
                    return_to_sections();
                } else if (character == 'a') {
                    if (current_section_index_ < sections_.size()) {
                        sections_[current_section_index_].select_all();
                        needs_redraw_ = true;
                    }
                } else if (character == 'n') {
                    if (current_section_index_ < sections_.size()) {
                        sections_[current_section_index_].clear_selections();
                        needs_redraw_ = true;
                    }
                }
            } else if (current_state_ == NavigationState::SECTION_SELECTION && std::isdigit(character)) {
                handle_number_input(character);
            }
            break;

        default:
            if (config_.enable_vim_keys) {
                if (character == 'j') {
                    move_selection_down();
                } else if (character == 'k') {
                    move_selection_up();
                } else if (character == 'h') {
                    return_to_sections();
                }
            }
            break;
        }
    }

    void NavigationTUI::move_selection_up() {
        if (current_state_ == NavigationState::SECTION_SELECTION) {
            if (current_selection_index_ > 0) {
                current_selection_index_--;
            }
        } else {
            if (current_selection_index_ > 0) {
                current_selection_index_--;
            } else {
                if (current_page_ > 0) {
                    go_to_page(current_page_ - 1);
                    auto bounds = get_current_page_bounds();
                    current_selection_index_ = bounds.second - bounds.first - 1;
                }
            }
        }
        needs_redraw_ = true;
    }

    // void NavigationTUI::move_selection_down() {
    //   size_t max_items = 0;

    //   if (current_state_ == NavigationState::SECTION_SELECTION) {
    //     max_items = sections_.size();
    //   } else {
    //     if (current_section_index_ < sections_.size()) {
    //       auto bounds = get_current_page_bounds();
    //       max_items = bounds.second - bounds.first;
    //     }
    //   }

    //   if (current_selection_index_ + 1 < max_items) {
    //     current_selection_index_++;
    //     needs_redraw_ = true;
    //   }
    // }

    void NavigationTUI::move_selection_down() {
        if (current_state_ == NavigationState::SECTION_SELECTION) {
            if (current_selection_index_ < sections_.size() - 1) {
                current_selection_index_++;
            }
        } else {
            auto [first, second] = get_current_page_bounds();

            if (const size_t items_on_page = second - first; current_selection_index_ < items_on_page - 1) {
                current_selection_index_++;
            } else {
                int total_pages = calculate_total_pages();
                if (current_page_ < total_pages - 1) {
                    go_to_page(current_page_ + 1);
                    current_selection_index_ = 0;
                }
            }
        }
        needs_redraw_ = true;
    }

    void NavigationTUI::select_current_item() {
        if (current_state_ == NavigationState::SECTION_SELECTION && current_selection_index_ < sections_.size()) {
            enter_section(current_section_index_);
        } else {
            toggle_current_item();
        }
    }

    void NavigationTUI::toggle_current_item() {
        if (current_state_ == NavigationState::ITEM_SELECTION && current_section_index_ < sections_.size()) {
            auto [start, end] = get_current_page_bounds();

            if (size_t global_index = start + current_selection_index_;
                sections_[current_section_index_].toggle_item(global_index)) {
                if (on_item_toggled_) {
                    if (const auto *item = sections_[current_section_index_].get_item(global_index)) {
                        on_item_toggled_(current_section_index_, global_index, item->selected);
                    }
                }
                needs_redraw_ = true;
            }
        }
    }

    void NavigationTUI::handle_number_input(const char digit) {
        const int number = digit - '0';

        if (current_state_ == NavigationState::SECTION_SELECTION) {
            if (number > 0 && number <= static_cast<int>(sections_.size())) {
                enter_section(number - 1);
            }
        } else {
            // Try to go to page
            if (number > 0) {
                go_to_page(number - 1);
            }
        }
    }

    int NavigationTUI::get_effective_content_width(const int term_width) const {
        // auto [height, width] = TerminalUtils::get_terminal_size();
        int content_width = term_width - 4;

        content_width = (config_.layout.auto_resize_content)
            ? std::clamp(content_width, config_.layout.min_content_width, config_.layout.max_content_width)
            : config_.layout.max_content_width;

        return content_width;
    }

    int NavigationTUI::get_effective_content_height() const {
        auto content_height = 0;

        if (current_state_ == NavigationState::SECTION_SELECTION) {
            content_height = 3 + static_cast<int>(sections_.size()) + 2;
        } else if (current_section_index_ < sections_.size()) {
            [[maybe_unused]] const auto &section = sections_[current_section_index_];

            auto [first, second] = get_current_page_bounds();
            content_height = 3 + static_cast<int>((second - first)) + 2;
        }

        return content_height;
    }

    void NavigationTUI::render() {
        TerminalManager::clear_screen();

        auto [term_height, term_width] = TerminalManager::get_terminal_size();
        const int content_width = get_effective_content_width(term_width);
        auto left_padding = 0;

        if (config_.layout.center_horizontally) {
            left_padding = (term_width - content_width) / 2;
        } else {
            left_padding = 1;
        }

        auto start_row = 1;
        if (config_.layout.center_vertically) {
            const int content_height = get_effective_content_height();
            start_row = std::max(1, (term_height - content_height) / 2);
        }

        if (current_state_ == NavigationState::SECTION_SELECTION) {
            render_section_selection(start_row, left_padding, content_width);
        } else {
            render_item_selection(start_row, left_padding, content_width);
        }

        const SelectableItem *current_item = nullptr;
        if (current_state_ == NavigationState::ITEM_SELECTION && current_section_index_ < sections_.size()) {
            const auto &section = sections_[current_section_index_];

            if (auto [first, second] = get_current_page_bounds(); current_selection_index_ < (second - first)) {
                const size_t global_index = first + current_selection_index_;
                current_item = section.get_item(global_index);
            }
        }

        render_footer(term_height, left_padding, content_width, current_item);
        TerminalManager::flush_output();
    }

    void NavigationTUI::render_header(int /*term_width*/, const int content_width, const std::string &title) {
        const std::string centered_title = center_string(title, content_width).content;
        const std::string separator = center_string(std::string(title.length(), '='), content_width).content;

        std::cout << centered_title << "\n";
        std::cout << separator << "\n\n";
    }

    void NavigationTUI::render_section_selection(const int start_row, const int left_padding, const int content_width) {
        // Header
        TerminalUtils::move_cursor(start_row, left_padding);
        std::cout << center_string(config_.text.section_selection_title, content_width).content;

        TerminalUtils::move_cursor(start_row + 1, left_padding);
        std::cout
            << center_string(std::string(config_.text.section_selection_title.length(), '='), content_width).content;

        // Sections
        for (size_t i = 0; i < sections_.size(); ++i) {
            TerminalUtils::move_cursor(static_cast<int>(start_row + 3 + i), left_padding);

            std::string display_text = std::to_string(i + 1) + ". " + sections_[i].name;

            if (config_.text.show_counters) {
                const size_t selected_count = sections_[i].get_selected_count();
                if (const size_t total_count = sections_[i].size(); total_count > 0) {
                    display_text += " (" + std::to_string(selected_count) + "/" + std::to_string(total_count) + ")";
                }
            }

            std::string prefix = (i == current_selection_index_) ? "> " : "  ";
            std::cout << center_string(prefix + display_text, content_width).content;
        }
    }

    void NavigationTUI::render_item_selection(const int start_row, const int left_padding, const int content_width) {
        if (current_section_index_ >= sections_.size()) {
            return;
        }

        const auto &section = sections_[current_section_index_];

        // Header
        const std::string title = config_.text.item_selection_prefix + section.name;
        TerminalUtils::move_cursor(start_row, left_padding);
        std::cout << center_string(title, content_width).content;

        TerminalUtils::move_cursor(start_row + 1, left_padding);
        std::cout << center_string(std::string(title.length(), '='), content_width).content;

        // Items
        if (section.empty()) {
            TerminalUtils::move_cursor(start_row + 3, left_padding);
            std::cout << center_string(config_.text.empty_section_message, content_width).content;
        } else {
            auto [first, second] = get_current_page_bounds();

            for (size_t i = first; i < second; ++i) {
                TerminalUtils::move_cursor(static_cast<int>(start_row + 3 + (i - first)), left_padding);

                if (const auto *item = section.get_item(i)) {
                    std::string display_text = format_item_with_theme(*item, (i - first) == current_selection_index_);

                    std::cout << center_string(display_text, content_width).content;
                }
            }
        }
    }

    void NavigationTUI::render_footer(const int term_height, const int left_padding, const int content_width,
                                      const SelectableItem *item = nullptr) {
        // footer (description)
        // TODO: description rendering for main sections will be added in a future
        auto description = (item) ? (item->description.empty() ? "No description provided" : item->description)
                                  : std::string("Description (placeholder)");

        auto [content, line_count] = center_string(description, content_width);

        const int description_anchor_row = term_height - 4;
        const int description_start_row = description_anchor_row - (line_count - 1);

        TerminalUtils::move_cursor(description_start_row, left_padding);

        std::istringstream stream(content);
        std::string line;
        int current_row = description_start_row;

        while (std::getline(stream, line)) {
            TerminalUtils::move_cursor(current_row, left_padding);
            std::cout << line;
            current_row++;
        }

        // footer (help text)
        std::string help_text;
        if (current_state_ == NavigationState::SECTION_SELECTION) {
            help_text = config_.text.help_text_sections;
        } else {
            help_text = config_.text.help_text_items;
            if (config_.text.show_page_numbers) {
                help_text += " | " + get_page_info_string();
            }
        }

        auto [help_content, help_line_count] = center_string(help_text, content_width);

        const int help_anchor_row = term_height - 2;
        const int help_start_row = help_anchor_row - (help_line_count - 1);

        TerminalUtils::move_cursor(help_start_row, left_padding);

        current_row = help_start_row;
        std::istringstream help_stream(help_content);
        while (std::getline(help_stream, line)) {
            TerminalUtils::move_cursor(current_row, left_padding);
            std::cout << line;
            current_row++;
        }
    }

    std::string NavigationTUI::format_item_with_theme(const SelectableItem &item, bool is_highlighted) const {
        const std::string prefix = item.selected ? config_.theme.selected_prefix : config_.theme.unselected_prefix;
        // TODO: maybe add configuration for highlighted prefix?
        std::string display_text = std::format("{}{} {}", (is_highlighted) ? "> " : " ", prefix, item.name);

        return display_text;
    }

    std::string NavigationTUI::get_page_info_string() const {
        int total_pages = calculate_total_pages();
        return std::format("Page {} of {}", current_page_ + 1, total_pages);
    }

    int NavigationTUI::calculate_total_pages() const {
        if (current_state_ == NavigationState::SECTION_SELECTION) {
            return 1; // Sections don't paginate for now
        }

        if (current_section_index_ < sections_.size()) {
            const size_t item_count = sections_[current_section_index_].size();
            if (item_count == 0) {
                return 1;
            }
            return static_cast<int>((item_count + config_.layout.items_per_page - 1) / config_.layout.items_per_page);
        }

        return 1;
    }

    std::pair<size_t, size_t> NavigationTUI::get_current_page_bounds() const {
        if (current_state_ != NavigationState::ITEM_SELECTION || current_section_index_ >= sections_.size()) {
            return {0, 0};
        }

        size_t item_count = sections_[current_section_index_].size();
        size_t start = current_page_ * config_.layout.items_per_page;
        size_t end = std::min(start + config_.layout.items_per_page, item_count);

        return {start, end};
    }

    void NavigationTUI::clamp_selection() {
        if (current_state_ == NavigationState::SECTION_SELECTION && current_section_index_ >= sections_.size()) {
            current_selection_index_ = !sections_.empty() ? sections_.size() - 1 : 0;
        } else {
            auto [first, second] = get_current_page_bounds();
            if (const size_t max_selection = second - first; current_selection_index_ >= max_selection) {
                current_selection_index_ = max_selection > 0 ? max_selection - 1 : 0;
            }
        }
    }

    void NavigationTUI::change_state(NavigationState new_state) {
        if (current_state_ != new_state) {
            NavigationState old_state = current_state_;
            current_state_ = new_state;

            if (on_state_changed_) {
                on_state_changed_(old_state, new_state);
            }
        }
    }

    void NavigationTUI::validate_indices() {
        if (current_section_index_ >= sections_.size()) {
            current_section_index_ = !sections_.empty() ? sections_.size() - 1 : 0;
        }
        clamp_selection();
    }

    NavigationTUI::FormattedText NavigationTUI::center_string(const std::string &text, const int width) const {
        if (!config_.layout.center_horizontally) {
            return {text, 1};
        }

        std::string result_content;
        auto total_lines = 0;
        std::string current_line;

        for (const char c : text) {
            if (c == '\n') {
                if (!current_line.empty()) {
                    int padding = (width - static_cast<int>(current_line.length())) / 2;
                    if (padding < 0) {
                        padding = 0;
                    }
                    result_content += std::string(padding, ' ') + current_line + '\n';
                    total_lines++;
                    current_line.clear();
                } else {
                    result_content += '\n';
                    total_lines++;
                }
            } else {
                if (static_cast<int>(current_line.length()) >= width) {
                    if (const size_t last_space = current_line.find_last_of(' ');
                        last_space != std::string::npos && last_space > 0) {
                        const std::string next_line = current_line.substr(last_space + 1);
                        current_line = current_line.substr(0, last_space);

                        int padding = (width - static_cast<int>(current_line.length())) / 2;
                        if (padding < 0) {
                            padding = 0;
                        }
                        result_content += std::string(padding, ' ') + current_line + '\n';
                        total_lines++;

                        current_line = next_line;
                    } else {
                        int padding = (width - static_cast<int>(current_line.length())) / 2;
                        if (padding < 0) {
                            padding = 0;
                        }
                        result_content += std::string(padding, ' ') + current_line + '\n';
                        total_lines++;
                        current_line.clear();
                    }
                }
                current_line += c;
            }
        }

        if (!current_line.empty()) {
            int padding = (width - static_cast<int>(current_line.length())) / 2;
            if (padding < 0) {
                padding = 0;
            }
            result_content += std::string(padding, ' ') + current_line;
            total_lines++;
        }

        return {result_content, total_lines};
    }

    NavigationBuilder &NavigationBuilder::theme_indicators(const char selected, const char unselected) {
        config_.theme.selected_indicator = selected;
        config_.theme.unselected_indicator = unselected;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_prefixes(const std::string &selected, const std::string &unselected) {
        config_.theme.selected_prefix = selected;
        config_.theme.unselected_prefix = unselected;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_unicode(bool enable) {
        config_.theme.use_unicode = enable;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_colors(bool enable) {
        config_.theme.use_colors = enable;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_border_style(const std::string &style) {
        config_.theme.border_style = style;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_accent_color(const std::string &color) {
        config_.theme.accent_color = color;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_centering(const bool horizontal, const bool vertical) {
        config_.layout.center_horizontally = horizontal;
        config_.layout.center_vertically = vertical;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_content_width(const int min_width, const int max_width) {
        config_.layout.min_content_width = min_width;
        config_.layout.max_content_width = max_width;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_padding(const int vertical_padding) {
        config_.layout.vertical_padding = vertical_padding;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_auto_resize(const bool enable) {
        config_.layout.auto_resize_content = enable;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_borders(const bool show) {
        config_.layout.show_borders = show;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_items_per_page(const int count) {
        config_.layout.items_per_page = count;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::text_titles(const std::string &section_title,
                                                      const std::string &item_prefix) {
        config_.text.section_selection_title = section_title;
        config_.text.item_selection_prefix = item_prefix;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::text_messages(const std::string &empty_message) {
        config_.text.empty_section_message = empty_message;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::text_help(const std::string &section_help, const std::string &item_help) {
        config_.text.help_text_sections = section_help;
        config_.text.help_text_items = item_help;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::text_show_help(const bool show) {
        config_.text.show_help_text = show;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::text_show_pages(const bool show) {
        config_.text.show_page_numbers = show;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::text_show_counters(const bool show) {
        config_.text.show_counters = show;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::keys_quick_select(const bool enable) {
        config_.enable_quick_select = enable;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::keys_vim_style(const bool enable) {
        config_.enable_vim_keys = enable;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::keys_custom_shortcut(const char key, const std::string &description) {
        config_.custom_shortcuts[key] = description;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::add_section(const Section &section) {
        sections_.push_back(section);
        return *this;
    }

    NavigationBuilder &NavigationBuilder::add_section(Section &&section) {
        sections_.push_back(std::move(section));
        return *this;
    }

    NavigationBuilder &NavigationBuilder::add_sections(const std::vector<Section> &sections) {
        sections_.insert(sections_.end(), sections.begin(), sections.end());
        return *this;
    }

    NavigationBuilder &NavigationBuilder::on_section_selected(NavigationTUI::SectionSelectedCallback callback) {
        section_selected_callback_ = std::move(callback);
        return *this;
    }

    NavigationBuilder &NavigationBuilder::on_item_toggled(NavigationTUI::ItemToggledCallback callback) {
        item_toggled_callback_ = std::move(callback);
        return *this;
    }

    NavigationBuilder &NavigationBuilder::on_page_changed(NavigationTUI::PageChangedCallback callback) {
        page_changed_callback_ = std::move(callback);
        return *this;
    }

    NavigationBuilder &NavigationBuilder::on_state_changed(NavigationTUI::StateChangedCallback callback) {
        state_changed_callback_ = std::move(callback);
        return *this;
    }

    NavigationBuilder &NavigationBuilder::on_exit(NavigationTUI::ExitCallback callback) {
        exit_callback_ = std::move(callback);
        return *this;
    }

    NavigationBuilder &NavigationBuilder::on_custom_command(NavigationTUI::CustomCommandCallback callback) {
        custom_command_callback_ = std::move(callback);
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_minimal() {
        config_.theme.use_unicode = false;
        config_.theme.use_colors = false;
        config_.theme.selected_prefix = "* ";
        config_.theme.unselected_prefix = "  ";
        config_.theme.border_style = "simple";
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_fancy() {
        config_.theme.use_unicode = true;
        config_.theme.use_colors = true;
        config_.theme.selected_prefix = "✓ ";
        config_.theme.unselected_prefix = "○ ";
        config_.theme.border_style = "rounded";
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_retro() {
        config_.theme.use_unicode = false;
        config_.theme.use_colors = false;
        config_.theme.selected_prefix = "[X] ";
        config_.theme.unselected_prefix = "[ ] ";
        config_.theme.border_style = "double";
        return *this;
    }

    NavigationBuilder &NavigationBuilder::theme_modern() {
        config_.theme.use_unicode = true;
        config_.theme.use_colors = true;
        config_.theme.selected_prefix = "● ";
        config_.theme.unselected_prefix = "○ ";
        config_.theme.border_style = "rounded";
        config_.theme.accent_color = "blue";
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_compact() {
        config_.layout.items_per_page = 25;
        config_.layout.show_borders = false;
        config_.layout.center_horizontally = false;
        config_.layout.center_vertically = false;
        config_.layout.min_content_width = 40;
        config_.layout.max_content_width = 60;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_comfortable() {
        config_.layout.items_per_page = 15;
        config_.layout.show_borders = true;
        config_.layout.center_horizontally = false;
        config_.layout.center_vertically = false;
        config_.layout.min_content_width = 60;
        config_.layout.max_content_width = 100;
        config_.layout.vertical_padding = 2;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_fullscreen() {
        config_.layout.items_per_page = 30;
        config_.layout.show_borders = true;
        config_.layout.center_horizontally = false;
        config_.layout.center_vertically = false;
        config_.layout.auto_resize_content = true;
        config_.layout.min_content_width = 80;
        config_.layout.max_content_width = 120;
        return *this;
    }

    NavigationBuilder &NavigationBuilder::layout_centered() {
        config_.layout.center_horizontally = true;
        config_.layout.center_vertically = false;
        config_.layout.items_per_page = 20;
        config_.layout.show_borders = true;
        config_.layout.min_content_width = 60;
        config_.layout.max_content_width = 80;
        config_.layout.vertical_padding = 3;
        return *this;
    }

    std::unique_ptr<NavigationTUI> NavigationBuilder::build() {
        auto tui = std::make_unique<NavigationTUI>(config_);

        for (auto &section : sections_) {
            tui->add_section(std::move(section));
        }

        if (section_selected_callback_) {
            tui->set_section_selected_callback(section_selected_callback_);
        }
        if (item_toggled_callback_) {
            tui->set_item_toggled_callback(item_toggled_callback_);
        }
        if (page_changed_callback_) {
            tui->set_page_changed_callback(page_changed_callback_);
        }
        if (state_changed_callback_) {
            tui->set_state_changed_callback(state_changed_callback_);
        }
        if (exit_callback_) {
            std::cin.clear();
            tui->set_exit_callback(exit_callback_);
        }
        if (custom_command_callback_) {
            tui->set_custom_command_callback(custom_command_callback_);
        }

        return tui;
    }

    const NavigationTUI::Config &NavigationBuilder::get_config() const { return config_; }

    NavigationBuilder &NavigationBuilder::reset() {
        config_ = NavigationTUI::Config{};
        sections_.clear();

        section_selected_callback_ = nullptr;
        item_toggled_callback_ = nullptr;
        page_changed_callback_ = nullptr;
        state_changed_callback_ = nullptr;
        exit_callback_ = nullptr;
        custom_command_callback_ = nullptr;

        return *this;
    }
} // namespace tui
