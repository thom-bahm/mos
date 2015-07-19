/* 
 * File:   Source.h
 * Author: morgan
 *
 * Created on July 28, 2014, 3:47 PM
 */

#ifndef SOURCE_H
#define	SOURCE_H

#include <glm/vec3.hpp>

namespace mos {

/*!
 * \brief The Source class
 *
 * Base class for the audio sources. Either streamed or direct from memory.
 */
class Source {
public:
    /*!
     * @brief Source constructor
     * @param position
     * @param velocity
     * @param pitch
     * @param gain
     * @param loop
     * @param playing
     */
    Source(const glm::vec3 position = glm::vec3(0.0f),
           const glm::vec3 velocity = glm::vec3(0.0f),
           const float pitch = 1.0f,
           const float gain = 1.0f,
           const bool loop = false,
           const bool playing = false,
           const float obstructed = 0.0f,
           const bool outside = false);
    virtual ~Source();

    /*!
     * @brief unique id
     * @return id
     */
    unsigned int id() const;

    /*!
     * @brief position
     */
    glm::vec3 position;

    /*!
     * @brief velocity
     */
    glm::vec3 velocity;

    /*!
     * @brief pitch
     */
    float pitch;

    /*!
     * @brief gain
     */
    float gain;

    /*!
     * @brief loop
     */
    bool loop;

    /*!
     * @brief playing
     */
    bool playing;

    /*!
     * @brief Obstructed by geometry.
     * Describes if the source is obstructed by geometry. 1.0 means fully obstructed, 0.0f means none.
     */
    float obstructed;

    /*!
     * @brief Sound otside or not.
     * Describes if the source is outside or inside. This affects the reverb effect.
     */
    bool outside;
private:
    unsigned int id_;

};

}

#endif	/* SOURCE_H */
