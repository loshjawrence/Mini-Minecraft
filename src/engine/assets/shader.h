#ifndef SHADER_H
#define SHADER_H

#include <openglcontext.h>
#include <vector>
#include "../../la.h"
#include "asset.h"
#include "mesh.h"

class Shader: public Asset
{
public:
    Shader();
    ~Shader();

private:
    OpenGLContext * context;

    std::string shaderName;
    std::string vertexFilename;
    std::string fragmentFilename;

    GLuint vertShader;
    GLuint fragShader;
    GLuint prog;

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrTan;
    int attrBi;
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader
    int attrUV; //Texturing and Texture Animation Jin_Kim
    int attrKind;

    bool loaded;

public:

    // Sets up the requisite GL data and shaders from the given .glsl files
    void Upload();

    // Tells our OpenGL context to use this shader to draw things
    void Bind();

    // Draw the given object to our screen using this ShaderProgram's shaders
    void Render(Mesh * mesh, GLenum drawMode);

    // Utility function used in create()
    char* textFileRead(const char*);

    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);

    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);

    int GetUniformLocation(const char *uniform);

    void SetIntUniform(int uniform, int value);
    void SetFloatUniform(int uniform, float value);
    void SetMatrixUniform(int uniform, const glm::mat4& matrix);
    void SetMatrixArrayUniform(int uniform, glm::mat4* matrix, int size);
    void SetVectorUniform(int uniform, const glm::vec4& v);
    void SetIVectorUniform(int uniform, const glm::ivec4 &v);
    void SetTextureUniform(int uniform, GLuint value, uint counter);
    void SetCubeTextureUniform(int uniform, GLuint value, uint &counter);
    QString qTextFileRead(const char*);

    std::string GetName();

    void LoadFromFilename(std::string filename);
};

#endif // SHADER_H
