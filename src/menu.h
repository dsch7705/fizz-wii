#pragma once

#include <gctypes.h>
#include <grrlib.h>
#include <functional>
#include <string>
#include <vector>

using MenuFunc = std::function<void(void* data)>;
struct MenuItem {
  MenuItem(const std::string& text, void* data, MenuFunc func) : text(text), data(data), m_func(func) {}

  std::string text;
  void* data;

 private:
  MenuFunc m_func;
  friend class Menu;
};

struct MenuStyle {
  u32 backgroundColor;
  u32 borderColor;
  u32 selectedColor;
  u8 borderWidth;

  GRRLIB_ttfFont* font;
  u32 fontColor;
  u8 fontSize;
  u8 textPadding;
};
constexpr MenuStyle kDefaultMenuStyle{.backgroundColor = 0xF0F0F0FF,
                                      .borderColor = 0x0F0F0FFF,
                                      .selectedColor = 0x00FF00FF,
                                      .borderWidth = 4,
                                      .fontColor = 0x000000FF,
                                      .fontSize = 24,
                                      .textPadding = 15};

class Menu {
 public:
  Menu() : m_style(kDefaultMenuStyle), m_width(0), m_height(0), m_currentIdx(0) {}

  void draw(uint16_t screenW, uint16_t screenH) const;
  void update(u32& wiimoteButtons);
  void show(bool s);
  void toggleShow() { show(!m_show); }

  void setStyle(MenuStyle style) { m_style = style; }
  MenuStyle& style() { return m_style; }

  void addItem(const std::string& text, MenuFunc func = [](void*) {}, void* data = nullptr);

 private:
  void nextItem(int dir);
  static void consumeInput(u32& wiimoteButtons, u32 toConsume);

  bool m_show;

  std::vector<MenuItem> m_items;

  MenuStyle m_style;
  u16 m_width;
  u16 m_height;

  s8 m_currentIdx;
};