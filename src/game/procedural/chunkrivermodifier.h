#ifndef CHUNKRIVERMODIFIER_H
#define CHUNKRIVERMODIFIER_H

#include <QMutex>
#include <unordered_map>
#include "../chunkmanager.h"
#include "lsystem.h"

#define RIVER_SECTION_SIZE 128
#define CHUNKS_PER_RIVER_SECTION (RIVER_SECTION_SIZE / CHUNK_DIM)

class ChunkRiverModifier;

typedef struct RiverSection {
    __forceinline uint8_t& operator[]( const int32_t& i) { return data[i]; }
    uint8_t data[RIVER_SECTION_SIZE*RIVER_SECTION_SIZE];
    glm::ivec2 position;
    bool initialized;
} RiverSection;

struct RiverData
{
    float branchWidthCoefficient;
    float currentAngle;
    float currentWidth;
    int currentHeight;
    glm::vec2 startPosition;
    glm::vec2 currentPosition;
    ChunkRiverModifier * modifier;
};

class ChunkRiverModifier : public ChunkModifier
{
protected:
    LSystem<RiverData> deltaRiverSystem;
    LSystem<RiverData> mainRiverSystem;
    std::unordered_map<uint64_t, RiverSection*> riverDistanceField; // A cache of 2d rivers per section

    void InitializeDistanceField(RiverSection * df, int sectionX, int sectionY);

    // This is needed to make the modifier thread safe
    QMutex riverMapMutex;

public:
    ChunkRiverModifier(ChunkManager * manager);
    void Evaluate(Chunk *chunk, std::unordered_map<Chunk*,int8_t>& touched);
    void Initialize();

    void SetDistanceFieldValue(int worldX, int worldY, uint8_t value);
    RiverSection * GetDistanceFieldForChunk(int chunkX, int chunkY);
    RiverSection * GetDistanceField(int x, int y);
    uint64_t Encode(uint32_t x, uint32_t y);
};

class RiverEndInstruction : public LInstruction<RiverData>
{
public:
    char GetSymbol() const;
    LContext<RiverData> Evaluate(const LContext<RiverData>& context, std::stack<LContext<RiverData>>& contextStack);
};

class RiverStartInstruction : public LInstruction<RiverData>
{
public:
    char GetSymbol() const;
    LContext<RiverData> Evaluate(const LContext<RiverData>& context, std::stack<LContext<RiverData>>& contextStack);
};

class RiverSectionInstruction : public LInstruction<RiverData>
{
public:
    char GetSymbol() const;
    LContext<RiverData> Evaluate(const LContext<RiverData>& context, std::stack<LContext<RiverData>>& contextStack);
};

class RiverBranchInstruction : public LInstruction<RiverData>
{
public:
    char GetSymbol() const;
    LContext<RiverData> Evaluate(const LContext<RiverData>& context, std::stack<LContext<RiverData>>& contextStack);
};

class RiverBranchRule : public LRule<RiverData>
{
protected:
    std::vector<LInstruction<RiverData>*> InternalPattern(LSystem<RiverData>& context) const;
    std::vector<LInstruction<RiverData>*> InternalEvaluate(LSystem<RiverData>& context) const;
    float probability;

public:
    RiverBranchRule(float probability);
    float GetProbability();
};

class RiverGrowRule : public LRule<RiverData>
{
protected:
    std::vector<LInstruction<RiverData>*> InternalPattern(LSystem<RiverData>& context) const;
    std::vector<LInstruction<RiverData>*> InternalEvaluate(LSystem<RiverData>& context) const;
};

#endif // CHUNKRIVERMODIFIER_H
