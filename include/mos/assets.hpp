/**
 * @file assets.hpp
 * @author: Morgan Bengtsson <bengtsson.morgan@gmail.com>
 */

#ifndef MOS_ASSETS_H
#define MOS_ASSETS_H

#include <unordered_map>
#include <map>
#include <memory>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "sound.hpp"
#include "character.hpp"
#include "mesh.hpp"
#include "texture2d.hpp"
#include "material.hpp"
#include "stream.hpp"
#include "model.hpp"

namespace mos {

/**
 * An asset class that manages heavy resources such as Textures, meshes, sounds
 * and sound streams.
 * Caches most things internally, so nothing is loaded twice.
 */
class Assets {
    public:
        /**
         * @brief Container for meshes.
         */
        using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;

        /**
         * @brief Container for textures.
         */
        using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture2D>>;

        /**
         * @brief Container for sounds.
         */
        using SoundMap = std::unordered_map<std::string, std::shared_ptr<Sound>>;

        /**
         * @brief Container for materials.
         */
        using MaterialMap = std::unordered_map<std::string, std::shared_ptr<Material>>;

        /**
         * @brief Pair for MeshMap.
         */
        using MeshPair = std::pair<std::string, std::shared_ptr<Mesh>>;

        /**
         * @brief Pair for TextureMap
         */
        using TexturePair = std::pair<std::string, std::shared_ptr<Texture2D>>;

        /**
         * @brief Pair for SoundMap
         */
        using SoundPair = std::pair<std::string, std::shared_ptr<Sound>>;

        /**
        * @brief Pair for MaterialMap.
        */
        using MaterialPair =  std::pair<std::string, std::shared_ptr<Material>>;

        /**
         * Constructor for the asset manager.
         *
         * @brief Assets
         * @param directory The directory where the assets exist, relative to the run directory, default is "assets/"
         */
        Assets(const std::string directory = "assets/");

        /**
         * @brief Assets non copyable.
         * @param assets
         */
        Assets(const Assets &assets) = delete; // Not copyable.
        virtual ~Assets();

        /**
         * @brief Loads a full Model from a *.model file.
         * @param file_name
         * @return Model object including associated meshes/textures/etc.
         */
        Model model(const std::string file_name);

        /**
         * Loads a *.material file into a Material object, and caches it internally.
         *
         * @param file_name
         * @return fShared pointer to Material object.
         */
        std::shared_ptr<Material> material_cached(const std::string file_name);

        /**
         *
         * Loads  a *.material file into a Material object. Not cached.
         *
         * @brief material
         * @param file_name
         * @return Shared pointer to a Material object.
         */
        std::shared_ptr<Material> material(const std::string file_name) const;

        /**
         * Loads a *.obj or *.mesh file into a mesh object, and caches it internally.
         * 
         * @param file_name
         * @return Shared pointer to Mesh object.
         */
        std::shared_ptr<Mesh> mesh_cached(const std::string file_name);

        /**
         * Loads a *.obj or *.mesh file into a mesh object. Not cached.
         * 
         * @param file_name
         * @return Shared pointer to Mesh object
         */
        std::shared_ptr<Mesh> mesh(const std::string file_name) const;

        /**
         * Loads a *.png file into a Texture2D object, and caches it internally.
         * 
         * @param file_name
         * @param mipmaps Use mipmaps or not.
         * @return Shared pointer to Texture2D object.
         */
        std::shared_ptr<Texture2D> texture_cached(const std::string file_name, const bool mipmaps = true);

        /**
         * Loads a *.png file into A Texture2D object. Not cached. 
         * 
         * @param file_name
         * @param mipmaps Use mipmaps or not.
         * @return Shared pointer to Texture2D object.
         */
        std::shared_ptr<Texture2D> texture(const std::string file_name, const bool mipmaps = true) const;

        /**
         * Loads a *.ogg file into a Sound object, and caches it internally.
         * 
         * @param file_name
         * @return Shared pointer to Sound object.
         */
        std::shared_ptr<Sound> sound_cached(const std::string file_name);

        /**
         * Loads an *. ogg file into a Sound object. Not cached.
         * 
         * @param file_name
         * @return Shared pointer to Sound object.
         */
        std::shared_ptr<Sound> sound(const std::string file_name) const;


        /**
         * Loads an *.ogg file into a Stream object. Not cached, since it is streamed.
         *
         * @brief stream
         * @param file_name
         * @return
         */
        std::shared_ptr<Stream> stream(const std::string file_name) const;

        /**
         * Load text from file. Not cached.
         * 
         * @param file_name
         * @return String.
         */
        std::string text(const std::string file_name) const;

        /**
         * Load a character map from an *.xml file in NGL format.
         * 
         * @param path
         * @return A map of chars with corresponding Character objects.
         */
        std::map<char, Character> character_map(std::string path);

        //TODO Make private.
        Model model(rapidjson::Value &value);

    private:
        std::string directory_;
        MeshMap meshes_;
        TextureMap textures_;
        SoundMap sounds_;
        MaterialMap materials_;
    };
}
#endif	/* MOS_ASSETS_H */

