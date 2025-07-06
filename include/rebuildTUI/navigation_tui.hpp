#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "section.hpp"
#include "styles.hpp"
#include "terminal_utils.hpp"

namespace tui {
    /**
     * @brief TUI interface prototype
     */
    class NavigationTUI {
    public:
        enum class NavigationState {
            SECTION_SELECTION, ///< User is selecting a section
            ITEM_SELECTION ///< User is selecting/managing items within a section
        };

        /**
         * @brief Display theme configuration
         */
        struct Theme {
            char selected_indicator = '*'; ///< Character for selected items
            char unselected_indicator = ' '; ///< Character for unselected items
            std::string selected_prefix = "✓ "; ///< Prefix for selected items
            std::string unselected_prefix = "  "; ///< Prefix for unselected items
            bool use_unicode = true; ///< Whether to use Unicode characters
            bool use_colors = true; ///< Whether to use ANSI colors
            tui_extras::BorderStyle border_style =
                tui_extras::BorderStyle::ROUNDED; ///< Border style: "rounded", "sharp", "double" and "ascii"
            std::string accent_color = "cyan"; ///< Accent color for highlights
        };

        /**
         * @brief Layout configuration
         */
        struct Layout {
            bool center_horizontally = true; ///< Center content horizontally on screen
            bool center_vertically = true; ///< Center content vertically on screen
            int max_content_width = 80; ///< Maximum width for content
            int min_content_width = 40; ///< Minimum width for content

            // TODO: implement vertical padding
            int vertical_padding = 2; ///< Padding from top/bottom when centering
            bool auto_resize_content = true; ///< Automatically resize content to fit terminal

            // TODO: implement show_borders
            bool show_borders = true; ///< Whether to show borders around content
            int items_per_page = 20; ///< Number of items to display per page
        };

        /**
         * @brief Text configuration
         */
        struct TextConfig {
            std::string section_selection_title = "Select Section";
            std::string item_selection_prefix = "Section: ";
            std::string empty_section_message = "No items in this section.";
            std::string help_text_sections = "Enter - select | q - quit | 1-9 - quick select";
            std::string help_text_items =
                "Space - toggle | Enter - select | b/Esc - back | "
                "1-9 - page";
            bool show_help_text = true; ///< Whether to show help text
            bool show_page_numbers = true; ///< Whether to show page navigation info
            bool show_counters = true; ///< Whether to show selection counters
        };

        /**
         * @brief Complete configuration structure
         */
        struct Config {
            Theme theme;
            Layout layout;
            TextConfig text;

            // Shortcuts
            std::map<char, std::string> custom_shortcuts; ///< Custom keyboard shortcuts
            bool enable_quick_select = true; ///< Enable number keys for quick selection
            bool enable_vim_keys = false; ///< Enable vim-style navigation (hjkl)
        };

        /**
         * @brief Event callback types
         */
        using SectionSelectedCallback = std::function<void(size_t section_index, const Section &section)>;
        using ItemToggledCallback = std::function<void(size_t section_index, size_t item_index, bool selected)>;
        using PageChangedCallback = std::function<void(int new_page, int total_pages)>;
        using StateChangedCallback = std::function<void(NavigationState old_state, NavigationState new_state)>;
        using ExitCallback = std::function<void(const std::vector<Section> &sections)>;
        using CustomCommandCallback = std::function<bool(char key, NavigationState state)>;

    private:
        std::vector<Section> sections_;
        NavigationState current_state_;
        size_t current_section_index_;
        size_t current_selection_index_;
        int current_page_;
        Config config_;
        bool running_;
        bool needs_redraw_;

        // previous terminal size
        int previous_width_;
        int previous_height_;

        // Event callbacks
        SectionSelectedCallback on_section_selected_;
        ItemToggledCallback on_item_toggled_;
        PageChangedCallback on_page_changed_;
        StateChangedCallback on_state_changed_;
        ExitCallback on_exit_;
        CustomCommandCallback on_custom_command_;

        // Terminal management
        std::unique_ptr<TerminalManager> terminal_manager_;

    public:
        NavigationTUI();
        explicit NavigationTUI(Config config);
        ~NavigationTUI() = default;

        NavigationTUI(const NavigationTUI &) = delete;
        NavigationTUI &operator=(const NavigationTUI &) = delete;

        NavigationTUI(NavigationTUI &&) = default;
        NavigationTUI &operator=(NavigationTUI &&) = default;

        /*
         * Section management
         */
        void add_section(const Section &section);
        void add_section(Section &&section);
        void add_sections(const std::vector<Section> &sections);
        void add_sections(std::vector<Section> &&sections);

