#include <navigation_tui.hpp>
#include "section_builder.hpp"
#include <fstream>

using namespace tui;

// this is example of "saving state"
void save_state(const std::vector<Section> &sections)
{
    std::ofstream file("config.ini");
    for (const auto &section : sections)
    {
        file << "[" << section.name << "]\n";
        for (const auto &item : section.items)
        {
            file << item.name << " = " << (item.selected ? "true" : "false") << "\n";
        }
    }
    std::cout << "\nConfiguration saved to config.ini\n";
}

int main()
{
    auto settings = SectionBuilder("System Settings")
                        .add_item("Dark Mode")
                        .add_item("Auto Updates")
                        .build();

    auto privacy = SectionBuilder("Privacy")
                       .add_item("Location Tracking")
                       .add_item("Diagnostic Data")
                       .build();

    NavigationBuilder()
        .add_sections({settings, privacy})
        .on_exit([](const auto &sections)
                 { save_state(sections); })
        .keys_custom_shortcut('s', "Save configuration")
        .on_custom_command([](char key, auto /*state*/)
                           {
            if (key == 's') {
                std::cout << "\nSaving configuration...\n";
                return true;
            }
            return false; })
        .build()
        ->run();

    return 0;
}