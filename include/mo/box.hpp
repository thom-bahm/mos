#ifndef MO_BOX_HPP
#define MO_BOX_HPP

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <algorithm>
#include <tuple>
#include "vertex.hpp"

namespace mo
{

/*!
 * Data from a box box intersection.
 *
 * \brief The BoxIntersection struct
 */
struct BoxIntersection {
    /*!
     * True if the box intersect.
     */
    bool intersects;
    /*!
     * Normal of the intersection.
     */
    glm::vec3 normal;
    /*!
     * Distance of the intersection.
     */
    float distance;
};

/*!
 * Intersection data from a ray and box intersection.
 *
 * \brief The RayIntersection struct
 */
struct RayIntersection {
    RayIntersection(bool hit, float distance = 0.0f): intersects(hit), distance(distance){
    }

    /*!
     * \brief intersects
     *
     * True if ray intersects with the box.
     */
    bool intersects;

    /*!
     * \brief distance
     *
     * Distance from
     */
    float distance;
};

class Box
{
public:
    template<class VertexIt>
    /*!
     * \brief Box constructor
     * \param begin Vertex iterator
     * \param end Vertex iterator
     * \param transform location of the box
     *
     * Constructor for an AABB box, created from vertices.
     *
     */
    Box(VertexIt begin, VertexIt end, const glm::mat4 & transform) : transform_(transform) {
        glm::vec3 min, max;

        if (begin != end) {
            auto x_extremes = std::minmax_element(begin, end, [](const Vertex& left, const Vertex& right) {
                return left.position.x < right.position.x;
            });

            auto y_extremes = std::minmax_element(begin, end, [](const Vertex& left, const Vertex& right) {
                return left.position.y < right.position.y;
            });

            auto z_extremes = std::minmax_element(begin, end, [](const Vertex& left, const Vertex& right) {
                return left.position.z < right.position.z;
            });

            min = glm::vec3(x_extremes.first->position.x, y_extremes.first->position.y, z_extremes.first->position.z);
            max = glm::vec3(x_extremes.second->position.x, y_extremes.second->position.y, z_extremes.second->position.z);
        }
        min_ = min;
        max_ = max;
    }

    /*!
     * @brief Box default constructor.
     */
    Box();

    /*!
     * \brief Box constructor.
     * \param min corner.
     * \param max corner.
     * \param transform location.
     */
    Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform);

    /*!
     * \brief min
     * \return
     */
    glm::vec3 min() const;

    /*!
     * \brief max
     * \return
     */
    glm::vec3 max() const;

    /*!
     * \brief intersect
     * \param point1
     * \param point2
     * \return
     */
    RayIntersection intersect(glm::vec3 point1, glm::vec3 point2);

    /*!
     * \brief intersect
     * \param origin
     * \param direction
     * \param t1
     * \param t2
     * \return
     */
    RayIntersection intersect(const glm::vec3 & origin, const glm::vec3 direction, float t1, float t2);

    /*!
     * \brief intersects
     * \param other
     * \return
     */
    BoxIntersection intersects(const Box & other);

    /*!
     * \brief transform
     */
    glm::mat4 transform() const;
    void transform(const glm::mat4 & transform);
private:
    glm::vec3 min_;
    glm::vec3 max_;
    glm::mat4 transform_;


};
}
#endif // MO_BOX_HPP
