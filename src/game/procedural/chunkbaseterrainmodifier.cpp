#include "chunkbaseterrainmodifier.h"

ChunkBaseTerrainModifier::ChunkBaseTerrainModifier(ChunkManager * chunkManager) : ChunkModifier(chunkManager), dimensions(64, 64, 64)
{
}

void ChunkBaseTerrainModifier::Initialize()
{
}

void ChunkBaseTerrainModifier::Evaluate(Chunk * chunk, std::unordered_map<Chunk *, int8_t> &touched)
{
    int chunkX = chunk->position.x;
    int chunkZ = chunk->position.y;

    int64_t hfieldX = std::floor(chunkX / (float)CHUNKS_PER_HFIELD);
    int64_t hfieldZ = std::floor(chunkZ / (float)CHUNKS_PER_HFIELD);

    mutex.lock();
    Terrain* heightfield = findHField(hfieldX, hfieldZ);    
    mutex.unlock();

    int32_t chunkX_offset = (std::abs(chunkX) % CHUNKS_PER_HFIELD) * CHUNK_DIM;
    int32_t chunkZ_offset = (std::abs(chunkZ) % CHUNKS_PER_HFIELD) * CHUNK_DIM;

    if(chunkX < 0 && chunkX_offset != 0) {
        chunkX_offset = HFIELD_DIM - chunkX_offset;
    }

    if(chunkZ < 0 && chunkZ_offset != 0) {
        chunkZ_offset = HFIELD_DIM - chunkZ_offset;
    }


    chunk->noise2d = Perlin::perlinNoise2D(chunkX, chunkZ);
    for(int y = 0; y < CHUNK_HEIGHT/CHUNK_DIM; y++) {
        chunk->noise3d[y] = Perlin::perlinNoise3D(chunkX, y, chunkZ);
    }

    int32_t height = heightfield->height[chunkX_offset][chunkZ_offset];

    ///PLANT CAVE SEED
    float seedtest = fabs(chunk->noise3d[0]) + fabs(chunk->noise3d[(CHUNK_HEIGHT/CHUNK_DIM) - 1]);
    if ( (height < CAVE_HEIGHT_THRESH_MAX && height > CAVE_HEIGHT_THRESH_MIN) && seedtest > CAVE_SEED_THRESH) {
//        std::cout << "\nchunk recieved a cave seed:" << "\nx: " << chunkX << "\nz: " << chunkZ;
        chunk->caveseed = true;

        CaveHead cavehead{
                                glm::vec4(chunkX*CHUNK_DIM, height+CHUNK_HALF_HEIGHT, chunkZ*CHUNK_DIM, 0),
                                CAVE_ITERATIONS
                         };

        chunk->caveheads.push_back(cavehead);
    }

    for(int32_t x = chunkX_offset; x < chunkX_offset + CHUNK_DIM; x++) {
        for(int32_t z = chunkZ_offset; z < chunkZ_offset + CHUNK_DIM; z++) {

            const uint32_t stoneheight = (CHUNK_HALF_HEIGHT - STONE_LEVEL) + STONE_HEIGHT_SCALE*Perlin::perlinNoise2D(x+chunkX*CHUNK_DIM,z+chunkZ*CHUNK_DIM);
            height = heightfield->height[x][z] + CHUNK_HALF_HEIGHT;
            int32_t width = x - chunkX_offset;
            int32_t length = z - chunkZ_offset;

            int32_t y = 0;
            int32_t key = y*CHUNK_DIM*CHUNK_DIM + length*CHUNK_DIM + width;
            (*chunk)[key] = BEDROCK;

            if(stoneheight < (uint32_t) height) {
                for(y = 1; y <= (int) stoneheight; y++) {
                    int32_t key = y*CHUNK_DIM*CHUNK_DIM + length*CHUNK_DIM + width;
                    //perlin3d for resource
                    BlockType myblock = STONE;
                    const float RESOURCE_THRESH = 0.975;
                    const float perlin = Perlin::noise3D(x+chunkX*CHUNK_DIM, y,z+chunkZ*CHUNK_DIM);
                    if( fabs(perlin) > RESOURCE_THRESH) {
                        if(perlin > 0.975){
                            myblock = COAL;
                        } else {
                            myblock = IRON_ORE;
                        }
                    }
                    (*chunk)[key] = myblock;
                }
                for(y = stoneheight+1; y<height; y++) {
                    int32_t key = y*CHUNK_DIM*CHUNK_DIM + length*CHUNK_DIM + width;
                    (*chunk)[key] = DIRT;
                }
                key = height*CHUNK_DIM*CHUNK_DIM + length*CHUNK_DIM + width;
                (*chunk)[key] = GRASS;
            } else {
                for(y = 1; y <= height; y++) {
                    int32_t key = y*CHUNK_DIM*CHUNK_DIM + length*CHUNK_DIM + width;
                    (*chunk)[key] = STONE;
                }
            }//if stoneheight<height
        }//z
    }//x
}

