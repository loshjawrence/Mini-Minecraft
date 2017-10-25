#include "chunkrivermodifier.h"
#include "random"

void ChunkRiverModifier::InitializeDistanceField(RiverSection *df, int sectionX, int sectionY)
{
    // Our key encoding is a little bit symmetrical, so lets add some prime in there
    uint64_t key = Encode(sectionX, sectionY) % 5298742572553L;

    // TODO: replace base seed with world seed
    int seed = XXHash(173975971).Evaluate(key);
    MersenneRandom random(seed);

    // Initialize the river before evaluating the l system, or we will have recursion problems
    df->initialized = true;

    bool delta = random.GetNormalizedValue() > .3f; // We want more deltas

    int riverBirthHeight = (CHUNK_HEIGHT / 2) + (int) (glm::pow(random.GetNormalizedValue(), 3.f) * 8) - 8;

    if(delta)
    {
        LInstructionChain<RiverData> chain = deltaRiverSystem.ExpandAxiom("RX", 20, random.GetValue());

        RiverData deltaRiverData;
        deltaRiverData.modifier = this;
        deltaRiverData.branchWidthCoefficient = 1.f; // No loss of volume
        deltaRiverData.currentWidth = glm::mix(.15f, 2.f, random.GetNormalizedValue()); // Small rivers
        deltaRiverData.currentAngle = random.GetNormalizedValue() * 360.f;
        deltaRiverData.currentPosition = (glm::vec2(sectionX, sectionY) + random.GetNormalized2D())* (float)RIVER_SECTION_SIZE;
        deltaRiverData.startPosition = deltaRiverData.currentPosition;
        deltaRiverData.currentHeight = riverBirthHeight;

        chain.Evaluate(deltaRiverData, random.GetValue());

//        Engine::LogInfo("Generated delta river... Height: " + std::to_string(deltaRiverData.currentHeight));
//        Engine::LogDebug(chain.ToString());
    }
    else
    {
        LInstructionChain<RiverData> chain = mainRiverSystem.ExpandAxiom("RX", 20, random.GetValue());

        RiverData mainRiverData;
        mainRiverData.modifier = this;
        mainRiverData.branchWidthCoefficient = .4f; // A lot of volume lost on branching
        mainRiverData.currentWidth = glm::mix(5.f, 8.f, random.GetNormalizedValue()); // Big rivers
        mainRiverData.currentAngle = glm::mix(30.f, 60.f, random.GetNormalizedValue()); // Our main river has a very clear direction...
        mainRiverData.currentPosition = (glm::vec2(sectionX, sectionY) + random.GetNormalized2D())* (float)RIVER_SECTION_SIZE;
        mainRiverData.startPosition = mainRiverData.currentPosition;
        mainRiverData.currentHeight = riverBirthHeight;

        chain.Evaluate(mainRiverData, random.GetValue());

//        Engine::LogInfo("Generated main river... Height: " + std::to_string(mainRiverData.currentHeight));
//        Engine::LogDebug(chain.ToString());
    }
}

ChunkRiverModifier::ChunkRiverModifier(ChunkManager *manager) : ChunkModifier(manager), riverMapMutex(QMutex::Recursive)
{
    // A river with no particular main body, consisting of many branches
    deltaRiverSystem.RegisterInstruction<PushInstruction<RiverData>>();
    deltaRiverSystem.RegisterInstruction<PopInstruction<RiverData>>();
    deltaRiverSystem.RegisterInstruction<RiverSectionInstruction>();
    deltaRiverSystem.RegisterInstruction<RiverBranchInstruction>();
    deltaRiverSystem.RegisterInstruction<RiverStartInstruction>();
    deltaRiverSystem.RegisterInstruction<RiverEndInstruction>();

    // Probability may seem small, but note that the exponential growth is inherent to branching...
    deltaRiverSystem.AddRule(new RiverBranchRule(.25f));
    deltaRiverSystem.AddRule(new RiverGrowRule());

    deltaRiverSystem.Initialize();

    // A river with few branches and a main body
    mainRiverSystem.RegisterInstruction<PushInstruction<RiverData>>();
    mainRiverSystem.RegisterInstruction<PopInstruction<RiverData>>();
    mainRiverSystem.RegisterInstruction<RiverSectionInstruction>();
    mainRiverSystem.RegisterInstruction<RiverBranchInstruction>();
    mainRiverSystem.RegisterInstruction<RiverStartInstruction>();
    mainRiverSystem.RegisterInstruction<RiverEndInstruction>();

    mainRiverSystem.AddRule(new RiverBranchRule(.05f));
    mainRiverSystem.AddRule(new RiverGrowRule());

    mainRiverSystem.Initialize();
}

