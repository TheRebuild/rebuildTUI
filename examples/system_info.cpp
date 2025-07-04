#include <format>
#include <navigation_tui.hpp>
#include <section_builder.hpp>
#include <sys/utsname.h>
#include <unistd.h>

using namespace tui;

std::vector<SelectableItem> get_system_info() {
    utsname info{};
    uname(&info);

    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    return {{"OS", std::format("{} {}", info.sysname, info.release)},
            {"Hostname", hostname},
            {"Architecture", info.machine},
            {"CPU", "AMD Ryzen 9 5900X (24) @ 3.700GHz"},
            {"Memory", "32GB DDR4 @ 3200MHz"},
            {"Disk", "1TB NVMe SSD"}};
}

int main() {
    const auto info_section = SectionBuilder("System Information").add_items(get_system_info()).build();

    NavigationBuilder()
        .add_section(info_section)
        .text_show_help(false) // Hide help text
        .text_show_counters(false) // Hide counters

        /*
         * Layout borders is a placeholder for future implementation
         */
        .layout_borders(true)
        .build()
        ->run();

    return 0;
}
