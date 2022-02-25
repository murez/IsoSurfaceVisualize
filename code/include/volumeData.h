#ifndef _VOLUME3D_H
#define _VOLUME3D_H

#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <iostream>
#include <cassert>
#include <sstream>
#include <omp.h>
#include <limits>


using namespace std;


template<typename T>
class volumeData {
public:
    volumeData(T *data, int z, int y, int x);

    ~volumeData();

    int getDimX() { return _dimX; }

    int getDimY() { return _dimY; }

    int getDimZ() { return _dimZ; }

    T *getDataBuffer() { return _data; }

    //status
    void updateMaxMin();

    T getMin() { return _min; }

    T getMax() { return _max; }


private:
    std::string _filename;
    T *_data;
    int _dimX, _dimY, _dimZ;
    T _min, _max;
    FILE *pFile{};
};

template<typename T>
volumeData<T>::volumeData(T *data, int z, int y, int x)
        :_dimX(x), _dimY(y), _dimZ(z) {
    _data = data;
    this->updateMaxMin();
}

template<typename T>
volumeData<T>::~volumeData() {
    if (_data)
        free(_data);
}

template<typename T>
void volumeData<T>::updateMaxMin() {
    _min = std::numeric_limits<T>::max();
    _max = std::numeric_limits<T>::min();
#pragma omp parallel for collapse(2)
    for (int k = 0; k < _dimZ; k++) {
        for (int j = 0; j < _dimY; j++)
            for (int i = 0; i < _dimX; i++) {
                T value = _data[_dimX * _dimY * (k) + _dimX * j + i];
                if (value < _min)
#pragma omp critical
                    _min = value;
                if (value > _max)
#pragma omp critical
                    _max = value;
            }
    }

}

#endif
