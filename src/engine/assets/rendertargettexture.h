#ifndef RENDERTARGETTEXTURE_H
#define RENDERTARGETTEXTURE_H

#include "../../la.h"
#include "asset.h"
#include "texture.h"
#include <openglcontext.h>

class RenderTargetTexture : public Texture
{
public:
    RenderTargetTexture();

    virtual ~RenderTargetTexture();

    void Create(float width, float height, OpenGLContext * openGLContext);

    /*
    void render()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
        glViewport(0, 0, m_Width, m_Height);
    }
    */

    OpenGLContext * openGLContext;

    GLuint m_Framebuffer;
    GLuint m_Renderbuffer;

    //GLuint m_DepthRenderTargetTexture;
};

#endif // RENDERTARGETTEXTURE_H
