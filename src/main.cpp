#include <grrlib.h>
#include <ogc/lwp_watchdog.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include "font.c"

#include "menu.h"

#include "fizz/Draw.h"
#include "fizz/System.h"

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

void resetCloth(System& sys, std::vector<int>& cIds)
{
  constexpr int gW = 15;
  constexpr int gH = 10;
  constexpr float spacing = 0.8;
  constexpr Vec2 offset{(-gW * spacing / 2.f) + spacing / 2.f, -gH * spacing * 0.8};

  constexpr float clothK = 1500.0;
  constexpr float clothD = 10.0;

  sys.clear();
  cIds.clear();

  std::vector<ID> lastRow;
  for (int y = 0; y < gH; ++y) {
    std::vector<ID> row;
    for (int x = 0; x < gW; ++x) {
      ID b = sys.createBody({x * spacing + offset.x, y * spacing + offset.y}, 0.1f, (y == 0));
      if (x > 0) {
        ID c = sys.createConstraint<SpringConstraint>(b, row[x - 1], clothK, clothD);
        cIds.push_back(c);
      }
      row.push_back(b);

      if (x < lastRow.size()) {
        ID c = sys.createConstraint<SpringConstraint>(b, lastRow[x], clothK, clothD);
        cIds.push_back(c);
      }
    }
    lastRow = std::move(row);
  }
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
  GRRLIB_ttfFont* font = GRRLIB_LoadTTF(NotoSansMono_Regular_ttf, NotoSansMono_Regular_ttf_len);

  u64 lastTime = gettime();
  float deltaTime = 0.0;

  Draw::setCircleCallback(grr_circle);
  Draw::setLineCallback(grr_line);

  auto& transform = Draw::getTransform();
  transform.scale = 30;
  transform.offset = Vec2(screenW, screenH) / transform.scale / 2.f;

  System* currentSystem = nullptr;
  Menu* currentMenu = nullptr;
  auto setCurrentSystem = [&](void* system) {
    assert(system != nullptr);

    currentSystem = static_cast<System*>(system);

    if (currentMenu)
      currentMenu->show(false);
  };

  System cloth;
  setCurrentSystem(&cloth);
  std::vector<int> cIds;
  resetCloth(cloth, cIds);

  System pendulum;
  {
    ID anchor = pendulum.createBody({0.f, 0.f}, 0.2, true);
    ID mass = pendulum.createBody({7.5f, 0.f}, 0.2);
    pendulum.createConstraint<DistanceConstraint>(anchor, mass);
  }

  System double_pendulum;
  {
    ID anchor = double_pendulum.createBody({0.f, 0.f}, 0.2, true);
    ID mass0 = double_pendulum.createBody({0.f, -3.75f}, 0.2);
    ID mass1 = double_pendulum.createBody({3.75f, -3.75f}, 0.2);

    double_pendulum.createConstraint<DistanceConstraint>(anchor, mass0);
    double_pendulum.createConstraint<DistanceConstraint>(mass0, mass1);
  }

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
  sceneMenu.addItem("Goo", setCurrentSystem, &cloth);
  sceneMenu.addItem("Pendulum", setCurrentSystem, &pendulum);
  sceneMenu.addItem("Double pendulum", setCurrentSystem, &double_pendulum);
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
    if (buttonsDown & WPAD_BUTTON_HOME) {
      currentMenu->toggleShow();
    }
    currentMenu->update(buttonsDown);

    if ((buttonsDown & WPAD_BUTTON_A) && !cIds.empty()) {
      int idIdx = rand() % cIds.size();
      int id = cIds.at(idIdx);

      cloth.removeConstraint(id);
      cIds.erase(cIds.begin() + idIdx);
    }

    if (buttonsDown & WPAD_BUTTON_B) {
      resetCloth(cloth, cIds);
    }

    // WPAD Data
    // WPADData* wpData = WPAD_Data(0);
    // grr_circle({wpData->ir.x, wpData->ir.y}, 5.f, {255, 0, 0, 255});

    currentSystem->draw({255, 255, 255, 255});
    currentMenu->draw(screenW, screenH);

    GRRLIB_Render();
  }

  GRRLIB_FreeTTF(font);
  GRRLIB_Exit();
  exit(0);
}
