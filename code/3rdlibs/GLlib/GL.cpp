#include "GL.h"
#include <stdio.h>

GL::GL() {}

GL::~GL() {}


void GL::InitGLStatus(glInitStatus &GLstatus) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, GLstatus.pixelUnpackedAlignment);
    glPixelStorei(GL_PACK_ALIGNMENT, GLstatus.pixelPackedAlignment);

}

void GL::SetupOrthogonalView(int viewport_ox, int viewport_oy, int viewport_w, int viewport_h,
                             int left, int right, int bottom, int top) {
    if (viewport_h == 0)
        viewport_h = 1;
    glViewport(viewport_ox, viewport_oy, viewport_w, viewport_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(left, right, bottom, top);//xmin, xmax, ymin, ymax

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GL::DrawRenderTarget(int o_x, int o_y, int x, int y, GLuint tex) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);
    SetupOrthogonalView(o_x, o_y, x, y);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    DrawFullScreenQuad();
    glDisable(GL_TEXTURE_2D);
}

void GL::DrawFullScreenQuad() {
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(1, 0);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(0, 1);
    glVertex2f(0, 1);

    glEnd();
}

void GL::DrawFullScreenQuadNegativeOneToOne() {
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(-1.0, -1.0);
    glEnd();
}


void GL::CheckErrors(void) {
    GLenum err = glGetError();
    const char *name;
    switch (err) {
        case GL_NO_ERROR:
            name = NULL;
            break;
        case GL_INVALID_ENUM:
            name = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            name = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            name = "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            name = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            name = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            name = "GL_OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
            name = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
            break;
        default:
            name = "unknown";
            break;
    }

    if (name)
        printf("Caught GL error 0x%04x (%s)!", err, name);
}
