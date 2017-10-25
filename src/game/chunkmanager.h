#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H
#include "common.h"
#include <QtConcurrent/QtConcurrent>

#include "procedural/perlin.h"

#define PI 3.141592654
#define CHUNK_DIM 16
#define CHUNK_HEIGHT 256
#define CHUNK_HALF_HEIGHT (CHUNK_HEIGHT / 2)
#define CHUNK_VIEWDIST 4
#define HFIELD_DIM 64
#define CHUNKS_PER_HFIELD (HFIELD_DIM / CHUNK_DIM)
#define HFIELD_VIEWDIST ((CHUNK_VIEWDIST / CHUNKS_PER_HFIELD) + 1) //+1 adds extra ring around chunks and simplifies logic

#define TERRAINSIZE 64
#define BASEMENT    64

const float CAVE_SEED_THRESH = 0.8;//0.8 is fine
const int32_t CAVE_HEIGHT_THRESH_MIN = -5;
const int32_t CAVE_HEIGHT_THRESH_MAX =  5;
const float CAVE_ENTROPY = 0.9;//0.9 greatern than 1 will sample perlin at higher freq so it's more chaotic, 1 is just regular perlin values
const float CAVE_X_RAD_CORRECTION = 30 * (PI/180);
const float CAVE_Y_RAD_RANGE = 180 * (PI/180);
const float CAVE_X_RAD_RANGE = 180 * (PI/180);
const int32_t CAVE_GUIDANCE_LEVEL = CHUNK_HEIGHT*3/8;
const float DIG_RADIUS = 5;
const uint32_t CAVE_ITERATIONS = 40;
const int32_t MAX_TERRAIN_HEIGHT = 24;//about the max positive noise value that scene.cpp produces
const uint32_t STONE_LEVEL = 16;
const uint32_t STONE_HEIGHT_SCALE = 3;
const float RESOURCE_THRESH = 0.8;


//Procedural_Terrain Jin_Kim
enum BlockType
{
    AIR, GRASS, DIRT, WOOD, LEAF, STONE, BEDROCK, COAL, IRON_ORE, LAVA, WATER, LAVA_FLOW, WATER_FLOW
};