Terrain *ChunkBaseTerrainModifier::findHField(const int64_t &x, const int64_t &z)
{
    uint64_t key = chunkManager->Encode(x,z);
    std::unordered_map<uint64_t, Terrain*>::iterator it = terrainMap.find(key);

    if(it == terrainMap.end())
    {
        Terrain * t =  CreateTerrain(x, z);
        terrainMap[key] = t;
        return t;
    }

    return it->second;
}


Terrain *ChunkBaseTerrainModifier::CreateTerrain(int X_Coordinate, int Z_Coordinate)
{
//    std::cout << std::to_string(X_Coordinate) << ", " + std::to_string(Z_Coordinate) << std::endl;
    double amp = 32.0;
    double freq = 1.0;
    double Wave[TERRAINSIZE][TERRAINSIZE];

    for(uint i = 0; i < TERRAINSIZE; i++)
    {
        for(uint j = 0; j < TERRAINSIZE; j++)
        {
             Wave[i][j] = 0;
        }
    }

    Terrain* terrain = new Terrain;
    terrain->X_coordinate = X_Coordinate;
    terrain->Z_coordinate = Z_Coordinate;

    //terrain->bVisible = true;

    bool bAdjXR = false;
    bool bAdjXL = false;
    bool bAdjZF = false;
    bool bAdjZB = false;

    glm::vec2 AdjGradient00[4][2][2];
    glm::vec2 AdjGradient01[4][3][3];
    glm::vec2 AdjGradient02[4][5][5];
    glm::vec2 AdjGradient03[4][9][9];
    glm::vec2 AdjGradient04[4][17][17];
    glm::vec2 AdjGradient05[4][33][33];
    glm::vec2 AdjGradient06[4][65][65];

//    Terrain *XLTerrain = nullptr;
//    Terrain *XRTerrain = nullptr;
//    Terrain *ZFTerrain = nullptr;
//    Terrain *ZBTerrain = nullptr;

    //Find Adjacent terrains
    for(uint i=0; i<m_Terrains.size(); i++)
    {
        Terrain *cur = m_Terrains.at(i);

        //X Left adj
        if(cur->X_coordinate + 1 == terrain->X_coordinate && cur->Z_coordinate == terrain->Z_coordinate)
        {
            bAdjXL = true;
//            XLTerrain = cur;
            CopyGradientVector(AdjGradient00, AdjGradient01, AdjGradient02, AdjGradient03, AdjGradient04, AdjGradient05,
                               AdjGradient06, cur, 0);

        }
        //X Right adj
        if(cur->X_coordinate - 1 == terrain->X_coordinate && cur->Z_coordinate == terrain->Z_coordinate)
        {
            bAdjXR = true;
//            XRTerrain = cur;
            CopyGradientVector(AdjGradient00, AdjGradient01, AdjGradient02, AdjGradient03, AdjGradient04, AdjGradient05,
                               AdjGradient06, cur, 1);
        }
        //Z Down adj
        if(cur->X_coordinate == terrain->X_coordinate && cur->Z_coordinate + 1 == terrain->Z_coordinate)
        {
            bAdjZB = true;
//            ZBTerrain = cur;
            CopyGradientVector(AdjGradient00, AdjGradient01, AdjGradient02, AdjGradient03, AdjGradient04, AdjGradient05,
                               AdjGradient06, cur, 2);
        }
        //Z Up adj
        if(cur->X_coordinate == terrain->X_coordinate && cur->Z_coordinate - 1 == terrain->Z_coordinate)
        {
            bAdjZF = true;
//            ZFTerrain = cur;
            CopyGradientVector(AdjGradient00, AdjGradient01, AdjGradient02, AdjGradient03, AdjGradient04, AdjGradient05,
                               AdjGradient06, cur, 3);
        }
    }

    double maxAmplitude = 0.0;

    //create perlin terrain
    for(uint i=0; i<7; i++)
    {
        maxAmplitude += amp;
        perlin(freq, freq, Wave, amp, terrain, AdjGradient00, AdjGradient01, AdjGradient02,
                                          AdjGradient03, AdjGradient04, AdjGradient05, AdjGradient06,
                                          bAdjXR, bAdjXL, bAdjZF, bAdjZB);
        freq *= 2;
        amp /= 2;
    }

    for(uint i = 0; i < TERRAINSIZE; i++)
    {
        for(uint j = 0; j < TERRAINSIZE; j++)
        {
             terrain->height[i][j] = Wave[i][j];
        }
    }

    m_Terrains.push_back(terrain);

    return terrain;
}

