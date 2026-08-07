#include <grrlib.h>
#include <ogc/lwp_watchdog.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

// #include "font.c"
#include "font.h"

#include "Menu.h"

#include "fizz/Draw.h"
#include "fizz/System.h"

#include "WiiSystem.h"

#include "fizz/constraints/DistanceConstraint.h"
#include "fizz/constraints/SpringConstraint.h"

u32 col_to_u32(Draw::Color c)
{
  return (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a;
}
void grr_circle(const Vec2& center, float radius, Draw::Color color)
{
  GRRLIB_Circle(center.x, center.y, radius, col_to_u32(color), 1);
}
void grr_line(const Vec2& p0, const Vec2& p1, Draw::Color color)
{
  GRRLIB_Line(p0.x, p0.y, p1.x, p1.y, col_to_u32(color));
}

int main(int argc, char** argv)
{
  srand(gettime());

  GRRLIB_Init();
  GRRLIB_SetBackgroundColour(0, 0, 0, 255);

  u16 screenW = rmode->fbWidth;
  u16 screenH = rmode->efbHeight;

  WPAD_Init();
  WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

  // Load font
  GRRLIB_ttfFont* font = GRRLIB_LoadTTF(noto_sans_mono, sizeof(noto_sans_mono));

  u64 lastTime = gettime();
  float deltaTime = 0.0;

  Draw::setCircleCallback(grr_circle);
  Draw::setLineCallback(grr_line);

  auto& transform = Draw::getTransform();
  transform.scale = 30;
  transform.offset = Vec2(screenW, screenH) / transform.scale / 2.f;

  WiiSystem* currentSystem = nullptr;
  Menu* currentMenu = nullptr;
  auto setCurrentSystem = [&](void* system) {
    assert(system != nullptr);

    currentSystem = static_cast<WiiSystem*>(system);

    if (currentMenu)
      currentMenu->show(false);
  };

  Goo goo(15, 10);
  setCurrentSystem(&goo);

  Pendulum pendulum(1, 7.5f);

  Pendulum double_pendulum(2, 7.5f);

  Worm worm(7.5f);

  // Menu
  auto setCurrentMenu = [&](void* menu) {
    assert(menu != nullptr);
    currentMenu = static_cast<Menu*>(menu);
    currentMenu->show(true);
  };

  Menu mainMenu{};
  MenuStyle& menuStyle = mainMenu.style();
  menuStyle.backgroundColor = 0x000000FF;
  menuStyle.borderColor = 0xFFFFFFFF;
  menuStyle.fontColor = menuStyle.borderColor;
  menuStyle.font = font;
  menuStyle.fontSize = 18;

  setCurrentMenu(&mainMenu);
  mainMenu.show(false);

  Menu sceneMenu{};
  sceneMenu.style() = menuStyle;
  sceneMenu.addItem("Goo", setCurrentSystem, &goo);
  sceneMenu.addItem("Pendulum", setCurrentSystem, &pendulum);
  sceneMenu.addItem("Double pendulum", setCurrentSystem, &double_pendulum);
  sceneMenu.addItem("Worm", setCurrentSystem, &worm);
  sceneMenu.addItem("Back", setCurrentMenu, &mainMenu);

  mainMenu.addItem("Load scene", setCurrentMenu, &sceneMenu);

  bool running = true;
  mainMenu.addItem("Quit", [&](void*) { running = false; });

  while (running) {
    // Delta time calculation
    u64 currentTime = gettime();
    u64 diff = currentTime - lastTime;
    deltaTime = (float)diff / (TB_TIMER_CLOCK * 1000);
    lastTime = currentTime;

    currentSystem->update(deltaTime);

    // WPAD Buttons
    WPAD_ScanPads();
    u32 buttonsDown = WPAD_ButtonsDown(0);
    u32 buttonsHeld = WPAD_ButtonsHeld(0);
    WPADData* wpData = WPAD_Data(0);

    if (buttonsDown & WPAD_BUTTON_HOME) {
      currentMenu->toggleShow();
    }
    currentMenu->update(buttonsDown);

    currentSystem->interact(buttonsDown, buttonsHeld, wpData);

    // WPAD Data
    Vec2 cursor = Draw::screenToWorld({wpData->ir.x, wpData->ir.y});

    grr_circle({wpData->ir.x, wpData->ir.y}, 5.f, {255, 0, 0, 255});

    currentSystem->draw({255, 255, 255, 255});
    currentMenu->draw(screenW, screenH);

    GRRLIB_Render();
  }

  GRRLIB_FreeTTF(font);
  GRRLIB_Exit();
  exit(0);
}
