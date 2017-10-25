#include "texture.h"

static short le_short(unsigned char *bytes)
{
    return bytes[0] | ((char)bytes[1] << 8);
}

static void *read_tga(const char *filename, int *width, int *height)
{
    struct tga_header {
        char  id_length;
        char  color_map_type;
        char  data_type_code;
        unsigned char  color_map_origin[2];
        unsigned char  color_map_length[2];
        char  color_map_depth;
        unsigned char  x_origin[2];
        unsigned char  y_origin[2];
        unsigned char  width[2];
        unsigned char  height[2];
        char  bits_per_pixel;
        char  image_descriptor;
    } header;
    int i, color_map_size, pixels_size;
    FILE *f;
    size_t read;
    void *pixels;
    /*errno_t err;
    err = */
    fopen_s(&f, filename, "rb");

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    read = fread(&header, 1, sizeof(header), f);

    if (read != sizeof(header)) {
        fprintf(stderr, "%s has incomplete tga header\n", filename);
        fclose(f);
        return NULL;
    }
    if (header.data_type_code != 2) {
        fprintf(stderr, "%s is not an uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }
    /*
    if (header.bits_per_pixel != 24) {
        fprintf(stderr, "%s is not a 24-bit uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }
    */
    for (i = 0; i < header.id_length; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete id string\n", filename);
            fclose(f);
            return NULL;
        }

    color_map_size = le_short(header.color_map_length) * (header.color_map_depth / 8);
    for (i = 0; i < color_map_size; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete color map\n", filename);
            fclose(f);
            return NULL;
        }

    *width = le_short(header.width); *height = le_short(header.height);
    pixels_size = *width * *height * (header.bits_per_pixel / 8);
    pixels = malloc(pixels_size);

    read = fread(pixels, 1, pixels_size, f);
    fclose(f);

    if (read != (unsigned int)pixels_size) {
        fprintf(stderr, "%s has incomplete image\n", filename);
        free(pixels);
        return NULL;
    }

    return pixels;

    fclose(f);
}

Texture::Texture() : Asset(), textureID(-1), width(0), height(0)
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureID);
}

int Texture::GetWidth()
{
    return width;
}

int Texture::GetHeight()
{
    return height;
}

GLuint Texture::GetTextureID()
{
    return textureID;
}

void Texture::LoadFromFilename(std::string filename)
{
    uint8_t * pixels = LoadFromImage(filename.data());
    free(pixels);
}

uint8_t *Texture::LoadFromImage(const char *path)
{
    return LoadFromImage(path, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
}

uint8_t *Texture::LoadFromImage(const char *path, GLint Min_Filter, GLint Mag_Filter, GLint Wrap_s, GLint Wrap_t)
{
    QString Curpath = QDir::currentPath();
    int t = Curpath.lastIndexOf('/');
    Curpath.chop(Curpath.size() - t);
    Curpath.append(QString(path));

    void *pixels = read_tga(Curpath.toStdString().c_str(), &width, &height);

    if (!pixels)
        return nullptr;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Min_Filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Mag_Filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_t);

    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level of detail */
        GL_RGBA,                    /* internal format */
        width, height, 0,           /* width, height, border */
        GL_BGRA_EXT, GL_UNSIGNED_BYTE,   /* external format, type */
        pixels                      /* pixels */
    );

    return (uint8_t *) pixels;
}

CubeTexture::CubeTexture()
{
}

CubeTexture::CubeTexture(std::vector<const char *> path, TimeLine TL)
{
    LoadFromImage(path, TL);
}

CubeTexture::~CubeTexture()
{
}

void CubeTexture::LoadFromImage(std::vector<const char *> path, TimeLine TL)
{
    glGenTextures(1, &m_Texture[TL]);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture[TL]);

    int width;
    int height;

    for(uint i = 0; i< path.size(); i++)
    {
        QString Curpath = QDir::currentPath();
        int t = Curpath.lastIndexOf('/');
        Curpath.chop(Curpath.size() - t);
        Curpath.append(QString(path[i]));

        /*
        t = Curpath.lastIndexOf('.');
        Curpath.chop(Curpath.size() - t);

        Curpath.append(QString::number(i));
        Curpath.append(".tga");
        */

        void *pixels = read_tga(Curpath.toStdString().c_str(), &width, &height);

        if (!pixels)
            return;

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,           /* target, level of detail */
            GL_RGB,                    /* internal format */
            width, height, 0,           /* width, height, border */
            GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
            pixels                      /* pixels */
        );

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        free(pixels);
    }
}

ReadableTexture::ReadableTexture() : pixels(nullptr)
{

}

ReadableTexture::~ReadableTexture()
{
}

uint8_t *ReadableTexture::GetPixels()
{
    return pixels;
}

void ReadableTexture::FreePixels()
{
    free(pixels);
    pixels = nullptr;
}

void ReadableTexture::LoadFromFilename(std::string filename)
{
    this->pixels = LoadFromImage(filename.data());
}
