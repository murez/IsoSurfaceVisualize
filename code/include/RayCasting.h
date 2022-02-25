#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "GL.h"
#include "GLTexture.h"
#include "Shader.h"
#include "GLTextureBuffer.h"
#include "volumeData.h"

#include "../3rdlibs/arcball/ArcBall.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct Renderparam {
    float stepsize;
    int preIntegration;

    Renderparam() {
        stepsize = 0.002f;
        preIntegration = 1;
    }
};

enum dataType {
    Uchar,
    Uint,
    Int,
    Float
};


class RayCasting {
public:
    RayCasting();

    ~RayCasting();

    void Init(int width, int height);

    void ReLoadShader();

    void Render();

    void Resize(int width, int height);

    void SetTexture(GLuint LookUpTable, GLuint CDFTable, GLuint PDFTable);

    int getViewportHeight();

    void SetShadingParameters(Renderparam &paraS);

    void SetPathParameters(glm::vec3 O, glm::vec3 Dir, glm::vec3 Up);

    bool LoadVolumeData(void *buffer, int z, int y, int x, dataType type);

    // camara controller
    ArcBall *arcBall;


private:
    Renderparam _param;
    Shader program;

    GLuint _lut, _CDFlut, _PDFlut;

    GLTexture *_texColorTable;
    GLTexture *texBlockPool;

    glm::fvec3 O_vec;
    glm::fvec3 Dir_vec;
    glm::fvec3 Up_vec;

    glm::mat4 _old_rotation_mat = glm::mat4(
            -1.0, 0.0, -0.5, 0,
            0.5, 0.0, -0.8, 0,
            0.0, -1.0, -0.2, 0,
            0, 0, 0, 1);
    glm::mat4 _new_rotation_mat;
    glm::vec3 _transfer_vec = glm::vec3(0, 0, 0);
    glm::vec3 _new_transfer_vec = glm::vec3(0, 0, 0);

    int _width, _height;
    float _aspectRatio;

private:
    int _sizeX;
    int _sizeY;
    int _sizeZ;
};


#endif
