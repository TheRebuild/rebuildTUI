#include <fstream>
#include <iostream>
#include <navigation_tui.hpp>
#include <vector>
#include "section_builder.hpp"

using namespace tui;

// this is example of "saving state"
void save_state(const std::vector<Section> &sections) {
    std::ofstream file("config.ini");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open config.ini for writing.\n";
        return;
    }

    for (const auto &section : sections) {
        file << std::format("[{}]\n", section.name);
        for (const auto &item : section.items) {
            file << std::format("{} = {}\n", item.name, (item.selected ? "true" : "false"));
        }
    }
    std::cout << "\nConfiguration saved to config.ini\n";
}

int main() {
    std::vector all_sections = {
        SectionBuilder("System Settings").add_item("Dark Mode").add_item("Auto Updates").build(),
        SectionBuilder("Privacy").add_item("Location Tracking").add_item("Diagnostic Data").build()};

    NavigationBuilder()
        .add_sections(all_sections)
        .on_exit([](const std::vector<Section> &sections) { save_state(sections); })
        .on_item_toggled([&all_sections](const size_t section_index, const size_t item_index, const bool selected) {
            if (section_index < all_sections.size() && item_index < all_sections[section_index].items.size()) {
                all_sections[section_index].items[item_index].selected = selected;
            }
        })
        .keys_custom_shortcut('s', "Save configuration")
        .on_custom_command([&all_sections](const char key, const NavigationTUI::NavigationState &) {
            if (key == 's') {
                std::cout << "\nSaving configuration...\n";
                save_state(all_sections);
                return true;
            }
            return false;
        })
        .build()
        ->run();

    return 0;
}
