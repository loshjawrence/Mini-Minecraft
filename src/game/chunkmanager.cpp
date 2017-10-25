#include "chunkmanager.h"
#include "../engine/assets/texture.h"
#include <QCoreApplication>

#include "procedural/chunkbaseterrainmodifier.h"
#include "procedural/chunkcavemodifier.h"
#include "procedural/chunkrivermodifier.h"

uint64_t ChunkManager::Encode(uint32_t x, uint32_t y)
{
    return (((uint64_t)x) << 32) | y;
}

class ChunkWorker
{
protected:
    const std::vector<ChunkModifier*>& modifiers;
    ChunkManager * manager;

public:
    ChunkWorker(ChunkManager * manager, const std::vector<ChunkModifier*>& modifiers) : manager(manager), modifiers(modifiers) { }

    typedef ConcurrentChunkResult result_type;

    ConcurrentChunkResult operator()(const glm::ivec2 &position)
    {
        Chunk * chunk = AllocateChunk(position.x, position.y);

        // Evaluate all procedural modifiers on the chunk
        EvaluateChunk(chunk);

        ConcurrentChunkResult result;
        result.chunk = chunk;
        result.mesh = BuildMesh(chunk);

        return result;
    }

    void EvaluateChunk(Chunk * chunk)
    {
        std::unordered_map<Chunk*,int8_t> touched;

        // Modify chunks as necessary
        for(unsigned int m = 0; m < modifiers.size(); m++)
        {
            ChunkModifier * modifier = modifiers[m];
            modifier->Evaluate(chunk,touched);
        }
    }

    Mesh * BuildMesh(Chunk * chunk)
    {
        int chunkX = chunk->position.x;
        int chunkZ = chunk->position.y;

        std::vector<GLuint> indices;
        std::vector<Vertex> interleavedData;

        glm::vec4 GREEN(0,0,0,1);

        glm::vec4 UPnormal(0,1,0,0);
        glm::vec4 UPtangent(1,0,0,0);
        glm::vec4 UPbinormal(0,0,-1,0);

        glm::vec4 DOWNnormal(0,-1,0,0);
        glm::vec4 DOWNtangent(1,0,0,0);
        glm::vec4 DOWNbinormal(0,0,1,0);

        glm::vec4 RIGHTnormal(1,0,0,0);
        glm::vec4 RIGHTtangent(0,0,-1,0);
        glm::vec4 RIGHTbinormal(0,1,0,0);

        glm::vec4 LEFTnormal(-1,0,0,0);
        glm::vec4 LEFTtangent(0,0,1,0);
        glm::vec4 LEFTbinormal(0,1,0,0);

        glm::vec4 FORWARDnormal(0,0,1,0);
        glm::vec4 FORWARDtangent(1,0,1,0);
        glm::vec4 FORWARDbinormal(0,1,0,0);

        glm::vec4 BACKnormal(0,0,-1,0);
        glm::vec4 BACKtangent(-1,0,0,0);
        glm::vec4 BACKbinormal(0,1,0,0);

        int32_t index = -1;

        BlockUVSet UVSet;

        for(int y = 0; y < CHUNK_HEIGHT; y++) {
            for(int z = 0; z < CHUNK_DIM; z++) {
                for(int x = 0; x < CHUNK_DIM; x++) {

                    int32_t key = y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + x;

                    ///CHUNKCHANGE
                    //if( mychunk->find(key) == mychunk->end() ) {
                    //    continue;
                    //}
                    if( (*chunk)[key] == AIR ) {
                        continue;
                    }

                    BlockType type = (BlockType) (*chunk)[key];

                    int32_t UPkey       = (y+1) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x  );
                    int32_t DOWNkey     = (y-1) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x  );
                    int32_t RIGHTkey    = (y  ) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x+1);
                    int32_t LEFTkey     = (y  ) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x-1);
                    int32_t FORWARDkey  = (y  ) *CHUNK_DIM*CHUNK_DIM + (z+1)*CHUNK_DIM + (x  );
                    int32_t BACKkey     = (y  ) *CHUNK_DIM*CHUNK_DIM + (z-1)*CHUNK_DIM + (x  );

                    float xref = chunkX*CHUNK_DIM + x;
                    float yref = y;
                    float zref = chunkZ*CHUNK_DIM + z;

