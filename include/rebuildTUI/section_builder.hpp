#pragma once

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "section.hpp"

namespace tui {

    /**
     * @brief Builder class for Section
     *
     * This builder allows for intuitive creation of sections with items,
     * callbacks, and configuration using method chaining.
     *
     * Example usage:
     * auto section = SectionBuilder("Audio Settings")
     *     .description("Configure audio-related options")
     *     .add_item("Enable surround sound")
     *     .add_item("Boost bass", "Enhance low frequency audio")
     *     .add_items({"Normalize volume", "Enable equalizer"})
     *     .on_enter([]() { std::cout << "Entered audio settings\n"; })
     *     .build();
     */
    class SectionBuilder {
    private:
        std::string name_;
        std::string description_;
        std::vector<SelectableItem> items_;
        std::any user_data_;
        std::function<void()> on_enter_;
        std::function<void()> on_exit_;
        std::function<void(size_t, bool)> on_item_toggled_;

    public:
        explicit SectionBuilder(std::string name) : name_(std::move(name)) {}

        /**
         * @brief Set section description
         */
        SectionBuilder &description(const std::string &desc) {
            description_ = desc;
            return *this;
        }

        SectionBuilder &add_item(const std::string &name) {
            items_.emplace_back(name);
            return *this;
        }
        SectionBuilder &add_item(const std::string &name, const std::string &desc) {
            items_.emplace_back(name, desc);
            return *this;
        }
        SectionBuilder &add_item(const std::string &name, const std::string &desc, int id, const std::any &data = {}) {
            items_.emplace_back(name, desc, id, data);
            return *this;
        }

        SectionBuilder &add_item(const SelectableItem &item) {
            items_.push_back(item);
            return *this;
        }

        SectionBuilder &add_items(const std::vector<std::string> &names) {
            for (const auto &name : names) {
                items_.emplace_back(name);
            }

            return *this;
        }

        SectionBuilder &add_items(const std::vector<std::pair<std::string, std::string>> &items) {
            for (const auto &[name, desc] : items) {
                items_.emplace_back(name, desc);
            }

            return *this;
        }

        SectionBuilder &add_items(const std::vector<SelectableItem> &items) {
            items_.insert(items_.end(), items.begin(), items.end());
            return *this;
        }

        template <typename Iterator>
        SectionBuilder &add_items_from_range(Iterator begin, Iterator end) {
            for (auto it = begin; it != end; ++it) {
                items_.emplace_back(*it);
            }
            return *this;
        }

        SectionBuilder &add_generated_items(const size_t count, const std::function<std::string(size_t)> &generator) {
            for (size_t i = 0; i < count; ++i) {
                items_.emplace_back(generator(i));
            }

            return *this;
        }

        SectionBuilder &add_generated_items(const size_t count,
                                            const std::function<SelectableItem(size_t)> &generator) {
            for (size_t i = 0; i < count; ++i) {
                items_.push_back(generator(i));
            }
            return *this;
        }

        template <typename T>
        SectionBuilder &user_data(const T &data) {
            user_data_ = data;
            return *this;
        }

        SectionBuilder &on_enter(std::function<void()> callback) {
            on_enter_ = std::move(callback);
            return *this;
        }

        SectionBuilder &on_exit(std::function<void()> callback) {
            on_exit_ = std::move(callback);
            return *this;
        }

        SectionBuilder &on_item_toggled(std::function<void(size_t, bool)> callback) {
            on_item_toggled_ = std::move(callback);
            return *this;
        }

        SectionBuilder &callbacks(std::function<void()> enter_cb, std::function<void()> exit_cb,
                                  std::function<void(size_t, bool)> toggle_cb = nullptr) {
            on_enter_ = std::move(enter_cb);
            on_exit_ = std::move(exit_cb);
            if (toggle_cb) {
                on_item_toggled_ = std::move(toggle_cb);
            }

            return *this;
        }

        SectionBuilder &select_items(const std::vector<size_t> &indices) {
            for (size_t index : indices) {
                if (index < items_.size()) {
                    items_[index].selected = true;
                }
            }
            return *this;
        }

