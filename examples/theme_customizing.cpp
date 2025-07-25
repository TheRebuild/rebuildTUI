#include <navigation_tui.hpp>
#include <print>
#include <section_builder.hpp>

// for custom layout borders
#include <styles.hpp>

using namespace tui;
// using namespace tui;

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
        .theme_indicators('+', '-')     // Custom indicators
        .theme_prefixes("[X] ", "[ ] ") // Custom prefixes

        // Set custom layout borders
        .layout_borders(true)
        .theme_border_style(extras::BorderStyle::ROUNDED)

        // currently placeholder
        // I'll implement in the future
        .theme_colors(true)
        .theme_accent_color(extras::AccentColor::CYAN)

        .layout_centering(true, // horizontal layout
                          true) // vertical layout
        .layout_items_per_page(3)

        // show 2 sections per page on main menu
        .layout_sections_per_page(2)
        // and paginate it
        .paginate_sections(true)
        .theme_gradient_support(true)    // Enable gradient support
        .theme_gradient_randomize(false) // Randomize gradient
        /*
         *  Available presets
         *
         *  NOTE: extras::v_styles => std::vector<std::tuple<uint8_t, uint8_t, uint8_t>>
         *
         *  GradientPreset::RAINBOW
         *  GradientPreset::BLUE_TO_PURPLE
         *  GradientPreset::RED_TO_GREEN
         *  GradientPreset::FIRE                        - Red to yellow
         *  GradientPreset::FOREST                      - Green to yellow-green
         *  GradientPreset::OCEAN                       - Blue to turquoise
         *  GradientPreset::WARM_TO_COLD                - From orange to cyan
         *  GradientPreset::SUNSET                      - Red -> orange -> violet
         *  GradientPreset::CUSTOM(v_styles{r, g, b})   - custom colors (vector of rgb styles)
         *  GradientPreset::NONE                        - None (by default)
         */
        .theme_gradient_preset(extras::GradientPreset::CUSTOM(extras::v_styles{{255, 0, 0}, // red
                                                                               {0, 255, 0}, // green
                                                                               {0, 0, 255}} // blue
                                                              ))

        // layout padding (you can enable if you want)
        // .layout_padding(3)

        .build()
        ->run();

    return 0;
}
