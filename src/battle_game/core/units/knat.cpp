#include "knat.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
}  // namespace

Knat::Knat(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
    if (!~tank_body_model_index) {
        auto mgr = AssetsManager::GetInstance();
        {
            /* Tank Body - Improved with Rounded Edges and More Detail */
            std::vector<ObjectVertex> body_vertices = {
                {{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Top Left
                {{-0.6f, 0.6f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Top Left Rounded
                {{0.6f, 0.6f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Top Right Rounded
                {{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Top Right
                {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Bottom Left
                {{-0.6f, -0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Bottom Left Rounded
                {{0.6f, -0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Bottom Right Rounded
                {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Bottom Right
                {{-0.4f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Front Left
                {{0.4f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Front Right
                {{-0.4f, 0.6f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Front Left Rounded
                {{0.4f, 0.6f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // Front Right Rounded
            };

            std::vector<uint32_t> body_indices = {
                0, 1, 2, 0, 2, 3, // Top
                4, 5, 6, 4, 6, 7, // Bottom
                8, 9, 10, 8, 10, 11, // Front
                0, 1, 5, 0, 5, 4, // Left Side
                1, 2, 6, 1, 6, 5, // Left Side Rounded
                2, 3, 7, 2, 7, 6, // Right Side
                3, 0, 4, 3, 4, 7  // Back
            };

            tank_body_model_index = mgr->RegisterModel(body_vertices, body_indices);
        }

        {
            /* Tank Turret - Improved with Smoother Edges and More Detail */
            std::vector<ObjectVertex> turret_vertices;
            std::vector<uint32_t> turret_indices;
            const int precision = 60;
            const float inv_precision = 1.0f / float(precision);

            // Turret Base (Circular)
            for (int i = 0; i < precision; i++) {
                auto theta = (float(i) + 0.5f) * inv_precision;
                theta *= glm::pi<float>() * 2.0f;
                auto sin_theta = std::sin(theta);
                auto cos_theta = std::cos(theta);
                turret_vertices.push_back({{sin_theta * 0.4f, cos_theta * 0.4f},
                                             {0.0f, 0.0f},
                                             {0.7f, 0.7f, 0.7f, 1.0f}});
                turret_indices.push_back(i);
                turret_indices.push_back((i + 1) % precision);
            }

            // Turret Top (Square)
            turret_vertices.push_back({{-0.2f, 0.6f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
            turret_vertices.push_back({{0.2f, 0.6f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
            turret_vertices.push_back({{-0.2f, 0.8f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
            turret_vertices.push_back({{0.2f, 0.8f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
            turret_indices.push_back(precision);
            turret_indices.push_back(precision + 1);
            turret_indices.push_back(precision + 2);
            turret_indices.push_back(precision + 1);
            turret_indices.push_back(precision + 2);
            turret_indices.push_back(precision + 3);

            // Connect Base to Top
            for (int i = 0; i < precision; i++) {
                turret_indices.push_back(i);
                turret_indices.push_back((i + 1) % precision);
                turret_indices.push_back(precision);
            }

            tank_turret_model_index = mgr->RegisterModel(turret_vertices, turret_indices);
        }
    }
}

void Knat::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

void Knat::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate(glm::radians(180.0f));
  Fire();
}

void Knat::TankMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    offset.y = (1.0f - mass_) * offset.y + mass_ * momentum_;
    momentum_ = offset.y;
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    } else {
        momentum_ = 0.0f;
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset = (1.0f - rotation_mass_) * rotation_offset +
                      rotation_mass_ * rotation_momentum_;
    rotation_momentum_ = rotation_offset;
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

void Knat::TurretRotate(float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      float mouse_direction_ = 0.0f;
      float rotation_offset = kSecondPerTick * rotate_angular_speed * GetSpeedScale();
      if (diff.x > 0.0f) {
        mouse_direction = std::atan(diff.y / diff.x);
        if (diff.y < 0.0f) { mouse_direction += glm::pi<float>() * 2.0f; }
      } else if (diff.x < 0.0f) {
        mouse_direction = std::atan(diff.y / diff.x) + glm::pi<float>();
      } else {
        mouse_direction = diff.y > 0.0f ? glm::pi<float>() * 0.5f : glm::pi<float>() * 1.5f;
      }
    }
  }
}

void Knat::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.
        // Calculate the recoil
        float recoil = std::cos(turret_rotation_ - rotation_);
        momentum_ -= recoil;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool Knat::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *Knat::UnitName() const {
  return "Tanky";
}

const char *Knat::Author() const {
  return "UntitledNotebook";
}
}  // namespace battle_game::unit
