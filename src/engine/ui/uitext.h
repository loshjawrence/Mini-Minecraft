#ifndef UITEXT_H
#define UITEXT_H

#include "uicomponent.h"
#include "../material.h"
#include "../assets/mesh.h"
#include "../components/renderer.h"
#include <string>

class UIText : public UIComponent
{
protected:
    UIRenderer * renderer;
    Material * material;
    Texture * texture;
    glm::vec4 color;
    std::string text;

    // Meshes are going to be discarded/regenerated all the time
    Mesh * RebuildMesh();
    void Rebuild();

public:
    void Awake();

    void SetTextSize(int size);
    void SetText(std::string text);
    void SetColor(const glm::vec4& color);
};

#endif // UITEXT_H
