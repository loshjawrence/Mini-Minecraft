#include "engine.h"
#include "material.h"
#include <iostream>
#include "assets/assetdatabase.h"

const std::string MODEL_UNIFORM = "Model";
const std::string MODEL_INVERSE_UNIFORM = "ModelInv";
const std::string MODEL_INV_TR_UNIFORM = "ModelInvTr";
const std::string VIEW_PROJ_UNIFORM = "ViewProj";
const std::string MODEL_VIEW_PROJECTION_UNIFORM = "ModelViewProj";

void Material::PrepareCoreUniforms()
{
    SetUniformValue(MODEL_UNIFORM, matrixUniforms, glm::mat4(), true, 1);
    SetUniformValue(MODEL_INVERSE_UNIFORM, matrixUniforms, glm::mat4(), true, 1);
    SetUniformValue(MODEL_INV_TR_UNIFORM, matrixUniforms, glm::mat4(), true, 1);

    SetUniformValue(MODEL_VIEW_PROJECTION_UNIFORM, matrixUniforms, glm::mat4(), true, 1);
    SetUniformValue(VIEW_PROJ_UNIFORM, matrixUniforms, glm::mat4(), true, 1);
}

const std::string Material::GetUniformName(std::string baseName)
{
    return (std::string("u_") + baseName);
}

Material::Material(Shader *shader) : blendOperation(BlendOperation(GL_ONE, GL_ZERO)), overrideDrawingMode(-1), queue(Material::Geometry), shader(nullptr), featureMap()
{
    // Default values
    this->featureMap[GL_DEPTH_TEST] = true;
    this->featureMap[GL_BLEND] = false;
    this->featureMap[GL_CULL_FACE] = false;

    // Should check if it exists..
    this->shader = shader;
    this->shader->Upload();

    PrepareCoreUniforms();
}

Material::Material(std::string shader) : Material(AssetDatabase::GetInstance()->LoadAsset<Shader>(shader))
{
}

Shader *Material::GetShader()
{
    return shader;
}

void Material::Render(Mesh * mesh, const glm::mat4& viewProj, const glm::mat4 &localToWorld, const glm::mat4 &worldToLocal, const glm::mat4 &invTranspose)
{
    // Important stuff manually, to prevent copying matrices all the time
    shader->SetMatrixUniform(matrixUniforms[MODEL_UNIFORM].id, localToWorld);
    shader->SetMatrixUniform(matrixUniforms[MODEL_INVERSE_UNIFORM].id, worldToLocal);
    shader->SetMatrixUniform(matrixUniforms[MODEL_INV_TR_UNIFORM].id, invTranspose);
    shader->SetMatrixUniform(matrixUniforms[MODEL_VIEW_PROJECTION_UNIFORM].id, viewProj * localToWorld);
    shader->SetMatrixUniform(matrixUniforms[VIEW_PROJ_UNIFORM].id, viewProj);

    // Send int uniforms!
    for(IntUniformIterator v = intUniforms.begin(); v != intUniforms.end(); v++)
        if(!(*v).second.perObjectUniform)
            shader->SetIntUniform((*v).second.id, (*v).second.value);

    // Send vector uniforms!
    for(VectorUniformIterator v = vectorUniforms.begin(); v != vectorUniforms.end(); v++)
        if(!(*v).second.perObjectUniform)
            shader->SetVectorUniform((*v).second.id, (*v).second.value);

    // Send matrix uniforms!

    for(MatrixUniformIterator m = matrixUniforms.begin(); m != matrixUniforms.end(); m++)
        if(!(*m).second.perObjectUniform)
            shader->SetMatrixUniform((*m).second.id, (*m).second.value);

    // Send matrix uniforms!
    for(MatrixArrayUniformIterator m = matrixArrayUniforms.begin(); m != matrixArrayUniforms.end(); m++)
        if(!(*m).second.perObjectUniform)
            shader->SetMatrixArrayUniform((*m).second.id, (*m).second.value, (*m).second.size);

    // Send float uniforms!
    for(FloatUniformIterator f = floatUniforms.begin(); f != floatUniforms.end(); f++)
        if(!(*f).second.perObjectUniform)
            shader->SetFloatUniform((*f).second.id, (*f).second.value);

    // Send texture uniforms!

    uint counter = 0;
    for(textureUniformIterator v = textureUniforms.begin(); v != textureUniforms.end(); v++)
    {
       if(!(*v).second.perObjectUniform)
       {
           shader->SetTextureUniform((*v).second.id, (*v).second.value->GetTextureID(), counter++);
       }
    }

    for(cubetextureUniformIterator v = cubetextureUniforms.begin(); v != cubetextureUniforms.end(); v++)
    {
       if(!(*v).second.perObjectUniform)
       {
           shader->SetCubeTextureUniform((*v).second.id, (*v).second.value, counter);
       }
    }

    // Update all features needed for this draw call!
    for(FeatureIterator f = featureMap.begin(); f != featureMap.end(); f++)
    {
        GLenum id = f->first;

        bool enabled = f->second;
        bool currentlyEnabled = glIsEnabled(id);

        if(currentlyEnabled != enabled)
        {
            if(enabled)
                glEnable(id);
            else
                glDisable(id);
        }
    }

    if(featureMap[GL_BLEND] == true)
        glBlendFunc(blendOperation.source, blendOperation.destination);

    if(overrideDrawingMode == -1)
        shader->Render(mesh, (GLenum) GL_TRIANGLES);
    else
        shader->Render(mesh,(GLenum) overrideDrawingMode);
}