    //                if( mychunk->find(UPkey) == mychunk->end() ) {
                    if( y == CHUNK_HEIGHT-1 || (*chunk)[UPkey] == AIR ) {
                        int32_t root = ++index;
                        indices.push_back(root);
                        indices.push_back(++index);
                        indices.push_back(++index);
                        indices.push_back(root);
                        indices.push_back(index);
                        indices.push_back(++index);

                        //Texturing and Texture Animation Jin_Kim
                        Vertex v1(glm::vec4( xref  , yref+1, zref  , 1), UPnormal, UPtangent, UPbinormal,GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][0], type);
                        Vertex v2(glm::vec4( xref+1, yref+1, zref  , 1), UPnormal,  UPtangent, UPbinormal,GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][0], type);
                        Vertex v3(glm::vec4( xref+1, yref+1, zref+1, 1), UPnormal,  UPtangent, UPbinormal,GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][0], type);
                        Vertex v4(glm::vec4( xref  , yref+1, zref+1, 1) ,UPnormal,  UPtangent, UPbinormal,GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][0], type);

                        interleavedData.push_back(v1);
                        interleavedData.push_back(v2);
                        interleavedData.push_back(v3);
                        interleavedData.push_back(v4);
                    }

    //                if( mychunk->find(DOWNkey) == mychunk->end() ) {
                    if( y == 0 || (*chunk)[DOWNkey] == AIR ) {
                        int32_t root = ++index;
                        indices.push_back(root);
                        indices.push_back(++index);
                        indices.push_back(++index);
                        indices.push_back(root);
                        indices.push_back(index);
                        indices.push_back(++index);

                        //Texturing and Texture Animation Jin_Kim
                        Vertex v1(glm::vec4( xref  , yref  , zref  , 1), DOWNnormal, DOWNtangent,DOWNbinormal,  GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][1], type);
                        Vertex v2(glm::vec4( xref  , yref  , zref+1, 1), DOWNnormal, DOWNtangent,DOWNbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][1], type);
                        Vertex v3(glm::vec4( xref+1, yref  , zref+1, 1), DOWNnormal, DOWNtangent, DOWNbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][1], type);
                        Vertex v4(glm::vec4( xref+1, yref  , zref  , 1) , DOWNnormal, DOWNtangent,DOWNbinormal, GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][1], type);

                        interleavedData.push_back(v1);
                        interleavedData.push_back(v2);
                        interleavedData.push_back(v3);
                        interleavedData.push_back(v4);
                    }

    //                if( mychunk->find(LEFTkey) == mychunk->end() || x == 0) {
                    if( x == 0 || (*chunk)[LEFTkey] == AIR) {

                        bool otherChunkIsAir = true;

                        // TODO: Do this after concurrent chunk eval
//                        if(x == 0) {
//                            uint64_t mykey = Encode(chunkX-1, chunkZ);
//                            if(chunks.find(mykey) != chunks.end()) {
//                                Chunk* thischunk = chunks[mykey];
//                                int32_t index = y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + CHUNK_DIM-1;
//                                if((*thischunk)[index] != AIR) {
//                                    otherChunkIsAir = false;
//                                }
//                            }
//                        }

                        if (otherChunkIsAir) {
                            int32_t root = ++index;
                            indices.push_back(root);
                            indices.push_back(++index);
                            indices.push_back(++index);
                            indices.push_back(root);
                            indices.push_back(index);
                            indices.push_back(++index);

                            //Texturing and Texture Animation Jin_Kim
                            Vertex v1(glm::vec4( xref  , yref  , zref  , 1) , LEFTnormal, LEFTtangent, LEFTbinormal,GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][2], type);
                            Vertex v2(glm::vec4( xref  , yref+1, zref  , 1), LEFTnormal, LEFTtangent,LEFTbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][2], type);
                            Vertex v3(glm::vec4( xref  , yref+1, zref+1, 1), LEFTnormal, LEFTtangent,LEFTbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][2], type);
                            Vertex v4(glm::vec4( xref  , yref  , zref+1, 1) , LEFTnormal, LEFTtangent,LEFTbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][2], type);

                            interleavedData.push_back(v1);
                            interleavedData.push_back(v2);
                            interleavedData.push_back(v3);
                            interleavedData.push_back(v4);
                        }
                    }

    //                if( mychunk->find(RIGHTkey) == mychunk->end()  || x == 15) {
                    if( x == CHUNK_DIM-1 || (*chunk)[RIGHTkey] == AIR ) {
                        bool otherChunkIsAir = true;

                        // TODO: do this after eval
//                        if(x == CHUNK_DIM-1) {
//                            uint64_t mykey = Encode(chunkX+1, chunkZ);
//                            if(chunks.find(mykey) != chunks.end()) {
//                                Chunk* thischunk = chunks[mykey];
//                                int32_t index = y *CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + 0;
//                                if((*thischunk)[index] != AIR) {
//                                    otherChunkIsAir = false;
//                                }
//                            }
//                        }

                        if(otherChunkIsAir) {
                            int32_t root = ++index;
                            indices.push_back(root);
                            indices.push_back(++index);
                            indices.push_back(++index);
                            indices.push_back(root);
                            indices.push_back(index);
                            indices.push_back(++index);

                            //Texturing and Texture Animation Jin_Kim
                            Vertex v1(glm::vec4( xref+1, yref  , zref  , 1), RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][3], type);
                            Vertex v2(glm::vec4( xref+1, yref  , zref+1, 1), RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0,     0) + UVSet.m_BlockUVSet[type][3], type);
                            Vertex v3(glm::vec4( xref+1, yref+1, zref+1, 1), RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][3], type);
                            Vertex v4(glm::vec4( xref+1, yref+1, zref  , 1) , RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][3], type);

                            interleavedData.push_back(v1);
                            interleavedData.push_back(v2);
                            interleavedData.push_back(v3);
                            interleavedData.push_back(v4);
                        }
                    }

    //                if( mychunk->find(FORWARDkey) == mychunk->end() || z == 15 ) {
                    if( z == CHUNK_DIM-1 || (*chunk)[FORWARDkey] == AIR ) {

                        bool otherChunkIsAir = true;

                        // Do this after eval
//                        if(z == CHUNK_DIM-1) {
//                            uint64_t mykey = manager->Encode(chunkX, chunkZ+1);
//                            if(chunks.find(mykey) != chunks.end()) {
//                                Chunk* thischunk = chunks[mykey];
//                                int32_t index = y*CHUNK_DIM*CHUNK_DIM + (0)*CHUNK_DIM + x;
//                                if((*thischunk)[index] != AIR) {
//                                    otherChunkIsAir = false;
//                                }
//                            }
//                        }

                        if (otherChunkIsAir) {
                            int32_t root = ++index;
                            indices.push_back(root);
                            indices.push_back(++index);
                            indices.push_back(++index);
                            indices.push_back(root);
                            indices.push_back(index);
                            indices.push_back(++index);

                            //Texturing and Texture Animation Jin_Kim
                            Vertex v1(glm::vec4( xref  , yref  , zref+1, 1), FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][4], type);
                            Vertex v2( glm::vec4( xref  , yref+1, zref+1, 1), FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][4], type);
                            Vertex v3(glm::vec4( xref+1, yref+1, zref+1, 1) , FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][4], type);
                            Vertex v4( glm::vec4( xref+1, yref  , zref+1, 1) , FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][4], type);

                            interleavedData.push_back(v1);
                            interleavedData.push_back(v2);
                            interleavedData.push_back(v3);
                            interleavedData.push_back(v4);
                        }
                    }

    //                if( mychunk->find(BACKkey) == mychunk->end()  || z == 0) {
                    if( z == 0 || (*chunk)[BACKkey] == AIR ) {

                        bool otherChunkIsAir = true;
                        // TODO: do this after eval
//                        if(z == 0) {
//                            uint64_t mykey = Encode(chunkX, chunkZ-1);
//                            if(chunks.find(mykey) != chunks.end()) {
//                                Chunk* thischunk = chunks[mykey];
//                                int32_t index = y*CHUNK_DIM*CHUNK_DIM + (CHUNK_DIM-1)*CHUNK_DIM + x;
//                                if((*thischunk)[index] != AIR) {
//                                    otherChunkIsAir = false;
//                                }
//                            }
//                        }

                        if (otherChunkIsAir) {
                            int32_t root = ++index;
                            indices.push_back(root);
                            indices.push_back(++index);
                            indices.push_back(++index);
                            indices.push_back(root);
                            indices.push_back(index);
                            indices.push_back(++index);

                            //Texturing and Texture Animation Jin_Kim
                            Vertex v1(glm::vec4( xref  , yref  , zref  , 1), BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][5], type);
                            Vertex v2( glm::vec4( xref+1, yref  , zref  , 1), BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][5], type);
                            Vertex v3(glm::vec4( xref+1, yref+1, zref  , 1) , BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][5], type);
                            Vertex v4(glm::vec4( xref  , yref+1, zref  , 1), BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][5], type);

                            interleavedData.push_back(v1);
                            interleavedData.push_back(v2);
                            interleavedData.push_back(v3);
                            interleavedData.push_back(v4);
                        }
                    }
                }//x
            }//z
        }//y

        Mesh* mesh = new Mesh();
        mesh->SetIndices(indices.data(), indices.size(), true);
        mesh->SetInterleaved(true);
        mesh->SetInterleavedData(interleavedData.data(), interleavedData.size(), true);
        mesh->SetWriteOnly(true);
        return mesh;
    }

    Chunk * AllocateChunk(int chunkX, int chunkZ)
    {
        Chunk* mychunk = new Chunk();
        mychunk->position = glm::ivec2(chunkX, chunkZ);
        return mychunk;
    }
};

