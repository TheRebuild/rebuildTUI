#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>

namespace tui::extras {
    using color = std::tuple<uint8_t, uint8_t, uint8_t>;
    using v_styles = std::vector<color>;

    enum class BorderStyle { ROUNDED, DOUBLE, SHARP, ASCII };

    enum class AccentColor {
        RESET = 0,
        RED = 31,
        GREEN = 32,
        YELLOW = 33,
        BLUE = 34,
        MAGENTA = 35,
        CYAN = 36,
        WHITE = 37,
        BRIGHT_RED = 91,
        BRIGHT_GREEN = 92,
        BRIGHT_YELLOW = 93,
        BRIGHT_BLUE = 94,
        BRIGHT_MAGENTA = 95,
        BRIGHT_CYAN = 96,
        BRIGHT_WHITE = 97
    };

    class GradientPreset {
    public:
        // Basic presets
        static GradientPreset NONE() { return GradientPreset(PresetType::NONE); }
        static GradientPreset WARM_TO_COLD() { return GradientPreset(PresetType::WARM_TO_COLD); }
        static GradientPreset RED_TO_GREEN() { return GradientPreset(PresetType::RED_TO_GREEN); }
        static GradientPreset BLUE_TO_PURPLE() { return GradientPreset(PresetType::BLUE_TO_PURPLE); }
        static GradientPreset SUNSET() { return GradientPreset(PresetType::SUNSET); }
        static GradientPreset OCEAN() { return GradientPreset(PresetType::OCEAN); }
        static GradientPreset FOREST() { return GradientPreset(PresetType::FOREST); }
        static GradientPreset FIRE() { return GradientPreset(PresetType::FIRE); }
        static GradientPreset RAINBOW() { return GradientPreset(PresetType::RAINBOW); }

        // Custom presets
        static GradientPreset CUSTOM(uint8_t r, uint8_t g, uint8_t b) {
            GradientPreset preset(PresetType::CUSTOM);
            preset.custom_colors_ = {{r, g, b}};
            return preset;
        }
        static GradientPreset CUSTOM(const v_styles& colors) {
            GradientPreset preset(PresetType::CUSTOM);
            preset.custom_colors_ = colors;
            return preset;
        }

        enum class PresetType {
            NONE,
            RED_TO_GREEN,
            BLUE_TO_PURPLE,
            WARM_TO_COLD, // From orange to cyan
            SUNSET,       // Red -> orange -> violet
            OCEAN,        // Blue to turquoise
            FOREST,       // Green to yellow-green
            FIRE,         // Red to yellow
            RAINBOW,      // Rainbow
            CUSTOM,       // Custom
        };

        [[nodiscard]] PresetType type() const { return type_; }
        [[nodiscard]] const v_styles& custom_colors() const { return custom_colors_; }

        bool operator==(const GradientPreset& other) const {
            return type_ == other.type_ && custom_colors_ == other.custom_colors_;
        }

    private:
        explicit GradientPreset(const PresetType type) : type_(type) {}

        PresetType type_;
        v_styles custom_colors_;
    };


    class GradientColor {
    public:
        explicit GradientColor() : r_(0), g_(0), b_(0) {}
        explicit GradientColor(const uint8_t r, const uint8_t g, const uint8_t b) : r_(r), g_(g), b_(b) {}

        void set_rgb(const uint8_t r, const uint8_t g, const uint8_t b) {
            this->r_ = r;
            this->g_ = g;
            this->b_ = b;
        }

