#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "navigation_tui.hpp"
#include "section_builder.hpp"

using namespace tui;

std::vector<Section> generate_comprehensive_configuration() {
    std::vector<Section> sections;

    auto privacy =
        SectionBuilder("Privacy & Security")
            .description("Control data collection and security settings")
            .add_item("Block Telemetry", "Prevent system from sending usage data")
            .add_item("Disable Location Tracking", "Stop apps from accessing location")
            .add_item("Clear Web Data", "Remove browsing history and cookies")
            .add_item("Disable Microphone Access", "Prevent unauthorized microphone use")
            .add_item("Disable Camera Access", "Block camera access for apps")
            .add_item("Enable Firewall", "Block unauthorized network connections")
            .add_item("Secure DNS", "Use encrypted DNS queries")
            .add_item("VPN Integration", "Route traffic through VPN")
            .select_items({"Block Telemetry", "Enable Firewall", "Secure DNS"})
            .on_enter([]() { std::cout << "🔒 Configuring privacy and security settings...\n"; })
            .on_item_toggled([](size_t idx, bool selected) {
                std::vector<std::string> items = {
                    "Block Telemetry",       "Disable Location Tracking", "Clear Web Data", "Disable Microphone Access",
                    "Disable Camera Access", "Enable Firewall",           "Secure DNS",     "VPN Integration"};
                if (idx < items.size()) {
                    std::cout << "🔐 Privacy setting '" << items[idx] << "' " << (selected ? "ENABLED" : "DISABLED")
                              << std::endl;
                }
            })
            .build();

    auto performance =
        SectionBuilder("Performance Optimization")
            .description("Improve system speed and responsiveness")
            .add_generated_items(
                12,
                [](size_t i) -> SelectableItem {
                    std::vector<std::pair<std::string, std::string>> optimizations = {
                        {"Disable Startup Programs", "Reduce boot time by disabling unnecessary startup apps"},
                        {"Clear Temporary Files", "Free up disk space by removing temp files"},
                        {"Optimize Memory Usage", "Better RAM management and cleanup"},
                        {"Disable Visual Effects", "Reduce GPU and CPU usage from animations"},
                        {"Enable Fast Boot", "Quick system startup mode"},
                        {"Optimize Network Settings", "Improve internet connection speed"},
                        {"Clean System Registry", "Remove obsolete registry entries"},
                        {"Defragment Storage", "Optimize hard drive performance"},
                        {"Update Device Drivers", "Install latest hardware drivers"},
                        {"Disable Background Apps", "Prevent apps from running in background"},
                        {"Enable Game Mode", "Optimize system for gaming performance"},
                        {"Power Plan Optimization", "Adjust power settings for performance"}};

                    if (i < optimizations.size()) {
                        return SelectableItem{optimizations[i].first, optimizations[i].second, static_cast<int>(i)};
                    }
                    return SelectableItem{"Optimization " + std::to_string(i + 1), "Performance optimization option"};
                })
            .select_items({"Clear Temporary Files", "Optimize Memory Usage", "Update Device Drivers"})
            .sort_items()
            .on_enter([]() { std::cout << "⚡ Configuring performance optimizations...\n"; })
            .build();

    auto customization = SectionBuilder("System Customization")
                             .description("Personalize your system appearance and behavior")
                             .add_items(std::vector<std::pair<std::string, std::string>>{
                                 {"Dark Mode", "Enable system-wide dark theme"},
                                 {"Large Text", "Increase font sizes for better readability"},
                                 {"High Contrast", "Improve visibility with high contrast colors"},
                                 {"Custom Wallpaper", "Set personalized desktop background"},
                                 {"Taskbar Customization", "Modify taskbar appearance and behavior"},
                                 {"Start Menu Layout", "Customize start menu organization"},
                                 {"Sound Scheme", "Change system sound effects"},
                                 {"Mouse Cursor Theme", "Customize mouse pointer appearance"},
                                 {"Window Animations", "Enable smooth window transitions"},
                                 {"Desktop Icons", "Show or hide desktop shortcuts"}})
                             .on_enter([]() { std::cout << "🎨 Configuring system customization...\n"; })
                             .build();

    auto dev_tools = SectionBuilder("Developer Tools")
                         .description("Tools and settings for software development")
                         .add_item("Enable Developer Mode", "Access advanced development features")
                         .add_item("Windows Subsystem for Linux", "Run Linux environment on Windows")
                         .add_item("Command Line Tools", "Install terminal and shell utilities")
                         .add_item("Package Managers", "Enable package management systems")
                         .add_item("Git Version Control", "Install Git for source code management")
                         .add_item("Code Editor Integration", "Setup IDE and editor support")
                         .add_item("Debugging Tools", "Install application debugging utilities")
                         .add_item("Performance Profilers", "Tools for code performance analysis")
                         .add_item("Container Support", "Docker and container runtime")
                         .add_item("Virtual Machines", "Hypervisor and VM support")
                         .on_enter([]() { std::cout << "👨‍💻 Configuring developer tools...\n"; })
                         .build();

    auto gaming = SectionBuilder("Gaming Optimization")
                      .description("Optimize system for gaming performance")
                      .add_item("Game Mode", "Prioritize system resources for games")
                      .add_item("GPU Optimization", "Optimize graphics card settings")
                      .add_item("Disable Game Bar", "Remove Xbox Game Bar overlay")
                      .add_item("High Performance Power Plan", "Maximum performance power settings")
                      .add_item("Disable Windows Update", "Prevent updates during gaming")
                      .add_item("Network Optimization", "Reduce network latency for online games")
                      .add_item("Audio Optimization", "Low-latency audio for gaming")
                      .add_item("Fullscreen Optimizations", "Disable fullscreen optimization")
                      .select_items({"Game Mode", "GPU Optimization", "High Performance Power Plan"})
                      .on_enter([]() { std::cout << "🎮 Configuring gaming optimizations...\n"; })
                      .build();

    std::vector<Section> temp_sections = {privacy, performance, customization, dev_tools, gaming};
    sections.insert(sections.end(), temp_sections.begin(), temp_sections.end());
    return sections;
}

