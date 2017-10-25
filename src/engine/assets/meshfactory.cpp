#include "meshfactory.h"

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec4 * cub_vert_pos)
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
    //LR
    cub_vert_pos[idx++] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
    //LL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
    //UL
    cub_vert_pos[idx++] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
}


void createCubeVertexNormals(glm::vec4 * cub_vert_nor)
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,0,1,0);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(1,0,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(-1,0,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,0,-1,0);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,1,0,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec4(0,-1,0,0);
    }
}

void createCubeVertexTBNs(glm::mat4 * cub_vert_TBN)
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_TBN[idx++] = glm::mat4(1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_TBN[idx++] = glm::mat4(1);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_TBN[idx++] = glm::mat4(1);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_TBN[idx++] = glm::mat4(1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_TBN[idx++] = glm::mat4(1, 0, 0, 0,
                                         0, 0, 1, 0,
                                        0, 1, 0, 0,
                                        0, 0, 0, 1);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_TBN[idx++] = glm::mat4(1);
    }
}



void createCubeVertexColors(glm::vec4 * cub_vert_nor)
{
    for(int i = 0; i < CUB_VERT_COUNT; i++)
        cub_vert_nor[i] = glm::vec4(1,1,1,1);
}

void createCubeVertexUVs(glm::vec2 * cub_vert_uv)
{
    //Texturing and Texture Animation Jin_Kim
    cub_vert_uv[0] =  glm::vec2(0, 0);
    cub_vert_uv[1] =  glm::vec2(0, 1);
    cub_vert_uv[2] =  glm::vec2(1, 1);
    cub_vert_uv[3] =  glm::vec2(1, 0);


    //Texturing and Texture Animation Jin_Kim
    cub_vert_uv[4] =  glm::vec2(0, 0);
    cub_vert_uv[5] =  glm::vec2(0, 1);
    cub_vert_uv[6] =  glm::vec2(1, 1);
    cub_vert_uv[7] =  glm::vec2(1, 0);


    //Texturing and Texture Animation Jin_Kim

    cub_vert_uv[8] = glm::vec2(1, 0);
    cub_vert_uv[9] = glm::vec2(0, 0);
    cub_vert_uv[10] =  glm::vec2(0, 1);
    cub_vert_uv[11] =  glm::vec2(1, 1);



    //Texturing and Texture Animation Jin_Kim
    cub_vert_uv[12] =  glm::vec2(1, 0);
    cub_vert_uv[13] =  glm::vec2(1, 1);
    cub_vert_uv[14] =  glm::vec2(0, 1);
    cub_vert_uv[15] =  glm::vec2(0, 0);

    //Texturing and Texture Animation Jin_Kim
    cub_vert_uv[16] =  glm::vec2(0, 0);
    cub_vert_uv[17] =  glm::vec2(0, 1);
    cub_vert_uv[18] =  glm::vec2(1, 1);
    cub_vert_uv[19] =  glm::vec2(1, 0);

    //Texturing and Texture Animation Jin_Kim
    cub_vert_uv[20] =  glm::vec2(0, 0);
    cub_vert_uv[21] =  glm::vec2(0, 1);
    cub_vert_uv[22] =  glm::vec2(1, 1);
    cub_vert_uv[23] =  glm::vec2(1, 0);
}

void createCubeIndices(GLuint * cub_idx)
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}


Mesh *MeshFactory::BuildCube(bool interleaved, bool writeOnly)
{
    GLuint * indices = new GLuint[CUB_IDX_COUNT];
    glm::vec4 * vertices = new glm::vec4[CUB_VERT_COUNT];
    glm::vec4 * normals = new glm::vec4[CUB_VERT_COUNT];
    glm::vec4 * tangents = new glm::vec4[CUB_VERT_COUNT];
    glm::vec4 * binormals = new glm::vec4[CUB_VERT_COUNT];
    glm::vec4 * colors = new glm::vec4[CUB_VERT_COUNT];
    glm::vec2 * UVs = new glm::vec2[CUB_VERT_COUNT];


    createCubeVertexPositions(vertices);
    createCubeVertexNormals(normals);

    createCubeVertexNormals(tangents);
    createCubeVertexNormals(binormals);

    createCubeIndices(indices);
    createCubeVertexColors(colors);
    createCubeVertexUVs(UVs);


    Mesh * output = new Mesh();
    output->SetIndices(indices, CUB_IDX_COUNT);
    output->SetVertices(vertices, CUB_VERT_COUNT);
    output->SetNormals(normals, CUB_VERT_COUNT);
    output->SetTangents(tangents, CUB_VERT_COUNT);
    output->SetBiNormals(binormals, CUB_VERT_COUNT);

    output->SetColors(colors, CUB_VERT_COUNT);
    output->SetUVs(UVs, CUB_VERT_COUNT);

    if(interleaved)
    {
        output->SetInterleaved(true);
        output->Interleave();
    }

    if(writeOnly)
        output->SetWriteOnly(true); // So we don't worry about these arrays being leaked...

    output->Upload();

    return output;
}

