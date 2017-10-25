#ifndef MESH_H
#define MESH_H

#include <openglcontext.h>
#include <vector>
#include "../../la.h"
#include "asset.h"

struct Vertex
{
public:
    glm::vec4 position;
    glm::vec4 normal;

    glm::vec4 tangent;
    glm::vec4 binormal;

    glm::vec4 color;
    glm::vec2 uv; //Texturing and Texture Animation Jin_Kim

    int Kind;

    Vertex(const glm::vec4& position, const glm::vec4& normal, const glm::vec4& tangent, const glm::vec4& binormal, const glm::vec4& color, const glm::vec2& uv, const int& Kind)
    {
        this->position = position;
        this->normal = normal;
        this->tangent = tangent;
        this->binormal = binormal;
        this->color = color;
        this->uv = uv;
        this->Kind = Kind;
    }

    Vertex() : Vertex(glm::vec4(0.0), glm::vec4(0.0), glm::vec4(0.0), glm::vec4(0.0), glm::vec4(0.0), glm::vec2(0.0), -1)
    {}
};

class Mesh final: public Asset
{
private:

    // If the mesh is writeOnly, after setting data it's going to be deleted from CPU
    bool writeOnly;
    bool interleaved;

    int vertexCount;
    int indicesCount;

    glm::uint * indices;
    glm::vec4 * vertices;
    glm::vec4 * normals;   
    glm::vec4 * tangents;
    glm::vec4 * binormals;
    glm::vec4 * colors;
    glm::vec2 * UVs; //Texturing and Texture Animation Jin_Kim
    int* Kinds;

    Vertex * interleavedData;

    // GPU related
    GLuint bufIdx;
    GLuint bufInterleaved;
    GLuint bufPos;
    GLuint bufNor;
    GLuint bufTan;
    GLuint bufBi;
    GLuint bufCol;

    GLuint bufUV; //Texturing and Texture Animation Jin_Kim
    GLuint bufKind;

    bool idxBound;
    bool interleavedBound; // For interleaved data
    bool posBound;
    bool norBound;

    bool tanBound;
    bool biBound;

    bool colBound;
    bool UVBound; //Texturing and Texture Animation Jin_Kim
    bool KindBound;

    OpenGLContext * context;

    void GenerateIndices();
    void GenerateInterleaved();
    void GenerateVertices();
    void GenerateNormals();

    void GenerateTangents();
    void GenerateBiNormals();

    void GenerateColors();

    void GenerateUVs(); //Texturing and Texture Animation Jin_Kim
    void GenerateKinds();

public:
    Mesh();

    void Interleave();
    void SetInterleaved(bool interleaved);
    bool IsInterleaved();

    void SetWriteOnly(bool writeOnly);

    int GetIndicesCount();
    int GetVertexCount();

    void SetIndices(glm::uint * indices, int indicesCount, bool copy = false);
    void SetInterleavedData(Vertex * data, int vertexCount, bool copy = false);
    void SetVertices(glm::vec4 * vertices, int vertexCount);
    void SetNormals(glm::vec4 * normals, int vertexCount);
    void SetTangents(glm::vec4 * tangents, int vertexCount);
    void SetBiNormals(glm::vec4 * binormals, int vertexCount);

    void SetColors(glm::vec4 * colors, int vertexCount);
    void SetUVs(glm::vec2 * UVs, int vertexCount);  //Texturing and Texture Animation Jin_Kim
    void SetKinds(int *Kinds, int vertexCount);


    void Upload();

    void UpdateForScreenQaud();
    glm::vec4 CalculateVertexforScreenQuad(float X, float Y, glm::mat4 InvViewProj);
    void UpdateForScreenQaud(glm::mat4 invVP, int w, int h);

    void Destroy();
    void Validate();

    bool BindIndices();
    bool BindInterleaved();
    bool BindVertices();
    bool BindNormals();
    bool BindTangents();
    bool BindBiNormals();
    bool BindColors();

    bool BindUVs();  //Texturing and Texture Animation Jin_Kim
    bool BindKinds();
};

#endif // MESH_H
