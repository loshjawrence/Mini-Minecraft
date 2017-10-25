#ifndef RAINMANAGER_H
#define RAINMANAGER_H

#include "common.h"
#include "chunkmanager.h"
#include "engine/components/weathersystem.h"
#include "engine/components/directionallight.h"

///WIND TUNERS
const int32_t WIND_ENABLED = 0;
const float RAIN_WIND_SCALING = 50*WIND_ENABLED;
const float SNOW_WIND_SCALING = 10*WIND_ENABLED;

///WEATEHR AND SLAB TUNERS
const int32_t WEATHER_DURATION_MIN = 12;
const int32_t WEATHER_DURATION_MAX = 12;
const int32_t WEATHER_VIEWDIST = CHUNK_VIEWDIST+(CHUNK_VIEWDIST*WIND_ENABLED);
const int32_t WEATHER_Y_OFFSET = CHUNK_HALF_HEIGHT/2;
const int32_t WEATHER_START_HEIGHT = CHUNK_HALF_HEIGHT + WEATHER_Y_OFFSET;
const int32_t WEATHER_VERT_DIST = WEATHER_Y_OFFSET + MAX_TERRAIN_HEIGHT;
const int32_t WEATHER_SLAB_VARIETY = 32;
const int32_t SLAB_DIM = CHUNK_DIM * WEATHER_VIEWDIST * 2;
const int32_t WEATHER_MAX_INTENSITY_STEPS = 2;
const float WEATHER_INTENSITY_STEP = 0.003;

///RAIN TUNERS
const float RAIN_PARTICLE_SPAWN_THRESH= 0.01;//probability
const float RAIN_LENGTH = 0.05;//0.1;
const float RAIN_WIDTH = RAIN_LENGTH;
const float RAIN_HEIGHT = 0.3;
const float RAIN_SPAWN_TIME = 0.05;//sec
const float RAIN_VELOCITY = 40; //units per sec
const glm::vec4 RAIN_COLOR = glm::vec4(0,1,1,1);
const float RAIN_FLIGHT_TIME = WEATHER_VERT_DIST / RAIN_VELOCITY;//sec
const int32_t RAIN_NUM_SLABS = RAIN_FLIGHT_TIME / RAIN_SPAWN_TIME;

///SLAYER TUNERS
const float SLAYER_PARTICLE_SPAWN_THRESH= 0.01;//probability
const float SLAYER_LENGTH = 0.05;//0.1;
const float SLAYER_WIDTH = SLAYER_LENGTH;
const float SLAYER_HEIGHT = 0.3;
const float SLAYER_SPAWN_TIME = 0.05;//sec
const float SLAYER_VELOCITY = 40; //units per sec
const glm::vec4 SLAYER_COLOR = glm::vec4(1,0,0,1);
const float SLAYER_FLIGHT_TIME = WEATHER_VERT_DIST / SLAYER_VELOCITY;//sec
const int32_t SLAYER_NUM_SLABS = SLAYER_FLIGHT_TIME / SLAYER_SPAWN_TIME;

///PURPLERAIN TUNERS
const float PURPLERAIN_PARTICLE_SPAWN_THRESH= 0.01;//probability
const float PURPLERAIN_LENGTH = 0.05;//0.1;
const float PURPLERAIN_WIDTH = PURPLERAIN_LENGTH;
const float PURPLERAIN_HEIGHT = 0.3;
const float PURPLERAIN_SPAWN_TIME = 0.05;//sec
const float PURPLERAIN_VELOCITY = 40; //units per sec
const glm::vec4 PURPLERAIN_COLOR = glm::vec4(1,0,1,1);
const float PURPLERAIN_FLIGHT_TIME = WEATHER_VERT_DIST / PURPLERAIN_VELOCITY;//sec
const int32_t PURPLERAIN_NUM_SLABS = PURPLERAIN_FLIGHT_TIME / PURPLERAIN_SPAWN_TIME;

///SNOW TUNERS
const float SNOW_PARTICLE_SPAWN_THRESH= 0.008;//probability
const float SNOW_LENGTH = 0.1;//0.1;
const float SNOW_WIDTH = SNOW_LENGTH;
const float SNOW_HEIGHT = 0.1;
const float SNOW_SPAWN_TIME = 0.1;//sec
const float SNOW_VELOCITY = 20; //units per sec
const glm::vec4 SNOW_COLOR = glm::vec4(1,1,1,1);
const float SNOW_FLIGHT_TIME = WEATHER_VERT_DIST / SNOW_VELOCITY;//sec
const int32_t SNOW_NUM_SLABS = SNOW_FLIGHT_TIME / SNOW_SPAWN_TIME;