        SectionBuilder &select_items(const std::vector<std::string> &names) {
            for (const auto &name : names) {
                auto it = std::find_if(items_.begin(), items_.end(),
                                       [&name](const SelectableItem &item) { return item.name == name; });
                if (it != items_.end()) {
                    it->selected = true;
                }
            }
            return *this;
        }

        SectionBuilder &select_all() {
            for (auto &item : items_) {
                item.selected = true;
            }
            return *this;
        }

        SectionBuilder &select_none() {
            for (auto &item : items_) {
                item.selected = false;
            }
            return *this;
        }

        SectionBuilder &sort_items() {
            std::sort(items_.begin(), items_.end(),
                      [](const SelectableItem &a, const SelectableItem &b) { return a.name < b.name; });
            return *this;
        }

        SectionBuilder &reverse_items() {
            std::reverse(items_.begin(), items_.end());
            return *this;
        }

        SectionBuilder &set_item_callbacks(const std::function<void(bool)> &callback) {
            for (auto &item : items_) {
                item.set_toggle_callback(callback);
            }
            return *this;
        }

        SectionBuilder &apply_to_items(const std::function<void(SelectableItem &)> &func) {
            for (auto &item : items_) {
                func(item);
            }
            return *this;
        }

        SectionBuilder &filter_items(std::function<bool(const SelectableItem &)> predicate) {
            items_.erase(std::remove_if(items_.begin(), items_.end(),
                                        [&predicate](const SelectableItem &item) { return !predicate(item); }),
                         items_.end());
            return *this;
        }

        Section build() {
            Section section(name_, description_, user_data_);
            section.items = std::move(items_);

            if (on_enter_) {
                section.set_enter_callback(on_enter_);
            }
            if (on_exit_) {
                section.set_exit_callback(on_exit_);
            }
            if (on_item_toggled_) {
                section.set_item_toggled_callback(on_item_toggled_);
            }

            return section;
        }

        std::unique_ptr<Section> build_unique() { return std::make_unique<Section>(build()); }

        std::shared_ptr<Section> build_shared() { return std::make_shared<Section>(build()); }

        [[nodiscard]] size_t item_count() const { return items_.size(); }
        [[nodiscard]] bool empty() const { return items_.empty(); }

        SectionBuilder &reset() {
            description_.clear();
            items_.clear();
            user_data_.reset();
            on_enter_ = nullptr;
            on_exit_ = nullptr;
            on_item_toggled_ = nullptr;
            return *this;
        }
    };

    /**
     * @brief Builder for creating multiple sections at once
     */
    class MultiSectionBuilder {
        std::vector<Section> sections_;

    public:
        MultiSectionBuilder &add_section(SectionBuilder &&builder) {
            sections_.push_back(builder.build());
            return *this;
        }

        MultiSectionBuilder &add_section(const Section &section) {
            sections_.push_back(section);
            return *this;
        }

        MultiSectionBuilder &add_section(const std::string &name,
                                         const std::function<void(SectionBuilder &)> &configurator) {
            SectionBuilder builder(name);
            configurator(builder);
            sections_.push_back(builder.build());
            return *this;
        }

        MultiSectionBuilder &add_sections(const std::vector<std::string> &names) {
            for (const auto &name : names) {
                sections_.emplace_back(name);
            }

            return *this;
        }

        MultiSectionBuilder &apply_to_all(const std::function<void(Section &)> &configurator) {
            for (auto &section : sections_) {
                configurator(section);
            }

            return *this;
        }

        MultiSectionBuilder &sort_sections() {
            std::ranges::sort(sections_, [](const Section &a, const Section &b) { return a.name < b.name; });
            return *this;
        }

        std::vector<Section> build() { return std::move(sections_); }
        [[nodiscard]] size_t section_count() const { return sections_.size(); }
        [[nodiscard]] bool empty() const { return sections_.empty(); }

        MultiSectionBuilder &clear() {
            sections_.clear();
            return *this;
        }
    };

} // namespace tui