Mesh *MeshFactory::BuildWireCube()
{
    int vertexCount = 8;
    int indicesCunt = 24;

    GLuint * indices = new GLuint[indicesCunt];
    glm::vec4 * vertices = new glm::vec4[vertexCount];

    int index = 0;
    vertices[index++] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    vertices[index++] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
    vertices[index++] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
    vertices[index++] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);

    vertices[index++] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
    vertices[index++] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
    vertices[index++] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
    vertices[index++] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);

    index = 0;

    // Upper side
    indices[index++] = 0;
    indices[index++] = 1;

    indices[index++] = 1;
    indices[index++] = 2;

    indices[index++] = 2;
    indices[index++] = 3;

    indices[index++] = 3;
    indices[index++] = 0;

    // Lower side
    indices[index++] = 4;
    indices[index++] = 5;

    indices[index++] = 5;
    indices[index++] = 6;

    indices[index++] = 6;
    indices[index++] = 7;

    indices[index++] = 7;
    indices[index++] = 4;

    // Connections
    indices[index++] = 0;
    indices[index++] = 4;

    indices[index++] = 1;
    indices[index++] = 5;

    indices[index++] = 2;
    indices[index++] = 6;

    indices[index++] = 3;
    indices[index++] = 7;

    Mesh * output = new Mesh();
    output->SetIndices(indices, indicesCunt);
    output->SetVertices(vertices, vertexCount);
    output->SetWriteOnly(true); // So we don't worry about these arrays being leaked...
    output->Upload();

    return output;
}

Mesh *MeshFactory::BuildCross()
{
    GLuint * indices = new GLuint[4];
    glm::vec4 * vertices = new glm::vec4[4];

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    vertices[0] = glm::vec4(-.5f, 0.f, 0.f, 1.f);
    vertices[1] = glm::vec4(.5f, 0.f, 0.f, 1.f);
    vertices[2] = glm::vec4(0.f, -.5f, 0.f, 1.f);
    vertices[3] = glm::vec4(0.f, .5f, 0.f, 1.f);

    Mesh * output = new Mesh();
    output->SetIndices(indices, 4);
    output->SetVertices(vertices, 4);
    output->SetWriteOnly(true); // So we don't worry about these arrays being leaked...
    output->Upload();

    return output;
}

Mesh *MeshFactory::BuildQuad(bool writeOnly)
{
    int vertexCount = 4;
    int indicesCunt = 6;

    GLuint * indices = new GLuint[indicesCunt];
    glm::vec4 * vertices = new glm::vec4[vertexCount];
    glm::vec2 * UVs = new glm::vec2[vertexCount];

    int index = 0;

    // Vertices are aligned to (0,0)
    vertices[index++] = glm::vec4(0, 0, 0, 1.0f);
    vertices[index++] = glm::vec4(1, 0, 0, 1.0f);
    vertices[index++] = glm::vec4(1, 1, 0, 1.0f);
    vertices[index++] = glm::vec4(0, 1, 0, 1.0f);

    index = 0;

    UVs[index++] = glm::vec2(0,0);
    UVs[index++] = glm::vec2(1,0);
    UVs[index++] = glm::vec2(1,1);
    UVs[index++] = glm::vec2(0,1);

    index = 0;

    // Upper side
    indices[index++] = 0;
    indices[index++] = 1;
    indices[index++] = 2;

    indices[index++] = 2;
    indices[index++] = 3;
    indices[index++] = 0;

    Mesh * output = new Mesh();
    output->SetIndices(indices, indicesCunt);
    output->SetVertices(vertices, vertexCount);
    output->SetUVs(UVs, vertexCount);
    output->SetWriteOnly(writeOnly);
    output->Upload();

    return output;
}