void ChunkBaseTerrainModifier::GeneratedNewWorld(double UserPosX, double UserPosZ)
{
    int CurX = floor((UserPosX/(double)TERRAINSIZE));
    int CurZ = floor((UserPosZ/(double)TERRAINSIZE));

    bool bExist_NX = false;
    bool bExist_PX = false;
    bool bExist_NZ = false;
    bool bExist_PZ = false;
    bool bExist_PXPZ = false;
    bool bExist_PXNZ = false;
    bool bExist_NXPZ = false;
    bool bExist_NXNZ = false;

    for(uint i=0; i<m_Terrains.size(); i++)
    {
        Terrain* curTerrain = m_Terrains.at(i);

        //X-
        if(curTerrain->X_coordinate == CurX - 1 && curTerrain->Z_coordinate == CurZ)
        {
            bExist_NX = true;
        }
        //X+
        else if(curTerrain->X_coordinate == CurX + 1 && curTerrain->Z_coordinate == CurZ)
        {
            bExist_PX = true;
        }
        //Z-
        else if(curTerrain->X_coordinate == CurX && curTerrain->Z_coordinate == CurZ - 1)
        {
            bExist_NZ = true;
        }
        //Z+
        else if(curTerrain->X_coordinate == CurX && curTerrain->Z_coordinate == CurZ + 1)
        {
            bExist_PZ = true;
        }
        //X+Z+
        else if(curTerrain->X_coordinate == CurX + 1 && curTerrain->Z_coordinate == CurZ + 1)
        {
            bExist_PXPZ = true;
        }
        //X+Z-
        else if(curTerrain->X_coordinate == CurX + 1 && curTerrain->Z_coordinate == CurZ - 1)
        {
            bExist_PXNZ = true;
        }
        //X-Z+
        if(curTerrain->X_coordinate == CurX - 1 && curTerrain->Z_coordinate == CurZ + 1)
        {
            bExist_NXPZ = true;
        }
        //X-Z-
        else if(curTerrain->X_coordinate == CurX - 1 && curTerrain->Z_coordinate == CurZ - 1)
        {
            bExist_NXNZ = true;
        }
    }

    if(bExist_NX == false)
    {
        CreateTerrain(CurX-1, CurZ);
    }
    if(bExist_PX == false)
    {
        CreateTerrain(CurX+1, CurZ);
    }
    if(bExist_NZ == false)
    {
        CreateTerrain(CurX, CurZ-1);
    }
    if(bExist_PZ == false)
    {
        CreateTerrain(CurX, CurZ+1);
    }
    if(bExist_PXPZ == false)
    {
        CreateTerrain(CurX+1, CurZ+1);
    }
    if(bExist_PXNZ == false)
    {
        CreateTerrain(CurX+1, CurZ-1);
    }
    if(bExist_NXPZ == false)
    {
        CreateTerrain(CurX-1, CurZ+1);
    }
    if(bExist_NXNZ == false)
    {
        CreateTerrain(CurX-1, CurZ-1);
    }
}

