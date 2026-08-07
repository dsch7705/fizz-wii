#pragma once

#include <gctypes.h>
#include <wiiuse/wpad.h>

#include "fizz/System.h"

#include "Menu.h"

class WiiSystem : public System {
 public:
  virtual void interact(uint32_t buttonsDown, uint32_t buttonsHeld, WPADData* wpadData) = 0;

  Menu* infoMenu() { return &m_infoMenu; }

  void setupInfoMenu(Menu* prevMenu);

 protected:
  virtual void setupInfoMenu_impl() = 0;
  Menu m_infoMenu;
};

class Goo : public WiiSystem {
 public:
  Goo(unsigned int w, unsigned int h);

  void interact(uint32_t buttonsDown, uint32_t buttonsHeld, WPADData* wpadData) override;

 private:
  void reset();
  bool intersects(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d) const;

  void setupInfoMenu_impl() override;

  unsigned int m_w;
  unsigned int m_h;
  ID m_mouse;
};

class Pendulum : public WiiSystem {
 public:
  Pendulum(unsigned int links, float length);

  void interact(uint32_t buttonsDown, uint32_t buttonsHeld, WPADData* wpadData) override;

 protected:
  ID m_anchor;
  ID m_mouse;
  ID m_mouse_link;

 private:
  void setupInfoMenu_impl() override;
};

class Worm : public Pendulum {
 public:
  Worm(float length);
};