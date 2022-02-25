#include "../include/TransferFunction.h"
#include <cmath>
#include <cstdlib>
#include "GL.h"
#include <omp.h>

//std::ostream &operator<<(std::ostream &os, const Key &key) {
//    os << key.ID << '\t';
//    os << key.posx << '\t' << key.posy << '\t';
//    os << key.color[0] << '\t' << key.color[1] << '\t' << key.color[2] << '\t' << key.color[3] << '\t';
//    os << key.active;
//    return os;
//}
//
//std::istream &operator>>(std::istream &is, Key &key) {
//    is >> key.ID;
//    is >> key.posx >> key.posy;
//    is >> key.color[0] >> key.color[1] >> key.color[2] >> key.color[3];
//    is >> key.active;
//    return is;
//}


extern "C" float **jet(int m);

TransferFunction::TransferFunction() {
    //setup status
    offsetx = 10;
    offsety = 10;
    dragenable = false;
    preIntegrationMode = 1;

    //ramp
    Key key1, key2;
    key1.ID = 0;
    key1.active = false;
    key1.color[0] = 1.0;
    key1.color[1] = 1.0;
    key1.color[2] = 1.0;
    key1.color[3] = 0.0;
    key1.posx = MARGIN;
    key1.posy = MARGIN;
    Keylist.push_back(key1);


    key2.ID = 255;
    key2.active = false;
    key2.color[0] = 1.0;
    key2.color[1] = 1.0;
    key2.color[2] = 1.0;
    key2.color[3] = 1.0;
    key2.posx = HISTO_X + MARGIN;
    key2.posy = MARGIN + HISTO_Y;

    Keylist.push_back(key2);
    //ramp end

    glGenTextures(1, &Lut);
    glBindTexture(GL_TEXTURE_1D, Lut);
    // set the texture parameters
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenTextures(1, &PLut);
    glBindTexture(GL_TEXTURE_1D, PLut);
    //need the parameter to make the texture works
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenTextures(1, &PLut2D);
    glBindTexture(GL_TEXTURE_2D, PLut2D);
    //need the parameter to make the texture works
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    this->_generatecolorpalette();

    _updatecolortable();
}

TransferFunction::~TransferFunction() {

}

void TransferFunction::Render() {

    glViewport(offsetx, offsety, MARGIN + HISTO_X + COLORPIKER_X + MARGIN,
               MARGIN + HISTO_Y + COLORPIKER_Y + MARGIN);
    // setup for planar 2D rendering
    glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    //glPushMatrix();
    glLoadIdentity();
    glOrtho(0, float(MARGIN + HISTO_X + COLORPIKER_X + MARGIN), 0.0, float(MARGIN + HISTO_Y + COLORPIKER_Y + MARGIN),
            0.0, 2.0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //draw colorshow
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 256; i++) {
        glColor4f(float(colortable[i][0]) / 255.0f, float(colortable[i][1]) / 255.0f, float(colortable[i][2]) / 255.0f,
                  float(colortable[i][3]) / 255.0f);
        glVertex2f(i * float(HISTO_X) / 255.0f, MARGIN + HISTO_Y + MARGIN);
        glVertex2f(i * float(HISTO_X) / 255.0f, MARGIN + HISTO_Y + MARGIN + COLORSHOW_Y);

        glColor4f(float(colortable[i + 1][0]) / 255.0f, float(colortable[i + 1][1]) / 255.0f,
                  float(colortable[i + 1][2]) / 255.0f, float(colortable[i + 1][3]) / 255.0f);
        glVertex2f((i + 1) * float(HISTO_X) / 255.0f, MARGIN + HISTO_Y + MARGIN);
        glVertex2f((i + 1) * float(HISTO_X) / 255.0f, MARGIN + HISTO_Y + COLORSHOW_Y + MARGIN);
    }
    glEnd();

    //draw background
    glBegin(GL_QUADS);
    glColor4f(0.3f, 0.3f, 0.3f, 0.4f);
    glVertex2i(0, 0);
    glVertex2i(MARGIN + HISTO_X + MARGIN, 0);
    glVertex2i(MARGIN + HISTO_X + MARGIN, MARGIN + HISTO_Y + MARGIN);
    glVertex2i(0, MARGIN + HISTO_Y + MARGIN);
    glEnd();


    //draw key line and kep point
    glBegin(GL_LINE_STRIP);
    for (auto it = Keylist.begin(); it != Keylist.end(); it++) {
        glColor3f(it->color[0], it->color[1], it->color[2]);
        glVertex2i(it->posx, it->posy);
    }
    glEnd();

    //non active point
    glPointSize(8.0);
    glBegin(GL_POINTS);
    for (auto it = Keylist.begin(); it != Keylist.end(); it++) {
        if (!it->active) {
            glColor3f(it->color[0], it->color[1], it->color[2]);
            glVertex2i(it->posx, it->posy);
        }
    }
    glEnd();

    //active point
    glPointSize(12.0);
    glBegin(GL_POINTS);
    for (auto it = Keylist.begin(); it != Keylist.end(); it++) {
        if (it->active) {
            glColor3f(it->color[0], it->color[1], it->color[2]);
            glVertex2i(it->posx, it->posy);
        }
    }
    glEnd();


    //draw colorpiker
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i < 256; i++) {
        glColor3f(colorpalette[i][0], colorpalette[i][1], colorpalette[i][2]);
        glVertex2i(MARGIN + HISTO_X + MARGIN * 7,
                   int(MARGIN + float(HISTO_Y + COLORSHOW_Y) / 256 * (i + 1) + MARGIN));
        glVertex2i(MARGIN + HISTO_X + MARGIN + COLORPIKER_X,
                   int(MARGIN + float(HISTO_Y + COLORSHOW_Y) / 256 * (i + 1) + MARGIN));
    }
    glEnd();

    glDisable(GL_BLEND);

}