        [[nodiscard]] size_t get_current_section_index() const;
        Section *get_section(size_t index);
        [[nodiscard]] const Section *get_section(size_t index) const;

        Section *get_section_by_name(const std::string &name);
        [[nodiscard]] const Section *get_section_by_name(const std::string &name) const;

        [[nodiscard]] size_t get_section_count() const;

        bool remove_section(size_t index);
        bool remove_section_by_name(const std::string &name);

        void clear_sections();

        /**
         * @brief Get all selections across all sections
         */
        [[nodiscard]] std::map<std::string, std::vector<std::string>> get_all_selections() const;

        /**
         * @brief Get selections for a specific section
         */
        [[nodiscard]] std::vector<std::string> get_section_selections(size_t section_index) const;

        /**
         * @brief Clear all selections across all sections
         */
        void clear_all_selections();

        /**
         * @brief Clear selections for a specific section
         */
        void clear_section_selections(size_t section_index);

        /*
         * Event callbacks
         */
        void set_section_selected_callback(SectionSelectedCallback callback);
        void set_item_toggled_callback(ItemToggledCallback callback);
        void set_page_changed_callback(PageChangedCallback callback);
        void set_state_changed_callback(StateChangedCallback callback);
        void set_exit_callback(ExitCallback callback);
        void set_custom_command_callback(CustomCommandCallback callback);

        /*
         * Navigation state
         */
        [[nodiscard]] NavigationState get_current_state() const;
        [[nodiscard]] int get_current_page() const;
        void return_to_sections();
        void enter_section(size_t section_index);
        void go_to_page(int page);
        void next_page();
        void previous_page();

        /**
         * @brief Get current selection index within current page
         */
        [[nodiscard]] size_t get_current_selection_index() const;

        /**
         * @brief Update configuration
         */
        void update_config(const Config &new_config);
        void update_theme(const Theme &new_theme);
        void update_layout(const Layout &new_layout);
        void update_text_config(const TextConfig &new_text_config);

        /**
         * @brief Get current configuration
         */
        [[nodiscard]] const Config &get_config() const;

        /*
         * Other methods
         */
        void run();
        void exit();

        /**
         * @brief Horizontal centering to text
         */
        [[nodiscard]] std::string apply_centering(const std::string &text) {
            // TODO: Implement horizontal centering logic
            if (!config_.layout.center_horizontally) { // oh fuck, oh fuck...
                return text;
            }

            auto [height, width] = TerminalUtils::get_terminal_size();
            auto padding = static_cast<int>((width - text.length()) / 2);

            if (padding < 0) {
                padding = 0;
            }

            return std::string(padding, ' ') + text;
        }

    private:
        void initialize();
        void process_events();

        void handle_input(TerminalUtils::Key key, char character);
        void draw_border(int top, int left, int width, int height) const;
        void render();

        /**
         * @brief Render section selection screen
         */
        void render_section_selection(int start_row, int left_padding, int content_width);

        /**
         * @brief Render item selection screen
         */
        void render_item_selection(int start_row, int left_padding, int content_width);

        /**
         * @brief Render header with title
         */
        void render_header(int term_width, int content_width, const std::string &title);

        /**
         * @brief Render footer with help text and page info
         */
        // void render_footer(int term_height, int left_padding, int content_width) const;
        void render_footer(int term_height, int left_padding, int content_width, const SelectableItem *item);

        /**
         * @brief Handle input in section selection mode
         */
        // void handle_section_input(TerminalUtils::Key key, char character);

        /**
         * @brief Handle input in item selection mode
         */
        void handle_item_input(TerminalUtils::Key key, char character);

        /**
         * @brief Navigation helpers
         */
        void move_selection_up();
        void move_selection_down();
        void select_current_item();
        void toggle_current_item();
        void handle_number_input(char digit);

        /**
         * @brief Pagination helpers
         */
        [[nodiscard]] int calculate_total_pages() const;
        [[nodiscard]] std::pair<size_t, size_t> get_current_page_bounds() const;
        void clamp_selection();

        /**
         * @brief Rendering helpers
         */
        // [[nodiscard]] std::vector<std::string> get_section_display_items() const;
        // [[nodiscard]] std::vector<std::string> get_current_item_display_items() const;
        [[nodiscard]] std::string format_item_with_theme(const SelectableItem &item, bool is_selected) const;
        [[nodiscard]] std::string get_page_info_string() const;

        /**
         * @brief Layout calculation
         */
        // [[nodiscard]] std::pair<int, int> calculate_content_dimensions() const;
        // std::tuple<int, int, int, int> get_content_area() const;
        // std::pair<int, int> apply_centering_offset(int row, int col) const;
        [[nodiscard]] int get_effective_content_width(int) const;
        [[nodiscard]] int get_effective_content_height() const;
        // bool should_center_horizontally() const;
        // bool should_center_vertically() const;

