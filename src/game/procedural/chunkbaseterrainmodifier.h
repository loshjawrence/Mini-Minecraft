#ifndef CHUNKBASETERRAINMODIFIER_H
#define CHUNKBASETERRAINMODIFIER_H

#include <QMutex>
#include <unordered_map>
#include "../chunkmanager.h"

// 64x64 size
struct Terrain
{
    double height[TERRAINSIZE][TERRAINSIZE];

    int X_coordinate;
    int Z_coordinate;

    //Store GradientVector
    glm::vec2 **Gradient00; //[2][2];
    glm::vec2 **Gradient01; //[3][3];
    glm::vec2 **Gradient02; //[5][5];
    glm::vec2 **Gradient03; //[9][9];
    glm::vec2 **Gradient04; //[17][17];
    glm::vec2 **Gradient05; //[33][33];
    glm::vec2 **Gradient06; //[65][65];
};

class ChunkBaseTerrainModifier : public ChunkModifier
{
protected:

    std::unordered_map<uint64_t,Terrain*> terrainMap;
    std::vector<Terrain*> m_Terrains;
    glm::ivec3 dimensions;

    QMutex mutex;

public:
    ChunkBaseTerrainModifier(ChunkManager * chunkManager);

    Terrain * CreateTerrain(int X_Coordinate, int Z_Coordinate);

    void GeneratedNewWorld(double UserPosX, double UserPosZ);

    void perlin(uint x, uint y, double (&Wave)[TERRAINSIZE][TERRAINSIZE], double amp, Terrain* o,
                  glm::vec2 AdjGradient00[4][2][2], glm::vec2 AdjGradient01[4][3][3], glm::vec2 AdjGradient02[4][5][5],
                  glm::vec2 AdjGradient03[4][9][9], glm::vec2 AdjGradient04[4][17][17], glm::vec2 AdjGradient05[4][33][33],
                  glm::vec2 AdjGradient06[4][65][65], bool bAdjXR, bool bAdjXL, bool bAdjZF, bool bAdjZB);


    void CopyGradientVector(glm::vec2 AdjGradient00[4][2][2], glm::vec2 AdjGradient01[4][3][3], glm::vec2 AdjGradient02[4][5][5],
                            glm::vec2 AdjGradient03[4][9][9], glm::vec2 AdjGradient04[4][17][17], glm::vec2 AdjGradient05[4][33][33],
                            glm::vec2 AdjGradient06[4][65][65], Terrain *cur, uint index);

    double Fadefuction(double t);

    void Initialize();
    void Evaluate(Chunk *chunk, std::unordered_map<Chunk *, int8_t> &touched);

    Terrain* findHField(const int64_t& x, const int64_t& z);
};

#endif // CHUNKBASETERRAINMODIFIER_H
