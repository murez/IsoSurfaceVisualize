#include <cassert>
#include <cstdio>
#include <fstream>

#include "../include/RayCasting.h"

RayCasting::RayCasting() {}

RayCasting::~RayCasting() {
    if (texBlockPool)
        delete texBlockPool;
}

void RayCasting::ReLoadShader() {
    program.MakeProgramFromString(
            Shader::LoadShaderToString("C:\\Users\\murez\\CG_Final\\glsl\\vert.glsl"),
            Shader::LoadShaderToString("C:\\Users\\murez\\CG_Final\\glsl\\frag.glsl"));
}

void RayCasting::Init(int width, int height) {
    printf("Init Raycaster");

    _width = width;
    _height = height;
    _aspectRatio = float(width) / float(height);

    glInitStatus status;
    GL::InitGLStatus(status); //enable unaligned texture uploading

    //create default glsl program
    ReLoadShader();

    arcBall = new ArcBall(width, height);

    GL::CheckErrors();
}

void RayCasting::Resize(int width, int height) {
    _width = width;
    _height = height;
    _aspectRatio = float(width) / float(height);
}


void RayCasting::Render() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 Rot = glm::make_mat4(arcBall->GetRotation().array);
    _new_rotation_mat = _old_rotation_mat * Rot;

    glm::mat4 invRotationMatrix = glm::transpose(_new_rotation_mat);


    GL::SetupOrthogonalView(0, 0, _width, _height, -1, 1, -1, 1);

    program.BeginProgram();
    _new_transfer_vec = _transfer_vec + glm::make_vec3(arcBall->GetTranslationVec());

    program.setUniformMat4("rotMat4", invRotationMatrix);
    program.setUniform3fv("cameraO", O_vec);
    program.setUniform3fv("cameraDir", Dir_vec);
    program.setUniform3fv("cameraUp", Up_vec);

    program.setUniform1f("stepsize", _param.stepsize);
    program.setUniform1f("aspectRatio", _aspectRatio);
    program.setUniform3fv("translateVec3", _new_transfer_vec);

    program.setUniform1f("sizeX", float(_sizeX));
    program.setUniform1f("sizeY", float(_sizeY));
    program.setUniform1f("sizeZ", float(_sizeZ));
    program.setUniform1f("isPreIntegration", float(_param.preIntegration));

    program.BindTexture(1, GL_TEXTURE_1D, _CDFlut, "CDFtranf");
    program.BindTexture(2, GL_TEXTURE_1D, _lut, "tranf");
    program.BindTexture(3, texBlockPool->GetTextureType(), texBlockPool->GetTextureID(), "blockPool");
    program.BindTexture(4, GL_TEXTURE_2D, _PDFlut, "preInt2D");

    GL::DrawFullScreenQuadNegativeOneToOne();

    program.EndProgram();
    GL::CheckErrors();
}

void RayCasting::SetShadingParameters(Renderparam &paraS) {
    _param = paraS;
    if (_param.stepsize < 0.0001f)
        _param.stepsize = 0.001f;
}

bool RayCasting::LoadVolumeData(void *buffer, int z, int y, int x, dataType type) {
    _sizeX = x;
    _sizeY = y;
    _sizeZ = z;
    if (type == Uchar)
        texBlockPool = new GLTexture(_sizeX, _sizeY, _sizeZ, GL_LUMINANCE, GL_INTENSITY);
    else if (type == Float)
        texBlockPool = new GLTexture(_sizeX, _sizeY, _sizeZ, GL_LUMINANCE, GL_INTENSITY, GL_FLOAT, GL_LINEAR);
    else if (type == Int)
        texBlockPool = new GLTexture(_sizeX, _sizeY, _sizeZ, GL_LUMINANCE, GL_INTENSITY, GL_INT);
    else if (type == Uint)
        texBlockPool = new GLTexture(_sizeX, _sizeY, _sizeZ, GL_LUMINANCE, GL_INTENSITY, GL_UNSIGNED_INT);

    texBlockPool->toOpenGL(buffer);


    return true;
}

void RayCasting::SetTexture(GLuint LookUpTable, GLuint CDFTable, GLuint PDFTable) {
    _lut = LookUpTable;
    _CDFlut = CDFTable;
    _PDFlut = PDFTable;
}

int RayCasting::getViewportHeight() { return _height; }

void RayCasting::SetPathParameters(glm::vec3 O, glm::vec3 Dir, glm::vec3 Up) {
    O_vec = O;
    Dir_vec = Dir;
    Up_vec = Up;
}