///CLOUD TUNERS
const float BLOCK_LENGTH = 1;
const float BLOCK_WIDTH = 1;
const float BLOCK_HEIGHT = 1;
const float CLOUD_LENGTH = 1;
const float CLOUD_WIDTH = 1;
const float CLOUD_HEIGHT = 1;
const glm::vec4 CLOUD_COLOR = glm::vec4(0.5,0.5,0.5,1);

///SNOWCOVER TUNERS
const float SNOWCOVER_LENGTH = 1;
const float SNOWCOVER_WIDTH = 1;
const float SNOWCOVER_HEIGHT = 0.1;
const glm::vec4 SNOWCOVER_COLOR = glm::vec4(0.8,0.8,0.8,0);
const glm::vec4 ICECOVER_COLOR = glm::vec4(0.7,1,1,0);

///ENUMS
enum FORCAST{CLEAR, RAIN, SNOW, SLAYER,PURPLERAIN};
const int32_t FORCAST_MAX_INDEX = 4;
enum WEATHERBLOCKTYPE {WEATHER,CLOUDS,SNOWCOVER,ICECOVER};

///STRUCTS
typedef struct Slab {
    //contains weather particles positions for this slab
    __forceinline glm::vec3& operator[](const int32_t& i) { return data[i]; }
   std::vector<glm::vec3> data;
   float slab_spawn_center_x;
   float slab_spawn_center_z;
} Slab;

class WeatherManager : public Component {
protected:
    ChunkManager* chunkManager;
    Player* player;
    WeatherSystem* weather_postprocesser;
    DirectionalLight* sun;

    //WEATHER BUFFER MESH INFO
    GameObject * weathercontainer;//register MeshRenderers with the GameObject
    GameObject * cloudcontainer;//register MeshRenderers with the GameObject
    GameObject * snowcovercontainer;//register MeshRenderers with the GameObject

    Material * weathermaterial;
    std::vector<MeshRenderer*> meshrenderers;
    std::vector<Mesh> meshes;
    std::vector< std::vector<GLuint> > slab_indices;
    std::vector< std::vector<Vertex> > slab_vertices;

    //CLOUDS
    MeshRenderer* cloudmeshrenderers;
    Mesh cloudmeshes;
    std::vector< GLuint > cloud_indices;
    std::vector< Vertex > cloud_vertices;

    //SNOW COVER
    MeshRenderer* snowcovermeshrenderers;
    Mesh snowcovermeshes;
    std::vector<GLuint> snowcover_indices;
    std::vector<Vertex> snowcover_vertices;

    //SLAB DATA
    std::vector<Slab> weather_buffer;
    std::vector<Slab> slab_pool;
    uint32_t wb_index; //current index into the weather_buffer (cyclical)
    uint32_t wb_size; //number of pushes to the weather buffer (caps at numslabs)
    uint32_t wb_retired_count; //number of retired slabs

    //WEATHER DETAILS
    glm::vec4 p_color;
    float p_length;
    float p_width;
    float p_height;
    float p_spawntime;
    float p_velocity;
    int32_t numslabs;
    glm::vec2 windvector;
    float WEATHER_PARTICLE_SPAWN_THRESH;

    //TIMERS
    FORCAST forcast;
    float forcast_start;
    float forcast_end;
    float last_tick;
    float last_spawn_tick;
    float elapsed_time;
    float elapsed_spawn_time;
    bool cloudsbuilt;
    bool snowbuilt;

    bool bDisable_weather;

public:
    void init(ChunkManager*,Player*,WeatherSystem*,DirectionalLight*);
    void initSlabPool();
    void updateForcast(const FORCAST);//change weather
    virtual void Update();
    void clearStackData();
    void printContainerSizes();

    void retireSlab();
    void buildSlab();
    void recycleSlab();
    void buildMesh(const int32_t&);
    void addRectanguloid(std::vector<GLuint>& indices, std::vector<Vertex>& interleaved_data,
                                     GLuint& curr_index, const glm::vec3 particle, WEATHERBLOCKTYPE type);
    void updateSlabs();
    void updateMeshes();

    void buildClouds();
    void updateClouds();

    void buildSnowCover();
    void updateSnowCover(const float&);

    void ToggleWeahter();
};

#endif // RAINMANAGER_H
