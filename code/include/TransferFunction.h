#ifndef TRANSFORFUNCTION_H
#define TRANSFORFUNCTION_H
//#include<windows.h>
#include<GL/glew.h>
#include <iostream>
#include <fstream>
#include <list>
#include <iterator>

#define TABLESIZE 256

#define FACTOR 4
#define MARGIN 2
#define COLORPIKER_Y 50*FACTOR
#define COLORPIKER_X 30*FACTOR
#define HISTO_X 150*FACTOR
#define HISTO_Y 50*FACTOR
#define COLORSHOW_Y 5*FACTOR

#define WIDTH MARGIN + COLORPIKER_X + HISTO_X + MARGIN
#define HEIGHT MARGIN + COLORPIKER_Y + MARGIN


#define LEFT_BUTTON 0
#define MIDDLE_BUTTON 1
#define RIGHT_BUTTON 2

#define MOUSE_DOWN 0
#define MOUSE_UP 1

#define BIAS 10

struct Key {
    int ID;
    int posx, posy;
    float color[4];
    bool active;
};

class TransferFunction {
public:
    TransferFunction();

    ~TransferFunction();

    void Render();

    bool MouseButton(int button, int state, int x, int y);

    bool MouseMove(int x, int y);

    void setHistogram(double *his) { histogram = his; }

    GLuint getTexture() const { return Lut; }

    GLuint getCDFTexture() const { return PLut; }

    GLuint getPreIntTexture2D() const { return PLut2D; }

    GLuint getPreIntTexture1D() const { return PLut1D; }

    //pre-integration
    void calculatePreIntegration1D();

    void calculatePreIntegration2D();

private:
    void _addkey(std::list<Key>::iterator it, Key key);

    void _setkeycolor(std::list<Key>::iterator it, float color[]);

    void _updatecolortable();

    bool _out_of_range(int x, int y) const;

    int _classify(int cur_mouse_pos_x, int cur_mouse_pos_y);

    void _generatecolorpalette();

    unsigned char colortable[TABLESIZE][4];
    float CDFtable[TABLESIZE][4];
    float PDFTable[TABLESIZE][TABLESIZE][4];

    float **colorpalette;
    double *histogram;
    GLuint Lut, PLut, PLut1D, PLut2D;
    std::list<Key> Keylist;

    //states
    int offsetx, offsety;
    std::list<Key>::iterator currentkey;
    int preIntegrationMode;

    //mouse
    bool dragenable;
};


#endif