RiverSection *ChunkRiverModifier::GetDistanceFieldForChunk(int chunkX, int chunkY)
{
    chunkX = glm::floor(chunkX / (float) CHUNKS_PER_RIVER_SECTION);
    chunkY = glm::floor(chunkY / (float) CHUNKS_PER_RIVER_SECTION);

    // For each section, we also build a ring around it to make sure stuff is preinitialized
    // TODO: As an idea, maybe this should be done based on player's position
    GetDistanceField(chunkX - 1, chunkY);
    GetDistanceField(chunkX + 1, chunkY);

    GetDistanceField(chunkX - 1, chunkY + 1);
    GetDistanceField(chunkX + 1, chunkY - 1);

    GetDistanceField(chunkX + 1, chunkY + 1);
    GetDistanceField(chunkX - 1, chunkY - 1);

    GetDistanceField(chunkX, chunkY + 1);
    GetDistanceField(chunkX, chunkY - 1);

    return GetDistanceField(chunkX, chunkY);
}

int GetWaterSlope(RiverSection * river, int x, int y, int & distance)
{
    int halfSize = 2;
    int height = -1;

    for(int i = -halfSize; i <= halfSize; i++)
    {
        for(int j = -halfSize; j <= halfSize; j++)
        {
            if(x + i >=0  && x + i < RIVER_SECTION_SIZE && y + j >= 0 && y +j < RIVER_SECTION_SIZE)
            {
                int riverDistance = river->data[(y + j) * RIVER_SECTION_SIZE + (x + i)];

                if(!(i==0 &&j==0) && riverDistance != 0 && riverDistance > height)
                {
                    height = riverDistance;
                    distance = glm::round(glm::length(glm::vec2(i, j)));
                }
            }
        }
    }

    // No water close
    if(height < 0)
        return height;

    return height;
}

void ChunkRiverModifier::Evaluate(Chunk *chunk, std::unordered_map<Chunk*,int8_t>& touched)
{
    riverMapMutex.lock();
    RiverSection * df = GetDistanceFieldForChunk(chunk->position.x, chunk->position.y);
    riverMapMutex.unlock();

    int offsetX = (glm::abs(chunk->position.x) % CHUNKS_PER_RIVER_SECTION) * CHUNK_DIM;
    int offsetY = (glm::abs(chunk->position.y) % CHUNKS_PER_RIVER_SECTION) * CHUNK_DIM;

    if(chunk->position.x < 0 && offsetX != 0)
        offsetX = RIVER_SECTION_SIZE - offsetX;

    if(chunk->position.y < 0 && offsetY != 0)
        offsetY = RIVER_SECTION_SIZE - offsetY;

    // We look for all water blocks and set them directly
    for(int x = 0; x < CHUNK_DIM; x++)
    {
        for(int z = 0; z < CHUNK_DIM; z++)
        {
            int riverX = x + offsetX;
            int riverY = z + offsetY;
            uint8_t riverDistance = df->data[riverY * RIVER_SECTION_SIZE + riverX];

            bool water = riverDistance != 0;

            // There's water, lets override the chunk data
            if(water)
            {
                for(int y = 0; y < CHUNK_HEIGHT; y++)
                {
                    int32_t key = y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + x;

                    if(y > riverDistance)
                        chunk->data[key] = AIR;
                    else if((y == riverDistance || chunk->data[key] == AIR)) // RIVERS MUST BE GENERATED BEFORE CAVES
                        chunk->data[key] = WATER_FLOW;
                }
            }
            else
            {
                int distance = 0;
                int slopeHeight = GetWaterSlope(df, riverX, riverY, distance);

                if(slopeHeight > 0)
                {
                    slopeHeight = slopeHeight + 3 - distance;

                    for(int y = 0; y < CHUNK_HEIGHT; y++)
                    {
                        int32_t key = y*CHUNK_DIM*CHUNK_DIM + z*CHUNK_DIM + x;

                        if(y > slopeHeight)
                            chunk->data[key] = AIR;
                        else if((y == slopeHeight || chunk->data[key] == AIR)) // RIVERS MUST BE GENERATED BEFORE CAVES
                            chunk->data[key] = (y == slopeHeight) ? GRASS : DIRT;
                    }
                }
            }
        }
    }
}

void ChunkRiverModifier::Initialize()
{
    // Preemptively build the rivers for the first section
    GetDistanceField(0,0);
}

