﻿#pragma once
#include <glad/glad.h>
#include <optional>
#include <initializer_list>
#include <unordered_map>
#include <array>
#include <future>
#include <mos/gfx/scene.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/text.hpp>
#include <mos/gfx/cloud.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/box.hpp>
#include <mos/gfx/scenes.hpp>
#include <mos/gfx/lights.hpp>

namespace mos {
namespace gfx {

/** Render geometry shapes with OpenGL. */
class Renderer final {
public:
  /** Inits the renderer, creates an OpenGL context with GLAD. */
  explicit Renderer(const glm::ivec2 &resolution, const int samples = 1);
  Renderer(const Renderer &renderer) = delete;
  Renderer(const Renderer &&renderer) = delete;
  Renderer & operator=(const Renderer & renderer) = delete;
  Renderer & operator=(const Renderer && renderer) = delete;
  ~Renderer() = default;

  /** Loads a model into renderers own memory. */
  void load(const Model &model);

  void load(const Models & models);

  /** Unloads a model from renderers own memory. */
  void unload(const Model &model);

  /** Load a mesh in to memory. */
  void load(const Mesh &mesh);
  void load(const Shared_mesh &mesh);

  /** Unloads a mesh from memory. */
  void unload(const Mesh &mesh);
  void unload(const Shared_mesh &mesh);

  /** Loads a shared texture into renderer memory. */
  void load(const Shared_texture_2D &texture);

  /** Loads a texture into renderer memory. */
  void load_or_update(const Texture_2D &texture);

  /** Unloads a shared texture from renderer memory. */
  void unload(const Shared_texture_2D &texture);

  /** Render multiple scenes. */
  void render(const Scenes &scenes,
              const glm::vec4 &color = {.0f, .0f, .0f, 1.0f},
              const glm::ivec2 &resolution = glm::ivec2(128, 128));

  /** Clear all internal buffers/memory. */
  void clear_buffers();

private:
  using Time_point =  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  class Buffer {
  public:
    Buffer(GLenum type, GLsizeiptr size, const void *data, GLenum hint, Time_point modified);
    Buffer(const Buffer &buffer) = delete;
    Buffer(Buffer &&buffer) noexcept;
    Buffer & operator=(const Buffer &buffer) = delete;
    Buffer & operator=(Buffer &&buffer) noexcept;
    ~Buffer();

    GLuint id{0};
    Time_point modified;
  private:
    void release();
  };

  class Render_buffer {
  public:
    explicit Render_buffer(int resolution);
    explicit Render_buffer(const glm::ivec2 &resolution);
    ~Render_buffer();
    Render_buffer(Render_buffer &&target) noexcept;
    Render_buffer(const Render_buffer &target) = delete;
    Render_buffer & operator=(const Render_buffer &target) = delete;
    Render_buffer & operator=(Render_buffer &&target) noexcept;
    GLuint id{0};
    glm::ivec2 resolution() const;
  private:
    void release();
  };

  class Vertex_array {
  public:
    explicit Vertex_array(const Cloud &cloud,
                          std::unordered_map<unsigned int, Buffer> &array_buffers);
    explicit Vertex_array(const Mesh & mesh,
                          std::unordered_map<unsigned int, Buffer> &array_buffers,
                          std::unordered_map<unsigned int, Buffer> &element_array_buffers);
    ~Vertex_array();
    Vertex_array(Vertex_array &&array) noexcept;
    Vertex_array(const Vertex_array &target) = delete;
    Vertex_array & operator=(const Vertex_array &array) = delete;
    Vertex_array & operator=(Vertex_array &&target) noexcept;
    GLuint id{0};
  private:
    void release();
  };

  class Texture_buffer_2D {
  public:
    explicit Texture_buffer_2D(const Texture_2D &texture_2d);
    Texture_buffer_2D(
        GLint internal_format, GLenum external_format, int width, int height,
        GLint filter_min, GLint filter_mag, GLint wrap, const void *data,
        const Time_point &modified = std::chrono::system_clock::now());