class BlockUVSet
{
public:
    BlockUVSet()
    {
        m_BlockUVSet[GRASS][0] = glm::vec2(8.0/16.0, 13.0/16.0);
        m_BlockUVSet[GRASS][1] = glm::vec2(2.0/16.0, 15.0/16.0);
        m_BlockUVSet[GRASS][2] = glm::vec2(3.0/16.0, 15.0/16.0);
        m_BlockUVSet[GRASS][3] = glm::vec2(3.0/16.0, 15.0/16.0);
        m_BlockUVSet[GRASS][4] = glm::vec2(3.0/16.0, 15.0/16.0);
        m_BlockUVSet[GRASS][5] = glm::vec2(3.0/16.0, 15.0/16.0);

        m_BlockUVSet[DIRT][0] = glm::vec2(2.0/16.0, 15.0/16.0);
        m_BlockUVSet[DIRT][1] = glm::vec2(2.0/16.0, 15.0/16.0);
        m_BlockUVSet[DIRT][2] = glm::vec2(2.0/16.0, 15.0/16.0);
        m_BlockUVSet[DIRT][3] = glm::vec2(2.0/16.0, 15.0/16.0);
        m_BlockUVSet[DIRT][4] = glm::vec2(2.0/16.0, 15.0/16.0);
        m_BlockUVSet[DIRT][5] = glm::vec2(2.0/16.0, 15.0/16.0);

        m_BlockUVSet[WOOD][0] = glm::vec2(4.0/16.0, 14.0/16.0);
        m_BlockUVSet[WOOD][1] = glm::vec2(4.0/16.0, 14.0/16.0);
        m_BlockUVSet[WOOD][2] = glm::vec2(4.0/16.0, 14.0/16.0);
        m_BlockUVSet[WOOD][3] = glm::vec2(4.0/16.0, 14.0/16.0);
        m_BlockUVSet[WOOD][4] = glm::vec2(4.0/16.0, 14.0/16.0);
        m_BlockUVSet[WOOD][5] = glm::vec2(4.0/16.0, 14.0/16.0);

        m_BlockUVSet[LEAF][0] = glm::vec2(4.0/16.0, 12.0/16.0);
        m_BlockUVSet[LEAF][1] = glm::vec2(4.0/16.0, 12.0/16.0);
        m_BlockUVSet[LEAF][2] = glm::vec2(4.0/16.0, 12.0/16.0);
        m_BlockUVSet[LEAF][3] = glm::vec2(4.0/16.0, 12.0/16.0);
        m_BlockUVSet[LEAF][4] = glm::vec2(4.0/16.0, 12.0/16.0);
        m_BlockUVSet[LEAF][5] = glm::vec2(4.0/16.0, 12.0/16.0);

        m_BlockUVSet[STONE][0] = glm::vec2(1.0/16.0, 15.0/16.0);
        m_BlockUVSet[STONE][1] = glm::vec2(1.0/16.0, 15.0/16.0);
        m_BlockUVSet[STONE][2] = glm::vec2(1.0/16.0, 15.0/16.0);
        m_BlockUVSet[STONE][3] = glm::vec2(1.0/16.0, 15.0/16.0);
        m_BlockUVSet[STONE][4] = glm::vec2(1.0/16.0, 15.0/16.0);
        m_BlockUVSet[STONE][5] = glm::vec2(1.0/16.0, 15.0/16.0);

        m_BlockUVSet[BEDROCK][0] = glm::vec2(1.0/16.0, 14.0/16.0);
        m_BlockUVSet[BEDROCK][1] = glm::vec2(1.0/16.0, 14.0/16.0);
        m_BlockUVSet[BEDROCK][2] = glm::vec2(1.0/16.0, 14.0/16.0);
        m_BlockUVSet[BEDROCK][3] = glm::vec2(1.0/16.0, 14.0/16.0);
        m_BlockUVSet[BEDROCK][4] = glm::vec2(1.0/16.0, 14.0/16.0);
        m_BlockUVSet[BEDROCK][5] = glm::vec2(1.0/16.0, 14.0/16.0);


        m_BlockUVSet[COAL][0] = glm::vec2(2.0/16.0, 13.0/16.0);
        m_BlockUVSet[COAL][1] = glm::vec2(2.0/16.0, 13.0/16.0);
        m_BlockUVSet[COAL][2] = glm::vec2(2.0/16.0, 13.0/16.0);
        m_BlockUVSet[COAL][3] = glm::vec2(2.0/16.0, 13.0/16.0);
        m_BlockUVSet[COAL][4] = glm::vec2(2.0/16.0, 13.0/16.0);
        m_BlockUVSet[COAL][5] = glm::vec2(2.0/16.0, 13.0/16.0);

        m_BlockUVSet[IRON_ORE][0] = glm::vec2(1.0/16.0, 13.0/16.0);
        m_BlockUVSet[IRON_ORE][1] = glm::vec2(1.0/16.0, 13.0/16.0);
        m_BlockUVSet[IRON_ORE][2] = glm::vec2(1.0/16.0, 13.0/16.0);
        m_BlockUVSet[IRON_ORE][3] = glm::vec2(1.0/16.0, 13.0/16.0);
        m_BlockUVSet[IRON_ORE][4] = glm::vec2(1.0/16.0, 13.0/16.0);
        m_BlockUVSet[IRON_ORE][5] = glm::vec2(1.0/16.0, 13.0/16.0);

        m_BlockUVSet[LAVA][0] = glm::vec2(0.0/16.0, 1.0/16.0);
        m_BlockUVSet[LAVA][1] = glm::vec2(0.0/16.0, 1.0/16.0);
        m_BlockUVSet[LAVA][2] = glm::vec2(0.0/16.0, 1.0/16.0);
        m_BlockUVSet[LAVA][3] = glm::vec2(0.0/16.0, 1.0/16.0);
        m_BlockUVSet[LAVA][4] = glm::vec2(0.0/16.0, 1.0/16.0);
        m_BlockUVSet[LAVA][5] = glm::vec2(0.0/16.0, 1.0/16.0);

        m_BlockUVSet[WATER][0] = glm::vec2(0.0/16.0, 2.0/16.0);
        m_BlockUVSet[WATER][1] = glm::vec2(0.0/16.0, 2.0/16.0);
        m_BlockUVSet[WATER][2] = glm::vec2(0.0/16.0, 2.0/16.0);
        m_BlockUVSet[WATER][3] = glm::vec2(0.0/16.0, 2.0/16.0);
        m_BlockUVSet[WATER][4] = glm::vec2(0.0/16.0, 2.0/16.0);
        m_BlockUVSet[WATER][5] = glm::vec2(0.0/16.0, 2.0/16.0);

        m_BlockUVSet[LAVA_FLOW][0] = glm::vec2(14.0/16.0, 0.0/16.0);
        m_BlockUVSet[LAVA_FLOW][1] = glm::vec2(14.0/16.0, 0.0/16.0);
        m_BlockUVSet[LAVA_FLOW][2] = glm::vec2(14.0/16.0, 0.0/16.0);
        m_BlockUVSet[LAVA_FLOW][3] = glm::vec2(14.0/16.0, 0.0/16.0);
        m_BlockUVSet[LAVA_FLOW][4] = glm::vec2(14.0/16.0, 0.0/16.0);
        m_BlockUVSet[LAVA_FLOW][5] = glm::vec2(14.0/16.0, 0.0/16.0);

        m_BlockUVSet[WATER_FLOW][0] = glm::vec2(14.0/16.0, 3.0/16.0);
        m_BlockUVSet[WATER_FLOW][1] = glm::vec2(14.0/16.0, 3.0/16.0);
        m_BlockUVSet[WATER_FLOW][2] = glm::vec2(14.0/16.0, 3.0/16.0);
        m_BlockUVSet[WATER_FLOW][3] = glm::vec2(14.0/16.0, 3.0/16.0);
        m_BlockUVSet[WATER_FLOW][4] = glm::vec2(14.0/16.0, 3.0/16.0);
        m_BlockUVSet[WATER_FLOW][5] = glm::vec2(14.0/16.0, 3.0/16.0);
    }

