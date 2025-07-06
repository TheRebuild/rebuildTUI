#include <navigation_tui.hpp>
#include <print>
#include <section_builder.hpp>

// for custom layout borders
#include <styles.hpp>

using namespace tui;
using namespace tui_extras;

int main() {
    auto theme_section = SectionBuilder("Theme Settings")
                             .add_items(std::vector<std::string>{"Dark Theme", "Light Theme", "High Contrast"})
                             .build();

    auto color_section =
        SectionBuilder("Color Scheme").add_items(std::vector<std::string>{"Blue", "Green", "Red", "Purple"}).build();

    auto ui_section = SectionBuilder("UI Settings").add_item("Enable Animations").add_item("Show Icons").build();

    NavigationBuilder()
        .add_sections({theme_section, color_section, ui_section})
        .on_item_toggled([](const size_t section_idx, const size_t item_idx, const bool selected) {
            if (section_idx == 0 && item_idx == 0 && selected) {
                std::println("Applying dark theme...");
            }
        })

        // Theme and styling
        // .theme_fancy()   // ✓  / ○
        // .theme_minimal() // * / nothing
        // .theme_modern()  // ● / "○
        .theme_indicators('+', '-') // Custom indicators
        .theme_prefixes("[X] ", "[ ] ") // Custom prefixes

        // Set custom layout borders
        .layout_borders(true)
        .theme_border_style(BorderStyle::ROUNDED)

        // currently placeholder
        // I'll implement in the future
        .theme_colors(true)
        .theme_accent_color("blue")

        .layout_centering(true, // horizontal layout
                          true) // vertical layout
        .layout_items_per_page(3)
        .build()
        ->run();

    return 0;
}