void ChunkManager::OnChunkGenerated(ConcurrentChunkResult result)
{
    Chunk * chunk = result.chunk;

    if(chunk != nullptr)
    {
        uint64_t key = Encode(chunk->position.x, chunk->position.y);
        chunks[key] = chunk;

        Mesh * mesh = result.mesh;

        if(mesh != nullptr)
        {
            // TODO: Here, we should add the missing faces from adjacent chunks
            mesh->Upload();

            MeshRenderer * renderer = chunkContainer->AddComponent<MeshRenderer>();
            renderer->SetMesh(mesh);
            renderer->SetMaterial(terrainMaterial);
            renderers[key] = renderer;
        }
    }
}

void ChunkManager::DispatchChunkGeneration(int x, int y)
{
    // For now, we just dispatch single chunks.
    // We can see later if we can dispatch chunk sections, but that requires more synchronization measures
    QList<glm::ivec2> positions;
    positions.push_back(glm::ivec2(x,y));
    futureChunks.push(QtConcurrent::mapped(positions, ChunkWorker(this, modifiers)));
}

void ChunkManager::UploadFinishedChunks()
{
    if(!futureChunks.empty())
    {
        QFuture<ConcurrentChunkResult>& r = futureChunks.front();

        if(r.isStarted() && r.isFinished())
        {
            // We assume futures only return one element
            ConcurrentChunkResult result = r.result();
            OnChunkGenerated(result);
            futureChunks.pop();
        }
    }
}

