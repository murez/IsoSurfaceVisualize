#ifndef PROGRAM_H
#define PROGRAM_H

#define GLEW_STATIC

#include <GL/glew.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define MAXPROGRAM 10

class Shader {
public:
    Shader();

    ~Shader();


    bool MakeProgramFromString(std::string *vertString, std::string *fragString);

    void BeginProgram() const; //glUseProgram(prog)
    static void EndProgram();

    void BindTexture(unsigned int unit, GLenum, GLuint, const char *nameinshader) const;

    void setUniformMat4(const char *nameinshader, const glm::mat4 &value) const;

    void setUniform1f(const char *, float) const;

    void setUniform3fv(const char *nameinshader, glm::vec3 value) const;

    void setUniform3f(const char *nameinshader, float *value) const;

    void setUniformMatf(const char *, float *) const;

    void setUniformMat3f(const char *, float *) const;

    void setUniform1uint(const char *, unsigned int) const;

    void setUniform3uint(const char *nameinshader, unsigned int x, unsigned int y, unsigned int z) const;

    static std::string *LoadShaderToString(const char *filename);


private:
    GLuint prog;


    void _printlog(GLuint obj);

    static void checkInput(const char *filename);

    //handle
    char *buffer;
    GLint len;

};


#endif
