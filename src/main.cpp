#include <grrlib.h>
#include <ogc/lwp_watchdog.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>

#include "font.c"

#include "ui/menu.h"

#include "fizz/Draw.h"
#include "fizz/System.h"

#include "fizz/constraints/DistanceConstraint.h"
#include "fizz/constraints/SpringConstraint.h"

u32 col_to_u32(Draw::Color c)
{
  return (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a;
}
void grr_circle(const DVec2& center, float radius, Draw::Color color)
{
  GRRLIB_Circle(center.x, center.y, radius, col_to_u32(color), 1);
}
void grr_line(const DVec2& p0, const DVec2& p1, Draw::Color color)
{
  GRRLIB_Line(p0.x, p0.y, p1.x, p1.y, col_to_u32(color));
}

void resetCloth(System& sys, std::vector<int>& cIds)
{
  constexpr int gW = 10;
  constexpr int gH = 5;
  constexpr double spacing = 0.8;
  constexpr double clothK = 1500.0;
  constexpr double clothD = 10.0;

  sys.clear();
  cIds.clear();

  std::vector<Body*> lastRow;
  for (int y = 0; y < gH; ++y) {
    std::vector<Body*> row;
    for (int x = 0; x < gW; ++x) {
      Body* b = sys.createBody({(double)x * spacing + 1.0, (double)y * spacing + 1.0}, 0.1, (y == 0));
      if (x > 0) {
        Constraint* c = sys.createConstraint<SpringConstraint>(b, row[x - 1], clothK, clothD);
        cIds.push_back(c->id());
      }
      row.push_back(b);

      if (x < lastRow.size()) {
        Constraint* c = sys.createConstraint<SpringConstraint>(b, lastRow[x], clothK, clothD);
        cIds.push_back(c->id());
      }
    }
    lastRow = std::move(row);
  }
}

int main(int argc, char** argv)
{
  srand(gettime());

  GRRLIB_Init();
  GRRLIB_SetBackgroundColour(255, 255, 255, 255);

  WPAD_Init();
  WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

  // Load font
  GRRLIB_ttfFont* font = GRRLIB_LoadTTF(NotoSansMono_Regular_ttf, NotoSansMono_Regular_ttf_len);

  u64 lastTime = gettime();
  double deltaTime = 0.0;

  Draw::setCircleFunc(grr_circle);
  Draw::setLineFunc(grr_line);

  System sys;
  std::vector<int> cIds;
  resetCloth(sys, cIds);

  // Menu
  struct pluh {
    void go() { SYS_Report("PLUH\n"); }
  };

  Menu menu;
  MenuStyle& menuStyle = menu.style();
  menuStyle.font = font;
  menuStyle.fontSize = 18;

  pluh p;
  menu.addItem("pluh", [&] { p.go(); });

  auto action = [] { SYS_Report("Click\n"); };
  menu.addItem("Menu Item", action);
  menu.addItem("Here's another", action);
  menu.addItem("Testing", action);
  menu.addItem("More testing", action);
  menu.addItem("The items seem to fit well", action);

  bool running = true;
  menu.addItem("Exit", [&] { running = false; });

  while (running) {
    // Delta time calculation
    u64 currentTime = gettime();
    u64 diff = currentTime - lastTime;
    deltaTime = (double)diff / (TB_TIMER_CLOCK * 1000);
    lastTime = currentTime;

    sys.update(deltaTime);

    // WPAD Buttons
    WPAD_ScanPads();
    u32 buttonsDown = WPAD_ButtonsDown(0);
    if (buttonsDown & WPAD_BUTTON_HOME) {
      menu.toggleShow();
    }
    menu.update(buttonsDown);

    if ((buttonsDown & WPAD_BUTTON_A) && !cIds.empty()) {
      int idIdx = rand() % cIds.size();
      int id = cIds.at(idIdx);

      sys.removeConstraint(id);
      cIds.erase(cIds.begin() + idIdx);
    }

    if (buttonsDown & WPAD_BUTTON_B) {
      resetCloth(sys, cIds);
    }

    // WPAD Data
    WPADData* wpData = WPAD_Data(0);
    grr_circle({wpData->ir.x, wpData->ir.y}, 5.f, {255, 0, 0, 255});

    sys.draw({0, 0, 0, 255});
    menu.draw();

    GRRLIB_Render();
  }

  GRRLIB_FreeTTF(font);
  GRRLIB_Exit();
  exit(0);
}