ChunkManager::~ChunkManager()
{
    delete  diffuseMap;
    delete  normalMap;
}

void ChunkManager::Update()
{
    UploadFinishedChunks();

    glm::vec3 playerpos =  player->GetGameObject()->GetTransform()->WorldPosition();

    int64_t new_chunkX = std::floor( playerpos[0] / CHUNK_DIM );
    int64_t new_chunkZ = std::floor( playerpos[2] / CHUNK_DIM );

    ///THESE LAST TWO ARE FOR NEW CHUNK GENERATION
    if(new_chunkX != curr_chunkX || new_chunkZ != curr_chunkZ)
        genChunkSlab(new_chunkX, new_chunkZ);

    std::unordered_map<uint64_t, Chunk*>::iterator chunkEntry;

    glm::vec2 vPos = glm::vec2(new_chunkX, new_chunkZ);

    for(chunkEntry = chunks.begin(); chunkEntry != chunks.end(); chunkEntry++)
    {
        Chunk * chunk = chunkEntry->second;

        if(glm::length(glm::vec2(chunk->position) - vPos) > CHUNK_VIEWDIST * 2)
        {
            int64_t key = Encode(chunk->position.x, chunk->position.y);
            MeshRenderer * r = renderers[key];

            if(r != nullptr)
            {
                Mesh * m = r->GetMesh();

                if(m != nullptr)
                {
                    m->Destroy();
                    delete m;

                    r->SetMesh(nullptr);
                }

                r->Destroy();
            }

            renderers[key] = nullptr;
        }
    }
}

void ChunkManager::modifyChunkSlab(const Direction dir, std::unordered_map<Chunk*,int8_t>& touched) {
    // Modify chunks as necessary
    int32_t xstart;
    int32_t xend;
    int32_t zstart;
    int32_t zend;

    switch (dir) {
    case FORWARD:
        xstart  = curr_chunkX - CHUNK_VIEWDIST;
        xend    = curr_chunkX + CHUNK_VIEWDIST - 1;
        zstart  = curr_chunkZ + CHUNK_VIEWDIST - 2;
        zend    = zstart+1;
        break;
    case BACK:
        xstart  = curr_chunkX - CHUNK_VIEWDIST;
        xend    = curr_chunkX + CHUNK_VIEWDIST - 1;
        zstart  = curr_chunkZ - CHUNK_VIEWDIST;
        zend    = zstart+1;
        break;
    case RIGHT:
        xstart  = curr_chunkX + CHUNK_VIEWDIST - 2;
        xend    = xstart+1;
        zstart  = curr_chunkZ - CHUNK_VIEWDIST;
        zend    = curr_chunkZ + CHUNK_VIEWDIST - 1;
         break;
    case LEFT:
        xstart  = curr_chunkX - CHUNK_VIEWDIST;
        xend    = xstart+1;
        zstart  = curr_chunkZ - CHUNK_VIEWDIST;
        zend    = curr_chunkZ + CHUNK_VIEWDIST - 1;
        break;
    default:
        break;
    }

    for(unsigned int m = 0; m < modifiers.size(); m++)
    {
        ChunkModifier * modifier = modifiers[m];
        Engine::LogInfo(std::string("Current modifier: ") + typeid(*modifier).name());

        for(int x = xstart; x <= xend; x++) {
            for(int z = zstart; z <= zend; z++) {
                uint64_t key = Encode(x,z);
                Chunk* mychunk = chunks[key];
                modifier->Evaluate(mychunk,touched);
                if(touched.find(mychunk) == touched.end()) {
                    touched[mychunk] = 1;
                }
            }//for z
        }//for x
    }//for m
}


void ChunkManager::genNewAndModifiedMeshes(std::unordered_map<Chunk*,int8_t>& touched) {//prob needs a vector of keys or a map of chunks*
    typedef std::unordered_map<Chunk*, int8_t>::iterator it_type;
    for(it_type it = touched.begin(); it != touched.end(); it++) {
        Chunk* mychunk = it->first;

        int32_t x = mychunk->position[0];
        int32_t z = mychunk->position[1];
        Mesh* mesh = genMesh(x,z, mychunk);

        uint64_t key = Encode(x,z);
        MeshRenderer* renderer;
        if(renderers.find(key) == renderers.end()) {
            renderer = chunkContainer->AddComponent<MeshRenderer>();
            renderer->SetMesh(mesh);
            renderer->SetMaterial(terrainMaterial);
            renderers[key] = renderer;
        } else {
            renderer = renderers[key];
            delete renderer->GetMesh();
            renderer->SetMesh(mesh);
        }
    }

}

Chunk *ChunkManager::GetChunk(int32_t x, int32_t z)
{
    uint64_t key = Encode(x,z);
    std::unordered_map<uint64_t, Chunk*>::iterator it = chunks.find(key);

    if(it == chunks.end())
        return nullptr;

    return  it->second;
}

