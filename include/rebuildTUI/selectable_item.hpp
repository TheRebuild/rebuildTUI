#pragma once

#include <any>
#include <format>
#include <functional>
#include <string>

namespace tui {

/**
 * @brief Represents a single selectable item that can be toggled
 */
struct SelectableItem {
  std::string name;        ///< Display name of the item
  std::string description; ///< Optional description or tooltip
  bool selected = false;   ///< Whether this item is currently selected
  int id = 0;              ///< Unique identifier for the item

  /**
   * @brief Optional user data that can be attached to this item
   *
   * This allows users to store any additional data they need
   * associated with this item (e.g., configuration values, callbacks, etc.)
   */
  std::any user_data;

  /**
   * @brief Optional callback function that gets called when item is toggled
   *
   * This allows for custom behavior when an item's selection state changes
   */
  std::function<void(bool)> on_toggle;

  explicit SelectableItem(const std::string &item_name) : name(item_name) {}

  SelectableItem(const std::string &item_name, const std::string &item_desc)
      : name(item_name), description(item_desc) {}

  SelectableItem(const std::string &item_name, const std::string &item_desc,
                 int item_id)
      : name(item_name), description(item_desc), id(item_id) {}

  SelectableItem(const std::string &item_name, const std::string &item_desc,
                 int item_id, const std::any &data)
      : name(item_name), description(item_desc), id(item_id), user_data(data) {}

  bool toggle() {
    selected = !selected;
    if (on_toggle)
      on_toggle(selected);
    return selected;
  }

  /**
   * @brief Set selection state explicitly
   *
   * @param new_state The new selection state
   * @return True if state actually changed, false if it was already in that
   * state
   */
  bool set_selected(bool new_state) {
    if (selected != new_state) {
      selected = new_state;
      if (on_toggle)
        on_toggle(selected);
      return true;
    }
    return false;
  }

  std::string get_display_string(char selected_char = '*',
                                 char unselected_char = ' ') const {
    char indicator = selected ? selected_char : unselected_char;
    return std::format("{} {}", std::string(1, indicator), name);
  }

  std::string get_display_string(const std::string &selected_prefix,
                                 const std::string &unselected_prefix) const {
    const std::string &prefix = selected ? selected_prefix : unselected_prefix;
    return prefix + name;
  }

  std::string get_full_description() const {
    return (!description.empty()) ? std::format("{} - {}", name, description)
                                  : name;
  }

  bool has_user_data() const { return user_data.has_value(); }

  template <typename T> T get_user_data() const {
    return std::any_cast<T>(user_data);
  }
  template <typename T> void set_user_data(const T &data) { user_data = data; }

  void set_toggle_callback(std::function<void(bool)> callback) {
    on_toggle = std::move(callback);
  }

  bool operator<(const SelectableItem &other) const {
    return name < other.name;
  }
  bool operator==(const SelectableItem &other) const {
    return id == other.id && name == other.name;
  }
  bool operator!=(const SelectableItem &other) const {
    return !(*this == other);
  }
};

} // namespace tui
