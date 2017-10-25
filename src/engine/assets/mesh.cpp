#include "mesh.h"
#include "../engine.h"


Mesh::Mesh() : Asset(), writeOnly(false), interleaved(false), vertexCount(0), indicesCount(0),
    indices(nullptr), interleavedData(nullptr), vertices(nullptr), normals(nullptr), tangents(nullptr), binormals(nullptr), colors(nullptr), UVs(nullptr), Kinds(nullptr),
    bufIdx(-1), bufInterleaved(-1),  bufPos(-1), bufNor(-1), bufTan(-1), bufBi(-1), bufCol(-1),  bufUV(-1), bufKind(-1),
    idxBound(false), interleavedBound(false), posBound(false), norBound(false), tanBound(false), biBound(false), colBound(false), UVBound(false), KindBound(false),
    context(Engine::GetInstance()->GetContext())
{
}

void Mesh::Interleave()
{
    if(interleaved && vertexCount != 0 && vertices != nullptr)
    {
        interleavedData = new Vertex[vertexCount];

        for(int i = 0; i < vertexCount; i++)
        {
            interleavedData[i].position = vertices[i];
            interleavedData[i].normal = normals != nullptr ? normals[i] : glm::vec4(1.f);

            interleavedData[i].tangent = tangents != nullptr ? tangents[i] : glm::vec4(1.f);
            interleavedData[i].binormal = binormals != nullptr ? binormals[i] : glm::vec4(1.f);

            interleavedData[i].color = colors != nullptr ? colors[i] : glm::vec4(1.f);
            interleavedData[i].uv = UVs != nullptr ? UVs[i] : glm::vec2(1.f);

            interleavedData[i].Kind = Kinds != nullptr ? Kinds[i] : 0;

        }
    }
}

void Mesh::SetInterleaved(bool interleaved)
{
    this->interleaved = interleaved;
}

bool Mesh::IsInterleaved()
{
    return interleaved;
}

void Mesh::SetWriteOnly(bool writeOnly)
{
    this->writeOnly = writeOnly;
}

int Mesh::GetIndicesCount()
{
    return indicesCount;
}

int Mesh::GetVertexCount()
{
    return vertexCount;
}

void Mesh::SetIndices(glm::uint *indices, int triangleCount, bool copy)
{
    this->indicesCount = triangleCount;

    if(copy)
    {
        this->indices = new glm::uint[indicesCount];
        std::memcpy(this->indices, indices, this->indicesCount * sizeof(glm::uint));
    }
    else
    {
        this->indices = indices;
    }
}

void Mesh::SetInterleavedData(Vertex *data, int vertexCount, bool copy)
{
    this->vertexCount = vertexCount;

    if(copy)
    {
        this->interleavedData = new Vertex[vertexCount];
        std::memcpy(this->interleavedData, data, vertexCount * sizeof(Vertex));
    }
    else
    {
        this->interleavedData = data;
    }
}

void Mesh::SetVertices(glm::vec4 *vertices, int vertexCount)
{
    this->vertices = vertices;
    this->vertexCount = vertexCount;
}


void Mesh::SetNormals(glm::vec4 *normals, int vertexCount)
{
    if(this->vertexCount == vertexCount)
        this->normals = normals;
    else
        Engine::LogError("Normals and vertices dont match!");
}

void Mesh::SetTangents(glm::vec4 *tangents, int vertexCount)
{
    if(this->vertexCount == vertexCount)
        this->tangents = tangents;
    else
        Engine::LogError("tangents and vertices dont match!");
}

void Mesh::SetBiNormals(glm::vec4 *binormals, int vertexCount)
{
    if(this->vertexCount == vertexCount)
        this->binormals = binormals;
    else
        Engine::LogError("biNormals and vertices dont match!");
}


void Mesh::SetColors(glm::vec4 *colors, int vertexCount)
{
    if(this->vertexCount == vertexCount)
        this->colors = colors;
    else
        Engine::LogError("Colors and vertices dont match!");
}

void Mesh::SetUVs(glm::vec2 *UVs, int vertexCount)
{
    if(this->vertexCount == vertexCount)
        this->UVs = UVs;
    else
        Engine::LogError("Uvs and vertices dont match!");
}


