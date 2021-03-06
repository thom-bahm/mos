#pragma once
#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Fog with exponential falloff and near/far color blending. */
class Fog final {
public:
  /** @param color Color of the fog.
   * @param attenuation_factor Fog density. */
  Fog(const glm::vec3 &color,
      float attenuation_factor = 0.0f);

  /** @param color_near Close fog color.
   * @param color_far Far fog color.
   * @param attenuation_factor Fog density. */
  explicit Fog(const glm::vec3 &color_near = glm::vec3(1.0f),
      const glm::vec3 &color_far = glm::vec3(1.0f),
      float attenuation_factor = 0.0f);

  glm::vec3 color_near;
  glm::vec3 color_far;
  float attenuation_factor;
};
}
}

