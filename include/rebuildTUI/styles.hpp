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
        enum class PresetType;

        // Basic presets
        static GradientPreset NONE();
        static GradientPreset WARM_TO_COLD();
        static GradientPreset RED_TO_GREEN();
        static GradientPreset BLUE_TO_PURPLE();
        static GradientPreset SUNSET();
        static GradientPreset OCEAN();
        static GradientPreset FOREST();
        static GradientPreset FIRE();
        static GradientPreset RAINBOW();

        // Custom presets
        static GradientPreset CUSTOM(uint8_t r, uint8_t g, uint8_t b);
        static GradientPreset CUSTOM(const std::vector<std::tuple<uint8_t, uint8_t, uint8_t>>& colors);

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

        [[nodiscard]] PresetType type() const;
        [[nodiscard]] const std::vector<std::tuple<uint8_t, uint8_t, uint8_t>>& custom_colors() const;

        bool operator==(const GradientPreset& other) const;

    private:
        explicit GradientPreset(const PresetType type) : type_(type) {}
        explicit GradientPreset(const PresetType type,
                                std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> custom_colors) :
            type_(type), custom_colors_(std::move(custom_colors)) {};

        PresetType type_;
        std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> custom_colors_;
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

        static std::vector<GradientColor> from_preset(const GradientPreset& preset, int steps);

        [[nodiscard]] std::tuple<uint8_t, uint8_t, uint8_t> get_color() const {
            return std::make_tuple(this->r_, this->g_, this->b_);
        };

    private:
        uint8_t r_;
        uint8_t g_;
        uint8_t b_;
    };
} // namespace tui::extras