Mesh *MeshFactory::BuildMeshFromTexture(ReadableTexture *texture)
{
    std::vector<glm::uint> indices;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec2> uvs;

    int width = texture->GetWidth();
    int height = texture->GetHeight();

    glm::vec2 pixelSize = glm::vec2(1.f / width, 1.f / height);
    uint8_t * pixels = texture->GetPixels();

    glm::vec4 vertexOffset = glm::vec4(.5f, .5f, 0, 0);

    int index = 0;

    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            int i = y * width + x;
            uint8_t alpha = pixels[i * 4 + 3];

            glm::vec2 uvOffset = pixelSize * glm::vec2(x,y);

            if(alpha > 0)
            {
                int sectionIndexOffset = index;

                glm::vec2 uv1 = glm::vec2(0,0) * pixelSize + uvOffset;
                glm::vec2 uv2 = glm::vec2(1,0) * pixelSize + uvOffset;
                glm::vec2 uv3 = glm::vec2(1,1) * pixelSize + uvOffset;
                glm::vec2 uv4 = glm::vec2(0,1) * pixelSize + uvOffset;

                // Upper side of the pixel
                vertices.push_back(glm::vec4(uv1.x, uv1.y, 0, 1.0f) - vertexOffset);
                vertices.push_back(glm::vec4(uv2.x, uv2.y, 0, 1.0f) - vertexOffset);
                vertices.push_back(glm::vec4(uv3.x, uv3.y, 0, 1.0f) - vertexOffset);
                vertices.push_back(glm::vec4(uv4.x, uv4.y, 0, 1.0f) - vertexOffset);

                uvs.push_back(uv1);
                uvs.push_back(uv2);
                uvs.push_back(uv3);
                uvs.push_back(uv4);

                normals.push_back(glm::vec4(0,0,1,0));
                normals.push_back(glm::vec4(0,0,1,0));
                normals.push_back(glm::vec4(0,0,1,0));
                normals.push_back(glm::vec4(0,0,1,0));

                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 2);

                indices.push_back(index + 2);
                indices.push_back(index + 3);
                indices.push_back(index);

                index += 4;

                // Lower side of the pixel
                vertices.push_back(glm::vec4(uv1.x, uv1.y, -1, 1.0f) - vertexOffset);
                vertices.push_back(glm::vec4(uv2.x, uv2.y, -1, 1.0f) - vertexOffset);
                vertices.push_back(glm::vec4(uv3.x, uv3.y, -1, 1.0f) - vertexOffset);
                vertices.push_back(glm::vec4(uv4.x, uv4.y, -1, 1.0f) - vertexOffset);

                uvs.push_back(uv1);
                uvs.push_back(uv2);
                uvs.push_back(uv3);
                uvs.push_back(uv4);

                normals.push_back(glm::vec4(0,0,-1,0));
                normals.push_back(glm::vec4(0,0,-1,0));
                normals.push_back(glm::vec4(0,0,-1,0));
                normals.push_back(glm::vec4(0,0,-1,0));


                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 2);

                indices.push_back(index + 2);
                indices.push_back(index + 3);
                indices.push_back(index);

                index += 4;

                // Now we need to check adjacent pixels
                // Note: readibility can be improved by having a set of
                // offset indices for each extrusion side (this is basically an extrusion)
                int adjX = x + 1;
                uint8_t adjAlpha = alpha;

                // Right side pixel comparison
                if(adjX < width)
                {
                    i = y * width + adjX;
                    adjAlpha = pixels[i * 4 + 3];

                    if(adjAlpha != alpha)
                    {
                        int v1 = sectionIndexOffset + 5;
                        int v2 = sectionIndexOffset + 6;
                        int v3 = sectionIndexOffset + 2;
                        int v4 = sectionIndexOffset + 1;

                        vertices.push_back(vertices[v1]);
                        vertices.push_back(vertices[v2]);
                        vertices.push_back(vertices[v3]);
                        vertices.push_back(vertices[v4]);

                        normals.push_back(glm::vec4(1,0,0,0));
                        normals.push_back(glm::vec4(1,0,0,0));
                        normals.push_back(glm::vec4(1,0,0,0));
                        normals.push_back(glm::vec4(1,0,0,0));

                        uvs.push_back(uvs[sectionIndexOffset]);
                        uvs.push_back(uvs[sectionIndexOffset + 1]);
                        uvs.push_back(uvs[sectionIndexOffset + 2]);
                        uvs.push_back(uvs[sectionIndexOffset + 3]);

                        indices.push_back(index);
                        indices.push_back(index + 1);
                        indices.push_back(index + 2);

                        indices.push_back(index + 2);
                        indices.push_back(index + 3);
                        indices.push_back(index);

                        index += 4;
                    }
                }

                adjX = x - 1;

                // Left side pixel comparison
                if(adjX >= 0)
                {
                    i = y * width + adjX;
                    adjAlpha = pixels[i * 4 + 3];

                    if(adjAlpha != alpha)
                    {
                        int v1 = sectionIndexOffset + 4;
                        int v2 = sectionIndexOffset + 7;
                        int v3 = sectionIndexOffset + 3;
                        int v4 = sectionIndexOffset + 0;

                        vertices.push_back(vertices[v1]);
                        vertices.push_back(vertices[v2]);
                        vertices.push_back(vertices[v3]);
                        vertices.push_back(vertices[v4]);

                        normals.push_back(glm::vec4(-1,0,0,0));
                        normals.push_back(glm::vec4(-1,0,0,0));
                        normals.push_back(glm::vec4(-1,0,0,0));
                        normals.push_back(glm::vec4(-1,0,0,0));

                        uvs.push_back(uvs[sectionIndexOffset]);
                        uvs.push_back(uvs[sectionIndexOffset + 1]);
                        uvs.push_back(uvs[sectionIndexOffset + 2]);
                        uvs.push_back(uvs[sectionIndexOffset + 3]);

                        indices.push_back(index);
                        indices.push_back(index + 1);
                        indices.push_back(index + 2);

                        indices.push_back(index + 2);
                        indices.push_back(index + 3);
                        indices.push_back(index);

                        index += 4;
                    }
                }

                int adjY = y - 1;

                // Lower side pixel comparison
                if(adjY >= 0)
                {
                    i = adjY * width + x;
                    adjAlpha = pixels[i * 4 + 3];

                    if(adjAlpha != alpha)
                    {
                        int v1 = sectionIndexOffset + 4;
                        int v2 = sectionIndexOffset + 5;
                        int v3 = sectionIndexOffset + 1;
                        int v4 = sectionIndexOffset + 0;

                        vertices.push_back(vertices[v1]);
                        vertices.push_back(vertices[v2]);
                        vertices.push_back(vertices[v3]);
                        vertices.push_back(vertices[v4]);

                        normals.push_back(glm::vec4(0,-1,0,0));
                        normals.push_back(glm::vec4(0,-1,0,0));
                        normals.push_back(glm::vec4(0,-1,0,0));
                        normals.push_back(glm::vec4(0,-1,0,0));

                        uvs.push_back(uvs[sectionIndexOffset]);
                        uvs.push_back(uvs[sectionIndexOffset + 1]);
                        uvs.push_back(uvs[sectionIndexOffset + 2]);
                        uvs.push_back(uvs[sectionIndexOffset + 3]);

                        indices.push_back(index);
                        indices.push_back(index + 1);
                        indices.push_back(index + 2);

                        indices.push_back(index + 2);
                        indices.push_back(index + 3);
                        indices.push_back(index);

                        index += 4;
                    }
                }

                adjY = y + 1;

                // Upper side pixel comparison
                if(adjY < height)
                {
                    i = adjY * width + x;
                    adjAlpha = pixels[i * 4 + 3];

                    if(adjAlpha != alpha)
                    {
                        int v1 = sectionIndexOffset + 7;
                        int v2 = sectionIndexOffset + 6;
                        int v3 = sectionIndexOffset + 2;
                        int v4 = sectionIndexOffset + 3;

                        vertices.push_back(vertices[v1]);
                        vertices.push_back(vertices[v2]);
                        vertices.push_back(vertices[v3]);
                        vertices.push_back(vertices[v4]);

                        normals.push_back(glm::vec4(0,1,0,0));
                        normals.push_back(glm::vec4(0,1,0,0));
                        normals.push_back(glm::vec4(0,1,0,0));
                        normals.push_back(glm::vec4(0,1,0,0));

                        uvs.push_back(uvs[sectionIndexOffset]);
                        uvs.push_back(uvs[sectionIndexOffset + 1]);
                        uvs.push_back(uvs[sectionIndexOffset + 2]);
                        uvs.push_back(uvs[sectionIndexOffset + 3]);

                        indices.push_back(index);
                        indices.push_back(index + 1);
                        indices.push_back(index + 2);

                        indices.push_back(index + 2);
                        indices.push_back(index + 3);
                        indices.push_back(index);

                        index += 4;
                    }
                }
            }
        }
    }

    int indicesCount = indices.size();
    int vertexCount = vertices.size();

    Mesh * output = new Mesh();
    output->SetIndices(indices.data(), indicesCount);
    output->SetVertices(vertices.data(), vertexCount);
    output->SetNormals(normals.data(), vertexCount);
    output->SetUVs(uvs.data(), vertexCount);

    // We dont need to delete the data, as the vectors will on their destructors
    output->Upload();

    return output;
}
