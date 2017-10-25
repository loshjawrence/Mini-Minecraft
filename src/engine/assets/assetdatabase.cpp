#include "assetdatabase.h"

AssetDatabase * AssetDatabase::instance = nullptr;

AssetDatabase::AssetDatabase()
{
}

AssetDatabase *AssetDatabase::GetInstance()
{
    if(instance == nullptr)
        instance = new AssetDatabase();

    return instance;
}
