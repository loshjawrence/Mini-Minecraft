#ifndef CHUNKCAVEMODIFIER_H
#define CHUNKCAVEMODIFIER_H

#include <QMutex>
#include "../chunkmanager.h"

class ChunkCaveModifier : public ChunkModifier
{
protected:
    QMutex mutex;

public:
    ChunkCaveModifier(ChunkManager * manager);

    void Initialize();
    void Evaluate(Chunk *chunk, std::unordered_map<Chunk*,int8_t>& touched);
    void MakeCave(Chunk*, std::unordered_map<Chunk*,int8_t>& touched);
    bool DigCave(Chunk *chunk, const glm::vec4 &pos, std::unordered_map<Chunk*,int8_t>& touched);

    glm::vec4 getLookVec(const glm::vec4& pos);
    bool MoveCave(glm::vec4 &pos);
    void saveCaveHead(const glm::vec4&, const int32_t&);
    bool chunkExists(const glm::vec4&);
    bool markChunk(const glm::vec4&, std::unordered_map<Chunk*,int8_t>& touched);
    BlockType randResource();
    bool DigCavern(glm::vec4 pos, std::unordered_map<Chunk*,int8_t>& touched);
    bool notDangling(const int32_t, const int32_t, const int32_t, const int32_t, const int32_t);

};

#endif // CHUNKCAVEMODIFIER_H
