#include "Menu.h"

#include <grrlib.h>
#include <ogc/system.h>
#include <stdio.h>
#include <wiiuse/wpad.h>

void Menu::show(bool s)
{
  m_show = s;
}

void Menu::draw(uint16_t screenW, uint16_t screenH) const
{
  if (!m_show)
    return;

  float innerX = screenW / 2.f - static_cast<f32>(m_width) / 2.f;
  float innerY = screenH / 2.f - static_cast<f32>(m_height) / 2.f;

  // Border rect
  GRRLIB_Rectangle(innerX - m_style.borderWidth, innerY - m_style.borderWidth, m_width + m_style.borderWidth * 2,
                   m_height + m_style.borderWidth * 2, m_style.borderColor, true);
  // Inner rect
  GRRLIB_Rectangle(innerX, innerY, m_width, m_height, m_style.backgroundColor, true);

  for (int i = 0; i < m_items.size(); ++i) {
    std::string text = m_items.at(i).text;
    if (i == m_currentIdx) {
      text = "[ " + text + " ]";
    }

    int x = (m_width / 2) - (GRRLIB_WidthTTF(m_style.font, text.c_str(), m_style.fontSize) / 2);
    int y = i * m_style.fontSize + (i + 1) * m_style.textPadding;

    GRRLIB_PrintfTTF(innerX + x, innerY + y, m_style.font, text.c_str(), m_style.fontSize, m_style.fontColor);
  }
}

void Menu::addItem(const std::string& text, MenuFunc func, void* data)
{
  // Update height
  m_height += m_style.fontSize;
  m_height += m_style.textPadding * (1 + (m_items.size() == 0));

  // Recalculate width if needed
  u16 minWidth = GRRLIB_WidthTTF(m_style.font, text.c_str(), m_style.fontSize);
  minWidth += 2 * m_style.textPadding;
  minWidth += GRRLIB_WidthTTF(m_style.font, "[  ]", m_style.fontSize);

  if (minWidth > m_width) {
    m_width = minWidth;
  }

  m_items.emplace_back(text, data, func);
}

void Menu::nextItem(int dir)
{
  m_currentIdx += dir;
  if (m_currentIdx < 0) {
    m_currentIdx = m_items.size() - 1;
  }
  else if (m_currentIdx >= m_items.size()) {
    m_currentIdx = 0;
  }
}

void Menu::update(u32& wiimoteButtons)
{
  if (!m_show)
    return;

  u32 toConsume = 0;
  if (wiimoteButtons & WPAD_BUTTON_DOWN) {
    nextItem(1);
    toConsume |= WPAD_BUTTON_DOWN;
  }
  if (wiimoteButtons & WPAD_BUTTON_UP) {
    nextItem(-1);
    toConsume |= WPAD_BUTTON_UP;
  }

  if (wiimoteButtons & WPAD_BUTTON_A) {
    MenuItem& item = m_items.at(m_currentIdx);
    item.m_func(item.data);
    toConsume |= WPAD_BUTTON_A;
  }

  consumeInput(wiimoteButtons, toConsume);
}
void Menu::consumeInput(u32& wiimoteButtons, u32 toConsume)
{
  wiimoteButtons &= ~toConsume;
}