char ChunkManager::GetBlock(int64_t x, int64_t y, int64_t z)
{
    if(y < 0 || y > CHUNK_HEIGHT)
        return -1;

    int64_t chunkX = glm::floor(x / (float) CHUNK_DIM);
    int64_t chunkZ = glm::floor(z / (float) CHUNK_DIM);

    Chunk * c = GetChunk(chunkX, chunkZ);

    if(c == nullptr)
        return -1;

    int offsetX = x - chunkX * CHUNK_DIM;
    int offsetZ = z - chunkZ * CHUNK_DIM;

    if(offsetX < 0)
        offsetX += CHUNK_DIM;

    if(offsetZ < 0)
        offsetZ += CHUNK_DIM;

    int32_t key = y*CHUNK_DIM*CHUNK_DIM + offsetZ*CHUNK_DIM + offsetX;

    ///CHUNKCHANGE
//    chunk::iterator it = c->find(key);

//    if(it == c->end())
//        return -1;

//    return it->second;
    if( (*c)[key] == AIR)
        return -1;

    return (*c)[key];
}

bool ChunkManager::SetBlock(int64_t x, int64_t y, int64_t z, char value, bool updateMesh)
{
    if(y < 0 || y > CHUNK_HEIGHT)
        return false;

    int64_t chunkX = glm::floor(x / (float) CHUNK_DIM);
    int64_t chunkZ = glm::floor(z / (float) CHUNK_DIM);

    Chunk * c = GetChunk(chunkX, chunkZ);

    if(c == nullptr)
        return false;

    int offsetX = x - chunkX * CHUNK_DIM;
    int offsetZ = z - chunkZ * CHUNK_DIM;

    if(offsetX < 0)
        offsetX += CHUNK_DIM;

    if(offsetZ < 0)
        offsetZ += CHUNK_DIM;

    int32_t key = y*CHUNK_DIM*CHUNK_DIM + offsetZ*CHUNK_DIM + offsetX;

    bool success = false;

    if(value < 0)
    {
    ///CHUNKCHANGE
//        success = c->find(key) != c->end();
//        c->erase(key);
        success = ((*c)[key] != AIR && (*c)[key] !=BEDROCK);
        (*c)[key] = AIR;
    }
    else
    {

        if((*c)[key] == BEDROCK) {
            std::cout << "TRIED TO SET BEDROCK";
            success = false;
        } else {
            (*c)[key] = value;
            success = true;
        }
    }

    if(success && updateMesh)
    {
        uint64_t chunkKey = Encode(chunkX, chunkZ);

        if(renderers.find(chunkKey) != renderers.end())
            renderers[chunkKey]->SetMesh(genMesh(chunkX,chunkZ, GetChunk(chunkX, chunkZ)));
    }

    return success;
}

void ChunkManager::Start()
{
    //generate hfield and chunk maps in the span of viewing distance

    glm::vec3 playerpos =  player->GetGameObject()->GetTransform()->WorldPosition();
    curr_chunkX = std::floor( playerpos[0] / CHUNK_DIM );
    curr_chunkZ = std::floor( playerpos[2] / CHUNK_DIM );

    curr_hfieldX = std::floor( curr_chunkX / (float)CHUNKS_PER_HFIELD);
    curr_hfieldZ = std::floor( curr_chunkZ / (float)CHUNKS_PER_HFIELD);

    this->modifiers.push_back(new ChunkBaseTerrainModifier(this));
    this->modifiers.push_back(new ChunkRiverModifier(this));
    this->modifiers.push_back(new ChunkCaveModifier(this));

    // Initialize modifiers
    for(unsigned int m = 0; m < modifiers.size(); m++)
    {
        ChunkModifier * modifier = modifiers[m];
        Engine::LogInfo(std::string("Initializing modifier ") + typeid(*modifier).name());
    }

    //generate chunks
    Engine::LogInfo("gen logical chunks");
    for(int x = curr_chunkX - CHUNK_VIEWDIST; x < curr_chunkX + CHUNK_VIEWDIST; x++) {
        for(int z = curr_chunkZ - CHUNK_VIEWDIST; z < curr_chunkZ + CHUNK_VIEWDIST; z++) {
            DispatchChunkGeneration(x,z);
        }
    }

    //build meshes
    Engine::LogInfo("Generating meshes...");
    diffuseMap = AssetDatabase::GetInstance()->LoadAsset<Texture>("/cis460-minecraft/textures/minecraft_textures_all.tga");
    normalMap = AssetDatabase::GetInstance()->LoadAsset<Texture>("/cis460-minecraft/textures/minecraft_normals_all.tga");

    terrainMaterial = new Material("lambert");
    terrainMaterial->SetVector("Color", glm::vec4(1,1,1,1));
    terrainMaterial->SetTexture("DiffuseMap",diffuseMap);
    terrainMaterial->SetTexture("NormalMap",normalMap);
    terrainMaterial->SetFeature(GL_CULL_FACE, true);
    glCullFace(GL_FRONT);

    chunkContainer = GameObject::Instantiate("ChunkContainer");

    Engine::LogInfo("ChunkManager start(): DONE");
}