   ~Texture_buffer_2D();
    Texture_buffer_2D(const Texture_buffer_2D &buffer) = delete;
    Texture_buffer_2D(Texture_buffer_2D &&buffer) noexcept;
    Texture_buffer_2D & operator=(const Texture_buffer_2D &buffer) = delete;
    Texture_buffer_2D & operator=(Texture_buffer_2D &&buffer) noexcept;
    GLuint texture{0};
    Time_point modified;
  private:
    void release();
  };

  using Render_buffers = std::unordered_map<unsigned int, Render_buffer>;
  using Texture_buffers = std::unordered_map<unsigned int, Texture_buffer_2D>;
  using Array_buffers = std::unordered_map<unsigned int, Buffer>;
  using Element_array_buffers = std::unordered_map<unsigned int, Buffer>;
  using Vertex_arrays = std::unordered_map<unsigned int, Vertex_array>;

  class Frame_buffer {
  public:
    explicit Frame_buffer(const Texture_target & target,
                          Texture_buffers &texture_buffers,
                          Render_buffers &render_buffers);
    ~Frame_buffer();
    Frame_buffer(Frame_buffer &&buffer) noexcept;
    Frame_buffer(const Frame_buffer &buffer) = delete;
    Frame_buffer & operator=(const Frame_buffer &buffer) = delete;
    Frame_buffer & operator=(Frame_buffer &&buffer) noexcept;
    GLuint id{0};
  private:
    void release();
  };

  using Frame_buffers = std::unordered_map<unsigned int, Frame_buffer>;

  class Shader {
  public:
    Shader(const std::string &source, GLuint type, const std::string& name);
    ~Shader();
    const GLuint id;
    Shader(const Shader &shader) = delete;
    Shader(Shader &&shader) = delete;
    Shader & operator=(const Shader &shader) = delete;
    Shader & operator=(Shader &&shader) = delete;
  };

  struct Program {
    Program();
    ~Program();
    Program(const Program &program) = delete;
    Program(const Program &&program) = delete;
    Program & operator=(const Program &program) = delete;
    Program & operator=(const Program &&program) = delete;
    const GLuint program;
    void check(const std::string &name);
    void link(const std::string &name);
  };

  struct Light_uniforms {
    GLint position;
    GLint color;
    GLint strength;
    GLint view;
    GLint projection;
    GLint angle;
    GLint direction;
  };

  /** Uniforms for the particle shader program. */
  struct Cloud_program : public Program {
    explicit Cloud_program(const std::string &name, const Shader &functions_shader);
    GLint model_view_projection;
    GLint model_view;
    GLint model{};
    GLint projection;
    GLint texture;
    GLint resolution;    
    GLint camera_resolution;
    GLint camera_position;
    GLint camera_far{};
    GLint camera_near{};

    struct Environment_uniforms {
      GLint map;
      GLint position;
      GLint extent;
      GLint strength;
      GLint falloff;
    };
    std::array<Environment_uniforms, 2> environment_maps {};
    std::array<Light_uniforms, 4> lights{};
  };

  /** Uniforms for the bounding box shader program. */
  struct Box_program : public Program {
    Box_program();
    GLint model_view_projection;
  };

  struct Bloom_program : public Program {
    Bloom_program();
    GLint color_sampler;
  };

  //TODO: rename
  struct Depth_of_field_program : public Program {
    Depth_of_field_program();
    GLint color_sampler;
    GLint blurred_color_sampler;
    GLint depth_sampler;
    GLint camera_far;
    GLint camera_near;
    GLint camera_focus_distance;
    GLint camera_fstop;
  };


  struct Compositing_program : public Program {
    Compositing_program();
    GLint color_sampler;
    GLint bloom_sampler;
    GLint bloom_strength;
  };

  struct Blur_program : public Program {
    Blur_program();
    GLint color_sampler;
    GLint horizontal;
  };

  struct Depth_program : public Program {
    Depth_program();
    GLint model_view_projection;
    GLint albedo_sampler;
    GLint albedo;
    GLint emission;
  };

  /** Uniforms for the propagate shader. */
  class Propagate_program : public Program {
  public:
    Propagate_program();
    GLint environment_sampler;
    GLint environment_albedo_sampler;
    GLint side;
  };

  /** Uniforms for the environment shader. */
  class Environment_program : public Program {
  public:
    Environment_program();
    GLint model_view_projection;
    GLint model_matrix;
    GLint normal_matrix;
    std::array<GLint, 4> depth_bias_mvps{};

