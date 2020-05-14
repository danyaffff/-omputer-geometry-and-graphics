#ifndef image_hpp
#define image_hpp
#include <cmath>
#include "enums.hpp"
#include "exception.hpp"

class Image {
    std::vector<std::vector<std::vector<unsigned char>>> image;
    
    int height;
    int width;
    
    bool inputCount;
    bool outputCount;
    
    int inputColorSpace;
    int outputColorSpace;
    
    int flag = 0;
    
public:
    Image(int, int, bool, bool, int, int);
    
    ~Image();
    
    void input(std::ifstream &);
    
    void convertToRGB();
    
    void convertFromRGB();
    
    void output(std::ofstream &);
    
    void output(std::ofstream &, std::ofstream &, std::ofstream &);
};

#endif