Direction ChunkManager::genChunkSlab(const int64_t& new_chunkX, const int64_t& new_chunkZ) {
    //generate chunk slab in world for the four different cases: forward back left or right

    //LEFT OR RIGHT
    if(new_chunkX < curr_chunkX) { //gen left

        //gen left
        int64_t xgen = (new_chunkX - CHUNK_VIEWDIST);
        for(int64_t z = curr_chunkZ - CHUNK_VIEWDIST; z < curr_chunkZ + CHUNK_VIEWDIST; z++) {
            uint64_t key = Encode(xgen,z);

            if(chunks.find(key) == chunks.end()) {
                DispatchChunkGeneration(xgen, z);
//                Chunk * c = genChunk(xgen,z);
//                chunks[key] = c;
            }
            else if(renderers[key] == nullptr)
            {
                DispatchChunkGeneration(xgen, z);
            }
        }
        curr_chunkX = new_chunkX;
        return LEFT;

    } else if(new_chunkX > curr_chunkX) {//gen right

        //gen right
        int64_t xgen = (new_chunkX + CHUNK_VIEWDIST - 1);
        for(int64_t z = curr_chunkZ - CHUNK_VIEWDIST; z < curr_chunkZ + CHUNK_VIEWDIST; z++) {
            uint64_t key = Encode(xgen,z);

            if(chunks.find(key) == chunks.end()) {
                DispatchChunkGeneration(xgen, z);
//                Chunk * c = genChunk(xgen,z);
//                chunks[key] = c;
            }
            else if(renderers[key] == nullptr)
            {
                DispatchChunkGeneration(xgen, z);
            }
        }
        curr_chunkX = new_chunkX;
        return RIGHT;

    //FORWARD OR BACK
    } else if(new_chunkZ < curr_chunkZ) {//gen back

        //gen back
        int64_t zgen = new_chunkZ - CHUNK_VIEWDIST;
        for(int64_t x = curr_chunkX - CHUNK_VIEWDIST; x < curr_chunkX + CHUNK_VIEWDIST; x++) {
            uint64_t key = Encode(x,zgen);

            if(chunks.find(key) == chunks.end()) {
                DispatchChunkGeneration(x, zgen);
//                Chunk * c = genChunk(x,zgen);
//                chunks[key] = c;
            }
            else if(renderers[key] == nullptr)
            {
                DispatchChunkGeneration(x, zgen);
            }
        }
        curr_chunkZ = new_chunkZ;
        return BACK;

    } else if(new_chunkZ > curr_chunkZ) {//gen forward

        //gen forward
        int64_t zgen = new_chunkZ + CHUNK_VIEWDIST - 1;
        for(int64_t x = curr_chunkX - CHUNK_VIEWDIST; x < curr_chunkX + CHUNK_VIEWDIST; x++) {
            uint64_t key = Encode(x,zgen);

            if(chunks.find(key) == chunks.end()) {
                DispatchChunkGeneration(x, zgen);
//                Chunk * c = genChunk(x,zgen);
//                chunks[key] = c;
            }
            else if(renderers[key] == nullptr)
            {
                DispatchChunkGeneration(x, zgen);
            }
        }
        curr_chunkZ = new_chunkZ;
        return FORWARD;
    }

    return FORWARD;
}

Mesh* ChunkManager::genMesh(const int64_t& chunkX, const int64_t& chunkZ, Chunk * mychunk) {
    std::vector<GLuint> indices;
    std::vector<Vertex> interleavedData;

    glm::vec4 GREEN(0,0,0,1);

    glm::vec4 UPnormal(0,1,0,0);
    glm::vec4 UPtangent(1,0,0,0);
    glm::vec4 UPbinormal(0,0,-1,0);

    glm::vec4 DOWNnormal(0,-1,0,0);
    glm::vec4 DOWNtangent(1,0,0,0);
    glm::vec4 DOWNbinormal(0,0,1,0);

    glm::vec4 RIGHTnormal(-1,0,0,0);
    glm::vec4 RIGHTtangent(0,0,1,0);
    glm::vec4 RIGHTbinormal(0,1,0,0);

    glm::vec4 LEFTnormal(1,0,0,0);
    glm::vec4 LEFTtangent(0,0,-1,0);
    glm::vec4 LEFTbinormal(0,1,0,0);

    glm::vec4 FORWARDnormal(0,0,1,0);
    glm::vec4 FORWARDtangent(1,0,1,0);
    glm::vec4 FORWARDbinormal(0,1,0,0);

    glm::vec4 BACKnormal(0,0,-1,0);
    glm::vec4 BACKtangent(-1,0,0,0);
    glm::vec4 BACKbinormal(0,1,0,0);

    int32_t index = -1;

    BlockUVSet UVSet;

    for(int y = 0; y < CHUNK_HEIGHT; y++) {
        for(int z = 0; z < CHUNK_DIM; z++) {
            for(int x = 0; x < CHUNK_DIM; x++) {

                int32_t key = y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + x;

                ///CHUNKCHANGE
                //if( mychunk->find(key) == mychunk->end() ) {
                //    continue;
                //}
                if( (*mychunk)[key] == AIR ) {
                    continue;
                }

                BlockType type = (BlockType) (*mychunk)[key];

                int32_t UPkey       = (y+1) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x  );
                int32_t DOWNkey     = (y-1) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x  );
                int32_t RIGHTkey    = (y  ) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x+1);
                int32_t LEFTkey     = (y  ) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x-1);
                int32_t FORWARDkey  = (y  ) *CHUNK_DIM*CHUNK_DIM + (z+1)*CHUNK_DIM + (x  );
                int32_t BACKkey     = (y  ) *CHUNK_DIM*CHUNK_DIM + (z-1)*CHUNK_DIM + (x  );

                float xref = chunkX*CHUNK_DIM + x;
                float yref = y;
                float zref = chunkZ*CHUNK_DIM + z;