void Mesh::SetKinds(int *Kinds, int vertexCount)
{
    if(this->vertexCount == vertexCount)
        this->Kinds = Kinds;
    else
        Engine::LogError("Uvs and vertices dont match!");
}

glm::vec4 Mesh::CalculateVertexforScreenQuad(float X, float Y, glm::mat4 InvViewProj)
{
    glm::vec4 vertex = InvViewProj* glm::vec4(X, Y, 0.9f, 1.0f);

    vertex.x = vertex.x / vertex.w;
    vertex.y = vertex.y / vertex.w;
    vertex.z = vertex.z / vertex.w;
    vertex.w = 1.0f;
    return vertex;
}

void Mesh::UpdateForScreenQaud(glm::mat4 invVP, int w, int h)
{
    float left = -1.0f;
    float right = 1.0f;
    float top = 1.0f;
    float bottom = -1.0f;

    // Load the vertex array with data.
    glm::vec4 vertex = CalculateVertexforScreenQuad(left, top, invVP);
    vertices[1] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);  // Top left.

    vertex = CalculateVertexforScreenQuad(right, bottom, invVP);
    vertices[3] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f); // Bottom right.

    vertex = CalculateVertexforScreenQuad(left, bottom, invVP);
    vertices[0] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f); // Bottom left.

    vertex = CalculateVertexforScreenQuad(right, top, invVP);
    vertices[2] = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);  // Top right.

    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(glm::vec4), vertices);
}


void Mesh::Upload()
{
    // Create a VBO on our GPU and store its handle in bufIdx
    if(indices != nullptr)
    {
//        GenerateIndices();
        if(!BindIndices()) {
            GenerateIndices();
            context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
        }

        context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
    }

    if(interleaved)
    {
        if(interleavedData != nullptr)
        {
            if(!BindInterleaved()) {
                GenerateInterleaved();
                context->glBindBuffer(GL_ARRAY_BUFFER, bufInterleaved);
            }
            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), interleavedData, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), interleavedData, GL_DYNAMIC_DRAW);
        }
    }
    else
    {
        // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
        // array buffers rather than element array buffers, as they store vertex attributes like position.
        if(vertices != nullptr)
        {
            GenerateVertices();
            context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);

            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), vertices, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), vertices, GL_DYNAMIC_DRAW);
        }

        if(normals != nullptr)
        {
            GenerateNormals();
            context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), normals, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), normals, GL_DYNAMIC_DRAW);
        }

        if(tangents != nullptr)
        {
            GenerateTangents();
            context->glBindBuffer(GL_ARRAY_BUFFER, bufTan);
            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), tangents, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), tangents, GL_DYNAMIC_DRAW);
        }

        if(binormals != nullptr)
        {
            GenerateBiNormals();
            context->glBindBuffer(GL_ARRAY_BUFFER, bufBi);
            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), binormals, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), binormals, GL_DYNAMIC_DRAW);
        }

        if(colors != nullptr)
        {
            GenerateColors();
            context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), colors, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec4), colors, GL_DYNAMIC_DRAW);
        }

        if(UVs != nullptr)
        {
            GenerateUVs();
            context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec2), UVs, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec2), UVs, GL_DYNAMIC_DRAW);
        }

        if(Kinds != nullptr)
        {
            GenerateKinds();
            context->glBindBuffer(GL_ARRAY_BUFFER, bufKind);
            if(writeOnly)
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(int), Kinds, GL_STATIC_DRAW);
            else
                context->glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(int), Kinds, GL_DYNAMIC_DRAW);
        }




    }

    // If we only want to write/upload, delete all data after it was uploaded
    if(writeOnly)
    {
        if(indices != nullptr)
            delete [] indices;

        if(interleavedData != nullptr)
            delete [] interleavedData;

        if(vertices != nullptr)
            delete [] vertices;


        if(normals != nullptr)
            delete [] normals;

        if(tangents != nullptr)
            delete [] tangents;

        if(binormals != nullptr)
            delete [] binormals;


        if(colors != nullptr)
            delete [] colors;


        if(UVs != nullptr)
            delete [] UVs;

        if(Kinds != nullptr)
            delete [] Kinds;


        indices = nullptr;
        interleavedData = nullptr;
        vertices = nullptr;
        normals = nullptr;
        tangents = nullptr;
        binormals = nullptr;
        colors = nullptr;
        UVs = nullptr;
        Kinds = nullptr;

    }
}