void ChunkRiverModifier::SetDistanceFieldValue(int worldX, int worldY, uint8_t value)
{
    int sectionX = glm::floor(worldX / (float) RIVER_SECTION_SIZE);
    int sectionY = glm::floor(worldY / (float) RIVER_SECTION_SIZE);

    RiverSection * df = nullptr;
    uint64_t key = Encode(sectionX, sectionY);
    std::unordered_map<uint64_t, RiverSection*>::iterator result = riverDistanceField.find(key);

    if(result == riverDistanceField.end())
    {
        df = new RiverSection();
        df->position = glm::ivec2(sectionX, sectionY);
        riverDistanceField[key] = df;
    }
    else
    {
        df = result->second;
    }

    int x = (glm::abs(worldX) % RIVER_SECTION_SIZE);
    int y = (glm::abs(worldY) % RIVER_SECTION_SIZE);

    if(worldX < 0 && x != 0)
        x = RIVER_SECTION_SIZE - x;

    if(worldY < 0 && y != 0)
        y = RIVER_SECTION_SIZE - y;

    df->data[y * RIVER_SECTION_SIZE + x] = value;
}

RiverSection *ChunkRiverModifier::GetDistanceField(int x, int y)
{
    uint64_t key = Encode(x,y);
    std::unordered_map<uint64_t, RiverSection*>::iterator result = riverDistanceField.find(key);

    if(result == riverDistanceField.end())
    {
        RiverSection * df = new RiverSection();
        df->position = glm::ivec2(x, y);
        riverDistanceField[key] = df; // It MUST be added to the map before initializing!
        InitializeDistanceField(df, x, y);
        return df;
    }

    if(!result->second->initialized)
        InitializeDistanceField(result->second, x, y);

    return result->second;
}

uint64_t ChunkRiverModifier::Encode(uint32_t x, uint32_t y)
{
    return (((uint64_t)x) << 32) | y;
}

char RiverSectionInstruction::GetSymbol() const
{
    return 'S';
}

LContext<RiverData> RiverSectionInstruction::Evaluate(const LContext<RiverData> &context, std::stack<LContext<RiverData>>& contextStack)
{
    // Dont try to draw very small rivers
    if(context.data.currentWidth < .1f || glm::length(context.data.currentPosition - context.data.startPosition) > RIVER_SECTION_SIZE * 2.5f)
        return context;

    // This is the section that actually erodes the terrain
    LContext<RiverData> result = context;

    float angle = glm::radians(result.data.currentAngle);
    glm::vec2 direction = glm::vec2(glm::cos(angle), glm::sin(angle));

    result.data.currentPosition += direction * (context.random->GetNormalizedValue() * 5.f + 5.f);
    result.data.currentAngle += (context.random->GetNormalizedValue() * 2.f - 1.f) * 30.f;

    result.data.currentWidth += (context.random->GetNormalizedValue() * 2.f - 1.f) * .4f;
    result.data.currentWidth = glm::clamp(result.data.currentWidth, .05f, 8.f);

    glm::vec2 fromPosition = context.data.currentPosition;
    glm::vec2 toPosition = result.data.currentPosition;

    glm::vec2 fromToPosition = toPosition - fromPosition;
    glm::vec2 normal(-direction.y, direction.x);

    float width = result.data.currentWidth;

    int lengthSubdivisions = 24;
    int widthHalfSubdivisions = 12;

    for(int i = 0; i < lengthSubdivisions; i++)
    {
        float t = i / (float)(lengthSubdivisions - 1);

        glm::vec2 p = fromPosition + fromToPosition * t;

        for(int j = -widthHalfSubdivisions; j <= widthHalfSubdivisions; j++)
        {
            float widthPercentage = (j / (float)widthHalfSubdivisions);
            glm::vec2 displacedP = glm::floor(p + normal * (widthPercentage * width));
            int x = displacedP.x;
            int y = displacedP.y;

            context.data.modifier->SetDistanceFieldValue(x, y, result.data.currentHeight);
        }
    }

    return result;
}

std::vector<LInstruction<RiverData> *> RiverBranchRule::InternalPattern(LSystem<RiverData> &context) const
{
    std::vector<LInstruction<RiverData> *> list;

    // Any long (at least 2) section can be branched
    list.push_back(context.GetInstruction<RiverEndInstruction>());

    return list;
}

std::vector<LInstruction<RiverData> *> RiverBranchRule::InternalEvaluate(LSystem<RiverData> &context) const
{
    std::vector<LInstruction<RiverData> *> list;

    // We push
    list.push_back(context.GetInstruction<PushInstruction<RiverData>>());

    // We diverge
    list.push_back(context.GetInstruction<RiverBranchInstruction>());

    // We continue
    list.push_back(context.GetInstruction<RiverSectionInstruction>());
    list.push_back(context.GetInstruction<RiverEndInstruction>());

    // We pop for the branch
    list.push_back(context.GetInstruction<PopInstruction<RiverData>>());

    // We continue as before and grow a bit
    list.push_back(context.GetInstruction<RiverSectionInstruction>());
    list.push_back(context.GetInstruction<RiverEndInstruction>());

    return list;
}

RiverBranchRule::RiverBranchRule(float probability) : LRule<RiverData>(true), probability(probability)
{
}

