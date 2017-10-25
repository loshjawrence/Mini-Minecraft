#include "chunkcavemodifier.h"

ChunkCaveModifier::ChunkCaveModifier(ChunkManager *manager) : ChunkModifier(manager)
{
}

void ChunkCaveModifier::Initialize()
{
}

void ChunkCaveModifier::Evaluate(Chunk * chunk, std::unordered_map<Chunk*,int8_t>& touched)
{
    mutex.lock();

    if(chunk->caveseed) {
        MakeCave(chunk, touched);
//        MakeCave(glm::vec4(0,50,0,0));
    }
    mutex.unlock();
}

void ChunkCaveModifier::MakeCave(Chunk * chunk, std::unordered_map<Chunk*,int8_t>& touched)
{
    int32_t cavehead_iters;

    for(uint32_t i = 0; i < chunk->caveheads.size(); i++) {

        //assume it can go all the way(-2), if not, hand off progress to another chunk with cavehead_iters and cave head
        glm::vec4 cavehead = chunk->caveheads[i].pos;
        cavehead_iters = chunk->caveheads[i].iters;
        chunk->caveheads[i].iters = -2;


        if( !DigCave(chunk, cavehead,touched) ) {
            //reset iters back to what it was, didnt get anywhere, process it again when new chunk slabs are available
            chunk->caveheads[i].iters = cavehead_iters;
            continue;
        }

        if(!MoveCave(cavehead)) {
            //reset iters back to what it was, didnt get anywhere, process it again when new chunk slabs are available
            chunk->caveheads[i].iters = cavehead_iters;
            continue;
        }
        cavehead_iters--;
        for(;  cavehead_iters > 0; cavehead_iters--) {
            if(!DigCave(chunk, cavehead,touched)) {
                saveCaveHead(cavehead,cavehead_iters);
                break;
            }
            if(!MoveCave(cavehead)) {
                saveCaveHead(cavehead,cavehead_iters);
                break;
            }
        }

        //do a random amount of walks
        if (cavehead_iters == 0) {
            const int32_t flatrandrange = 100;
            int32_t flatrand = rand() % flatrandrange;

            while(flatrand < flatrandrange/2){
                if(!DigCave(chunk, cavehead,touched)) {
                    saveCaveHead(cavehead,cavehead_iters);
                    break;
                }
                if(!MoveCave(cavehead)) {
                    saveCaveHead(cavehead,cavehead_iters);
                    break;
                }
                flatrand = rand() % flatrandrange;
            }
            cavehead_iters = -1;
        }

        if (cavehead_iters == -1) {
            if(!DigCavern(cavehead,touched)) {//save orginal pos if move or dig is bad, return immediately if move is bad
                saveCaveHead(cavehead,cavehead_iters);
            } else {
                cavehead_iters = -2;
            }
        }
    } //for caveheads.size()
}

void ChunkCaveModifier::saveCaveHead(const glm::vec4& cavehead, const int32_t& cavehead_iters) {
    CaveHead newcavehead{cavehead, cavehead_iters};
    int32_t chunkx = std::floor(cavehead[0] / (float)CHUNK_DIM);
    int32_t chunkz = std::floor(cavehead[2] / (float)CHUNK_DIM);
    uint64_t key = chunkManager->Encode(chunkx,chunkz);
    Chunk* mychunk = chunkManager->chunks[key];
    mychunk->caveheads.push_back(newcavehead);
}

bool ChunkCaveModifier::MoveCave(glm::vec4& pos) {
    glm::vec4 lookvec = getLookVec(pos);
    glm::vec4 test = pos + (lookvec * DIG_RADIUS);
    if (chunkExists(test)) {
        pos = test;
        return true;
    } else {
        return false;
    }
}