    struct Environment_uniforms {
      GLint map;
      GLint position;
      GLint extent;
      GLint strength;
    };

    GLint material_albedo_sampler;
    GLint material_emission_sampler;
    GLint material_albedo;
    GLint material_roughness;
    GLint material_metallic;
    GLint material_index_of_refraction;
    GLint material_alpha;
    GLint material_transmission;
    GLint material_emission;
    GLint material_ambient_occlusion;

    GLint camera_resolution;
    GLint camera_position;
    GLint camera_near;
    GLint camera_far;

    std::array<GLuint, 4> shadow_samplers{};
    std::array<Light_uniforms, 4> lights{};

    GLint fog_color_near;
    GLint fog_color_far;
    GLint fog_attenuation_factor;

    GLint brdf_lut;
  };

  /** Uniforms for the standard shader. */
  class Standard_program : public Program {
  public:
    explicit Standard_program(const Shader & functions_shader);
    GLint model_view_projection;
    GLint model_matrix;
    GLint normal_matrix;
    std::array<GLint, 4> depth_bias_mvps{};

    struct Environment_uniforms {
      GLint map;
      GLint position;
      GLint extent;
      GLint strength;
      GLint falloff;
    };
    std::array<Environment_uniforms, 2> environment_maps {};

    GLint material_albedo_sampler;
    GLint material_normal_sampler;
    GLint material_metallic_sampler;
    GLint material_roughness_sampler;
    GLint material_emission_sampler;
    GLint material_ambient_occlusion_sampler;
    GLint material_albedo;
    GLint material_roughness;
    GLint material_metallic;
    GLint material_index_of_refraction;
    GLint material_alpha;
    GLint material_transmission;
    GLint material_emission;
    GLint material_ambient_occlusion;

    GLint camera_resolution;
    GLint camera_position;

    struct Light_uniforms {
      GLint position;
      GLint color;
      GLint strength;
      GLint view;
      GLint projection;
      GLint angle;
      GLint direction;
    };

    std::array<GLuint, 4> shadow_maps {};
    std::array<Light_uniforms, 4> lights{};

    GLint fog_color_near;
    GLint fog_color_far;
    GLint fog_attenuation_factor;

    GLint brdf_lut;
  };

  struct Standard_target {
    Standard_target(const glm::ivec2 &resolution, const int samples);
    ~Standard_target();
    Standard_target(const Standard_target &target) = delete;
    Standard_target(Standard_target &&target) = delete;
    Standard_target & operator=(const Standard_target &target) = delete;
    Standard_target & operator=(Standard_target &&target) = delete;
    const GLuint frame_buffer{};
    const GLuint color_texture{};
    const GLuint depth_texture{};
  };

  struct Blit_target {
    explicit Blit_target(const glm::ivec2 &resolution, GLint precision = GL_RGB16F);
    ~Blit_target();
    Blit_target(const Blit_target &target) = delete;
    Blit_target(Blit_target &&target) = delete;
    Blit_target & operator=(const Blit_target &target) = delete;
    Blit_target & operator=(Blit_target &&target) = delete;
    const GLuint frame_buffer{};
    const GLuint texture{};
    const GLuint depth_texture{};
    const glm::ivec2 resolution;
  };

  struct Post_target {
    explicit Post_target(const glm::ivec2 &resolution, GLint precision = GL_RGB16F);
    ~Post_target();
    Post_target(const Post_target &target) = delete;
    Post_target(Post_target &&target) = delete;
    Post_target & operator=(const Post_target &target) = delete;
    Post_target & operator=(Post_target &&target) = delete;
    const GLuint frame_buffer{};
    const GLuint texture{};
    const glm::ivec2 resolution;
  };

  struct Shadow_map_target {
    explicit Shadow_map_target(const Render_buffer &render_buffer);
    ~Shadow_map_target();
    Shadow_map_target(const Shadow_map_target &target) = delete;
    Shadow_map_target(Shadow_map_target &&target) = delete;
    Shadow_map_target & operator=(const Shadow_map_target &target) = delete;
    Shadow_map_target & operator=(Shadow_map_target &&target) = delete;
    const GLuint texture{};
    const GLuint frame_buffer{};
  };

