#ifndef TEXTURE_H
#define TEXTURE_H

#define PHASE 4

#include <openglcontext.h>
#include "../../la.h"
#include "asset.h"
#include <QDir>

class Texture : public Asset
{
protected:
    int width;
    int height;
    GLuint textureID;

public:
    Texture();
    virtual ~Texture();

    int GetWidth();
    int GetHeight();

    GLuint GetTextureID();

    uint8_t * LoadFromImage(const char *path);
    uint8_t * LoadFromImage(const char *path, GLint Min_Filter, GLint Mag_Filter, GLint Wrap_s, GLint Wrap_t);

    virtual void LoadFromFilename(std::string filename);
};

class ReadableTexture : public Texture
{
protected:
    uint8_t * pixels;

public:
    ReadableTexture();
    virtual ~ReadableTexture();

    uint8_t * GetPixels();
    void FreePixels();

    virtual void LoadFromFilename(std::string filename);
};

enum TimeLine
{
    _0H, _6H, _12H, _18H
};

class CubeTexture : public Asset
{
public:
    CubeTexture();
    CubeTexture(std::vector<const char*>path, TimeLine TL);
    ~CubeTexture();

    void LoadFromImage(std::vector<const char*>path, TimeLine TL);

    GLuint m_Texture[PHASE];
};

#endif // TEXTURE_H