bool ChunkCaveModifier::chunkExists(const glm::vec4& test) {
    int32_t chunkx = std::floor(test[0] / (float)CHUNK_DIM);
    int32_t chunkz = std::floor(test[2] / (float)CHUNK_DIM);
    uint64_t key = chunkManager->Encode(chunkx,chunkz);
    if(chunkManager->chunks.find(key) != chunkManager->chunks.end()) {
        return true;
    } else {
        return false;
    }
}
bool ChunkCaveModifier::markChunk(const glm::vec4& test, std::unordered_map<Chunk*,int8_t>& touched) {
    int32_t chunkx = std::floor(test[0] / (float)CHUNK_DIM);
    int32_t chunkz = std::floor(test[2] / (float)CHUNK_DIM);
    uint64_t key = chunkManager->Encode(chunkx,chunkz);
    if(chunkManager->chunks.find(key) != chunkManager->chunks.end()) {
        touched[ chunkManager->chunks[key] ] = 1;
        return true;
    } else {
        return false;
    }
}

BlockType ChunkCaveModifier::randResource()
{
    const int32_t flatrandrange = 100;
    const int32_t flatrandom = rand() % flatrandrange + 1;
    const int32_t IRONTHRESH = 50;
    const int32_t COALTHRESH = 0;
    if(flatrandom > IRONTHRESH) {
        return IRON_ORE;
    } else if (flatrandom <=IRONTHRESH && flatrandom > COALTHRESH) {
        return COAL;
    } else {
        return AIR;
    }
}

bool ChunkCaveModifier::DigCave(Chunk * chunk, const glm::vec4& pos, std::unordered_map<Chunk*,int8_t>& touched) {
    bool success = true;
    //snap to block grid
    int32_t refx = pos[0];
    int32_t refy = pos[1];
    int32_t refz = pos[2];
    glm::vec4 refpos(refx,refy,refz,0);

    if (!markChunk( refpos + glm::vec4(DIG_RADIUS-1,0,0           ,0) , touched)
     || !markChunk( refpos + glm::vec4(-DIG_RADIUS ,0,0           ,0) , touched)
     || !markChunk( refpos + glm::vec4(0           ,0,DIG_RADIUS-1,0) , touched)
     || !markChunk( refpos + glm::vec4(0           ,0,-DIG_RADIUS ,0) , touched) )
    {
        success = false;
    }

//    glm::vec4 lookvec = glm::normalize(getLookVec(pos));

    //sphere
    for(int32_t x = -DIG_RADIUS; x < DIG_RADIUS; x++) {
        for(int32_t y = -DIG_RADIUS/2; y < DIG_RADIUS; y++) {
            for(int32_t z = -DIG_RADIUS; z < DIG_RADIUS; z++) {

                int32_t delx = refx + x;
                int32_t dely = refy + y;
                int32_t delz = refz + z;

                glm::vec4 blockpos(delx,dely,delz,0);
                float distance = glm::length(blockpos-refpos);

                float ratio = distance/DIG_RADIUS;

                if(ratio > 1) {
                    continue;
                } else if( dely < 0 || dely > CHUNK_HALF_HEIGHT + MAX_TERRAIN_HEIGHT) {
                    continue;
                } else if ( delx < (chunkManager->curr_chunkX - CHUNK_VIEWDIST)*CHUNK_DIM || delx > (chunkManager->curr_chunkX + CHUNK_VIEWDIST)*CHUNK_DIM ) {
                    continue;
                } else if ( delz < (chunkManager->curr_chunkZ - CHUNK_VIEWDIST)*CHUNK_DIM || delz > (chunkManager->curr_chunkZ + CHUNK_VIEWDIST)*CHUNK_DIM ) {
                    continue;
                }

                int32_t chunkx = std::floor(delx / (float)CHUNK_DIM);
                int32_t chunkz = std::floor(delz / (float)CHUNK_DIM);

                delx = delx - (chunkx*CHUNK_DIM);
                delz = delz - (chunkz*CHUNK_DIM);

                if(chunk->position.x == chunkx && chunk->position.y == chunkz)
                {

        //                uint64_t key = chunkManager->Encode(chunkx,chunkz);

        //                if(chunkManager->chunks.find(key) == chunkManager->chunks.end()) {
        //                    continue;
        //                }

                    int key = dely*CHUNK_DIM*CHUNK_DIM + delz*CHUNK_DIM + delx;
                    BlockType myblock = AIR;

        //                ///DROP A RESOURCE IN CAVE
        //                glm::vec4 normdist = glm::normalize(blockpos-refpos);
        //                float dotprod = glm::dot(lookvec,normdist);
        //                if(ratio > 0.999 && dotprod > 0.2 && dotprod < 0.3) {//edge of delete distance and to the left or right
        //                    if(notDangling(chunkx, chunkz, delx, dely, delz)) {
        //                        myblock = randResource();
        //                    }
        //                }

                    chunk->data[key] = myblock;
                }
            }//z
        }//y
    }//x
    return success;

}


