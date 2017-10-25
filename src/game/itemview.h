#ifndef ITEMVIEW_H
#define ITEMVIEW_H

#include "../engine/common.h"
#include "player.h"
#include <unordered_map>
#include <functional>

// An item view is a representation of an item
class ItemView : public Component
{
private:

    // Item Type -> (Item ID -> Mesh)
    static std::unordered_map<int, std::unordered_map<int, Mesh*>> itemMesh;
    static std::unordered_map<int, std::unordered_map<int, Texture*>> itemTextures;

    static Mesh * BuildBlockMesh(const Item& item);
    static Mesh * BuildToolMesh(const Item& item);
    static Mesh * BuildItemMesh(const Item& item);
    static Texture * BuildItemTexture(const Item& item);

    template <class T>
    static T * GetItemData(const Item& item, std::unordered_map<int, std::unordered_map<int, T*>>& map, std::function<T*(const Item&)> buildFunction)
    {
        int type = item.type;
        int id = item.id;
        T * output = nullptr;

        if(item.type == Item::NONE)
            return nullptr;

        auto typeEntry = map.find(type);

        if(typeEntry == map.end())
        {
            // There's not even a map for this type of item, so just assign a new mesh
            output = buildFunction(item);

            if(output != nullptr)
                map[type][id] = output;
        }
        else
        {
            auto idEntry = typeEntry->second.find(id);

            // We found the type, but this id is not set
            if(idEntry == typeEntry->second.end())
            {
                output = buildFunction(item);

                if(output != nullptr)
                    map[type][id] = output;
            }
            else
            {
                // The mesh was already built
//                Engine::LogDebug("Returning cached item data!");
                output = idEntry->second;
            }
        }

        return output;
    }

protected:
    MeshRenderer * itemRenderer;
    Transform * itemViewTransform;
    Item currentItem;

    virtual void UpdateItem();
    virtual void OnItemQuantityChanged();

    virtual MeshRenderer * InstantiateRenderer(GameObject *container);

public:
    virtual void Awake();
    void SetItem(Item item);
    Item GetItem();
    void Update();

    static Mesh *GetItemMesh(const Item& item);
    static Texture * GetItemTexture(const Item& item);
};

#endif // ITEMVIEW_H