    glm::vec2 m_BlockUVSet[256][6];
};

struct BlockInfo
{
    //glm::vec3 Position;
    BlockType type;
    bool bVisible;
};

typedef struct CaveHead {
    __forceinline float& operator[](const uint32_t& i) { return pos[i]; }
    glm::vec4 pos;
    int32_t iters;
} CaveHead;


///x86 intrinsics for fast init to 0
///http://stackoverflow.com/questions/2516096/fastest-way-to-zero-out-a-2d-array-in-c
typedef struct Chunk {//maybe put Mesh and MeshRenderer data in here as well, other chunk members can be added over time
    //index into array using y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + x
    __forceinline uint8_t& operator[]( const uint32_t& i) { return data[i]; }
    uint8_t data[CHUNK_DIM*CHUNK_DIM*CHUNK_HEIGHT];

    glm::ivec2 position;

    //use noises to check if you want to spawn something at that chunk
    float noise2d;
    float noise3d[CHUNK_HEIGHT/CHUNK_DIM];
    bool caveseed = false;
    std::vector<CaveHead> caveheads;//in world coords
} Chunk;

enum Direction {FORWARD,RIGHT,BACK,LEFT,NONE};

class ChunkManager;

class ChunkModifier
{
protected:
    ChunkManager * chunkManager;

public:
    ChunkModifier(ChunkManager * chunkManager);
    virtual ~ChunkModifier();

    virtual void Initialize() = 0;
    virtual void Evaluate(Chunk * chunk, std::unordered_map<Chunk*,int8_t>& touched) = 0;
};

struct ConcurrentChunkResult
{
    Chunk * chunk = nullptr;
    Mesh * mesh = nullptr;
};

class ChunkManager : public Component
{
protected:
    std::queue<QFuture<ConcurrentChunkResult>> futureChunks; // Results of concurrent chunk evaluation
    std::vector<ChunkModifier*> modifiers; // Procedural modifiers

    GameObject * chunkContainer;
    Material * terrainMaterial;    

    Texture * diffuseMap;
    Texture * normalMap;

    void OnChunkGenerated(ConcurrentChunkResult result);
    void DispatchChunkGeneration(int x, int y);
    void UploadFinishedChunks();

public:    
    ~ChunkManager();

    Player* player;
    std::unordered_map<uint64_t, MeshRenderer*> renderers;

    //key is spliced chunk coord, X top-half of int64_t, Z bottom-half of int64_t
    std::unordered_map<uint64_t, Chunk*> chunks;

    //chunk coords are chunk-resolution x-z coords in LL corner
    int curr_chunkX;
    int curr_chunkZ;

    //hfield coords are hfield-resolution x-z coords in LL corner
    int curr_hfieldX;
    int curr_hfieldZ;

    virtual void Update();
    virtual void Start();

    Direction genChunkSlab(const int64_t&, const int64_t&);

    Mesh* genMesh(const int64_t&, const int64_t&, Chunk *mychunk);

    void modifyChunkSlab(const Direction, std::unordered_map<Chunk*,int8_t>& touched);
    void genNewAndModifiedMeshes(std::unordered_map<Chunk*,int8_t>& touched);

    Chunk * GetChunk(int32_t x, int32_t z);

    char GetBlock(int64_t x, int64_t y, int64_t z);
    bool SetBlock(int64_t x, int64_t y, int64_t z, char value, bool updateMesh = true);

    uint64_t Encode(uint32_t x, uint32_t y);
};

#endif // CHUNKMANAGER_H