//                if( mychunk->find(UPkey) == mychunk->end() ) {
                if( y == CHUNK_HEIGHT-1 || (*mychunk)[UPkey] == AIR ) {
                    int32_t root = ++index;
                    indices.push_back(root);
                    indices.push_back(++index);
                    indices.push_back(++index);
                    indices.push_back(root);
                    indices.push_back(index);
                    indices.push_back(++index);

                    //Texturing and Texture Animation Jin_Kim
                    Vertex v1(glm::vec4( xref  , yref+1, zref  , 1), UPnormal, UPtangent, UPbinormal,GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][0], type);
                    Vertex v2(glm::vec4( xref+1, yref+1, zref  , 1), UPnormal,  UPtangent, UPbinormal,GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][0], type);
                    Vertex v3(glm::vec4( xref+1, yref+1, zref+1, 1), UPnormal,  UPtangent, UPbinormal,GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][0], type);
                    Vertex v4(glm::vec4( xref  , yref+1, zref+1, 1) ,UPnormal,  UPtangent, UPbinormal,GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][0], type);

                    interleavedData.push_back(v1);
                    interleavedData.push_back(v2);
                    interleavedData.push_back(v3);
                    interleavedData.push_back(v4);
                }

//                if( mychunk->find(DOWNkey) == mychunk->end() ) {
                if( y == 0 || (*mychunk)[DOWNkey] == AIR ) {
                    int32_t root = ++index;
                    indices.push_back(root);
                    indices.push_back(++index);
                    indices.push_back(++index);
                    indices.push_back(root);
                    indices.push_back(index);
                    indices.push_back(++index);

                    //Texturing and Texture Animation Jin_Kim
                    Vertex v1(glm::vec4( xref  , yref  , zref  , 1), DOWNnormal, DOWNtangent,DOWNbinormal,  GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][1], type);
                    Vertex v2(glm::vec4( xref  , yref  , zref+1, 1), DOWNnormal, DOWNtangent,DOWNbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][1], type);
                    Vertex v3(glm::vec4( xref+1, yref  , zref+1, 1), DOWNnormal, DOWNtangent, DOWNbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][1], type);
                    Vertex v4(glm::vec4( xref+1, yref  , zref  , 1) , DOWNnormal, DOWNtangent,DOWNbinormal, GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][1], type);

                    interleavedData.push_back(v1);
                    interleavedData.push_back(v2);
                    interleavedData.push_back(v3);
                    interleavedData.push_back(v4);
                }

//                if( mychunk->find(LEFTkey) == mychunk->end() || x == 0) {
                if( x == 0 || (*mychunk)[LEFTkey] == AIR) {

                    bool otherChunkIsAir = true;
                    if(x == 0) {
                        uint64_t mykey = Encode(chunkX-1, chunkZ);
                        if(chunks.find(mykey) != chunks.end()) {
                            Chunk* thischunk = chunks[mykey];
                            int32_t index = y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + CHUNK_DIM-1;
                            if((*thischunk)[index] != AIR) {
                                otherChunkIsAir = false;
                            }
                        }
                    }

                    if (otherChunkIsAir) {
                        int32_t root = ++index;
                        indices.push_back(root);
                        indices.push_back(++index);
                        indices.push_back(++index);
                        indices.push_back(root);
                        indices.push_back(index);
                        indices.push_back(++index);

                        //Texturing and Texture Animation Jin_Kim
                        Vertex v1(glm::vec4( xref  , yref  , zref  , 1) , LEFTnormal, LEFTtangent, LEFTbinormal,GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][2], type);
                        Vertex v2(glm::vec4( xref  , yref+1, zref  , 1), LEFTnormal, LEFTtangent,LEFTbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][2], type);
                        Vertex v3(glm::vec4( xref  , yref+1, zref+1, 1), LEFTnormal, LEFTtangent,LEFTbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][2], type);
                        Vertex v4(glm::vec4( xref  , yref  , zref+1, 1) , LEFTnormal, LEFTtangent,LEFTbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][2], type);

                        interleavedData.push_back(v1);
                        interleavedData.push_back(v2);
                        interleavedData.push_back(v3);
                        interleavedData.push_back(v4);
                    }
                }

