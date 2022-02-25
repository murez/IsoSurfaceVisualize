#ifndef GL_H
#define GL_H

#define GLEW_STATIC

#include <GL/glew.h>
//a GL wrapper
struct glInitStatus
{
  glInitStatus()
    :pixelPackedAlignment(1),pixelUnpackedAlignment(1)
  {

  };
  //PixelStorei
  unsigned int pixelPackedAlignment, pixelUnpackedAlignment;

  //
};
/*
* A collection of reusable function on top of OpenGL
*
*/

class GL
{
public:
  GL();
  ~GL();
  //init GL
  static void InitGLContext(); //called when there is no GLUT
  static void InitGLStatus(glInitStatus &GLstatus);

  static void DrawFullScreenQuad();
  static void DrawFullScreenQuadNegativeOneToOne();

  static void SetupPerspectView(int w, int h, float *rotMat4X4, float *transVec3, float *eye = 0 );
  static void SetupOrthogonalView(int viewport_ox, int viewport_oy,int viewport_w, int viewport_h,
                    int left=0, int right=1, int bottom=0, int top=1);

  static void DrawRenderTarget(int o_x, int o_y, int x, int y, GLuint tex);

  //Matrix
  static void BuildViewingMatrix(float* modelViewMatrix, float translate[], float invRotMat[]);//tran vec3 rot vec16
  static void BuildViewingMatrixNoTranslation(GLfloat* modelViewMatrix, GLfloat invRotMat[]);

  //error checking
  static void CheckErrors();


};

#endif