float RiverBranchRule::GetProbability()
{
    return probability;
}

char RiverBranchInstruction::GetSymbol() const
{
    return 'B';
}

LContext<RiverData> RiverBranchInstruction::Evaluate(const LContext<RiverData> &context, std::stack<LContext<RiverData>>& contextStack)
{
    // A river branch instruction is just an instruction that increments the angle by a huge margin
    // making sure that we diverge from the previous section
    LContext<RiverData> c = context;
    float r = glm::pow(context.random->GetNormalizedValue(), 2);
    float delta = glm::mix(20.f, 50.f, r) * (context.random->GetNormalizedValue() > .5f ? 1 : -1);
    c.data.currentAngle += delta;
    c.data.currentWidth *= c.data.branchWidthCoefficient;
    return c;
}

std::vector<LInstruction<RiverData> *> RiverGrowRule::InternalPattern(LSystem<RiverData> &context) const
{
    std::vector<LInstruction<RiverData> *> list;
    list.push_back(context.GetInstruction<RiverEndInstruction>());
    return list;
}

std::vector<LInstruction<RiverData> *> RiverGrowRule::InternalEvaluate(LSystem<RiverData> &context) const
{
    std::vector<LInstruction<RiverData> *> list;
    list.push_back(context.GetInstruction<RiverSectionInstruction>());
    list.push_back(context.GetInstruction<RiverSectionInstruction>());
    list.push_back(context.GetInstruction<RiverEndInstruction>());
    return list;
}

char RiverEndInstruction::GetSymbol() const
{
    return 'X';
}

LContext<RiverData> RiverEndInstruction::Evaluate(const LContext<RiverData> &context, std::stack<LContext<RiverData> > &contextStack)
{
    // This is the section that actually erodes the terrain
    LContext<RiverData> result = context;

    float angle = glm::radians(result.data.currentAngle);
    glm::vec2 direction = glm::vec2(glm::cos(angle), glm::sin(angle));

    result.data.currentPosition += direction * (context.random->GetNormalizedValue() * 5.f + 5.f);
    result.data.currentAngle += (context.random->GetNormalizedValue() * 2.f - 1.f) * 30.f;

    result.data.currentWidth += (context.random->GetNormalizedValue() * 2.f - 1.f) * .4f;
    result.data.currentWidth = glm::clamp(result.data.currentWidth, .05f, 8.f);

    result.data.currentHeight += glm::round(context.random->GetNormalizedValue() * 4 - 2.0);

    glm::vec2 fromPosition = context.data.currentPosition;
    glm::vec2 toPosition = result.data.currentPosition;

    glm::vec2 fromToPosition = toPosition - fromPosition;
    glm::vec2 normal(-direction.y, direction.x);

    float width = result.data.currentWidth;

    int lengthSubdivisions = 24;
    int widthHalfSubdivisions = 12;

    for(int i = 0; i < lengthSubdivisions; i++)
    {
        // Dont try to draw very small rivers
        if(width < .25f)
            return result;

        float t = i / (float) (lengthSubdivisions - 1);
        glm::vec2 p = fromPosition + fromToPosition * t;

        int h = glm::mix(context.data.currentHeight, result.data.currentHeight, t);

        for(int j = -widthHalfSubdivisions; j <= widthHalfSubdivisions; j++)
        {
            float widthPercentage = (j / (float)widthHalfSubdivisions);
            glm::vec2 displacedP = glm::floor(p + normal * (widthPercentage * width));
            int x = displacedP.x;
            int y = displacedP.y;

            context.data.modifier->SetDistanceFieldValue(x, y, h);
        }

        // Its a river end, it should lose all width eventually
        width *= .8f;
    }

    return context; // We dont really modify it
}

char RiverStartInstruction::GetSymbol() const
{
    return 'R';
}

LContext<RiverData> RiverStartInstruction::Evaluate(const LContext<RiverData> &context, std::stack<LContext<RiverData> > &contextStack)
{
    // Very similar to the RiverEnd instruction -- a lake when a river starts
    glm::vec2 position = context.data.currentPosition;

    int maxArea = 32;
    float radius = glm::mix(4, maxArea / 2, context.random->GetNormalizedValue());

    for(int x = 0; x < maxArea; x++)
    {
        for(int y = 0; y < maxArea; y++)
        {
            glm::vec2 p = glm::vec2(x, y) - (glm::vec2(maxArea, maxArea) * .5f);
            float noise = Perlin::smoothedNoise2D(position.x + x, position.y + y);
            float d = glm::length(p);

            if(d + noise * 6.f < radius)
            {
                glm::vec2 displacedP = glm::floor(position + p);
                context.data.modifier->SetDistanceFieldValue(displacedP.x, displacedP.y, context.data.currentHeight);
            }
        }
    }

    return context; // We dont really modify it
}