        /**
         * @brief State management
         */
        void change_state(NavigationState new_state);
        void refresh_display(); // TODO: implement this automatic resizing window
        void trigger_callbacks(); // TODO: ???

        /**
         * @brief Utility methods
         */
        void validate_indices();
        // not impl
        // [[nodiscard]] std::string apply_theme_formatting(const std::string &text, const std::string &type) const;

        struct FormattedText {
            std::string content;
            int line_count;
        };
        /*
         * @brief Center a string within a given width
         */
        [[nodiscard]] FormattedText center_string(const std::string &text, int width) const;
    };

    /**
     * @brief Builder class for easy NavigationTUI configuration
     */
    class NavigationBuilder {
        NavigationTUI::Config config_;
        std::vector<Section> sections_;

        // Callbacks
        NavigationTUI::SectionSelectedCallback section_selected_callback_;
        NavigationTUI::ItemToggledCallback item_toggled_callback_;
        NavigationTUI::PageChangedCallback page_changed_callback_;
        NavigationTUI::StateChangedCallback state_changed_callback_;
        NavigationTUI::ExitCallback exit_callback_;
        NavigationTUI::CustomCommandCallback custom_command_callback_;

    public:
        /*
         * TODO: may I move theme_* to styles.hpp (or extras.hpp)?
         */


        /**
         * @brief Theme configuration methods
         */
        NavigationBuilder &theme_indicators(char selected, char unselected);
        NavigationBuilder &theme_prefixes(const std::string &selected, const std::string &unselected);
        NavigationBuilder &theme_unicode(bool enable);
        NavigationBuilder &theme_colors(bool enable);
        NavigationBuilder &theme_border_style(const tui_extras::BorderStyle &style);
        NavigationBuilder &theme_accent_color(const std::string &color);

        /**
         * @brief Layout configuration methods
         */
        NavigationBuilder &layout_centering(bool horizontal, bool vertical);
        NavigationBuilder &layout_content_width(int min_width, int max_width);
        NavigationBuilder &layout_padding(int vertical_padding);
        NavigationBuilder &layout_auto_resize(bool enable);
        NavigationBuilder &layout_borders(bool show);
        NavigationBuilder &layout_items_per_page(int count);

        /**
         * @brief Text configuration methods
         */
        NavigationBuilder &text_titles(const std::string &section_title, const std::string &item_prefix);
        NavigationBuilder &text_messages(const std::string &empty_message);
        NavigationBuilder &text_help(const std::string &section_help, const std::string &item_help);
        NavigationBuilder &text_show_help(bool show);
        NavigationBuilder &text_show_pages(bool show);
        NavigationBuilder &text_show_counters(bool show);

        /**
         * @brief Keyboard configuration methods
         */
        NavigationBuilder &keys_quick_select(bool enable);
        NavigationBuilder &keys_vim_style(bool enable);
        NavigationBuilder &keys_custom_shortcut(char key, const std::string &description);

        /**
         * @brief Section management methods
         */
        NavigationBuilder &add_section(const Section &section);
        NavigationBuilder &add_section(Section &&section);
        NavigationBuilder &add_sections(const std::vector<Section> &sections);

        /**
         * @brief Callback configuration methods
         */
        NavigationBuilder &on_section_selected(NavigationTUI::SectionSelectedCallback callback);
        NavigationBuilder &on_item_toggled(NavigationTUI::ItemToggledCallback callback);
        NavigationBuilder &on_page_changed(NavigationTUI::PageChangedCallback callback);
        NavigationBuilder &on_state_changed(NavigationTUI::StateChangedCallback callback);
        NavigationBuilder &on_exit(NavigationTUI::ExitCallback callback);
        NavigationBuilder &on_custom_command(NavigationTUI::CustomCommandCallback callback);

        /**
         * @brief Pre-configured themes
         */
        NavigationBuilder &theme_minimal();
        NavigationBuilder &theme_fancy();
        NavigationBuilder &theme_retro();
        NavigationBuilder &theme_modern();

        /**
         * @brief Pre-configured layouts
         */
        NavigationBuilder &layout_compact();
        NavigationBuilder &layout_comfortable();
        NavigationBuilder &layout_fullscreen();
        NavigationBuilder &layout_centered();

        std::unique_ptr<NavigationTUI> build();

        [[nodiscard]] const NavigationTUI::Config &get_config() const;

        NavigationBuilder &reset();
    };

} // namespace tui