void Material::SetInt(std::string name, int i)
{
    SetUniformValue(name, intUniforms, i, false, 1);
}

void Material::SetFloat(std::string name, float f)
{
    SetUniformValue(name, floatUniforms, f, false, 1);
}

void Material::SetVector(std::string name, const glm::vec4 &v)
{
    SetUniformValue(name, vectorUniforms, v, false, 1);
}

void Material::SetColor(std::string name, const glm::vec4 &v)
{
    SetVector(name, v);
}

void Material::SetMatrix(std::string name, const glm::mat4 &m)
{
    SetUniformValue(name, matrixUniforms, m, false, 1);
}

void Material::SetMatrixArray(std::string name, glm::mat4 * m, int size)
{
    SetUniformValue(name, matrixArrayUniforms, m, false, size);
}

void Material::SetTexture(std::string name, Texture *texture)
{
    SetUniformValue(name, textureUniforms, texture, false, 1);
}

void Material::SetCubeTexture(std::string name, GLuint i)
{
    SetUniformValue(name, cubetextureUniforms, i, false, 1);
}

void Material::SetOverrideDrawingMode(GLenum mode)
{
    this->overrideDrawingMode = mode;
}

void Material::SetFeature(GLenum id, bool value)
{
    this->featureMap[id] = value;
}

bool Material::GetFeature(GLenum id)
{
    return this->featureMap[id];
}

void Material::SetBlendOperation(Material::BlendOperation op)
{
    this->blendOperation = op;
}

void Material::SetRenderingQueue(Material::RenderingQueue queue)
{
    this->queue = queue;
}

Material::RenderingQueue Material::GetRenderingQueue()
{
    return queue;
}

float Material::GetFloat(std::string name)
{
    return GetUniformValue<float>(name, floatUniforms);
}

glm::vec4 Material::GetVector(std::string name)
{
    return GetUniformValue<glm::vec4>(name, vectorUniforms);
}

glm::mat4 Material::GetMatrix(std::string name)
{
    return GetUniformValue<glm::mat4>(name, matrixUniforms);
}

bool Material::Compare(const Material &lhs, const Material &rhs)
{
    return lhs.queue < rhs.queue;
}

template<typename T>
T Material::GetUniformValue(std::string name, std::unordered_map<std::string, MaterialUniform<T> >& map)
{
    typename std::unordered_map<std::string, MaterialUniform<T>>::iterator it = map.find(name);

    if(it == map.end())
        return T();

    return (*it).second.value;
}

template<typename T>
void Material::SetUniformValue(std::string name, std::unordered_map<std::string, MaterialUniform<T> > &map, const T &value, bool perObjectUniform, int size)
{
    typename std::unordered_map<std::string, MaterialUniform<T>>::iterator it = map.find(name);

    if(it != map.end())
    {
        MaterialUniform<T> uniform = map[name];
        uniform.value = value;
        uniform.size = size; // Can be modified!
        map[name] = uniform;
    }
    else
    {
        MaterialUniform<T> uniform;
        std::string internalName = GetUniformName(name);
        uniform.id = shader->GetUniformLocation(internalName.data());
        uniform.size = size;
        uniform.value = value;
        uniform.perObjectUniform = perObjectUniform;
        map[name] = uniform;
    }
}
