#include "weathermanager.h"


void WeatherManager::init(ChunkManager* chunkman, Player* player1, WeatherSystem* weathersys, DirectionalLight* thesun) {
    chunkManager = chunkman;
    player = player1;
    weather_postprocesser = weathersys;
    sun = thesun;
    wb_index = 0; wb_size = 0; forcast = CLEAR; forcast_start = 0; forcast_end = 0; last_tick = 0;
    bDisable_weather = false;

    weather_buffer = std::vector<Slab>(0);
    meshrenderers = std::vector<MeshRenderer*>(0);
    weathermaterial = new Material("flat");
    weathermaterial->SetVector("Color", glm::vec4(1,1,1,1));
    weathermaterial->SetFeature(GL_BLEND, true);
    weathermaterial->SetBlendOperation(Material::BlendOperation(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    weathermaterial->SetRenderingQueue(Material::Transparent);
//    weathermaterial->SetFeature(GL_CULL_FACE, true);
//    glCullFace(GL_FRONT);
    weathercontainer = GameObject::Instantiate("weathercontainer");
    cloudcontainer = GameObject::Instantiate("cloudcontainer");
    snowcovercontainer = GameObject::Instantiate("snowcovercontainer");
    FORCAST startforcast = RAIN;//(FORCAST)(rand() % FORCAST_MAX_INDEX + 1);//+1 becuase we dont want to start clear
    updateForcast(startforcast);
}

void WeatherManager::Update() {
    float curr_time = Engine::Time();
    if(forcast != CLEAR) {
        elapsed_time = curr_time - last_tick;
        elapsed_spawn_time = curr_time - last_spawn_tick;
        updateSlabs();
        updateMeshes();
        if(curr_time < forcast_end) {
            if(elapsed_spawn_time > p_spawntime) {
                if(wb_size < numslabs) {
                    buildSlab();
                } else { //buffer full, start recycling
                    recycleSlab();
                }
                last_spawn_tick = curr_time;
            }
            if(forcast == SNOW) {
                updateSnowCover(curr_time);
            }
        } else { //forcast_end, begin retiring slabs
            if(elapsed_spawn_time > p_spawntime) {
                if(wb_size < numslabs) {
                    //let fall, but update retire count
                    wb_retired_count++;
                } else {
                    retireSlab();
                }
                if(forcast == SNOW) {
                    updateSnowCover(curr_time);
                }
                last_spawn_tick = curr_time;
            }
            //clean up
            if(wb_retired_count >= numslabs) {
                updateForcast(CLEAR);
            }
        }
        last_tick = curr_time;
    } else { //clear
        if(curr_time < forcast_end) {
            //nothing, for now
        } else {
            FORCAST nextforcast = (FORCAST)(rand() % FORCAST_MAX_INDEX + 1);//+1 becuase we want non-CLEAR weather
            updateForcast(nextforcast);
        }

    }
}

void WeatherManager::retireSlab() {
    std::vector<glm::vec3>& retired = weather_buffer[wb_index].data;
    const int32_t retired_size = retired.size();

    for(uint32_t i = 0; i < retired_size; i++) {
        retired[i][0] = 0;
        retired[i][1] = 0;
        retired[i][2] = 0;
    }

    //update mesh data too
    std::vector<Vertex>& myvertices = slab_vertices[wb_index];
    uint32_t myvertices_size = myvertices.size();
    for(int j = 0; j < myvertices_size; j++) {
        myvertices[j].position[0] = 0;
        myvertices[j].position[1] = 0;
        myvertices[j].position[2] = 0;
    }

    std::vector<Vertex>& interleaved_data = slab_vertices[wb_index];
    Mesh* mesh = &(meshes[wb_index]);
    mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
    mesh->Upload();

    wb_index = (wb_index + 1) % numslabs;
    wb_retired_count++;
}

void WeatherManager::buildMesh(const int32_t& index) {
        std::vector<GLuint>& indices = slab_indices[index];
        std::vector<Vertex>& interleaved_data = slab_vertices[index];
        Mesh* mesh = &(meshes[index]);

        Slab& myslab = weather_buffer[index];
        const int32_t myslab_size = myslab.data.size();
        GLuint curr_index = 0;

        for(uint32_t i = 0; i < myslab_size; i++) {
            const glm::vec3 particle = myslab[i];
            addRectanguloid(indices, interleaved_data, curr_index, particle,WEATHER);
        }

        mesh->SetIndices(indices.data(), indices.size(), false);
        mesh->SetInterleaved(true);
        mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
        mesh->SetWriteOnly(false);
        mesh->Upload();

        MeshRenderer * renderer = weathercontainer->AddComponent<MeshRenderer>();
        renderer->SetMesh(mesh);
        renderer->SetMaterial(weathermaterial);
        meshrenderers[index] = renderer;
}

void WeatherManager::updateSlabs() {
    float dist_traveled = p_velocity * elapsed_time;
    float dist_windx = windvector[0] * elapsed_time;
    float dist_windz = windvector[1] * elapsed_time;
    if(wb_size < numslabs) {
        //update only those allocated in weather buffer so far
       for(uint32_t i = 0; i < wb_size; i++) {
           std::vector<glm::vec3>& myslab = weather_buffer[i].data;
           const int32_t myslab_size = myslab.size();
           myslab[0][1] -= dist_traveled;
           const float y_slab = myslab[0][1];
           for(uint32_t j = 1; j < myslab_size; j++) {
               myslab[j][0] += dist_windx;
               myslab[j][1] = y_slab;
               myslab[j][2] += dist_windz;
           }
       }
    } else {
        //update all
       for(uint32_t i = 0; i < numslabs; i++) {
           std::vector<glm::vec3>& myslab = weather_buffer[i].data;
           const int32_t myslab_size = myslab.size();
           myslab[0][1] -= dist_traveled;
           const float y_slab = myslab[0][1];
           for(uint32_t j = 1; j < myslab_size; j++) {
               myslab[j][0] += dist_windx;
               myslab[j][1] = y_slab;
               myslab[j][2] += dist_windz;
           }
       }
    }
}

void WeatherManager::updateMeshes() {
    float dist_traveled = p_velocity * elapsed_time;
    float dist_windx = windvector[0] * elapsed_time;
    float dist_windz = windvector[1] * elapsed_time;
    if(wb_size < numslabs) { //update only those allocated in weather buffer so far
       for(uint32_t i = 0; i < wb_size; i++) {
           std::vector<Vertex>& myvertices = slab_vertices[i];
           uint32_t myvertices_size = myvertices.size();
           for(int j = 0; j < myvertices_size; j++) {
               myvertices[j].position[0] += dist_windx;
               myvertices[j].position[1] -= dist_traveled;
               myvertices[j].position[2] += dist_windz;
           }

           std::vector<Vertex>& interleaved_data = slab_vertices[i];
           Mesh* mesh = &(meshes[i]);
           mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
           mesh->Upload();
       }
    } else { //update entire buffer since its full
       for(uint32_t i = 0; i < numslabs; i++) {
           std::vector<Vertex>& myvertices = slab_vertices[i];
           uint32_t myvertices_size = myvertices.size();
           for(int j = 0; j < myvertices_size; j++) {
               myvertices[j].position[0] += dist_windx;
               myvertices[j].position[1] -= dist_traveled;
               myvertices[j].position[2] += dist_windz;
           }

           std::vector<Vertex>& interleaved_data = slab_vertices[i];
           Mesh* mesh = &(meshes[i]);
           mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
           mesh->Upload();
       }
    }
}

void WeatherManager::recycleSlab() {
    glm::vec3 playerpos =  player->GetGameObject()->GetTransform()->WorldPosition();
    float x_new = playerpos[0];
    float z_new = playerpos[2];
    float x_offset = x_new - weather_buffer[wb_index].slab_spawn_center_x;
    float z_offset = z_new - weather_buffer[wb_index].slab_spawn_center_z;
    weather_buffer[wb_index].slab_spawn_center_x = x_new;
    weather_buffer[wb_index].slab_spawn_center_z = z_new;

    std::vector<glm::vec3>& recycled = weather_buffer[wb_index].data;
    const int32_t recycled_size = recycled.size();
    const float reset_displacement = WEATHER_START_HEIGHT - recycled[0][1];

    for(uint32_t i = 0; i < recycled_size; i++) {
        recycled[i][0] += x_offset;
        recycled[i][1] += reset_displacement;
        recycled[i][2] += z_offset;
    }

    //update mesh data too
    std::vector<Vertex>& myvertices = slab_vertices[wb_index];
    uint32_t myvertices_size = myvertices.size();
    for(int j = 0; j < myvertices_size; j++) {
        myvertices[j].position[0] += x_offset;
        myvertices[j].position[1] += reset_displacement;
        myvertices[j].position[2] += z_offset;
    }

    std::vector<Vertex>& interleaved_data = slab_vertices[wb_index];
    Mesh* mesh = &(meshes[wb_index]);
    mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
    mesh->Upload();

    wb_index = (wb_index + 1) % numslabs;
    wb_size++;
}

void WeatherManager::buildSlab() {
    int32_t sample_index = rand() % WEATHER_SLAB_VARIETY;

    weather_buffer[wb_index].data = slab_pool[sample_index].data;

    glm::vec3 playerpos =  player->GetGameObject()->GetTransform()->WorldPosition();
    ///set these to a constant number to view from affar
    float x_offset = playerpos[0];
    float z_offset = playerpos[2];
    weather_buffer[wb_index].slab_spawn_center_x = x_offset;
    weather_buffer[wb_index].slab_spawn_center_z = z_offset;

    std::vector<glm::vec3>& justadded = weather_buffer[wb_index].data;
    const int32_t justadded_size = justadded.size();
    for(uint32_t i = 0; i < justadded_size; i++) {
        justadded[i][0] += x_offset;
        justadded[i][2] += z_offset;
    }

    buildMesh(wb_index);
    wb_index = (wb_index + 1) % numslabs;
    wb_size++;
}
void WeatherManager::clearStackData() {
    //CLEAR SLABS
    weather_buffer.clear();
    slab_pool.clear();
    meshrenderers.clear();
    meshes.clear();
    slab_indices.clear();
    slab_vertices.clear();

    //CLEAR CLOUDS
    cloudmeshrenderers = nullptr;
    cloud_indices.clear();
    cloud_vertices.clear();
    cloudmeshes = Mesh();

    //CLEAR snowcover
    snowcovermeshrenderers = nullptr;
    snowcover_indices.clear();
    snowcover_vertices.clear();
    snowcovermeshes = Mesh();

    wb_size = 0;
    wb_index = 0;
    wb_retired_count = 0;
}

void WeatherManager::ToggleWeahter()
{
    bDisable_weather = !bDisable_weather;
}

void WeatherManager::updateForcast(FORCAST weathertype) {

    if(bDisable_weather)
        forcast = CLEAR;
    else
        forcast = weathertype;

    forcast_start = Engine::Time();
    int32_t forcast_duration = rand() % WEATHER_DURATION_MAX + WEATHER_DURATION_MIN;
    forcast_end = forcast_start + forcast_duration;
    last_tick = forcast_start;


    if(wb_size > numslabs) {
        wb_size = numslabs;
    }

    for(uint32_t i = 0; i < wb_size; i++) {
        ///remove the meshrenderer components from weathercontainer
    }

    clearStackData();

    int32_t intensity_step = rand() % WEATHER_MAX_INTENSITY_STEPS;
    float intensity = intensity_step * WEATHER_INTENSITY_STEP;
    windvector[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    windvector[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    switch(forcast) {
    case CLEAR:
        p_color = glm::vec4(0,0,0,0);
        weathermaterial->SetVector("Color", p_color);
        p_length = 0;
        p_width = 0;
        p_height = 0;
        p_spawntime = 0;
        p_velocity = 0;
        numslabs = 0;
        numslabs = 0;
        WEATHER_PARTICLE_SPAWN_THRESH = 0;
        weather_postprocesser->Instance->WEATHER_MODE = 0;
        break;
    case RAIN:
        buildClouds();
        p_color = RAIN_COLOR;
        weathermaterial->SetVector("Color", p_color);
        p_length = RAIN_LENGTH;
        p_width = RAIN_WIDTH;
        p_height = RAIN_HEIGHT;
        p_spawntime = RAIN_SPAWN_TIME;
        p_velocity = RAIN_VELOCITY;
        numslabs = RAIN_NUM_SLABS;
        WEATHER_PARTICLE_SPAWN_THRESH = RAIN_PARTICLE_SPAWN_THRESH + intensity;
        windvector = windvector * RAIN_WIND_SCALING;
        weather_postprocesser->Instance->WEATHER_MODE = 1;
        break;
    case SNOW:
        buildClouds();
        buildSnowCover();
        p_color = SNOW_COLOR;
        weathermaterial->SetVector("Color", p_color);
        p_length = SNOW_LENGTH;
        p_width = SNOW_WIDTH;
        p_height = SNOW_HEIGHT;
        p_spawntime = SNOW_SPAWN_TIME;
        p_velocity = SNOW_VELOCITY;
        numslabs = SNOW_NUM_SLABS;
        WEATHER_PARTICLE_SPAWN_THRESH = SNOW_PARTICLE_SPAWN_THRESH + intensity;
        windvector = windvector * SNOW_WIND_SCALING;
        weather_postprocesser->Instance->WEATHER_MODE = 2;
        break;
    case SLAYER:
        buildClouds();
        p_color = SLAYER_COLOR;
        weathermaterial->SetVector("Color", p_color);
        p_length = SLAYER_LENGTH;
        p_width = SLAYER_WIDTH;
        p_height = SLAYER_HEIGHT;
        p_spawntime = SLAYER_SPAWN_TIME;
        p_velocity = SLAYER_VELOCITY;
        numslabs = SLAYER_NUM_SLABS;
        WEATHER_PARTICLE_SPAWN_THRESH = SLAYER_PARTICLE_SPAWN_THRESH + intensity;
        windvector = windvector * RAIN_WIND_SCALING;
        weather_postprocesser->Instance->WEATHER_MODE = 1;
        break;
    case PURPLERAIN:
        buildClouds();
        p_color = PURPLERAIN_COLOR;
        weathermaterial->SetVector("Color", p_color);
        p_length = PURPLERAIN_LENGTH;
        p_width = PURPLERAIN_WIDTH;
        p_height = PURPLERAIN_HEIGHT;
        p_spawntime = PURPLERAIN_SPAWN_TIME;
        p_velocity = PURPLERAIN_VELOCITY;
        numslabs = PURPLERAIN_NUM_SLABS;
        WEATHER_PARTICLE_SPAWN_THRESH = PURPLERAIN_PARTICLE_SPAWN_THRESH + intensity;
        windvector = windvector * RAIN_WIND_SCALING;
        weather_postprocesser->Instance->WEATHER_MODE = 1;
        break;
    default:
        break;
    }

    initSlabPool();
    //reserve the max amount the weather buffer will hold at any given time
    weather_buffer.reserve(numslabs);
    weather_buffer.resize(numslabs);
    meshrenderers.reserve(numslabs);
    meshrenderers.resize(numslabs);
    meshes.reserve(numslabs);
    meshes.resize(numslabs);
    slab_indices.reserve(numslabs);
    slab_indices.resize(numslabs);
    slab_vertices.reserve(numslabs);
    slab_vertices.resize(numslabs);
}

void WeatherManager::initSlabPool() {
    slab_pool.reserve(WEATHER_SLAB_VARIETY);
    slab_pool.resize(WEATHER_SLAB_VARIETY);

    for (uint32_t i = 0; i < WEATHER_SLAB_VARIETY; i++) {
        Slab& curr_slab = slab_pool[i];
        for(int32_t x = -SLAB_DIM/2; x < SLAB_DIM/2; x++) {
            for(int32_t z = -SLAB_DIM/2; z < SLAB_DIM/2; z++) {
               float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                if(random < WEATHER_PARTICLE_SPAWN_THRESH) {
                    curr_slab.data.push_back(glm::vec3(x,WEATHER_START_HEIGHT,z));
                }
            }//z
        }//x
    }//i
}

void WeatherManager::updateSnowCover(const float& curr_time) {
    std::vector<Vertex>& interleaved_data = snowcover_vertices;
    Mesh* mesh = &snowcovermeshes;

    const int32_t interleaved_data_size = interleaved_data.size();

    int count = 0;
    const float time_to_ground = WEATHER_Y_OFFSET / SNOW_VELOCITY;
    if(curr_time < forcast_start + time_to_ground + 5 && curr_time > forcast_start + time_to_ground) {
        for(int i = 0; i < interleaved_data_size; i++) {
            interleaved_data[i].color[3] +=0.01;
            count++;
        }
        std::cout << "\ncalled times: " << count;
    } else if (curr_time > forcast_end) {
        for(int i = 0; i < interleaved_data_size; i++) {
            interleaved_data[i].color[3] -=0.04;
        }
    }
    mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
    mesh->Upload();

    snowcovermeshrenderers->SetMesh(mesh);
}

void WeatherManager::buildSnowCover() {
    glm::vec3 playerpos =  player->GetGameObject()->GetTransform()->WorldPosition();
    ///set these to a constant number to view from affar
    int32_t x_offset = floor(playerpos[0] / CHUNK_DIM);
    int32_t z_offset = floor(playerpos[2] / CHUNK_DIM);

    GLuint curr_index = 0;
    std::vector<GLuint>& indices = snowcover_indices;
    std::vector<Vertex>& interleaved_data = snowcover_vertices;
    Mesh* mesh = &snowcovermeshes;

    for(int32_t z = z_offset - CHUNK_VIEWDIST; z < z_offset + CHUNK_VIEWDIST; z++) {
        for(int32_t x = x_offset - CHUNK_VIEWDIST; x < x_offset + CHUNK_VIEWDIST; x++) {
            Chunk* mychunk = chunkManager->GetChunk(x,z);

            if(mychunk == nullptr) {
                continue;
            }

            for(int32_t yin = CHUNK_HALF_HEIGHT - MAX_TERRAIN_HEIGHT - 1; yin < CHUNK_HALF_HEIGHT + MAX_TERRAIN_HEIGHT + 1; yin++) {
                for(int32_t zin = 0; zin < CHUNK_DIM; zin++) {
                    for(int32_t xin = 0; xin < CHUNK_DIM; xin++) {
                        int32_t key = yin*CHUNK_DIM*CHUNK_DIM + zin*CHUNK_DIM + xin;
                        BlockType type = (BlockType)(*mychunk)[key];
                        if (type == GRASS && (BlockType)(*mychunk)[key+CHUNK_DIM*CHUNK_DIM] == AIR) {
                            const glm::vec3 particle(x*CHUNK_DIM+xin,yin+1,z*CHUNK_DIM+zin);
                            addRectanguloid(indices, interleaved_data, curr_index, particle, SNOWCOVER);
                        } else if (type == WATER_FLOW && (BlockType)(*mychunk)[key+CHUNK_DIM*CHUNK_DIM] == AIR) {
                            const glm::vec3 particle(x*CHUNK_DIM+xin,yin+1,z*CHUNK_DIM+zin);
                            addRectanguloid(indices, interleaved_data, curr_index, particle, ICECOVER);
                        }
                    }//xin
                }//zin
            }//yin

        }//x
    }//z

    mesh->SetIndices(indices.data(), indices.size(), false);
    mesh->SetInterleaved(true);
    mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
    mesh->SetWriteOnly(false);
    mesh->Upload();

    MeshRenderer * renderer = snowcovercontainer->AddComponent<MeshRenderer>();
    renderer->SetMesh(mesh);
    renderer->SetMaterial(weathermaterial);
    snowcovermeshrenderers = renderer;
}

void WeatherManager::buildClouds() {
    int32_t sample_height = 8;//rand() % MAX_TERRAIN_HEIGHT + MAX_TERRAIN_HEIGHT/2;
    sample_height += CHUNK_HALF_HEIGHT;

    glm::vec3 playerpos =  player->GetGameObject()->GetTransform()->WorldPosition();
    ///set these to a constant number to view from affar
    int32_t x_offset = floor(playerpos[0] / CHUNK_DIM);
    int32_t z_offset = floor(playerpos[2] / CHUNK_DIM);

    GLuint curr_index = 0;
    std::vector<GLuint>& indices = cloud_indices;
    std::vector<Vertex>& interleaved_data = cloud_vertices;
    Mesh* mesh = &cloudmeshes;

    for(int32_t z = z_offset - CHUNK_VIEWDIST; z < z_offset + CHUNK_VIEWDIST; z++) {
        for(int32_t x = x_offset - CHUNK_VIEWDIST; x < x_offset + CHUNK_VIEWDIST; x++) {
            Chunk* mychunk = chunkManager->GetChunk(x,z);

            if(mychunk == nullptr) {
                continue;
            }

            for(int32_t zin = 0; zin < CHUNK_DIM; zin++) {
                for(int32_t xin = 0; xin < CHUNK_DIM; xin++) {
                    int32_t key = sample_height*CHUNK_DIM*CHUNK_DIM + zin*CHUNK_DIM + xin;
                    BlockType type = (BlockType)(*mychunk)[key];
                    if (type != AIR) {
                        const glm::vec3 particle(x*CHUNK_DIM+xin,WEATHER_START_HEIGHT,z*CHUNK_DIM+zin);
                        addRectanguloid(indices, interleaved_data, curr_index, particle, CLOUDS);
                    }
                }//xin
            }//zin
        }//x
    }//z

    mesh->SetIndices(indices.data(), indices.size(), false);
    mesh->SetInterleaved(true);
    mesh->SetInterleavedData(interleaved_data.data(), interleaved_data.size(), false);
    mesh->SetWriteOnly(false);
    mesh->Upload();

    MeshRenderer * renderer = cloudcontainer->AddComponent<MeshRenderer>();
    renderer->SetMesh(mesh);
    renderer->SetMaterial(weathermaterial);
    cloudmeshrenderers = renderer;
}


void WeatherManager::addRectanguloid(std::vector<GLuint>& indices, std::vector<Vertex>& interleaved_data,
                                     GLuint& curr_index, const glm::vec3 particle,WEATHERBLOCKTYPE type)
{

    float x_offset = 0;
    float y_offset = 0;
    float z_offset = 0;
    glm::vec4 color(0,0,0,0);
    float xref = 0;
    float yref = 0;
    float zref = 0;

    if(type == WEATHER){
        x_offset = p_width/2.f;
        y_offset = p_height/2.f;
        z_offset = p_length/2.f;
        color = p_color;
        xref = particle[0];
        yref = particle[1];
        zref = particle[2];
    } else if (type == CLOUDS) {
        x_offset = CLOUD_WIDTH / 2.f;
        y_offset = CLOUD_HEIGHT / 2.f;
        z_offset = CLOUD_LENGTH / 2.f;
        color = CLOUD_COLOR;
        xref = particle[0] + BLOCK_LENGTH/2.f;
        yref = (float)WEATHER_START_HEIGHT;
        zref = particle[2] + BLOCK_WIDTH/2.f;
    } else if (type == SNOWCOVER) {
        x_offset = SNOWCOVER_WIDTH / 2.f;
        y_offset = SNOWCOVER_HEIGHT / 2.f;
        z_offset = SNOWCOVER_LENGTH / 2.f;
        color = SNOWCOVER_COLOR;
        xref = particle[0] + BLOCK_LENGTH/2.f;
        yref = particle[1] + SNOWCOVER_HEIGHT/2.f;
        zref = particle[2] + BLOCK_WIDTH/2.f;
    } else if (type == ICECOVER) {
        x_offset = SNOWCOVER_WIDTH / 2.f;
        y_offset = SNOWCOVER_HEIGHT / 2.f;
        z_offset = SNOWCOVER_LENGTH / 2.f;
        color = ICECOVER_COLOR;
        xref = particle[0] + BLOCK_LENGTH/2.f;
        yref = particle[1] + SNOWCOVER_HEIGHT/2.f;
        zref = particle[2] + BLOCK_WIDTH/2.f;
    }


    Vertex v1,v2,v3,v4;
    GLuint root;
    glm::vec4 facenormal;

    //we dont need these
    const glm::vec4 tangent(0,0,0,0);
    const glm::vec4 bitangent(0,0,0,0);
    const glm::vec2 uv(0,0);
    const int32_t kind = 0;

    //particle is in the center of the object

    ///TOP FACE
    root = curr_index;
    indices.push_back(root);
    indices.push_back(++curr_index);
    indices.push_back(++curr_index);
    indices.push_back(root);
    indices.push_back(curr_index++);
    indices.push_back(curr_index++);

    facenormal = glm::vec4(0,1,0,0);
    v1 = Vertex(glm::vec4( xref - x_offset, yref + y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v2 = Vertex(glm::vec4( xref + x_offset, yref + y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v3 = Vertex(glm::vec4( xref + x_offset, yref + y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v4 = Vertex(glm::vec4( xref - x_offset, yref + y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    interleaved_data.push_back(v1);
    interleaved_data.push_back(v2);
    interleaved_data.push_back(v3);
    interleaved_data.push_back(v4);

    if(type != ICECOVER) {
    ///FRONT FACE
    root = curr_index;
    indices.push_back(root);
    indices.push_back(++curr_index);
    indices.push_back(++curr_index);
    indices.push_back(root);
    indices.push_back(curr_index++);
    indices.push_back(curr_index++);

    facenormal = glm::vec4(0,0,1,0);
    v1 = Vertex(glm::vec4( xref - x_offset, yref - y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v2 = Vertex(glm::vec4( xref + x_offset, yref - y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v3 = Vertex(glm::vec4( xref + x_offset, yref + y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v4 = Vertex(glm::vec4( xref - x_offset, yref + y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    interleaved_data.push_back(v1);
    interleaved_data.push_back(v2);
    interleaved_data.push_back(v3);
    interleaved_data.push_back(v4);

    ///RIGHT FACE
    root = curr_index;
    indices.push_back(root);
    indices.push_back(++curr_index);
    indices.push_back(++curr_index);
    indices.push_back(root);
    indices.push_back(curr_index++);
    indices.push_back(curr_index++);

    facenormal = glm::vec4(1,0,0,0);
    v1 = Vertex(glm::vec4( xref + x_offset, yref - y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v2 = Vertex(glm::vec4( xref + x_offset, yref - y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v3 = Vertex(glm::vec4( xref + x_offset, yref + y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v4 = Vertex(glm::vec4( xref + x_offset, yref + y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    interleaved_data.push_back(v1);
    interleaved_data.push_back(v2);
    interleaved_data.push_back(v3);
    interleaved_data.push_back(v4);

    ///BOTTOM FACE
//    if (type != SNOWCOVER) {
    root = curr_index;
    indices.push_back(root);
    indices.push_back(++curr_index);
    indices.push_back(++curr_index);
    indices.push_back(root);
    indices.push_back(curr_index++);
    indices.push_back(curr_index++);

    facenormal = glm::vec4(0,-1,0,0);
    v1 = Vertex(glm::vec4( xref - x_offset, yref - y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v2 = Vertex(glm::vec4( xref + x_offset, yref - y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v3 = Vertex(glm::vec4( xref + x_offset, yref - y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v4 = Vertex(glm::vec4( xref - x_offset, yref - y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    interleaved_data.push_back(v1);
    interleaved_data.push_back(v2);
    interleaved_data.push_back(v3);
    interleaved_data.push_back(v4);
//    }

    ///LEFT FACE
    root = curr_index;
    indices.push_back(root);
    indices.push_back(++curr_index);
    indices.push_back(++curr_index);
    indices.push_back(root);
    indices.push_back(curr_index++);
    indices.push_back(curr_index++);

    facenormal = glm::vec4(-1,0,0,0);
    v1 = Vertex(glm::vec4( xref - x_offset, yref - y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v2 = Vertex(glm::vec4( xref - x_offset, yref - y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v3 = Vertex(glm::vec4( xref - x_offset, yref + y_offset, zref + z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v4 = Vertex(glm::vec4( xref - x_offset, yref + y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    interleaved_data.push_back(v1);
    interleaved_data.push_back(v2);
    interleaved_data.push_back(v3);
    interleaved_data.push_back(v4);

    ///BACK FACE
    root = curr_index;
    indices.push_back(root);
    indices.push_back(++curr_index);
    indices.push_back(++curr_index);
    indices.push_back(root);
    indices.push_back(curr_index++);
    indices.push_back(curr_index++);

    facenormal = glm::vec4(0,0,-1,0);
    v1 = Vertex(glm::vec4( xref + x_offset, yref - y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v2 = Vertex(glm::vec4( xref - x_offset, yref - y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v3 = Vertex(glm::vec4( xref - x_offset, yref + y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    v4 = Vertex(glm::vec4( xref + x_offset, yref + y_offset, zref - z_offset  , 1), facenormal, tangent, bitangent, color, uv, kind);
    interleaved_data.push_back(v1);
    interleaved_data.push_back(v2);
    interleaved_data.push_back(v3);
    interleaved_data.push_back(v4);
    }
}

void WeatherManager::printContainerSizes() {
    int32_t meshrenderers_size = meshrenderers.size();
    int32_t meshes_size = meshes.size();
    int32_t slab_indices_size = slab_indices.size();
    int32_t slab_vertices_size = slab_vertices.size();
    int32_t weather_buffer_size = weather_buffer.size();
    int32_t slab_pool_size = slab_pool.size();
    std::cout << "\n\nnumslabs: " << numslabs;
    int32_t total = 0;
    for(int i = 0; i < meshrenderers_size; i++) {
        total += sizeof(*(meshrenderers[i]));
    }
    std::cout << "\nmeshrenders total internal: " << total;

    total = 0;
    for(int i = 0; i < meshes_size; i++) {
//        total += sizeof(*(meshes[i]));
        total += sizeof(meshes[i]);
    }
    std::cout << "\nmeshes total internal: " << total;

    total = 0;
    for(int i = 0; i < slab_indices_size; i++) {
        total += slab_indices[i].size();
    }
    std::cout << "\nslab_indices total internal: " << total;

    total = 0;
    for(int i = 0; i < slab_vertices_size; i++) {
        total += slab_vertices[i].size();
    }
    std::cout << "\nslab_vertices total internal: " << total;

    total = 0;
    for(int i = 0; i < weather_buffer_size; i++) {
        total += sizeof(weather_buffer[i]);
    }
    std::cout << "\nweather_buffer total internal: " << total;

    total = 0;
    for(int i = 0; i < slab_pool_size; i++) {
        total += sizeof(slab_pool[i]);
    }
    std::cout << "\nslab_pool total internal: " << total;
}
