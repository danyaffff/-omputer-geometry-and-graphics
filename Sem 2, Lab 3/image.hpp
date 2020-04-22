#ifndef image_hpp
#define image_hpp
#include <fstream>

enum Dithering {
    noDithering,
    orderedDithering,
    randomDithering,
    fsDithering,
    jjnDithering,
    sierra,
    atkinson,
    halftone,
};

class Image {
    std::vector<std::vector<unsigned char>> image;
    int width;
    int height;
    int color;
    
    std::vector<std::vector<int>> error;
    
    double gamma;
    
    double gammaCorrection(double);
    
    double reverseGammaCorrection(double);
    
public:
    Image(int, int, int);
    
    ~Image();
    
    void input(std::ifstream &);
    
    void print(std::ofstream &);
    
    void setGamma(double);
    
    void makeGradient();
    
    void makeDither(Dithering, int);
};

#endif
