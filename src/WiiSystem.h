#pragma once

#include <gctypes.h>
#include <wiiuse/wpad.h>

#include "fizz/System.h"

class WiiSystem : public System {
 public:
  virtual void interact(uint32_t buttonsDown, uint32_t buttonsHeld, WPADData* wpadData) = 0;
};

class Goo : public WiiSystem {
 public:
  Goo(unsigned int w, unsigned int h);

  void interact(uint32_t buttonsDown, uint32_t buttonsHeld, WPADData* wpadData) override;

 private:
  void reset();
  bool intersects(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d) const;

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
};

class Worm : public Pendulum {
 public:
  Worm(float length);
};