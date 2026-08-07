#include "WiiSystem.h"

Goo::Goo(unsigned int w, unsigned int h) : m_w(w), m_h(h)
{
  reset();

  m_mouse = createBody({0.f, 0.f}, 0.2f, true);
  Body& mouse = getBody(m_mouse);
  mouse.isVisible = false;
}

void Goo::interact(uint32_t buttonsDown, uint32_t buttonsHeld, WPADData* wpadData)
{
  Body& mouse = getBody(m_mouse);
  mouse.setPos(Draw::screenToWorld({wpadData->ir.x, wpadData->ir.y}));

  if (buttonsDown & WPAD_BUTTON_MINUS) {
    reset();
    return;
  }

  if (buttonsHeld & WPAD_BUTTON_B) {
    static std::vector<ID> toDelete;

    for (auto& constraint : constraints()) {
      auto& s = std::get<SpringConstraint>(constraint);
      const auto& b0 = getBody(s.b0());
      const auto& b1 = getBody(s.b1());

      if (intersects(mouse.pos(), mouse.lastPos(), b0.pos(), b1.pos())) {
        toDelete.push_back(s.id());
      }
    }

    for (ID id : toDelete) {
      removeConstraint(id);
    }
    toDelete.clear();
  }
}

bool Goo::intersects(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d) const
{
  auto orient = [](const Vec2& p, const Vec2& q, const Vec2& r) {
    return (q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x);
  };

  double o1 = orient(a, b, c);
  double o2 = orient(a, b, d);
  double o3 = orient(c, d, a);
  double o4 = orient(c, d, b);

  return (o1 * o2 < 0.0) && (o3 * o4 < 0.0);
}

void Goo::reset()
{
  float spacing = 0.8;
  Vec2 offset{(m_w * spacing / -2.f) + spacing / 2.f, m_h * spacing * -0.8f};

  constexpr float clothK = 100.0;
  constexpr float clothD = 5.0;

  System::clear();

  std::vector<ID> lastRow;
  for (int y = 0; y < m_h; ++y) {
    std::vector<ID> row;
    for (int x = 0; x < m_w; ++x) {
      ID b = createBody({x * spacing + offset.x, y * spacing + offset.y}, 0.1f, (y == 0), 0.1f);
      if (x > 0) {
        ID c = createConstraint<SpringConstraint>(b, row[x - 1], clothK, clothD);
      }
      row.push_back(b);

      if (x < lastRow.size()) {
        ID c = createConstraint<SpringConstraint>(b, lastRow[x], clothK, clothD);
      }
    }
    lastRow = std::move(row);
  }
}

Pendulum::Pendulum(unsigned int links, float length)
{
  m_anchor = createBody({0.f, 0.f}, 0.2f, true);

  float link_length = length / links;
  ID last_body = m_anchor;
  for (unsigned int i = 1; i <= links; ++i) {
    ID body = createBody({link_length * i, 0.f}, 0.2f, false, 0.1f);
    createConstraint<DistanceConstraint>(body, last_body);
    last_body = body;
  }

  m_mouse = createBody({length, 0.f}, 0.2f, true);
  Body& mouse = getBody(m_mouse);
  mouse.isVisible = false;

  m_mouse_link = createConstraint<SpringConstraint>(m_mouse, last_body, 30.f);
  Constraint& mouse_link = getConstraintBase(m_mouse_link);
  mouse_link.isVisible = false;
  mouse_link.isEnabled = false;
}

void Pendulum::interact(uint32_t buttonsDown, uint32_t buttonsHeld, WPADData* wpadData)
{
  Body& mouse = getBody(m_mouse);
  mouse.setPos(Draw::screenToWorld({wpadData->ir.x, wpadData->ir.y}));

  Constraint& mouse_link = getConstraintBase(m_mouse_link);
  if (buttonsHeld & WPAD_BUTTON_B) {
    mouse_link.isEnabled = true;
  }
  else {
    mouse_link.isEnabled = false;
  }
}

Worm::Worm(float length) : Pendulum(50, length)
{
  Body& anchor = getBody(m_anchor);
  anchor.isKinematic = false;

  ID border_id =
      createConstraint<PositionConstraint>(Draw::screenToWorld({0.f, 0.f}), Draw::screenToWorld({640.f, 480.f}), 1.f);
  PositionConstraint* border = static_cast<PositionConstraint*>(&getConstraintBase(border_id));
  border->addSystem();
}