//                if( mychunk->find(RIGHTkey) == mychunk->end()  || x == 15) {
                if( x == CHUNK_DIM-1 || (*mychunk)[RIGHTkey] == AIR ) {
                    bool otherChunkIsAir = true;
                    if(x == CHUNK_DIM-1) {
                        uint64_t mykey = Encode(chunkX+1, chunkZ);
                        if(chunks.find(mykey) != chunks.end()) {
                            Chunk* thischunk = chunks[mykey];
                            int32_t index = y *CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + 0;
                            if((*thischunk)[index] != AIR) {
                                otherChunkIsAir = false;
                            }
                        }
                    }

                    if(otherChunkIsAir) {
                        int32_t root = ++index;
                        indices.push_back(root);
                        indices.push_back(++index);
                        indices.push_back(++index);
                        indices.push_back(root);
                        indices.push_back(index);
                        indices.push_back(++index);

                        //Texturing and Texture Animation Jin_Kim
                        Vertex v1(glm::vec4( xref+1, yref  , zref  , 1), RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][3], type);
                        Vertex v2(glm::vec4( xref+1, yref  , zref+1, 1), RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0,     0) + UVSet.m_BlockUVSet[type][3], type);
                        Vertex v3(glm::vec4( xref+1, yref+1, zref+1, 1), RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][3], type);
                        Vertex v4(glm::vec4( xref+1, yref+1, zref  , 1) , RIGHTnormal, RIGHTtangent,RIGHTbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][3], type);

                        interleavedData.push_back(v1);
                        interleavedData.push_back(v2);
                        interleavedData.push_back(v3);
                        interleavedData.push_back(v4);
                    }
                }

//                if( mychunk->find(FORWARDkey) == mychunk->end() || z == 15 ) {
                if( z == CHUNK_DIM-1 || (*mychunk)[FORWARDkey] == AIR ) {

                    bool otherChunkIsAir = true;
                    if(z == CHUNK_DIM-1) {
                        uint64_t mykey = Encode(chunkX, chunkZ+1);
                        if(chunks.find(mykey) != chunks.end()) {
                            Chunk* thischunk = chunks[mykey];
                            int32_t index = y*CHUNK_DIM*CHUNK_DIM + (0)*CHUNK_DIM + x;
                            if((*thischunk)[index] != AIR) {
                                otherChunkIsAir = false;
                            }
                        }
                    }

                    if (otherChunkIsAir) {
                        int32_t root = ++index;
                        indices.push_back(root);
                        indices.push_back(++index);
                        indices.push_back(++index);
                        indices.push_back(root);
                        indices.push_back(index);
                        indices.push_back(++index);

                        //Texturing and Texture Animation Jin_Kim
                        Vertex v1(glm::vec4( xref  , yref  , zref+1, 1), FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][4], type);
                        Vertex v2( glm::vec4( xref  , yref+1, zref+1, 1), FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][4], type);
                        Vertex v3(glm::vec4( xref+1, yref+1, zref+1, 1) , FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][4], type);
                        Vertex v4( glm::vec4( xref+1, yref  , zref+1, 1) , FORWARDnormal, FORWARDtangent,FORWARDbinormal, GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][4], type);

                        interleavedData.push_back(v1);
                        interleavedData.push_back(v2);
                        interleavedData.push_back(v3);
                        interleavedData.push_back(v4);
                    }
                }

//                if( mychunk->find(BACKkey) == mychunk->end()  || z == 0) {
                if( z == 0 || (*mychunk)[BACKkey] == AIR ) {

                    bool otherChunkIsAir = true;
                    if(z == 0) {
                        uint64_t mykey = Encode(chunkX, chunkZ-1);
                        if(chunks.find(mykey) != chunks.end()) {
                            Chunk* thischunk = chunks[mykey];
                            int32_t index = y*CHUNK_DIM*CHUNK_DIM + (CHUNK_DIM-1)*CHUNK_DIM + x;
                            if((*thischunk)[index] != AIR) {
                                otherChunkIsAir = false;
                            }
                        }
                    }

                    if (otherChunkIsAir) {
                        int32_t root = ++index;
                        indices.push_back(root);
                        indices.push_back(++index);
                        indices.push_back(++index);
                        indices.push_back(root);
                        indices.push_back(index);
                        indices.push_back(++index);

                        //Texturing and Texture Animation Jin_Kim
                        Vertex v1(glm::vec4( xref  , yref  , zref  , 1), BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0.0625, 0) + UVSet.m_BlockUVSet[type][5], type);
                        Vertex v2( glm::vec4( xref+1, yref  , zref  , 1), BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0, 0) + UVSet.m_BlockUVSet[type][5], type);
                        Vertex v3(glm::vec4( xref+1, yref+1, zref  , 1) , BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0, 0.0625) + UVSet.m_BlockUVSet[type][5], type);
                        Vertex v4(glm::vec4( xref  , yref+1, zref  , 1), BACKnormal, BACKtangent,BACKbinormal, GREEN, glm::vec2(0.0625, 0.0625) + UVSet.m_BlockUVSet[type][5], type);

                        interleavedData.push_back(v1);
                        interleavedData.push_back(v2);
                        interleavedData.push_back(v3);
                        interleavedData.push_back(v4);
                    }
                }
            }//x
        }//z
    }//y

    Mesh* mesh = new Mesh();
    mesh->SetIndices(indices.data(), indices.size());
    mesh->SetInterleaved(true);
    mesh->SetInterleavedData(interleavedData.data(), interleavedData.size());
    mesh->Upload();
    return mesh;
}

ChunkModifier::ChunkModifier(ChunkManager *chunkManager) : chunkManager(chunkManager)
{
}

ChunkModifier::~ChunkModifier()
{
}
