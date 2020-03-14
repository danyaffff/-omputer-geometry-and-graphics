#ifndef image_hpp
#define image_hpp
#include <fstream>
#include "exceptions.hpp"

class Image {
    int height;
    int width;
    int color;
    string format;
    const unsigned short int grayscale = 3;
    int *** image = new int ** [height];
    
    enum Operation {
        inversion,
        horizontalFlip,
        verticalFlip,
        clockwiseRotation,
        anticlockwisseRotation,
    };
    
public:
    Image(int, int, int, string);
    
    ~Image();
    
    void input(ifstream &);
    
    void operate(int);
    
    void print(ofstream &);
};

#endif
