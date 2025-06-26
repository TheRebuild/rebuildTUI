## 📖 API Documentation

### Core Classes

#### `SelectableItem`

Represents a single item that can be toggled on/off.

```cpp
SelectableItem item("Item Name", "Optional description");
item.toggle();                    // Toggle selection
item.set_selected(true);          // Set selection state
item.set_user_data(custom_data);  // Attach custom data
```

#### `Section`

Container for multiple selectable items.

```cpp
Section section("Section Name", "Optional description");
section.add_item("Item 1");
section.add_item("Item 2", "With description");
section.toggle_item(0);           // Toggle first item
section.get_selected_names();     // Get selected item names
```

#### `NavigationTUI`

Main TUI interface for navigation.

```cpp
NavigationTUI tui;
tui.add_section(section);
tui.set_item_toggled_callback([](size_t section_idx, size_t item_idx, bool selected) {
    // Handle item toggle
});
tui.run();
```

### Builder Pattern

#### `SectionBuilder`

Fluent interface for creating sections:

```cpp
auto section = SectionBuilder("My Section")
    .description("Section description")
    .add_item("Item 1")
    .add_items({"Item 2", "Item 3"})
    .select_items({"Item 1"})       // Pre-select items
    .sort_items()                   // Sort alphabetically
    .on_enter([]() { /* callback */ })
    .build();
```

#### `NavigationBuilder`

Fluent interface for creating the TUI:

```cpp
auto tui = NavigationBuilder()
    // Text configuration
    .text_titles("Main Title", "Section Prefix: ")
    .text_help("Custom help text", "Custom item help")

    // Theme and styling
    .theme_modern()                 // or theme_minimal(), theme_fancy(), theme_retro()
    .theme_indicators('✓', '○')     // Custom selection indicators

    // Layout options
    .layout_centered()              // Center content
    .layout_items_per_page(20)      // Pagination
    .layout_content_width(40, 100)  // Min/max width

    // Keyboard shortcuts
    .keys_vim_style(true)           // Enable hjkl navigation
    .keys_custom_shortcut('s', "Save config")

    // Add sections
    .add_section(section1)
    .add_sections({section2, section3})

    // Event callbacks
    .on_item_toggled([](size_t sec, size_t item, bool selected) { /* ... */ })
    .on_exit([](const std::vector<Section>& sections) { /* ... */ })

    .build();
```

## 🎨 Themes and Styling

### Built-in Themes

```cpp
NavigationBuilder()
    .theme_minimal()    // Clean, simple appearance
    .theme_modern()     // Contemporary with Unicode chars
    .theme_fancy()      // Rich styling with colors
    .theme_retro()      // Classic terminal look
```

### Custom Styling

```cpp
NavigationBuilder()
    .theme_indicators('★', '☆')                    // Custom selection chars
    .theme_unicode(true)                            // Enable Unicode characters
    .theme_prefixes("✅ ", "⬜ ")                   // Custom prefixes (requires .theme_unicode(true))
    .theme_border_style("rounded")                  // Border style
    .theme_accent_color("cyan")                     // Highlight color
```

### Layout Options

```cpp
NavigationBuilder()
    .layout_centering(true, false)                 // Horizontal, vertical
    .layout_content_width(50, 120)                 // Min, max width
    .layout_items_per_page(15)                     // Pagination size
    .layout_borders(true)                          // Show borders
```

## 🌟 Use Cases

### Configuration Management

```cpp
auto privacy = SectionBuilder("Privacy Settings")
    .add_item("Block Telemetry")
    .add_item("Disable Location")
    .build();
```

### Setup Wizards

```cpp
auto frontend = SectionBuilder("Frontend Framework")
    .add_items({"React", "Vue", "Angular"})
    .build();
```

### Feature Toggles

```cpp
auto features = SectionBuilder("Beta Features")
    .add_item("New Search Algorithm")
    .add_item("ML Recommendations")
    .build();
```

## ⌨️ Keyboard Controls

### Section Selection

- `↑/↓` or `j/k` - Navigate sections
- `Enter` - Enter selected section
- `1-9` - Quick select by number
- `q` - Quit application

### Item Selection

- `↑/↓` or `j/k` - Navigate items
- `Space` - Toggle current item
- `Enter` - Toggle current item
- `a` - Select all items
- `n` - Select no items
- `1-9` - Jump to page number
- `b/Esc` - Back to sections

### Custom Shortcuts

Add your own shortcuts with the builder:

```cpp
.keys_custom_shortcut('s', "Save configuration")
.on_custom_command([](char key, NavigationState state) -> bool {
    if (key == 's') {
        save_config();
        return true;  // Handled
    }
    return false;     // Not handled
})
```

## 🔧 Advanced Features

### Event Callbacks

```cpp
.on_section_selected([](size_t index, const Section& section) {
    std::cout << "Entered: " << section.name << std::endl;
})
.on_item_toggled([](size_t section_idx, size_t item_idx, bool selected) {
    std::cout << "Item toggled: " << selected << std::endl;
})
.on_state_changed([](NavigationState old_state, NavigationState new_state) {
    std::cout << "Navigation state changed" << std::endl;
})
.on_exit([](const std::vector<Section>& sections) {
    std::cout << "Exiting with final configuration" << std::endl;
})
```

### User Data Attachment

```cpp
// Attach custom data to sections and items
section.set_user_data(MyCustomData{"config_file.json"});
item.set_user_data(42);

// Retrieve data later
auto data = section.get_user_data<MyCustomData>();
int value = item.get_user_data<int>();
```

### Dynamic Content

```cpp
auto section = SectionBuilder("Generated Items")
    .add_generated_items(10, [](size_t i) {
        return SelectableItem("Item " + std::to_string(i + 1));
    })
    .build();
```
