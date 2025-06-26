#pragma once

#include "selectable_item.hpp"
#include <algorithm>
#include <any>
#include <format>
#include <functional>
#include <string>
#include <vector>

namespace tui {

/**
 * @brief Represents a section containing multiple selectable items
 *
 * This is a generic container that can represent any logical grouping
 * of selectable items - categories, groups, folders, sections, etc.
 */
class Section {
public:
  std::string name;        ///< Name of the section
  std::string description; ///< Optional description of the section
  std::vector<SelectableItem>
      items; ///< Collection of selectable items in this section

  /**
   * @brief Optional user data that can be attached to this section
   */
  std::any user_data;

  /**
   * @brief Optional callback function that gets called when section is entered
   */
  std::function<void()> on_enter;

  /**
   * @brief Optional callback function that gets called when section is exited
   */
  std::function<void()> on_exit;

  /**
   * @brief Optional callback function that gets called when any item in section
   * is toggled
   */
  std::function<void(size_t, bool)> on_item_toggled;

  // Constructors
  explicit Section(const std::string &section_name) : name(section_name) {}
  Section(const std::string &section_name, const std::string &section_desc)
      : name(section_name), description(section_desc) {}
  Section(const std::string &section_name, const std::string &section_desc,
          const std::any &data)
      : name(section_name), description(section_desc), user_data(data) {}

  void add_item(const SelectableItem &item) { items.push_back(item); }
  void add_item(const std::string &item_name) { items.emplace_back(item_name); }
  void add_item(const std::string &item_name, const std::string &item_desc) {
    items.emplace_back(item_name, item_desc);
  }
  void add_item(const std::string &item_name, const std::string &item_desc,
                int item_id, const std::any &item_data = {}) {
    items.emplace_back(item_name, item_desc, item_id, item_data);
  }

  void add_items(const std::vector<SelectableItem> &new_items) {
    items.insert(items.end(), new_items.begin(), new_items.end());
  }
  void add_items(const std::vector<std::string> &names) {
    for (const auto &name : names) {
      add_item(name);
    }
  }

  size_t size() const { return items.size(); }

  bool empty() const { return items.empty(); }

  SelectableItem *get_item(size_t index) {
    if (index < items.size()) {
      return &items[index];
    }
    return nullptr;
  }
  const SelectableItem *get_item(size_t index) const {
    if (index < items.size()) {
      return &items[index];
    }
    return nullptr;
  }

  SelectableItem *get_item_by_name(const std::string &name) {
    auto it = std::find_if(
        items.begin(), items.end(),
        [&name](const SelectableItem &item) { return item.name == name; });
    return (it != items.end()) ? &(*it) : nullptr;
  }
  SelectableItem *get_item_by_id(int id) {
    auto it = std::find_if(
        items.begin(), items.end(),
        [id](const SelectableItem &item) { return item.id == id; });
    return (it != items.end()) ? &(*it) : nullptr;
  }

  bool toggle_item(size_t index) {
    if (index < items.size()) {
      bool new_state = items[index].toggle();
      if (on_item_toggled)
        on_item_toggled(index, new_state);
      return true;
    }
    return false;
  }

  bool set_item_selected(size_t index, bool selected) {
    if (index < items.size()) {
      bool changed = items[index].set_selected(selected);
      if (changed && on_item_toggled)
        on_item_toggled(index, selected);
      return changed;
    }
    return false;
  }

  size_t get_selected_count() const {
    return std::count_if(
        items.begin(), items.end(),
        [](const SelectableItem &item) { return item.selected; });
  }

  std::vector<std::string> get_selected_names() const {
    std::vector<std::string> selected;
    for (const auto &item : items)
      if (item.selected)
        selected.push_back(item.name);
    return selected;
  }

  std::vector<SelectableItem> get_selected_items() const {
    std::vector<SelectableItem> selected;
    for (const auto &item : items)
      if (item.selected)
        selected.push_back(item);
    return selected;
  }

  std::vector<size_t> get_selected_indices() const {
    std::vector<size_t> indices;
    for (size_t i = 0; i < items.size(); ++i)
      if (items[i].selected)
        indices.push_back(i);
    return indices;
  }

  void clear_selections() {
    for (size_t i = 0; i < items.size(); ++i) {
      if (items[i].selected) {
        items[i].set_selected(false);
        if (on_item_toggled) {
          on_item_toggled(i, false);
        }
      }
    }
  }

  void select_all() {
    for (size_t i = 0; i < items.size(); ++i) {
      if (!items[i].selected) {
        items[i].set_selected(true);
        if (on_item_toggled)
          on_item_toggled(i, true);
      }
    }
  }

  void invert_selections() {
    for (size_t i = 0; i < items.size(); ++i) {
      bool new_state = items[i].toggle();
      if (on_item_toggled)
        on_item_toggled(i, new_state);
    }
  }

  std::string get_display_string() const {
    return (!description.empty()) ? name + " - " + description : name;
  }

  std::string get_display_string_with_count() const {
    size_t selected = get_selected_count();
    size_t total = size();

    std::string base =
        (total > 0)
            ? std::format("{} ({}/{})", get_display_string(), selected, total)
            : get_display_string();

    return base;
  }

  bool remove_item(size_t index) {
    if (index < items.size()) {
      items.erase(items.begin() + index);
      return true;
    }
    return false;
  }

  bool remove_item_by_name(const std::string &name) {
    auto it = std::find_if(
        items.begin(), items.end(),
        [&name](const SelectableItem &item) { return item.name == name; });
    if (it != items.end()) {
      items.erase(it);
      return true;
    }
    return false;
  }

  void clear_items() { items.clear(); }

  void sort_items_by_name() {
    std::sort(items.begin(), items.end(),
              [](const SelectableItem &a, const SelectableItem &b) {
                return a.name < b.name;
              });
  }

  void sort_items_by_selection(bool selected_first = true) {
    std::sort(
        items.begin(), items.end(),
        [selected_first](const SelectableItem &a, const SelectableItem &b) {
          if (selected_first) {
            return a.selected && !b.selected;
          } else {
            return !a.selected && b.selected;
          }
        });
  }

  bool has_user_data() const { return user_data.has_value(); }

  template <typename T> T get_user_data() const {
    return std::any_cast<T>(user_data);
  }

  template <typename T> void set_user_data(const T &data) { user_data = data; }

  // Callbacks
  void set_enter_callback(std::function<void()> callback) {
    on_enter = std::move(callback);
  }

  void set_exit_callback(std::function<void()> callback) {
    on_exit = std::move(callback);
  }

  void set_item_toggled_callback(std::function<void(size_t, bool)> callback) {
    on_item_toggled = std::move(callback);
  }

  void trigger_enter() {
    if (on_enter)
      on_enter();
  }

  void trigger_exit() {
    if (on_exit)
      on_exit();
  }

  bool operator==(const Section &other) const { return name == other.name; }
  bool operator!=(const Section &other) const { return !(*this == other); }
  bool operator<(const Section &other) const { return name < other.name; }
};

} // namespace tui
