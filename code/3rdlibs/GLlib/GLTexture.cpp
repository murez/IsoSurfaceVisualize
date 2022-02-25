
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "GL.h"
#include "GLTexture.h"


GLTexture::GLTexture(int width, int height, int depth, GLenum elementFormat, GLint internalFormat, GLenum dataType,
                     GLenum filterType, GLenum borderType) {
    _elementType = dataType;
    _data = nullptr;
    InitTexture(width, height, depth, elementFormat, internalFormat, filterType, borderType);
}

GLTexture::~GLTexture() {
    if (_data)
        free(_data);
    glDeleteTextures(1, &_tex);
}

void GLTexture::InitTexture(int width, int height, int depth, GLenum elementFormat,
                            GLint internalFormat, GLenum filterType, GLenum borderType) {
    _dim[0] = width;
    _dim[1] = height;
    _dim[2] = depth;

    _borderType = borderType;
    _filterType = filterType;
    _elementFormat = elementFormat;
    _internalFormat = internalFormat;

    glGenTextures(1, &_tex);

    if (height == 0 && depth == 0) {
        _textureType = GL_TEXTURE_1D;
        this->Bind();
        glTexParameteri(_textureType, GL_TEXTURE_WRAP_S, _borderType);
        glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, _filterType);
        glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, _filterType);

    } else if (depth == 0) {
        _textureType = GL_TEXTURE_2D;
        this->Bind();
        glTexParameteri(_textureType, GL_TEXTURE_WRAP_S, _borderType);
        glTexParameteri(_textureType, GL_TEXTURE_WRAP_T, _borderType);
        glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, _filterType);
        glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, _filterType);
    } else {
        _textureType = GL_TEXTURE_3D;
        this->Bind();
        glTexParameteri(_textureType, GL_TEXTURE_WRAP_S, _borderType);
        glTexParameteri(_textureType, GL_TEXTURE_WRAP_T, _borderType);
        glTexParameteri(_textureType, GL_TEXTURE_WRAP_R, _borderType);
        glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, _filterType);
        glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, _filterType);
    }
}

GLuint GLTexture::GetTextureID() {
    return _tex;
}

GLuint GLTexture::GetHeight() {
    return _dim[0];
}

GLuint GLTexture::GetWidth() {
    return _dim[1];
}

GLuint GLTexture::GetDepth() {
    return _dim[2];
}

GLenum GLTexture::GetTextureType() {
    return _textureType;
}

//Operations
void GLTexture::Bind() {
    glBindTexture(_textureType, _tex);
}

void GLTexture::toOpenGL(void *data) {
    switch (_textureType) {
        case (GL_TEXTURE_1D):
            glTexImage1D(_textureType, 0, _internalFormat, _dim[0], 0, _elementFormat, _elementType, data);
            break;
        case (GL_TEXTURE_2D):
            glTexImage2D(_textureType, 0, _internalFormat, _dim[0], _dim[1], 0, _elementFormat, _elementType, data);
            break;
        case (GL_TEXTURE_3D):
            glTexImage3D(_textureType, 0, _internalFormat, _dim[0], _dim[1], _dim[2], 0, _elementFormat, _elementType,
                         data);
            break;
    }
}
