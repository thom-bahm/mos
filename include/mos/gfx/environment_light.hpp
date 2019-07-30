#pragma once
#include <mos/gfx/box.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/cube_camera.hpp>

namespace mos {
namespace gfx {

class Environment_light;
using Optional_environment_light = std::optional<Environment_light>;

/** Environment light, based on a cube map. */
class Environment_light final {
public:
  /** @param extent Describes how big the environment is, for parallax/box correction. */
  explicit Environment_light(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.0f),
                   const glm::vec3 &extent = glm::vec3(50.0f),
                   float strength = 0.0f,
                   float falloff = 0.1f);

  Environment_light(const std::string &directory, const std::string &path,
      const glm::mat4 &parent_transform = glm::mat4(1.0f));

  /** Set position. */
  void position(const glm::vec3 &position);

  /** Get position. */
  glm::vec3 position() const;

  /** Set extent and recalculate cube camera projections.*/
  void extent(const glm::vec3 &extent);

  /** Get extent. */
  glm::vec3 extent() const;

  /** Check if a point is inside the environment light box. */
  bool inside(const glm::vec3 &point) const;

  /** Get one of the six cube cameras, for rendering. */
  mos::gfx::Camera camera(size_t index) const;

  /** Strength. */
  float strength;

  /** Falloff. */
  float falloff{0.1f};
private:
  Box box_;
  Cube_camera cube_camera_;
};
}
}