bool ChunkCaveModifier::DigCavern(glm::vec4 cavehead, std::unordered_map<Chunk*,int8_t>& touched) {
    glm::vec4 pos = cavehead;
    bool success = true;
    const int32_t flatrandrange = DIG_RADIUS*2;//max ellipse axis radius
    const float ellradx = rand() % flatrandrange + DIG_RADIUS;//bigger than dig_radius
    const float ellrady = rand() % flatrandrange + DIG_RADIUS;
    const float ellradz = rand() % flatrandrange + DIG_RADIUS;
    const float ellradx_sqrd = ellradx*ellradx;
    const float ellrady_sqrd = ellrady*ellrady;
    const float ellradz_sqrd = ellradz*ellradz;
    const float LAVA_LEVEL = 0.3;

    int32_t maxrad = ellradx;
    if(ellrady > maxrad) {
        maxrad = ellrady;
    }
    if (ellradz > maxrad) {
        maxrad = ellradz;
    }


    ///GET ORIENTATION OF CAVERN
    //convert world pos to chunk coordinates
    float chunkx = (pos[0] / CHUNK_DIM) * CAVE_ENTROPY;
    float chunky = (pos[1] / CHUNK_DIM) * CAVE_ENTROPY;
    float chunkz = (pos[2] / CHUNK_DIM) * CAVE_ENTROPY;

    float perlinY = Perlin::perlinNoise2D(chunkx, chunkz);
    float perlinX = Perlin::perlinNoise3D(chunkx, chunky, chunkz);
    float roty = perlinY * CAVE_Y_RAD_RANGE;
    float rotx = perlinX * CAVE_X_RAD_RANGE;
    if(pos[1] > CAVE_GUIDANCE_LEVEL) {
        rotx = CAVE_X_RAD_CORRECTION;
    }

    glm::mat4 rotymat = glm::rotate( glm::mat4(1), roty, glm::vec3(0,1,0));
    glm::mat4 rotxmat = glm::rotate( glm::mat4(1), rotx, glm::vec3(1,0,0));
    glm::mat4 composite = rotymat * rotxmat;

    glm::vec4 xaxis = composite[0] * ellradx;
    glm::vec4 yaxis = composite[1] * ellrady;
    glm::vec4 zaxis = composite[2] * ellradz;

    ///find ellipse bounding box
    ///x
    int32_t xbound = round(fabs(xaxis[0]));
    if(fabs(yaxis[0]) > xbound) {
        xbound = round(fabs(yaxis[0]));
    }
    if(fabs(zaxis[0]) > xbound) {
        xbound = round(fabs(zaxis[0]));
    }
    ///y
    int32_t ybound = round(fabs(yaxis[1]));
    if(fabs(xaxis[1]) > ybound) {
        ybound = round(fabs(xaxis[1]));
    }
    if(fabs(zaxis[1]) > ybound) {
        ybound = round(fabs(zaxis[1]));
    }
    ///z
    int32_t zbound = round(fabs(zaxis[2]));
    if(fabs(xaxis[2]) > zbound) {
        zbound = round(fabs(xaxis[2]));
    }
    if(fabs(yaxis[2]) > zbound) {
        zbound = fabs(yaxis[2]);
    }

    glm::vec4 lookvec = getLookVec(pos);
    pos += (lookvec*(ellradz*0.5f));

    int32_t refx = pos[0];
    int32_t refy = pos[1];
    int32_t refz = pos[2];
    glm::vec4 refpos(refx,refy,refz,1);
    composite[3] = refpos;
    glm::mat4 invcomposite = glm::inverse(composite);


    if (!markChunk( refpos + glm::vec4(maxrad-1,0,0           ,0) , touched)
     || !markChunk( refpos + glm::vec4(-maxrad ,0,0           ,0) , touched)
     || !markChunk( refpos + glm::vec4(0           ,0,maxrad-1,0) , touched)
     || !markChunk( refpos + glm::vec4(0           ,0,-maxrad ,0) , touched) )
    {
        success = false;
    }

    //ellipse EQ: (x*x)/(xrad*xrad) + (y*y)/(yrad*yrad) + (z*z)/(zrad*zrad) = 1
    for(int32_t x = -maxrad; x < maxrad; x++) {
        for(int32_t y = -maxrad; y < maxrad; y++) {
            for(int32_t z = -maxrad; z < maxrad; z++) {

                int32_t delx = refx + x;
                int32_t dely = refy + y;
                int32_t delz = refz + z;

                if( dely < 0 || dely > CHUNK_HALF_HEIGHT + MAX_TERRAIN_HEIGHT) {
                    continue;
                } else if ( delx < (chunkManager->curr_chunkX - CHUNK_VIEWDIST)*CHUNK_DIM || delx > (chunkManager->curr_chunkX + CHUNK_VIEWDIST)*CHUNK_DIM ) {
                    continue;
                } else if ( delz < (chunkManager->curr_chunkZ - CHUNK_VIEWDIST)*CHUNK_DIM || delz > (chunkManager->curr_chunkZ + CHUNK_VIEWDIST)*CHUNK_DIM ) {
                    continue;
                }

                glm::vec4 delpos_W(delx,dely,delz,1);//worldspace
                glm::vec4 delpos_ell = invcomposite*delpos_W;//ellipsespace

                float ellipseEQ = (delpos_ell[0]*delpos_ell[0]) / (ellradx_sqrd)
                                + (delpos_ell[1]*delpos_ell[1]) / (ellrady_sqrd)
                                + (delpos_ell[2]*delpos_ell[2]) / (ellradz_sqrd);

                if(ellipseEQ > 1.f) {
                    continue;
                }

                //find chunk coord
                int32_t chunkx = std::floor(delx / (float)CHUNK_DIM);
                int32_t chunkz = std::floor(delz / (float)CHUNK_DIM);

                //find coord within chunk
                delx = delx - (chunkx*CHUNK_DIM);
                delz = delz - (chunkz*CHUNK_DIM);

                uint64_t key = chunkManager->Encode(chunkx,chunkz);
                if(chunkManager->chunks.find(key) == chunkManager->chunks.end()) {
                    continue;
                }


                ///PLANT LAVA
                BlockType myblock = AIR;
                if( y+ybound < (LAVA_LEVEL)*2*ybound ) {
                    myblock = LAVA;
                }
                Chunk* mychunk = chunkManager->chunks[key];
                int32_t index = dely*CHUNK_DIM*CHUNK_DIM + delz*CHUNK_DIM + delx;
                (*mychunk)[index] = myblock;

            }//z
        }//y
    }//x
    return success;
}

