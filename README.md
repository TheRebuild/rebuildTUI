# rebuildTui (TUI for Rebuild Tool) [WIP]

Simple, cross-platform (should be) text-based user interface (TUI) library. Initially created for Rebuild Tool, but also suitable for other applications.

- [rebuildTui (TUI for Rebuild Tool) \[WIP\]](#rebuildtui-tui-for-rebuild-tool-wip)
  - [Key Features](#key-features)
  - [Dependencies](#dependencies)
  - [Quick Start](#quick-start)
    - [Basic Example](#basic-example)
    - [Build Instructions \& Integration](#build-instructions--integration)
  - [Manual build](#manual-build)
      - [Header-Only Integration (applies for manual build)](#header-only-integration-applies-for-manual-build)
      - [CMake Integration](#cmake-integration)
  - [Examples](#examples)
  - [Contributing](#contributing)
  - [License](#license)


## Key Features

- **Easy to use**: This library provides a Fluent-like builder for quick setup and customization
- **Hierarchical Organization**: Sections with nested selectable items
- **Customizable Themes**: Multiple built-in themes + custom styling (real custom theming will be implemented soon)
- **Keyboard Navigation**: Intuitive controls with vim-style shortcuts (note: vim-style shortcuts are optional and you should enable them)
- **Realtime Callbacks**: Event handlers for all user interactions
- **Smart Pagination**: Automatic pagination for large datasets
- ~~**Compatibility**: Works on Windows, Linux~~ (Need recheck if it works on Windows)
- **Selection Tracking**: Built-in counters and selection management

## Dependencies

- Cmake 3.20+
- С++20 and later

## Quick Start

### Basic Example

```cpp
#include "rebuildTUI/navigation_tui.hpp"
#include "rebuildTUI/section_builder.hpp"

using namespace tui;

int main() {
    auto graphics = SectionBuilder("Graphics Settings")
        .add_item("Enable VSync")
        .add_item("Anti-Aliasing")
        .add_item("Motion Blur")
        .build();

    auto audio = SectionBuilder("Audio Settings")
        .add_items({"Master Volume", "Sound Effects", "Music Volume"})
        .select_items({"Master Volume"}) // Pre-select items
        .build();

    // Build the TUI interface
    auto tui = NavigationBuilder()
        .text_titles("🎮 Game Settings", "⚙️ Configure: ")
        .theme_modern()
        .layout_centered()
        .add_section(graphics)
        .add_section(audio)
        .build();

    // Run the interface
    tui->run();

    // Get user selections
    auto selections = tui->get_all_selections();
    for (const auto& [section_name, items] : selections) {
        std::cout << section_name << ":\n";
        for (const auto& item : items) {
            std::cout << "  ✓ " << item << "\n";
        }
    }

    return 0;
}
```

### Build Instructions & Integration

## Manual build
```bash
git clone https://github.com/TheRebuild/rebuildTUI

cd rebuildTUI && mkdir build && cd build && cmake ..
cmake --build .

./test_tui
```

#### Header-Only Integration (applies for manual build)

```cpp
#include "rebuildTUI/navigation_tui.hpp"
#include "rebuildTUI/section_builder.hpp"
```

#### CMake Integration

##### Latest (development)
```cmake
include(FetchContent)

FetchContent_Declare(
    rebuildtui
    GIT_REPOSITORY https://github.com/TheRebuild/rebuildtui.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(rebuildtui)

add_executable(example_app main.cpp)
target_link_libraries(example_app PRIVATE rebuildTUI::rebuildTUI)
```

##### Stable
```cmake
include(FetchContent)

FetchContent_Declare(
    rebuildtui
    GIT_REPOSITORY https://github.com/TheRebuild/rebuildtui.git
    GIT_TAG v0.0.6
)

FetchContent_MakeAvailable(rebuildtui)

add_executable(example_app main.cpp)
target_link_libraries(example_app PRIVATE rebuildTUI::rebuildTUI)
```


## Examples

Check out the example files:

- `test_tui.cpp` - UI Example
- `state_saving_demo.cpp` - Example of UI + saving state
- `system_info` - System Info (hardcoded)
- `theme_customizing.cpp` - Available themes in UI and more

## Contributing

Soon...

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) file for details.