int main() {
    try {
        auto sections = generate_comprehensive_configuration();

        auto tui = NavigationBuilder()
                       .text_titles("Example Windows Tweaker", "Configure: ")
                       .text_help("Up/Down: Navigate | Enter: Select | 1-9: Quick | Q: Quit",
                                  "Up/Down: Navigate | Space: Toggle | Enter: Back | Q: Quit")
                       .text_messages("No options available in this section.")
                       .text_show_help(true)
                       .text_show_pages(true)
                       .text_show_counters(true)

                       // Theme and styling
                       // .theme_fancy()   // ✓  / ○
                       // .theme_minimal() // * / nothing
                       // .theme_modern()  // ● / "○

                       .theme_unicode(true)
                       .theme_prefixes("✅", "❌") // requires theme_unicode(true)

                       // currently placeholder
                       // i'll implement in the future
                       // .theme_colors(true)
                       // .theme_accent_color("#00ff48")

                       // Layout configuration
                       /*
                        * It seems that layout_centering works only with vertical layout?
                        *
                        * Will fix this in the future
                        */
                       .layout_centering(false, // horizontal
                                         true) // vertical

                       .layout_content_width(60, 80)
                       .layout_items_per_page(15) // Show 15 items per page

                       /*
                        * Layout borders is a placeholder for future implementation
                        */
                       .layout_borders(false) // Don't show borders

                       .layout_auto_resize(true) // Auto-adjust to terminal size

                       // Keyboard shortcuts
                       .keys_custom_shortcut('h', "Show detailed help")
                       .keys_custom_shortcut('s', "Save configuration")
                       .keys_custom_shortcut('r', "Reset to defaults")
                       .keys_custom_shortcut('i', "Show system info")
                       // .keys_vim_style(true)           // Enable hjkl navigation

                       .add_sections(sections)

                       .on_section_selected([](size_t /*index*/, const Section &section) {
                           std::cout << "📂 Entered section: " << section.name;
                           if (!section.description.empty()) {
                               std::cout << " - " << section.description;
                           }
                           std::cout << std::endl;
                       })
                       .on_item_toggled([](size_t section_idx, size_t item_idx, bool selected) {
                           std::cout << "🔄 Section " << section_idx << ", Item " << item_idx << " is now "
                                     << (selected ? "ENABLED" : "DISABLED") << std::endl;
                       })
                       .on_page_changed([](int new_page, int total_pages) {
                           std::cout << "📄 Page changed to " << (new_page + 1) << " of " << total_pages << std::endl;
                       })
                       .on_state_changed(
                           [](NavigationTUI::NavigationState old_state, NavigationTUI::NavigationState new_state) {
                               std::vector<std::string> state_names = {"Section Selection", "Item Selection"};
                               std::cout << "🔄 Navigation state: " << state_names[static_cast<int>(old_state)] << " → "
                                         << state_names[static_cast<int>(new_state)] << std::endl;
                           })
                       .on_custom_command([](char key, NavigationTUI::NavigationState /*state*/) -> bool {
                           switch (key) {
                           case '\n':
                               return true;
                           case 'h':
                               std::cout << "\n📖 HELP:\n";
                               std::cout << "========\n";
                               std::cout << "This universal TUI system can be used for any "
                                            "configuration interface.\n";
                               std::cout << "Navigate with arrow keys or hjkl (vim-style).\n";
                               std::cout << "Use Space to toggle options, Enter to enter "
                                            "sections.\n";
                               std::cout << "Press 'q' to quit, 'b' to go back.\n";
                               std::cout << "Custom shortcuts: s=save, r=reset, i=info, h=help\n\n";
                               return true;

                           case 's':
                               std::cout << "\n💾 Configuration saved to profile!\n";
                               std::cout << "All your settings have been applied successfully.\n\n";
                               return true;

                           case 'r':
                               std::cout << "\n🔄 Reset to default configuration\n";
                               std::cout << "All settings have been restored to defaults.\n\n";
                               return true;

                           case 'i':
                               std::cout << "\n💻 SYSTEM INFO:\n";
                               std::cout << "===============\n";
                               std::cout << "Universal TUI Configuration System v2.0\n";
                               std::cout << "Built with modern C++ and FTXUI\n";
                               std::cout << "Supports any type of hierarchical configuration\n\n";
                               return true;

                           default:
                               return false; // Not handled
                           }
                       })
                       .on_exit([](const std::vector<Section> &sections) {
                           std::cout << "\n🎉 Configuration Complete!\n";
                           std::cout << "==========================\n";
                           std::cout << "📊 Final Configuration Summary:\n\n";

                           size_t total_selected = 0;
                           size_t total_sections_with_selections = 0;

                           for (const auto &section : sections) {
                               auto selected_items = section.get_selected_names();
                               if (!selected_items.empty()) {
                                   total_sections_with_selections++;
                                   std::cout << "🔹 " << section.name << " (" << selected_items.size() << " items):\n";
                                   for (const auto &item : selected_items) {
                                       std::cout << "   ✅ " << item << "\n";
                                       total_selected++;
                                   }
                                   std::cout << "\n";
                               }
                           }

                           if (total_selected == 0) {
                               std::cout << "ℹ️  No options were selected.\n";
                           } else {
                               std::cout << "📈 Statistics:\n";
                               std::cout << "   • Total options selected: " << total_selected << "\n";
                               std::cout << "   • Sections configured: " << total_sections_with_selections << " of "
                                         << sections.size() << "\n";
                           }

                           std::cout << "\n🚀 Your system is now configured!\n";
                           std::cout << "💡 This same framework can be used for any "
                                        "hierarchical selection interface.\n";
                       })

                       .build();

        tui->run();

        // exit seems not working, need to investigate this issue later
        std::cout << "\nPress Enter to exit...\n" << std::endl;

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown error occurred" << std::endl;
        return 1;
    }
}
