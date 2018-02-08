#ifndef MOS_AUDIO_BATCH_HPP
#define MOS_AUDIO_BATCH_HPP

#include <initializer_list>
#include <vector>
#include <mos/audio/audio_buffer_source.hpp>
#include <mos/audio/audio_listener.hpp>
#include <mos/audio/audio_stream_source.hpp>

namespace mos {
/**
* @brief The AudioBatch class updates everything in the audio system.
*/
class AudioScene final{
public:
  using AudioBufferSources = std::vector<AudioBufferSource>;
  using AudioStreamSources = std::vector<AudioStreamSource>;

  template <class Tb, class Ts>
  /**
   * @brief AudioBatch
   * @param buffers_begin
   * @param buffers_end
   * @param streams_begin
   * @param streams_end
   * @param listener
   */
  AudioScene(const Tb buffers_begin, const Tb buffers_end,
             const Ts streams_begin, const Ts streams_end,
             const AudioListener &listener)
      : buffer_sources(buffers_begin, buffers_end),
        stream_sources(streams_begin, streams_end), listener(listener) {}

  /**
   * @brief AudioBatch
   * @param buffer_sources
   * @param stream_sources
   * @param listener
   */
  AudioScene(const std::initializer_list<AudioBufferSource> buffer_sources,
             const std::initializer_list<AudioStreamSource> stream_sources,
             const AudioListener &listener);

  AudioScene();

  /**
   * @brief buffer_sources to update.
   */
  const AudioBufferSources buffer_sources;

  /**
   * @brief stream_sources to update.
   */
  const AudioStreamSources stream_sources;

  /**
   * @brief listener to update
   */
  const AudioListener listener;
};
}

#endif // MOS_AUDIO_BATCH_HPP
