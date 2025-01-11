#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Sprint : public Unit {
 public:
  Sprint(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  void RenderLifeBar() override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
  float sprint_{1.0f};
  float sprint_scale_{2.0f};
  float sprint_time_{1.0f};
  float sprint_recover_time_{2.0f};
  glm::vec2 sprintbar_offset_{0.0f, 1.5f};
  glm::vec4 sprintbar_front_color_{0.0f, 0.0f, 1.0f, 0.9f};

 private:
  float fadeout_health_{1.0f};
};
}  // namespace battle_game::unit