bool TransferFunction::MouseMove(int x, int y) {
    //position in fransferfunction viewport
    int fx = x - offsetx;
    int fy = y - offsety;

    //is it in the TF rang
    if (_out_of_range(x, y))
        return false;

    if (dragenable) {
        std::list<Key>::iterator it = Keylist.begin();
        for (; it != Keylist.end(); it++) {
            it->active = false;
        }
        currentkey->active = true;
        if (!(currentkey->posx == HISTO_X + MARGIN && currentkey->posy == MARGIN + HISTO_Y))
            if (currentkey != Keylist.begin() && currentkey != Keylist.end()) {
                currentkey--;
                if (fx < currentkey->posx) {
                    fx = currentkey->posx + 1;
                }
                currentkey++;
                currentkey++;
                if (fx > currentkey->posx) {
                    fx = currentkey->posx - 1;
                }
                currentkey--;

                currentkey->posx = fx;
                currentkey->posy = fy;
                currentkey->ID = int(float(fx - MARGIN) / float(HISTO_X) * 255);
                float alpha = float(fy - MARGIN) / float(HISTO_Y);
                if (alpha < 0 || alpha > 1) alpha = 0;
                currentkey->color[3] = alpha;
            }
    }
    _updatecolortable();
    return true;

}

bool TransferFunction::MouseButton(int button, int state, int x, int y) {
    int fx = x - offsetx;
    int fy = y - offsety;
    //is it in the TF rang
    if (_out_of_range(x, y))
        return false;

    if (fx > MARGIN * 8 + HISTO_X && fx < MARGIN + HISTO_X + MARGIN + COLORPIKER_X
        && fy > MARGIN && fy < HISTO_Y + COLORSHOW_Y + MARGIN * 2) {
        int index = int(float(fy - MARGIN) / float(HISTO_Y + COLORSHOW_Y + MARGIN * 2) * 255);
        float setcolor[4];

        setcolor[0] = this->colorpalette[index][0];
        setcolor[1] = this->colorpalette[index][1];
        setcolor[2] = this->colorpalette[index][2];
        setcolor[3] = 0;


        for (auto it = Keylist.begin(); it != Keylist.end(); it++)
            if (it->active == true) {
                setcolor[3] = it->color[3];
                this->_setkeycolor(it, setcolor);
            }
    }

    if (LEFT_BUTTON == button) {
        switch (state) {
            case MOUSE_DOWN:
            {
                int classifyresult = _classify(x, y);

                if (classifyresult == 1) {
                    auto it = Keylist.begin();
                    for (; it != Keylist.end(); it++) {
                        it->active = false;
                    }
                    currentkey->active = true;

                    dragenable = true;
                }
                if (classifyresult == 2) {
                    Key key{};
                    key.active = true;
                    key.ID = int(float(fx - MARGIN) / float(HISTO_X) * 255);
                    key.posx = fx;
                    key.posy = fy;

                    float alpha = float(fy - MARGIN) / float(HISTO_Y);
                    if (alpha < 0 || alpha > 1) alpha = 0;
                    key.color[0] = float(colortable[key.ID][0]) / 255;
                    key.color[1] = float(colortable[key.ID][1]) / 255;
                    key.color[2] = float(colortable[key.ID][2]) / 255;
                    key.color[3] = alpha;
                    this->_addkey(currentkey, key);
                    dragenable = true;
                }
                break;
            }

            case MOUSE_UP: {
                dragenable = false;
            }
                return true;
        }
    } else if (RIGHT_BUTTON == button) {
        if (_classify(x, y) == MIDDLE_BUTTON)
            Keylist.erase(currentkey);
    }
    _updatecolortable();
    return true;

}

