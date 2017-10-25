#include "uitext.h"
#include "../assets/assetdatabase.h"
#include "../assets/texture.h"

Mesh *UIText::RebuildMesh()
{
    Mesh * output = new Mesh();

    int letterCount = this->text.size();

    int vertexCount = letterCount * 4;
    int indicesCount = letterCount * 2 * 3;

    int lettersPerScanline = 16;
    int lettersPerHeight = 16;

    glm::vec2 letterSize = glm::vec2(1 / (float)lettersPerScanline, 1 / (float)lettersPerHeight);

    glm::vec4 * vertices = new glm::vec4[vertexCount];
    glm::vec2 * uvs = new glm::vec2[vertexCount];
    glm::uint * indices = new glm::uint[indicesCount];

    int index = 0;
    int linebreaks = 0;
    int currentLetterIndex = 0;

    for(int letterIndex = 0; letterIndex < letterCount; letterIndex++)
    {
        char letter = text[letterIndex];

        if(letter == '\n')
        {
            currentLetterIndex = 0;
            linebreaks++;
            continue;
        }

        int x = letter % lettersPerScanline;
        int y = lettersPerHeight - (letter / lettersPerScanline) - 1;

        glm::vec2 uvOffset = glm::vec2(x,y) * letterSize;

        glm::vec2 uv1 = glm::vec2(0,0) * letterSize;
        glm::vec2 uv2 = glm::vec2(1,0) * letterSize;
        glm::vec2 uv3 = glm::vec2(1,1) * letterSize;
        glm::vec2 uv4 = glm::vec2(0,1) * letterSize;

        glm::vec4 vertexOffset = glm::vec4(letterSize.x * currentLetterIndex * .65f,  letterSize.y * linebreaks * -1.1f, 0.f, 0.f);

        // Upper side of the pixel
        vertices[index + 0] = glm::vec4(uv1.x, uv1.y, 0, 1.0f) + vertexOffset;
        vertices[index + 1] = glm::vec4(uv2.x, uv2.y, 0, 1.0f) + vertexOffset;
        vertices[index + 2] = glm::vec4(uv3.x, uv3.y, 0, 1.0f) + vertexOffset;
        vertices[index + 3] = glm::vec4(uv4.x, uv4.y, 0, 1.0f) + vertexOffset;

        uvs[index + 0] = uv1 + uvOffset;
        uvs[index + 1] = uv2 + uvOffset;
        uvs[index + 2] = uv3 + uvOffset;
        uvs[index + 3] = uv4 + uvOffset;

        int indicesOffset = letterIndex * 2 * 3;

        indices[indicesOffset++] = index + 0;
        indices[indicesOffset++] = index + 1;
        indices[indicesOffset++] = index + 2;

        indices[indicesOffset++] = index + 2;
        indices[indicesOffset++] = index + 3;
        indices[indicesOffset++] = index + 0;

        index += 4;
        currentLetterIndex++;
    }

    output->SetVertices(vertices, vertexCount);
    output->SetUVs(uvs, vertexCount);
    output->SetIndices(indices, indicesCount);
    output->SetWriteOnly(true);
    output->Upload();

    return output;
}

void UIText::Rebuild()
{
    Mesh * mesh = renderer->GetMesh();

    if(mesh != nullptr)
        delete mesh;

    mesh = RebuildMesh();
    this->renderer->SetMesh(mesh);
}

void UIText::Awake()
{
    this->color = glm::vec4(1,1,1,1);
    this->texture = AssetDatabase::GetInstance()->LoadAsset<Texture>("/cis460-minecraft/textures/ui/font.tga");
    this->renderer = gameObject->AddComponent<UIRenderer>();
    this->material = UIComponent::GetUIMaterialInstance();

    this->material->SetTexture("MainTexture", this->texture);
    this->material->SetColor("Color", color);

    this->renderer->SetMaterial(material);
    this->renderer->SetMesh(nullptr);

    this->text = std::string();
}

void UIText::SetTextSize(int size)
{
    this->GetTransform()->UISetSize(glm::vec2(size * 16));
}

void UIText::SetText(std::string text)
{
    if(text != this->text)
    {
        this->text = text;
        Rebuild();
    }
}

void UIText::SetColor(const glm::vec4 &color)
{
    this->color = color;
    this->material->SetColor("Color", color);
}