        static std::vector<GradientColor> from_preset(const GradientPreset& preset, const int steps) {
            std::vector<GradientColor> gradient;
            gradient.reserve(steps);

            std::vector<GradientColor> color_points;

            switch (preset.type()) {
            case GradientPreset::PresetType::WARM_TO_COLD:
                color_points = {GradientColor{255, 10, 0}, GradientColor{255, 255, 200}, GradientColor{100, 200, 255}};
                break;
            case GradientPreset::PresetType::RED_TO_GREEN:
                color_points = {GradientColor{255, 50, 50}, GradientColor{255, 255, 100}, GradientColor{50, 255, 50}};
                break;
            case GradientPreset::PresetType::BLUE_TO_PURPLE:
                color_points = {GradientColor{50, 100, 255}, GradientColor{150, 50, 255}, GradientColor{255, 50, 255}};
                break;
            case GradientPreset::PresetType::SUNSET:
                color_points = {GradientColor{255, 0, 100}, GradientColor{255, 100, 0}, GradientColor{150, 0, 255}};
                break;
            case GradientPreset::PresetType::OCEAN:
                color_points = {GradientColor{0, 50, 150}, GradientColor{0, 150, 255}, GradientColor{0, 255, 255}};
                break;
            case GradientPreset::PresetType::FOREST:
                color_points = {
                    GradientColor{0, 100, 0},
                    GradientColor{50, 200, 50},
                    GradientColor{150, 255, 100},
                };
                break;
            case GradientPreset::PresetType::FIRE:
                color_points = {
                    GradientColor{255, 0, 0},
                    GradientColor{255, 100, 0},
                    GradientColor{255, 255, 0},
                };
                break;
            case GradientPreset::PresetType::RAINBOW:
                color_points = {
                    GradientColor{255, 0, 0},   // Red
                    GradientColor{255, 255, 0}, // Yellow
                    GradientColor{0, 255, 0},   // Green
                    GradientColor{0, 255, 255}, // Cyan
                    GradientColor{0, 0, 255},   // Blue
                    GradientColor{255, 0, 255}, // Magenta
                    GradientColor{255, 0, 0}    // Red
                };
                break;
            case GradientPreset::PresetType::CUSTOM:
                {
                    if (const auto& custom_colors = preset.custom_colors(); custom_colors.empty()) {
                        color_points = {GradientColor{255, 255, 255}};
                    } else {
                        for (const auto& color : custom_colors) {
                            auto [r, g, b] = color;
                            color_points.emplace_back(r, g, b);
                        }
                    }
                    break;
                }
            case GradientPreset::PresetType::NONE:
            default:
                return std::vector(steps, GradientColor{255, 255, 255});
            }

            const int segments = static_cast<int>(color_points.size()) - 1;
            if (segments <= 0) {
                for (auto i = 0; i < steps; ++i) {
                    gradient.push_back(color_points.empty() ? GradientColor{} : color_points.front());
                }
                return gradient;
            }

            const auto segment_steps = static_cast<float>(steps) / static_cast<float>(segments);

            for (auto seg = 0; seg < segments; seg++) {
                const auto& start = color_points[seg];
                const auto& end = color_points[seg + 1];

                const auto seg_start = static_cast<int>(static_cast<float>(seg) * segment_steps);
                const auto seg_end = static_cast<int>(static_cast<float>(seg + 1) * segment_steps);

                const int final_seg_end = std::min(seg_end, steps);
                const int seg_steps = final_seg_end - seg_start;

                for (auto i = 0; i < seg_steps; i++) {
                    const float ratio =
                        (seg_steps > 1) ? static_cast<float>(i) / static_cast<float>(seg_steps - 1) : 0.0f;
                    auto [s_r, s_g, s_b] = start.get_color();
                    auto [e_r, e_g, e_b] = end.get_color();
                    auto r = static_cast<uint8_t>(static_cast<float>(s_r) + ratio * static_cast<float>(e_r - s_r));
                    auto g = static_cast<uint8_t>(static_cast<float>(s_g) + ratio * static_cast<float>(e_g - s_g));
                    auto b = static_cast<uint8_t>(static_cast<float>(s_b) + ratio * static_cast<float>(e_b - s_b));
                    gradient.emplace_back(r, g, b);
                }
            }

            if (gradient.size() > static_cast<size_t>(steps)) {
                gradient.resize(steps);
            } else if (gradient.size() < static_cast<size_t>(steps)) {
                do {
                    gradient.push_back(color_points.back());
                } while (gradient.size() < static_cast<size_t>(steps));
            }

            return gradient;
        }

        [[nodiscard]] color get_color() const { return std::make_tuple(this->r_, this->g_, this->b_); }

    private:
        uint8_t r_;
        uint8_t g_;
        uint8_t b_;
    };
} // namespace tui::extras
