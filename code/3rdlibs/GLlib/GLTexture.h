#ifndef _GL_TEXTURE_H
#define  _GL_TEXTURE_H

#define GLEW_STATIC

#include <GL/glew.h>
#include <string>



class GLTexture {
public:
    GLTexture(int width, int height = 0, int depth = 0, GLenum elementFormat = GL_RGBA,
              GLint internalFormat = GL_RGBA8, GLenum dataType = GL_UNSIGNED_BYTE, GLenum filterType = GL_LINEAR,
              GLenum borderType = GL_CLAMP_TO_EDGE);

    ~GLTexture();

    void InitTexture(int width, int height = 0, int depth = 0, GLenum elementFormat = GL_RGBA,
                     GLint internalFormat = GL_RGBA8, GLenum filterType = GL_LINEAR,
                     GLenum borderType = GL_CLAMP_TO_EDGE);

    GLuint GetTextureID();

    GLuint GetHeight();

    GLuint GetWidth();

    GLuint GetDepth();

    GLenum GetTextureType();

    void Bind();
    void toOpenGL(void *data);

protected:
    std::string _fileName;
    std::string _fileType;
    void *_data;
    int _dataSize;


    GLenum _filterType;
    GLenum _borderType;
    GLenum _textureType;
    GLenum _elementFormat;
    unsigned int _internalFormat;
    GLenum _elementType;

    GLuint _tex;
    GLuint _dim[3];

    bool _isBufferAllocated;
};

#endif
