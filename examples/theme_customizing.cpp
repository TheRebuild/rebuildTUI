#include <navigation_tui.hpp>
#include <section_builder.hpp>

using namespace tui;

int main() {
  auto theme_section = SectionBuilder("Theme Settings")
                           .add_items(std::vector<std::string>{
                               "Dark Theme", "Light Theme", "High Contrast"})
                           .build();

  auto color_section =
      SectionBuilder("Color Scheme")
          .add_items(std::vector<std::string>{"Blue", "Green", "Red", "Purple"})
          .build();

  NavigationBuilder config;

  auto ui_section = SectionBuilder("UI Settings")
                        .add_item("Enable Animations")
                        .add_item("Show Icons")
                        .build();

  config.add_sections({theme_section, color_section, ui_section})
      .on_item_toggled([](size_t section_idx, size_t item_idx, bool selected) {
        if (section_idx == 0 && item_idx == 0 && selected)
          std::cout << "Applying dark theme...\n";
      })

      // Theme and styling
      // .theme_fancy()   // ✓  / ○
      // .theme_minimal() // * / nothing
      // .theme_modern()  // ● / "○
      .theme_indicators('+', '-')     // Custom indicators
      .theme_prefixes("[X] ", "[ ] ") // Custom prefixes

      // currently placeholder
      // i'll implement in the future
      .theme_colors(true)
      .theme_accent_color("blue")

      /*
       * It seems that layout_centering works only with vertical layout?
       *
       * Will fix this in the future
       */
      .layout_centering(true, // horizontal layout
                        true) // vertical layout
      .layout_items_per_page(3)
      .build()
      ->run();

  return 0;
}
