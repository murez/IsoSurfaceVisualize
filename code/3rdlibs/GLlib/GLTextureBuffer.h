#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#define GLEW_STATIC
#include <GL/glew.h>



template<typename T>
class GLTextureBuffer
{
public:
  GLTextureBuffer(GLenum internalFormat, size_t length, T* data=NULL, GLenum usage=GL_STATIC_DRAW);
  ~GLTextureBuffer();
  void UpdateBuffer(T* data, size_t size);
  GLenum GetTextureBufferID(){return _texID;}

  void Bind();

private:
  GLenum _internalFormat;
  size_t _size;
  GLuint _bufferID, _texID;
  GLenum _usage;


};

template<typename T>
GLTextureBuffer<T>::GLTextureBuffer(GLenum internalFormat, size_t length, T* data, GLenum usage)
:_internalFormat(internalFormat),
_size(length)
{
  //create buffer
  _usage = usage;
  glGenBuffers(1, &_bufferID );
  glBindBuffer(GL_TEXTURE_BUFFER_EXT, _bufferID);
  glBufferData(GL_TEXTURE_BUFFER_EXT, sizeof(T)*_size, data, usage);

  glGenTextures(1, &_texID);
  glBindTexture(GL_TEXTURE_BUFFER_EXT, _texID);
  glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, internalFormat, _bufferID);

  GL::CheckErrors();

}

template<typename T>
GLTextureBuffer<T>::~GLTextureBuffer()
{
  glDeleteBuffers(1, &_bufferID);
  glDeleteTextures(1, &_texID);
}



template<typename T>
void GLTextureBuffer<T>::UpdateBuffer(T* data, size_t size)
{
  _size = size;
  glBindBuffer(GL_TEXTURE_BUFFER_EXT, _bufferID);
  glBufferData(GL_TEXTURE_BUFFER_EXT, sizeof(T)*_size,data, _usage);

  glBindTexture(GL_TEXTURE_BUFFER_EXT, _texID);
  glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, _internalFormat, _bufferID);

  GL::CheckErrors();
}

template<typename T>
void GLTextureBuffer<T>::Bind()
{
  glBindTexture(GL_TEXTURE_BUFFER_EXT, _texID);
}


#endif
