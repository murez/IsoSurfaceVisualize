#include"Shader.h"

#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

Shader::Shader() {}

Shader::~Shader() {
    if (buffer)
        delete buffer;
}

void Shader::checkInput(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        cout << "Can't open - " << filename;
        system("pause");
        exit(0);
    } else {
        fclose(fp);
        return;
    }
}

std::string *Shader::LoadShaderToString(const char *filename) {
    checkInput(filename);
    int size = 100000;
    char *cbuffer = new GLchar[size];
    memset(cbuffer, 0, sizeof(GLchar) * size);

    ifstream shader;
    shader.open(filename, ios::in);
    shader.seekg(0, ios::end);
    int length = int(shader.tellg());
    shader.seekg(0, ios::beg);
    shader.read(cbuffer, length);
    shader.close();
    auto *str = new std::string(cbuffer);

    return str;
}

bool Shader::MakeProgramFromString(std::string *vertString, std::string *fragString) {
    GLuint vertexglsl, fragmentglsl;
    //vertex
    vertexglsl = glCreateShader(GL_VERTEX_SHADER);
    GLchar const *vertC = vertString->c_str();
    auto const vertSize = (GLint const) vertString->size();
    glShaderSource(vertexglsl, 1, &vertC, &vertSize);
    glCompileShader(vertexglsl);
    _printlog(vertexglsl);
    cout << "\nvertexshader compiled!\n" << endl;

    //fragment
    fragmentglsl = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar const *fragC = fragString->c_str();
    GLint const fragSize = (GLint const) fragString->size();
    glShaderSource(fragmentglsl, 1, &fragC, &fragSize);
    glCompileShader(fragmentglsl);
    _printlog(fragmentglsl);
    cout << "fragmentshader compiled!\n" << endl;

    ///clear up
    delete vertString;
    delete fragString;

    GLint vertcompiled, fragmentcompiled, linked;

    glGetShaderiv(vertexglsl, GL_COMPILE_STATUS, &vertcompiled);
    glGetShaderiv(fragmentglsl, GL_COMPILE_STATUS, &fragmentcompiled);

    if (!vertcompiled)
        return false;

    prog = glCreateProgram();

    glAttachShader(prog, vertexglsl);
    glAttachShader(prog, fragmentglsl);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    _printlog(prog);

    if (!linked) {
        cout << "Link failed! \n" << endl;
        return false;
    }
    return true;
}

void Shader::BindTexture(unsigned int unit, GLenum texturetype, GLuint textureid, const char *nameinshader) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniform1i(loc, unit);

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(texturetype, textureid);
}

void Shader::setUniformMat4(const char *nameinshader, const glm::mat4 &value) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniformMatrix4fv(loc, 1, GL_FALSE,
                       glm::value_ptr(value));
}

void Shader::setUniform1f(const char *nameinshader, float value) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniform1f(loc, value);
}

void Shader::setUniform3fv(const char *nameinshader, glm::vec3 value) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::setUniform3f(const char *nameinshader, float *value) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniform3f(loc, value[0], value[1], value[2]);
}

void Shader::setUniformMatf(const char *nameinshader, float *value) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniformMatrix4fv(loc, 1, 0, value);
}

void Shader::setUniformMat3f(const char *nameinshader, float *value) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniformMatrix3fv(loc, 1, 0, value);
}

void Shader::setUniform1uint(const char *nameinshader, unsigned int value) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniform1uiEXT(loc, value);
}

void Shader::setUniform3uint(const char *nameinshader, unsigned int x, unsigned int y, unsigned int z) const {
    GLuint loc = glGetUniformLocation(prog, nameinshader);
    glUniform3uiEXT(loc, x, y, z);
}


void Shader::BeginProgram() const {
    glUseProgram(prog);
}

void Shader::EndProgram() {
    glUseProgram(0);
}

void Shader::_printlog(GLuint obj) {
    int infologLength = 0;
    int maxLength;

    if (glIsShader(obj))
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
    else
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);

    char *infoLog = new char[maxLength];

    if (glIsShader(obj))
        glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
    else
        glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);

    if (infologLength > 0)
        printf("%s\n", infoLog);
}