void ChunkBaseTerrainModifier::perlin(uint x, uint y, double (&Wave)[TERRAINSIZE][TERRAINSIZE], double amp, Terrain* o,
                                        glm::vec2 AdjGradient00[4][2][2], glm::vec2 AdjGradient01[4][3][3], glm::vec2 AdjGradient02[4][5][5],
                                        glm::vec2 AdjGradient03[4][9][9], glm::vec2 AdjGradient04[4][17][17], glm::vec2 AdjGradient05[4][33][33],
                                        glm::vec2 AdjGradient06[4][65][65], bool bAdjXR, bool bAdjXL, bool bAdjZF, bool bAdjZB)
{
    glm::vec2 **Gradient;
    if(x == 1)
    {
        o->Gradient00 = new glm::vec2*[x + 1];
        Gradient = o->Gradient00;
    }
    else if(x == 2)
    {
        o->Gradient01 = new glm::vec2*[x + 1];
        Gradient = o->Gradient01;
    }
    else if(x == 4)
    {
        o->Gradient02 = new glm::vec2*[x + 1];
        Gradient = o->Gradient02;
    }
    else if(x == 8)
    {
        o->Gradient03 = new glm::vec2*[x + 1];
        Gradient = o->Gradient03;
    }
    else if(x == 16)
    {
        o->Gradient04 = new glm::vec2*[x + 1];
        Gradient = o->Gradient04;
    }
    else if(x == 32)
    {
        o->Gradient05 = new glm::vec2*[x + 1];
        Gradient = o->Gradient05;
    }
    else if(x == 64)
    {
        o->Gradient06 = new glm::vec2*[x + 1];
        Gradient = o->Gradient06;
    }

    //generate Perlin
    //Gradient = new glm::vec2*[x + 1];
    for(uint i=0; i<x+1; i++)
    {
        Gradient[i] = new glm::vec2[y + 1];
    }

    glm::vec2 DGradient[2][2];

    for(uint i=0; i<x+1; i++)
    {
        for(uint j=0; j<y+1; j++)
        {
            glm::vec2 temp = glm::vec2( ((((double)rand())/RAND_MAX)-0.5)*2 , ((((double)rand())/RAND_MAX)-0.5)*2 );
            temp = glm::normalize(temp);
            Gradient[i][j] = temp;
            DGradient[i][j] = temp;
        }
    }

    if(bAdjXL)
    {
        for(uint i=0; i<x+1; i++)
        {
            if(x == 1)
            {
                Gradient[0][i] = AdjGradient00[0][x][i];
            }
            else if(x == 2)
            {
                Gradient[0][i] = AdjGradient01[0][x][i];
            }
            else if(x == 4)
            {
                Gradient[0][i] = AdjGradient02[0][x][i];
            }
            else if(x == 8)
            {
                Gradient[0][i] = AdjGradient03[0][x][i];
            }
            else if(x == 16)
            {
                Gradient[0][i] = AdjGradient04[0][x][i];
            }
            else if(x == 32)
            {
                Gradient[0][i] = AdjGradient05[0][x][i];
            }
            else if(x == 64)
            {
                Gradient[0][i] = AdjGradient06[0][x][i];
            }
        }
    }
    if(bAdjXR)
    {
        for(uint i=0; i<x+1; i++)
        {
            if(x == 1)
            {
                Gradient[x][i] = AdjGradient00[1][0][i];
            }
            else if(x == 2)
            {
                Gradient[x][i] = AdjGradient01[1][0][i];
            }
            else if(x == 4)
            {
                Gradient[x][i] = AdjGradient02[1][0][i];
            }
            else if(x == 8)
            {
                Gradient[x][i] = AdjGradient03[1][0][i];
            }
            else if(x == 16)
            {
                Gradient[x][i] = AdjGradient04[1][0][i];
            }
            else if(x == 32)
            {
                Gradient[x][i] = AdjGradient05[1][0][i];
            }
            else if(x == 64)
            {
                Gradient[x][i] = AdjGradient06[1][0][i];
            }
        }
    }
    if(bAdjZB)
    {
        for(uint i=0; i<y+1; i++)
        {
            if(y == 1)
            {
                Gradient[i][0] = AdjGradient00[2][i][y];
            }
            else if(y == 2)
            {
                Gradient[i][0] = AdjGradient01[2][i][y];
            }
            else if(y == 4)
            {
                Gradient[i][0] = AdjGradient02[2][i][y];
            }
            else if(y == 8)
            {
                Gradient[i][0] = AdjGradient03[2][i][y];
            }
            else if(y == 16)
            {
                Gradient[i][0] = AdjGradient04[2][i][y];
            }
            else if(y == 32)
            {
                Gradient[i][0] = AdjGradient05[2][i][y];
            }
            else if(y == 64)
            {
                Gradient[i][0] = AdjGradient06[2][i][y];
            }
        }
    }
    if(bAdjZF)
    {
        for(uint i=0; i<y+1; i++)
        {
            if(y == 1)
            {
                Gradient[i][y] = AdjGradient00[3][i][0];
            }
            else if(y == 2)
            {
                Gradient[i][y] = AdjGradient01[3][i][0];
            }
            else if(y == 4)
            {
                Gradient[i][y] = AdjGradient02[3][i][0];
            }
            else if(y == 8)
            {
                Gradient[i][y] = AdjGradient03[3][i][0];
            }
            else if(y == 16)
            {
                Gradient[i][y] = AdjGradient04[3][i][0];
            }
            else if(y == 32)
            {
                Gradient[i][y] = AdjGradient05[3][i][0];
            }
            else if(y == 64)
            {
                Gradient[i][y] = AdjGradient06[3][i][0];
            }
        }
    }


    double Max = -9999;
    double Min = 9999;
    for(uint i=0; i<TERRAINSIZE; i++)
    {
        for(uint j=0; j<TERRAINSIZE; j++)
        {
            glm::vec2 StandardDot = glm::vec2( ((double)x/TERRAINSIZE)*i + ((double)x/TERRAINSIZE)*0.5, ((double)y/TERRAINSIZE)*j + ((double)y/TERRAINSIZE)*0.5 );

            uint X0 = (int)(((double)x/TERRAINSIZE)*i);
            uint X1 = X0 + 1;
            uint Y0 = (int)(((double)y/TERRAINSIZE)*j);
            uint Y1 = Y0 + 1;

            glm::vec2 dist00 = glm::vec2(StandardDot.x - X0, StandardDot.y - Y0);
            glm::vec2 dist10 = glm::vec2(StandardDot.x - (X1), StandardDot.y - Y0);
            glm::vec2 dist01 = glm::vec2(StandardDot.x - X0, StandardDot.y - (Y1));
            glm::vec2 dist11 = glm::vec2(StandardDot.x - (X1), StandardDot.y - (Y1));

            int GX = (int)(((double)x/TERRAINSIZE)*i);
            int GY = (int)(((double)y/TERRAINSIZE)*j);

            double DotResult00 =  glm::dot(dist00, Gradient[GX][GY]);
            double DotResult10 =  glm::dot(dist10, Gradient[GX+1][GY]);
            double DotResult01 =  glm::dot(dist01, Gradient[GX][GY+1]);
            double DotResult11 =  glm::dot(dist11, Gradient[GX+1][GY+1]);

            //Bi-cubic interpolation
            // X00 ~ X01
            double X00_X01 = StandardDot.x - X0;//((((double)x/TERRAINSIZE)*i)  -  (int)(((double)x/TERRAINSIZE)*i));
            double Y00 = Fadefuction( 1 - X00_X01 )*DotResult00 +
                             Fadefuction( X00_X01 )*DotResult10;

            // X10 ~ X11
            double X10_X11 = StandardDot.x - X0;//((((double)x/TERRAINSIZE)*i)  -  (int)(((double)x/TERRAINSIZE)*i));
            double Y11 = Fadefuction(1 - X10_X11 )*DotResult01 +
                             Fadefuction(  X10_X11 )*DotResult11;

            // Y0 ~ Y1
            double Y0_Y1 = StandardDot.y - Y0;//((((double)y/TERRAINSIZE)*j)  -  (int)(((double)y/TERRAINSIZE)*j));
            double result = Fadefuction(1 - Y0_Y1)*Y00 +  Fadefuction(Y0_Y1)*Y11;


            Wave[i][j] += result*amp;

            if(Max <  Wave[i][j])
            {
                Max =  Wave[i][j];
            }

            if(Min >  Wave[i][j])
            {
                Min =  Wave[i][j];
            }
        }
    }


    //Normalize

    /*
    for(uint i=0; i<TERRAINSIZE; i++)
    {
        for(uint j=0; j<TERRAINSIZE; j++)
        {
            Min < 0 ? Min = -1.0*Min : Min = Min;
            Wave[i][j] += Min;
            Wave[i][j] /= (Max + Min);
            Wave[i][j] *= 16.0;
        }
    }
    */
}

