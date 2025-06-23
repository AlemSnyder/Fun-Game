#pragma once

#include "gui/render/gpu_data/data_types.hpp"
#include "gui/ui/render_programs.hpp"
#include "types.hpp"

#include <glaze/glaze.hpp>

#include <string>
#include <vector>

// virtual object class
// if it has an ID then it's instanced
// and im always doing elements

namespace world {

namespace entity {

struct global_illumination_t {
    float ambient;
    float specular;
    float diffuse;
};

struct remapping_t {
    std::unordered_map<ColorInt, ColorInt> color_map;

    void read_map(std::unordered_map<std::string, std::string> input);

    std::unordered_map<std::string, std::string> write_map() const;
};

enum class OBJECT_TYPE {
    ENTITY,
    TILE_OBJECT,
};

struct model_t {
    std::filesystem::path path;
    std::optional<std::vector<remapping_t>> colors;
    global_illumination_t globals;
};

struct object_t {
    std::string name;
    OBJECT_TYPE type;
    std::vector<model_t> models;
    // define interactions
    // like on drop etc
    // maybe it has a health
    std::optional<std::string> listeners;
    std::optional<std::filesystem::path> ai;
};

/**
 * @brief Complex things in the world. Anything more complex than a tile. Represents all
 * things with the same id.
 */
class Object {
 public:
    virtual ~Object() {}

    //    virtual void render()

    /**
     * @brief Unique String identifier. Must start with module name.
     *
     * @return std::string string identification
     */
    virtual std::string identification() const = 0;

    // one for each mesh
    //    virtual std::vector<const gui::gpu_data::GPUDataElementsInstanced*>
    //    renderable_data() const = 0;

    /**
     * @brief Set renderer for this object type.
     *
     * @param render_program_t& programs
     */
    virtual void init_render(render_programs_t& programs) const = 0;

    /**
     * @brief Update to be run once per frame.
     */
    virtual void update() = 0;
};

/**
 * @brief Instance of an object
 */
class ObjectInstance {
 public:
    virtual ~ObjectInstance() {}

    /**
     * @brief Destroy this ObjectInstance.
     * 
     * @details Destroys Ojbect in world. May drop items or other action. Calling this method should remove this instance from what ever stores it, thus calling the deconstructor.
     */
    virtual void destroy() = 0;
    /**
     * @brief Get the health of the object.
     * 
     * @return size_t health
     */
    virtual size_t get_health() const = 0;
    /**
     * @brief Deal damage to this object
     * 
     * @param damage amount of damage to be dealt
     */
    virtual void take_damage(size_t damage) = 0;
    /**
     * @brief Get the position of this object in the world.
     * 
     * @return glm::vec3 position
     */
    virtual glm::vec3 get_position() const = 0;
    /**
     * @brief Get object this object instance belongs to.
     */
    virtual std::shared_ptr<Object> get_object() = 0;
    /**
     * @brief Get object this object instance belongs to.
     */
    virtual std::shared_ptr<const Object> get_object() const = 0;
};

} // namespace entity

} // namespace world

template <>
struct glz::meta<world::entity::OBJECT_TYPE> {
    using enum world::entity::OBJECT_TYPE;
    static constexpr auto value = enumerate(ENTITY, TILE_OBJECT);
};

template <>
struct glz::meta<world::entity::remapping_t> {
    using T = world::entity::remapping_t;

    static constexpr auto value = object("map", custom<&T::read_map, &T::write_map>);
};

template <>
inline glz::detail::any_t::operator std::vector<world::entity::remapping_t>() const {
    assert(false && "Not Implemented");
    return {};
}
