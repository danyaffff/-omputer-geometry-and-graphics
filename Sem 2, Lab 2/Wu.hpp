#ifndef Wu_hpp
#define Wu_hpp
#include <iostream>
#include <fstream>
#include <cmath>
#include "point.hpp"
using namespace std;

class Image {
    unsigned int width;
    unsigned int height;
    unsigned short int color;
    
    short int brightness;
    double thickness;
    double gamma = 2.2;
    
    int **image = new int*[height];
    
    point start;
    point end;
    
public:
    Image(int, int, int);
    
    ~Image();
    
    void setBrightness(short int);
    
    void setThickness(double);
    
    void setCoordinates(double, double, double, double);
    
    void setGamma(double);
    
    void input(ifstream &);
    
    int doGammaCorrection(int);
    
    void putPoint(double, double, int);
    
    void putPoint(int, int, bool);
    
    void doAntiAliasing(bool, double, double, double);
    
    void drawLine(bool, double, double);
    
    void drawWuLine();
    
    void drawBresenhamLine();
    
    void print(ofstream &);
};

#endif 