void ChunkBaseTerrainModifier::CopyGradientVector(glm::vec2 AdjGradient00[4][2][2], glm::vec2 AdjGradient01[4][3][3], glm::vec2 AdjGradient02[4][5][5],
glm::vec2 AdjGradient03[4][9][9], glm::vec2 AdjGradient04[4][17][17], glm::vec2 AdjGradient05[4][33][33],
glm::vec2 AdjGradient06[4][65][65], Terrain *cur, uint index)
{
    for(uint i=0; i<2; i++)
    {
        for(uint j=0; j<2; j++)
        {
             AdjGradient00[index][i][j] = cur->Gradient00[i][j];
        }
    }

    for(uint i=0; i<3; i++)
    {
        for(uint j=0; j<3; j++)
        {
             AdjGradient01[index][i][j] = cur->Gradient01[i][j];
        }
    }

    for(uint i=0; i<5; i++)
    {
        for(uint j=0; j<5; j++)
        {
             AdjGradient02[index][i][j] = cur->Gradient02[i][j];
        }
    }

    for(uint i=0; i<9; i++)
    {
        for(uint j=0; j<9; j++)
        {
             AdjGradient03[index][i][j] = cur->Gradient03[i][j];
        }
    }

    for(uint i=0; i<17; i++)
    {
        for(uint j=0; j<17; j++)
        {
             AdjGradient04[index][i][j] = cur->Gradient04[i][j];
        }
    }

    for(uint i=0; i<33; i++)
    {
        for(uint j=0; j<33; j++)
        {
             AdjGradient05[index][i][j] = cur->Gradient05[i][j];
        }
    }

    for(uint i=0; i<65; i++)
    {
        for(uint j=0; j<65; j++)
        {
             AdjGradient06[index][i][j] = cur->Gradient06[i][j];
        }
    }
}

double ChunkBaseTerrainModifier::Fadefuction(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}