glm::vec4 ChunkCaveModifier::getLookVec(const glm::vec4 &pos)
{
    //convert world pos to chunk coordinates
    float chunkx = (pos[0] / CHUNK_DIM) * CAVE_ENTROPY;
    float chunky = (pos[1] / CHUNK_DIM) * CAVE_ENTROPY;
    float chunkz = (pos[2] / CHUNK_DIM) * CAVE_ENTROPY;

    float perlinY = Perlin::perlinNoise2D(chunkx, chunkz);
    float perlinX = Perlin::perlinNoise3D(chunkx, chunky, chunkz);
    float roty = perlinY * CAVE_Y_RAD_RANGE;
    float rotx = perlinX * CAVE_X_RAD_RANGE;
    if(pos[1] > CAVE_GUIDANCE_LEVEL) {
        rotx = CAVE_X_RAD_CORRECTION;
    }
    glm::mat4 rotymat = glm::rotate( glm::mat4(1), roty, glm::vec3(0,1,0));
    glm::mat4 rotxmat = glm::rotate( glm::mat4(1), rotx, glm::vec3(1,0,0));
    glm::mat4 composite = rotymat * rotxmat;
    return composite[2];//z is look
}

bool ChunkCaveModifier::notDangling(const int32_t chunkX, const int32_t chunkZ, const int32_t x, const int32_t y, const int32_t z) {
    const int32_t thresh = 2;
    int32_t notair = 0;
    int32_t UPkey       = (y+1) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x  );
    int32_t DOWNkey     = (y-1) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x  );
    int32_t RIGHTkey    = (y  ) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x+1);
    int32_t LEFTkey     = (y  ) *CHUNK_DIM*CHUNK_DIM + (z  )*CHUNK_DIM + (x-1);
    int32_t FORWARDkey  = (y  ) *CHUNK_DIM*CHUNK_DIM + (z+1)*CHUNK_DIM + (x  );
    int32_t BACKkey     = (y  ) *CHUNK_DIM*CHUNK_DIM + (z-1)*CHUNK_DIM + (x  );

    uint64_t key = chunkManager->Encode(chunkX,chunkZ);
    Chunk* mychunk = chunkManager->chunks[key];


    if( y == CHUNK_HEIGHT-1 || (*mychunk)[UPkey] != AIR ) {
        if(++notair > thresh) return true;
    }

    if( y == 0 || (*mychunk)[DOWNkey] == AIR ) {
        if(++notair > thresh) return true;
    }

    if( x == 0 || (*mychunk)[LEFTkey] == AIR) {
        bool otherChunkIsAir = true;
        if(x == 0) {
            uint64_t mykey = chunkManager->Encode(chunkX-1, chunkZ);
            if(chunkManager->chunks.find(mykey) != chunkManager->chunks.end()) {
                Chunk* thischunk = chunkManager->chunks[mykey];
                int32_t index = y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + CHUNK_DIM-1;
                if((*thischunk)[index] != AIR) {
                    otherChunkIsAir = false;
                }
            }
        }
        if(otherChunkIsAir) {
            if(++notair > thresh) return true;
        }
    }

    if( x == CHUNK_DIM-1 || (*mychunk)[RIGHTkey] == AIR ) {
        bool otherChunkIsAir = true;
        if(x == CHUNK_DIM-1) {
            uint64_t mykey = chunkManager->Encode(chunkX+1, chunkZ);
            if(chunkManager->chunks.find(mykey) != chunkManager->chunks.end()) {
                Chunk* thischunk = chunkManager->chunks[mykey];
                int32_t index = y *CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + 0;
                if((*thischunk)[index] != AIR) {
                    otherChunkIsAir = false;
                }
            }
        }
        if(otherChunkIsAir) {
            if(++notair > thresh) return true;
        }
    }

    if( z == CHUNK_DIM-1 || (*mychunk)[FORWARDkey] == AIR ) {
        bool otherChunkIsAir = true;
        if(z == CHUNK_DIM-1) {
            uint64_t mykey = chunkManager->Encode(chunkX, chunkZ+1);
            if(chunkManager->chunks.find(mykey) != chunkManager->chunks.end()) {
                Chunk* thischunk = chunkManager->chunks[mykey];
                int32_t index = y*CHUNK_DIM*CHUNK_DIM + (0)*CHUNK_DIM + x;
                if((*thischunk)[index] != AIR) {
                    otherChunkIsAir = false;
                }
            }
        }
        if (otherChunkIsAir) {
            if(++notair > thresh) return true;
        }
    }
    if( z == 0 || (*mychunk)[BACKkey] == AIR ) {

        bool otherChunkIsAir = true;
        if(z == 0) {
            uint64_t mykey = chunkManager->Encode(chunkX, chunkZ-1);
            if(chunkManager->chunks.find(mykey) != chunkManager->chunks.end()) {
                Chunk* thischunk = chunkManager->chunks[mykey];
                int32_t index = y*CHUNK_DIM*CHUNK_DIM + (CHUNK_DIM-1)*CHUNK_DIM + x;
                if((*thischunk)[index] != AIR) {
                    otherChunkIsAir = false;
                }
            }
        }

        if (otherChunkIsAir) {
            if(++notair > thresh) return true;
        }
    }
    return false;
}
