#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Knat : public Unit {
 public:
  Knat(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate(float rotate_angular_speed);
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  float momentum_{0.0f}, rotation_momentum_{0.0f};
  const float mass_{0.6f}, rotation_mass_{0.3f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
};
}  // namespace battle_game::unit