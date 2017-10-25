#ifndef ASSETDATABASE_H
#define ASSETDATABASE_H

#include "../engine.h"
#include "asset.h"
#include "texture.h"
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <vector>

// Basically all heavy things like shaders, meshes or textures
// are cached in memory. They can be instanced independently if
// no caching is desired.
class AssetDatabase
{
private:
    static AssetDatabase * instance;
    std::unordered_map<std::type_index, std::unordered_map<std::string, Asset*>> assetMap;
    AssetDatabase();

public:
    static AssetDatabase * GetInstance();

    template <class T>
    T* LoadAsset(std::string id)
    {
        T * t = FindAsset<T>(id);

        if(t != nullptr)
            return t;

        t = new T();
        t->LoadFromFilename(id);
        assetMap[typeid(T)][id] = t;
        return t;
    }

    // A specific method for textures... In the future, this should receive a tuple and make asset accept parameters
    // on initialization
    Texture * LoadTexture(std::string id, GLint Min_Filter, GLint Mag_Filter, GLint Wrap_s, GLint Wrap_t)
    {
        Texture * t = FindAsset<Texture>(id);

        if(t != nullptr)
            return t;

        t = new Texture();
        t->LoadFromImage(id.c_str(), Min_Filter, Mag_Filter, Wrap_s, Wrap_t);
        assetMap[typeid(Texture)][id] = t;
        return t;
    }

    template<class T>
    T * FindAsset(std::string id)
    {
        std::unordered_map<std::string, Asset*>& idMap = assetMap[typeid(T)];

        if(idMap.find(id) != idMap.end())
            return dynamic_cast<T*>(idMap[id]);

        return nullptr;
    }

};

#endif // ASSETDATABASE_H
