#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(Sounds sounds, Sound_streams sound_streams, Listener listener)
    : sounds(std::move(sounds)), sound_streams(std::move(sound_streams)),
      listener(listener) {}
} // namespace aud
} // namespace mos
