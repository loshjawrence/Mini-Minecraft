#include "rendertargettexture.h"

RenderTargetTexture::RenderTargetTexture() : Texture()
{
}

RenderTargetTexture::~RenderTargetTexture()
{
    openGLContext->glDeleteFramebuffers(1, &m_Framebuffer);
    openGLContext->glDeleteFramebuffers(1, &m_Renderbuffer);
}

void RenderTargetTexture::Create(float width, float height, OpenGLContext *openGLContext)
{
    this->width = width;
    this->height = height;

    openGLContext->glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    openGLContext->glBindTexture(GL_TEXTURE_2D, textureID);

    // Poor filtering. Needed !
    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Give an empty image to OpenGL ( the last "0" )
    openGLContext->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0,GL_RGBA, GL_FLOAT, 0);


    openGLContext->glGenFramebuffers(1, &m_Framebuffer);
    openGLContext->glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);


    openGLContext->glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,textureID,0);
    //openGLContext->glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textureID,0);

    /*
    openGLContext->glGenTextures(1, &m_DepthRenderTargetTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    openGLContext->glBindTexture(GL_TEXTURE_2D, m_DepthRenderTargetTexture);

    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    openGLContext->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Give an empty image to OpenGL ( the last "0" )
    openGLContext->glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, m_Width, m_Height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    */

    //-------------------------
    openGLContext->glGenRenderbuffers(1, &m_Renderbuffer);
    openGLContext->glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);
    openGLContext->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

    //-------------------------
    //Attach depth buffer to FBO
    openGLContext->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);

    //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    //openGLContext->glDrawBuffers(1, DrawBuffers);

    // Always check that our framebuffer is ok
    if(openGLContext->glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        bool bSuccess = true;
    }

    openGLContext->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