void TransferFunction::_addkey(std::list<Key>::iterator current, Key key) {
    Keylist.insert(current, key);

    for (auto it = Keylist.begin(); it != Keylist.end(); it++) {
        it->active = false;
    }
    current--;
    current->active = true;
    currentkey = current;

}

void TransferFunction::_setkeycolor(std::list<Key>::iterator it, float color[]) {
    it->color[0] = color[0];
    it->color[1] = color[1];
    it->color[2] = color[2];
    it->color[3] = color[3];
}

void TransferFunction::calculatePreIntegration1D() {
#pragma omp parallel for
    for (int i = 0; i < TABLESIZE; i++) {
        CDFtable[i][0] = 0.0f;
        CDFtable[i][1] = 0.0f;
        CDFtable[i][2] = 0.0f;
        CDFtable[i][3] = 0.0f;
    }

    float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int i = 0; i < TABLESIZE; i++) {
        sum[0] += float(colortable[i][0]) / 255.0f * (float(colortable[i][3]) / 255.0f);
        sum[1] += float(colortable[i][1]) / 255.0f * (float(colortable[i][3]) / 255.0f);
        sum[2] += float(colortable[i][2]) / 255.0f * (float(colortable[i][3]) / 255.0f);
        sum[3] += float(colortable[i][3]) / 255.0f;

        CDFtable[i][0] = sum[0];
        CDFtable[i][1] = sum[1];
        CDFtable[i][2] = sum[2];
        CDFtable[i][3] = sum[3];
    }
    glBindTexture(GL_TEXTURE_1D, PLut);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F_ARB, TABLESIZE, 0, GL_RGBA, GL_FLOAT, CDFtable);
    glBindTexture(GL_TEXTURE_1D, Lut);
    GL::CheckErrors();
}

void TransferFunction::calculatePreIntegration2D() {
#pragma omp parallel for collapse(2)
    for (int j = 0; j < TABLESIZE; j++)
        for (int i = 0; i < TABLESIZE; i++) {
            PDFTable[j][i][0] = 0.0f;
            PDFTable[j][i][1] = 0.0f;
            PDFTable[j][i][2] = 0.0f;
            PDFTable[j][i][3] = 0.0f;
        }

    for (int j = 0; j < TABLESIZE; j++)
        for (int i = 0; i < TABLESIZE; i++) {
            if (j != i) {
                float delta = 1.0f;
                float stepCompensation = 1.0f / float(abs(j - i));
                if (j > i) {
                    for (float fj = float(j); fj > float(i); fj -= delta) {
                        float alpha = (float(colortable[int(fj)][3]) / 255.0f) * stepCompensation;
                        PDFTable[j][i][0] +=
                                (1 - PDFTable[j][i][3]) * (float(colortable[int(fj)][0]) / 255.0f) * alpha;
                        PDFTable[j][i][1] +=
                                (1 - PDFTable[j][i][3]) * (float(colortable[int(fj)][1]) / 255.0f) * alpha;
                        PDFTable[j][i][2] +=
                                (1 - PDFTable[j][i][3]) * (float(colortable[int(fj)][2]) / 255.0f) * alpha;

                        PDFTable[j][i][3] += (1 - PDFTable[j][i][3]) * alpha;
                    }
                } else {
                    for (float fj = float(j); fj < float(i); fj += delta) {
                        float alpha = (float(colortable[int(fj)][3]) / 255.0f) * stepCompensation;
                        PDFTable[j][i][0] +=
                                (1 - PDFTable[j][i][3]) * (float(colortable[int(fj)][0]) / 255.0f) * alpha;
                        PDFTable[j][i][1] +=
                                (1 - PDFTable[j][i][3]) * (float(colortable[int(fj)][1]) / 255.0f) * alpha;
                        PDFTable[j][i][2] +=
                                (1 - PDFTable[j][i][3]) * (float(colortable[int(fj)][2]) / 255.0f) * alpha;

                        PDFTable[j][i][3] += (1 - PDFTable[j][i][3]) * alpha;
                    }
                }
                if (PDFTable[j][i][3] != 0.0) {
                    //convert back to C for c
                    PDFTable[j][i][0] /= PDFTable[j][i][3];
                    PDFTable[j][i][1] /= PDFTable[j][i][3];
                    PDFTable[j][i][2] /= PDFTable[j][i][3];
                }
            } else {
                PDFTable[j][i][0] = float(colortable[i][0]) / 255.0f;
                PDFTable[j][i][1] = float(colortable[i][1]) / 255.0f;
                PDFTable[j][i][2] = float(colortable[i][2]) / 255.0f;
                PDFTable[j][i][3] = float(colortable[i][3]) / 255.0f;
            }
        }

    glBindTexture(GL_TEXTURE_2D, PLut2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, TABLESIZE, TABLESIZE, 0, GL_RGBA, GL_FLOAT, PDFTable);
    GL::CheckErrors();
    glBindTexture(GL_TEXTURE_1D, Lut);
}