void Mesh::Destroy()
{
    context->glDeleteBuffers(1, &bufIdx);
    context->glDeleteBuffers(1, &bufPos);
    context->glDeleteBuffers(1, &bufNor);
    context->glDeleteBuffers(1, &bufTan);
    context->glDeleteBuffers(1, &bufBi);

    context->glDeleteBuffers(1, &bufCol);
    context->glDeleteBuffers(1, &bufUV);
    context->glDeleteBuffers(1, &bufKind);

    context->glDeleteBuffers(1, &bufInterleaved);
}

void Mesh::Validate()
{
    bool valid = true;

    if(indices != nullptr)
    {
        if(vertices == nullptr && interleavedData == nullptr)
        {
            valid = false;
        }
        else
        {
            for(int i = 0; i < indicesCount; i++)
                if(indices[i] >= (uint) vertexCount)
                    valid = false;
        }
    }

    Engine::LogDebug("VALID? " + std::to_string(valid));
}

void Mesh::GenerateIndices()
{
    idxBound = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    context->glGenBuffers(1, &bufIdx);
}

void Mesh::GenerateInterleaved()
{
    interleavedBound = true;

    // Create a VBO for interleaved data
    context->glGenBuffers(1, &bufInterleaved);
}

void Mesh::GenerateVertices()
{
    posBound = true;
    // Create a VBO on our GPU and store its handle in bufPos
    context->glGenBuffers(1, &bufPos);
}


void Mesh::GenerateNormals()
{
    norBound = true;
    // Create a VBO on our GPU and store its handle in bufNor
    context->glGenBuffers(1, &bufNor);
}

void Mesh::GenerateTangents()
{
    tanBound = true;
    // Create a VBO on our GPU and store its handle in bufNor
    context->glGenBuffers(1, &bufTan);
}


void Mesh::GenerateBiNormals()
{
    biBound = true;
    // Create a VBO on our GPU and store its handle in bufNor
    context->glGenBuffers(1, &bufBi);
}



void Mesh::GenerateColors()
{
    colBound = true;
    // Create a VBO on our GPU and store its handle in bufCol
    context->glGenBuffers(1, &bufCol);
}

void Mesh::GenerateUVs() //Texturing and Texture Animation Jin_Kim
{
   UVBound = true;
   context->glGenBuffers(1, &bufUV);
}

void Mesh::GenerateKinds() //Texturing and Texture Animation Jin_Kim
{
   KindBound = true;
   context->glGenBuffers(1, &bufKind);
}

bool Mesh::BindIndices()
{
    if(idxBound)
        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);

    return idxBound;
}

bool Mesh::BindInterleaved()
{
    if(interleavedBound)
        context->glBindBuffer(GL_ARRAY_BUFFER, bufInterleaved);

    return interleavedBound;
}

bool Mesh::BindVertices()
{
    if(posBound)
        context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);

    return posBound;
}


bool Mesh::BindNormals()
{
    if(norBound)
        context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);

    return norBound;
}

bool Mesh::BindTangents()
{
    if(tanBound)
        context->glBindBuffer(GL_ARRAY_BUFFER, bufTan);

    return tanBound;
}

bool Mesh::BindBiNormals()
{
    if(biBound)
        context->glBindBuffer(GL_ARRAY_BUFFER, bufBi);

    return biBound;
}


bool Mesh::BindColors()
{
    if(colBound)
        context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);

    return colBound;
}

bool Mesh::BindUVs() //Texturing and Texture Animation Jin_Kim
{
   if(UVBound)
       context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);

   return UVBound;
}

bool Mesh::BindKinds() //Texturing and Texture Animation Jin_Kim
{
   if(KindBound)
       context->glBindBuffer(GL_ARRAY_BUFFER, bufKind);

   return KindBound;
}