  struct Environment_map_target {
    explicit Environment_map_target(const Render_buffer &render_buffer);
    ~Environment_map_target();
    Environment_map_target(const Environment_map_target &target) = delete;
    Environment_map_target(Environment_map_target &&target) = delete;
    Environment_map_target & operator=(const Environment_map_target &target) = delete;
    Environment_map_target & operator=(Environment_map_target &&target) = delete;
    const GLuint texture{};
    const GLuint albedo{};
    const GLuint frame_buffer{};
  };

  struct Quad {
    Quad();
    ~Quad();
    Quad(const Quad &target) = delete;
    Quad(const Quad &&target) = delete;
    Quad & operator=(const Quad &target) = delete;
    Quad & operator=(const Quad &&target) = delete;
    const GLuint vertex_array{};
    const GLuint buffer{};
  };

  struct Box {
    Box();
    ~Box();
    Box(const Box &target) = delete;
    Box(const Box &&target) = delete;
    Box & operator=(const Box &target) = delete;
    Box & operator=(const Box &&target) = delete;
    const GLuint buffer{};
    const GLuint element_buffer{};
    const GLuint vertex_array{};
  };

  void render_texture_targets(const Scene &scene);

  void render_scene(const Camera &camera,
                    const Scene &scene,
                    const glm::ivec2 &resolution);

  void render_shadow_maps(const Models &models,
                          const Lights &lights);

  void render_environment(const Scene &scene,
                          const glm::vec4 &clear_color);

  void render_boxes(const Boxes & boxes,
                    const mos::gfx::Camera &camera);

  void render_clouds(const Clouds &clouds,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const mos::gfx::Camera &camera,
                    const glm::ivec2 &resolution,
                    const Cloud_program &program,
                    const GLenum &draw_mode);

  void render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const Standard_program& program);

  void render_model(const Model &model,
                    const glm::mat4 &transform,
                    const Camera &camera,
                    const Lights &lights,
                    const Environment_lights &environment_lights,
                    const Fog &fog,
                    const glm::vec2 &resolution,
                    const Environment_program& program);

  void render_model_depth(const Model &model,
                          const glm::mat4 &transform,
                          const Camera &camera,
                          const glm::vec2 &resolution,
                          const Depth_program& program);

  /** Clear color and depth. */
  void clear(const glm::vec4 &color);
  void clear_depth();
  void clear_color(const glm::vec4 &color);
  void blur(GLuint input_texture,
            const Post_target &buffer_target,
            const Post_target &output_target,
            float iterations = 6);

  const bool context_;

  const Shader functions_shader_;
  const Propagate_program propagate_program_;
  const Standard_program standard_program_;
  const Environment_program environment_program_;
  const Cloud_program point_cloud_program_;
  const Cloud_program line_cloud_program_;
  const Box_program box_program_;
  const Depth_program depth_program_;
  const Bloom_program bloom_program_;
  const Compositing_program compositing_program_;
  const Blur_program blur_program_;
  const Depth_of_field_program depth_of_field_program_;

  Frame_buffers frame_buffers_;
  Render_buffers render_buffers_;
  Texture_buffers textures_;
  Array_buffers array_buffers_;
  Element_array_buffers element_array_buffers_;
  Vertex_arrays vertex_arrays_;

  const Standard_target standard_target_;
  const Blit_target multisample_target_;
  const Post_target screen_target_;
  const Post_target bloom_target_;
  const Post_target depth_of_field_target_;
  const Post_target post_target_;
  const Post_target temp_target_;

  const Quad quad_;
  const Box box;

  const Texture_buffer_2D black_texture_;
  const Texture_buffer_2D white_texture_;
  const Texture_buffer_2D brdf_lut_texture_;

  std::array<int,2> cube_camera_index_;

  /** Shadow maps. */
  const Render_buffer shadow_maps_render_buffer_;
  const std::array<Shadow_map_target, 4> shadow_maps_;
  const Post_target shadow_map_blur_target_;
  const std::array<Post_target, 4> shadow_map_blur_targets_;

  /** Environment map targets. */
  const Render_buffer environment_render_buffer_;
  const std::array<Environment_map_target, 2> environment_maps_targets_;
  const Environment_map_target propagate_target_;
};
}
}
