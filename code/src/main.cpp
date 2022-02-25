#include <cmath>

#include <cstdio>
#include <cstring>
#include <cassert>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>

#include "../include/RayCasting.h"
#include "../include/TransferFunction.h"

#include "../include/volumeData.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

int window_id;

int showTransferFunc = 0;


RayCasting *raycaster;
TransferFunction *transfer_function;

Renderparam param;

glm::vec3 O_vec(0.0, 0.0, 4.0);
glm::vec3 Up(0, 1, 0);

void display();

void reshape(int w, int h);

void keyboard(unsigned char key, int _x, int _y);

void mouse(int button, int state, int x, int y);

void move(int x, int y);

int main(int argc, char *argv[]) {

    if (argc != 3) {
        cout << "Usage: cthead.bin uchar" << endl;
        return 1;
    }

    string input_file_name = argv[1];

    glutInit(&argc, argv);
    glutInitWindowSize(1024, 1024);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    window_id = glutCreateWindow("Isosurface");

    glewInit();

    raycaster = new RayCasting;
    raycaster->Init(1024, 1024);

    FILE *fp = fopen(input_file_name.c_str(), "rb");
    int resolution[3];
    float dim[3] = {0, 0, 0};
    fread(resolution, sizeof(int), 3, fp);
    fread(dim, sizeof(float), 3, fp);
    auto *D = new float[resolution[0] * resolution[1] * resolution[2]];
    fread(D, sizeof(float), resolution[0] * resolution[1] * resolution[2], fp);
    fclose(fp);
    auto *volume_data = new volumeData<float>(D, resolution[2], resolution[1], resolution[0]);

    printf("isovalue -- max: %f - min: %f\n", float(volume_data->getMax()), float(volume_data->getMin()));

    if (string(argv[2]) == "uchar") {
        raycaster->LoadVolumeData(volume_data->getDataBuffer(), volume_data->getDimZ(), volume_data->getDimY(),
                                  volume_data->getDimX(), Uchar);
    } else if (string(argv[2]) == "float")
        raycaster->LoadVolumeData(volume_data->getDataBuffer(), volume_data->getDimZ(), volume_data->getDimY(),
                                  volume_data->getDimX(), Float);
    else {
        cout << "Error: wrong input type" << endl;
        return 1;
    }


    transfer_function = new TransferFunction;


    raycaster->SetTexture(transfer_function->getTexture(),
                          transfer_function->getCDFTexture(),
                          transfer_function->getPreIntTexture2D());


    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(move);

    glutMainLoop();

    return 0;
}

void keyboard(unsigned char key, int _x, int _y) {
    switch (key) {
        case 27:
            exit(0);
        case 't':
            if (showTransferFunc == 0)
                showTransferFunc = 1;
            else
                showTransferFunc = 0;
            break;
        case 'r':
            raycaster->ReLoadShader();
            break;
        case '-':
            param.stepsize -= 0.002;
            break;
        case '=':
            param.stepsize += 0.002;
            break;
        case 'p':
            if (param.preIntegration == 1) {
                transfer_function->calculatePreIntegration2D();
                param.preIntegration++;
            } else {
                param.preIntegration = 1;
            }
            printf("preIntegration status: %d\n", param.preIntegration);
            break;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    raycaster->Resize(w, h);
    raycaster->arcBall->SetWindowSize(w, h);
}

void display() {
    raycaster->SetShadingParameters(param);


    glm::vec3 dir(0, 0, -3);
    dir = glm::make_mat3(raycaster->arcBall->GetRotation().array) * dir;
    dir = glm::normalize(dir);

    raycaster->SetPathParameters(O_vec, dir, Up);

    raycaster->Render();

    if (showTransferFunc == 1)
        transfer_function->Render();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (showTransferFunc == 0) {
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN)
                raycaster->arcBall->OnLeftButtonDown(x, y);
            else
                raycaster->arcBall->OnLeftButtonUp(x, y);
        } else if (button == GLUT_RIGHT_BUTTON) {
            if (state == GLUT_DOWN)
                raycaster->arcBall->OnRightButtonDown(x, y);
            else
                raycaster->arcBall->OnRightButtonUp(x, y);
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (state == GLUT_DOWN)
                raycaster->arcBall->OnMiddleButtonDown(x, y);
            else
                raycaster->arcBall->OnMiddleButtonUp(x, y);
        }
    } else
        transfer_function->MouseButton(button, state, x, raycaster->getViewportHeight() - y);
    glutPostRedisplay();

}

void move(int x, int y) {
    transfer_function->MouseMove(x, raycaster->getViewportHeight() - y);

    raycaster->arcBall->OnMouseMove(x, y);

    glutPostRedisplay();
}