void TransferFunction::_updatecolortable() {
    int interval = 0;
    float incrementcolor[4];

    for (auto it = Keylist.begin(), oldit = it++; it != Keylist.end(); it++) {
        interval = abs(it->ID - oldit->ID);

        incrementcolor[0] = float((it->color[0] - oldit->color[0])) / float(interval);
        incrementcolor[1] = float((it->color[1] - oldit->color[1])) / float(interval);
        incrementcolor[2] = float((it->color[2] - oldit->color[2])) / float(interval);
        incrementcolor[3] = float((it->color[3] - oldit->color[3])) / float(interval);

        for (int i = 1; i <= interval; i++) {
            colortable[oldit->ID + i][0] = (unsigned char) ((oldit->color[0] + incrementcolor[0] * i) * 255);
            colortable[oldit->ID + i][1] = (unsigned char) ((oldit->color[1] + incrementcolor[1] * i) * 255);
            colortable[oldit->ID + i][2] = (unsigned char) ((oldit->color[2] + incrementcolor[2] * i) * 255);
            colortable[oldit->ID + i][3] = (unsigned char) ((oldit->color[3] + incrementcolor[3] * i) * 255);
        }
        oldit = it;
    }
    colortable[0][3] = 0;
    glBindTexture(GL_TEXTURE_1D, Lut);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, TABLESIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, colortable);

    if (preIntegrationMode == 1)
        calculatePreIntegration1D();

}


bool TransferFunction::_out_of_range(int x, int y) const {
    if (x > offsetx && x < offsetx + WIDTH && y > offsety && y < offsety + HEIGHT)
        return false;
    else
        return true;
}

float distancePointToLineSegment(const int linepoint1[], const int linepoint2[], const int point[]) {
    float linedist;
    float segdist;
    auto r_numerator = float((point[0] - linepoint1[0]) * (linepoint2[0] - linepoint1[0]) +
                             (point[1] - linepoint1[1]) * (linepoint2[1] - linepoint1[1]));
    auto r_denomenator = float((linepoint2[0] - linepoint1[0]) * (linepoint2[0] - linepoint1[0]) +
                               (linepoint2[1] - linepoint1[1]) * (linepoint2[1] - linepoint1[1]));
    float r = r_numerator / r_denomenator;

    auto s = float(((linepoint1[1] - point[1]) * (linepoint2[0] - linepoint1[0]) -
                    (linepoint1[0] - point[0]) * (linepoint2[1] - linepoint1[1])) / r_denomenator);

    linedist = fabs(s) * sqrt(r_denomenator);

    if ((r >= 0) && (r <= 1)) {
        segdist = linedist;
    } else {
        auto dist1 = float((point[0] - linepoint1[0]) * (point[0] - linepoint1[0]) +
                           (point[1] - linepoint1[1]) * (point[1] - linepoint1[1]));
        auto dist2 = float((point[0] - linepoint2[0]) * (point[0] - linepoint2[0]) +
                           (point[1] - linepoint2[1]) * (point[1] - linepoint2[1]));
        if (dist1 < dist2) {
            segdist = sqrt(dist1);
        } else {
            segdist = sqrt(dist2);
        }
    }
    return segdist;
}

int TransferFunction::_classify(int cur_mouse_pos_x, int cur_mouse_pos_y) {
    int fposx = cur_mouse_pos_x - offsetx;
    int fposy = cur_mouse_pos_y - offsety;

    for (auto it = Keylist.begin(); it != Keylist.end(); it++) {
        if (abs(it->posx - fposx) < BIAS && abs(it->posy - fposy) < BIAS) {
            currentkey = it;
            return 1;
        }
    }
    int linepoint1[2], linepoint2[2], pos[2];

    for (auto it = Keylist.begin(), oldit = it++; it != Keylist.end(); it++) {
        linepoint1[0] = it->posx;
        linepoint1[1] = it->posy;
        linepoint2[0] = oldit->posx;
        linepoint2[1] = oldit->posy;
        pos[0] = fposx;
        pos[1] = fposy;

        if (distancePointToLineSegment(linepoint1, linepoint2, pos) < BIAS) {
            currentkey = it;
            return 2;
        }
        oldit = it;
    }
    return 0;
}


void TransferFunction::_generatecolorpalette() {
    this->colorpalette = jet(256);
}
