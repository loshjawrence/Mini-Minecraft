#include "itemview.h"
#include "chunkmanager.h"

std::unordered_map<int, std::unordered_map<int, Texture*>> ItemView::itemTextures;
std::unordered_map<int, std::unordered_map<int, Mesh*>> ItemView::itemMesh;

Mesh *ItemView::BuildBlockMesh(const Item &item)
{
    Mesh * mesh = MeshFactory::BuildCube(false, false);
    int vertexCount = mesh->GetVertexCount();
    glm::vec2 * uvs = new glm::vec2[vertexCount];

    BlockUVSet UVSet;
    BlockType blockType = (BlockType)item.id;

    int index = 0;
    float tileSize = 1.0 / 16.f;

    // This is not the most readable thing, but no time to refactor all block uv set things...
    uvs[index++] =  glm::vec2(0, 1) * tileSize + UVSet.m_BlockUVSet[blockType][2];
    uvs[index++] =  glm::vec2(0, 0) * tileSize + UVSet.m_BlockUVSet[blockType][2];
    uvs[index++] =  glm::vec2(1, 0) * tileSize + UVSet.m_BlockUVSet[blockType][2];
    uvs[index++] =  glm::vec2(1, 1) * tileSize + UVSet.m_BlockUVSet[blockType][2] ;

    uvs[index++] =  glm::vec2(0, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][3];
    uvs[index++] =  glm::vec2(0, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][3];
    uvs[index++] =  glm::vec2(1, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][3];
    uvs[index++] =  glm::vec2(1, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][3];

    uvs[index++] =  glm::vec2(0, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][4];
    uvs[index++] =  glm::vec2(0, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][4];
    uvs[index++] =  glm::vec2(1, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][4];
    uvs[index++] =  glm::vec2(1, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][4];

    uvs[index++] =  glm::vec2(0, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][5];
    uvs[index++] =  glm::vec2(0, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][5];
    uvs[index++] =  glm::vec2(1, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][5];
    uvs[index++] =  glm::vec2(1, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][5];

    uvs[index++] = glm::vec2(0, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][0];
    uvs[index++] = glm::vec2(0, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][0];
    uvs[index++] = glm::vec2(1, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][0];
    uvs[index++] = glm::vec2(1, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][0];

    uvs[index++] = glm::vec2(0, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][1];
    uvs[index++] = glm::vec2(0, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][1];
    uvs[index++] = glm::vec2(1, 1) * tileSize +  UVSet.m_BlockUVSet[blockType][1];
    uvs[index++] = glm::vec2(1, 0) * tileSize +  UVSet.m_BlockUVSet[blockType][1];

    mesh->SetUVs(uvs, vertexCount);
    mesh->Upload();

    // UVs are going to be replaced either way...
    delete [] uvs;

    return mesh;
}

Mesh *ItemView::BuildToolMesh(const Item &item)
{
    Texture * t = GetItemTexture(item);

    if(t != nullptr)
        return MeshFactory::BuildMeshFromTexture(dynamic_cast<ReadableTexture*>(t));

    return nullptr;
}

Mesh *ItemView::BuildItemMesh(const Item &item)
{
    if(item.type == Item::BLOCK)
        return BuildBlockMesh(item);
    else if(item.type == Item::TOOL)
        return BuildToolMesh(item);

    return nullptr;
}

Texture *ItemView::BuildItemTexture(const Item &item)
{
    if(item.type == Item::TOOL)
    {
        switch (item.id)
        {
            case Tool::DIAMOND_PICKAXE:
                return AssetDatabase::GetInstance()->LoadAsset<ReadableTexture>("/cis460-minecraft/textures/items/diamond_pickaxe.tga");
        case Tool::DIAMOND_SWORD:
            return AssetDatabase::GetInstance()->LoadAsset<ReadableTexture>("/cis460-minecraft/textures/items/diamond_sword.tga");
        }
    }

    return nullptr;
}

void ItemView::UpdateItem()
{
    itemRenderer->SetMesh(ItemView::GetItemMesh(currentItem));
    itemRenderer->GetMaterial()->SetTexture("MainTexture", ItemView::GetItemTexture(currentItem));
    itemRenderer->GetMaterial()->SetColor("Color", glm::vec4(1,1,1,1));
}

void ItemView::OnItemQuantityChanged()
{

}

MeshRenderer *ItemView::InstantiateRenderer(GameObject * container)
{
    return container->AddComponent<MeshRenderer>();
}

void ItemView::Awake()
{
    GameObject * itemContainer = GameObject::Instantiate();
    itemRenderer = InstantiateRenderer(itemContainer);
    itemRenderer->SetMaterial(new Material("uimesh"));
    itemRenderer->SetMesh(nullptr);
    itemRenderer->SetEnabled(false);

    itemViewTransform = itemContainer->GetTransform();

    this->gameObject->GetTransform()->AddChild(itemViewTransform);
    this->currentItem = Item();
}

void ItemView::SetItem(Item item)
{
    if(!(currentItem == item))
    {
        currentItem = item;
        UpdateItem();
    }
    else if(currentItem.type == item.type && currentItem.id == item.id && currentItem.quantity != item.quantity)
    {
        OnItemQuantityChanged();
    }

    this->currentItem = item;
    this->itemRenderer->SetEnabled(item.type != Item::NONE);
}

Item ItemView::GetItem()
{
    return currentItem;
}

void ItemView::Update()
{
    //    itemViewTransform->SetLocalRotation(glm::vec3(glm::radians(-15.f), Engine::Time() * .5, 0));
}

Mesh *ItemView::GetItemMesh(const Item &item)
{
    return GetItemData<Mesh>(item, ItemView::itemMesh, &(ItemView::BuildItemMesh));
}

Texture *ItemView::GetItemTexture(const Item &item)
{
    // All blocks are atlased here
    if(item.type == Item::BLOCK)
        return AssetDatabase::GetInstance()->LoadAsset<Texture>("/cis460-minecraft/textures/minecraft_textures_all.tga");

    return GetItemData<Texture>(item, ItemView::itemTextures, &(ItemView::BuildItemTexture));
}
