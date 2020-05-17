#include "image.hpp"

Image::Image(int width, int height, bool inputCount, bool outputCount, int inputColorSpace, int outputColorSpace): width(std::move(width)), height(std::move(height)), inputCount(inputCount), outputCount(outputCount), inputColorSpace(std::move(inputColorSpace)), outputColorSpace(std::move(outputColorSpace)) {
    image.resize(this->height);
    for (int i = 0; i < this->height; i++) {
        image[i].resize(this->width);
    }
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            image[i][j].resize(3);
        }
    }
}

Image::~Image() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j].clear();
        }
    }
    for (int i = 0; i < height; i++) {
        image.clear();
    }
    image.clear();
}

void Image::input(std::ifstream & fin) {
    fin.get();
    if (inputCount == oneFile) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                for (int k = 0; k < 3; k++) {
                    image[i][j][k] = fin.get();
                }
            }
        }
    } else {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j][flag] = fin.get();
            }
        }
        
        flag++;
    }
}

void Image::convertToRGB() {
    if (inputColorSpace == RGB || inputColorSpace == outputColorSpace) {
        return;
    }
    
    double red;
    double green;
    double blue;
    
    double Y;
    double Cb;
    double Cr;
    
    switch (inputColorSpace) {
        case HSL:
        case HSV: {
            double hue;
            double saturation;
            double lightness;
            
            double C;
            double X;
            double m;
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    hue = (image[i][j][0] / 255.0) * 360.0;
                    saturation = image[i][j][1] / 255.0;
                    lightness = image[i][j][2] / 255.0;
                    
                    if (inputColorSpace == HSL) {
                        C = (1 - abs(2 * lightness - 1)) * saturation;
                        X = C * (1 - abs(fmod(hue / 60, 2) - 1));
                        m = lightness - C / 2;
                    } else {
                        C = saturation * lightness;
                        X = C * (1 - abs(fmod(hue / 60, 2) - 1));
                        m = lightness - C;
                    }
                    
                    m *= 255;
                    
                    if (hue >= 0 && hue < 60) {
                        image[i][j][0] = 255 * C + m;
                        image[i][j][1] = 255 * X + m;
                        image[i][j][2] = m;
                    } else if (hue >= 60 && hue < 120) {
                        image[i][j][0] = X * 255 + m;
                        image[i][j][1] = C * 255 + m;
                        image[i][j][2] = m;
                    } else if (hue >= 120 && hue < 180) {
                        image[i][j][0] = m;
                        image[i][j][1] = C * 255 + m;
                        image[i][j][2] = X * 255 + m;
                    } else if (hue >= 180 && hue < 240) {
                        image[i][j][0] = m;
                        image[i][j][1] = X * 255 + m;
                        image[i][j][2] = C * 255 + m;
                    } else if (hue >= 240 && hue < 300) {
                        image[i][j][0] = X * 255 + m;
                        image[i][j][1] = m;
                        image[i][j][2] = C * 255 + m;
                    } else if (hue >= 300 && hue <= 360) {
                        image[i][j][0] = C * 255.0 + m;
                        image[i][j][1] = m;
                        image[i][j][2] = X * 255 + m;
                    }
                }
            }
            break;
        }
            
        case YCbCr601:
        case YCbCr709: {
            double Kr;
            double Kg;
            double Kb;
            
            if (inputColorSpace == YCbCr601) {
                Kr = 0.299;
                Kg = 0.587;
                Kb = 0.114;
            } else {
                Kr = 0.0722;
                Kg = 0.2126;
                Kb = 0.7152;
            }
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    Y = image[i][j][0] / 255.0;
                    Cb = image[i][j][1] / 255.0 - 0.5;
                    Cr = image[i][j][2] / 255.0 - 0.5;
                    
                    red = (Y + Cr * (2 - 2 * Kr));
                    green = (Y - (Kb / Kg) * (2 - 2 * Kb) * Cb - (Kr / Kg) * (2 - 2 * Kr) * Cr);
                    blue = (Y + (2 - 2 * Kb) * Cb);
                    
                    if (red < 0) {
                        red = 0;
                    } else if (red > 1) {
                        red = 1;
                    }
                    if (green < 0) {
                        green = 0;
                    } else if (green > 1) {
                        green = 1;
                    }
                    if (blue < 0) {
                        blue = 0;
                    } else if (blue > 1) {
                        blue = 1;
                    }
                    
                    image[i][j][0] = 255 * red;
                    image[i][j][1] = 255 * green;
                    image[i][j][2] = 255 * blue;
                }
            }
            break;
        }
            
        case YCoCg: {
            double Co;
            double Cg;
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    Y = image[i][j][0] / 255.0;
                    Co = image[i][j][1] / 255.0 - 0.5;
                    Cg = image[i][j][2] / 255.0 - 0.5;
                    
                    red = Y + Co - Cg;
                    green = Y + Cg;
                    blue = Y - Co - Cg;
                    
                    if (red < 0) {
                        red = 0;
                    } else if (red > 1) {
                        red = 1;
                    }
                    if (green < 0) {
                        green = 0;
                    } else if (green > 1) {
                        green = 1;
                    }
                    if (blue < 0) {
                        blue = 0;
                    } else if (blue > 1) {
                        blue = 1;
                    }
                    
                    image[i][j][0] = 255 * red;
                    image[i][j][1] = 255 * green;
                    image[i][j][2] = 255 * blue;
                }
            }
            break;
        }
            
        case CMY: {
            double cyan;
            double magenta;
            double yellow;
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    cyan = image[i][j][0] / 255.0;
                    magenta = image[i][j][1] / 255.0;
                    yellow = image[i][j][2] / 255.0;
                    
                    image[i][j][0] = 255 * (1 - cyan);
                    image[i][j][1] = 255 * (1 - magenta);
                    image[i][j][2] = 255 * (1 - yellow);
                }
            }
            break;
        }
    }
}

void Image::convertFromRGB() {
    if (outputColorSpace == RGB && inputColorSpace == outputColorSpace) {
        return;
    }
    
    double red;
    double green;
    double blue;
    
    switch (outputColorSpace) {
        case HSL:
        case HSV: {
            double hue = 0;
            double saturation = 0;
            double lightness;
            double value;
            
            double C;
            
            double max;
            double min;
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    red = image[i][j][0] / 255.0;
                    green = image[i][j][1] / 255.0;
                    blue = image[i][j][2] / 255.0;
                        
                    max = std::max(red, std::max(green, blue));
                    min = std::min(red, std::min(green, blue));
                        
                    value = max;
                    C = max - min;
                    lightness = value - C / 2;
                        
                    if (C != 0) {
                        if (value == red) {
                            hue = 60 * ((green - blue) / C);
                        } else if (value == green) {
                            hue = 60 * (2 + (blue - red) / C);
                        } else if (value == blue) {
                            hue = 60 * (4 + (red - green) / C);
                        }
                    }
                    
                    if (outputColorSpace == HSV) {
                        if (value != 0) {
                            saturation = C / value;
                        }
                        
                        image[i][j][2] = value * 255;
                    } else if (outputColorSpace == HSL) {
                        if (lightness != 0 && lightness != 1) {
                            saturation = (value - lightness) / std::min(lightness, 1 - lightness);
                        }
                        
                        image[i][j][2] = 255 * lightness;
                    }
                        
                    image[i][j][0] = 255 * (hue / 360);
                    image[i][j][1] = 255 * saturation;
                        
                    }
                }
            break;
        }
            
        case YCbCr601:
        case YCbCr709: {
            double Y;
            double Cb;
            double Cr;
            
            double Kr;
            double Kg;
            double Kb;
            
            if (outputColorSpace == YCbCr601) {
                Kr = 0.299;
                Kg = 0.587;
                Kb = 0.114;
            } else {
                Kr = 0.0722;
                Kg = 0.2126;
                Kb = 0.7152;
            }
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    red = image[i][j][0] / 255.0;
                    green = image[i][j][1] / 255.0;
                    blue = image[i][j][2] / 255.0;
                    
                    Y = Kr * red + Kg * green + Kb * blue;
                    Cb = 0.5 * ((blue - Y) / (1 - Kb));
                    Cr = 0.5 * ((red - Y) / (1 - Kr));
                    
                    image[i][j][0] =  255 * Y;
                    image[i][j][1] =  255 * (Cb + 0.5);
                    image[i][j][2] = 255 * (Cr + 0.5);
                }
            }
            break;
        }
            
        case YCoCg: {
            double Y;
            double Co;
            double Cg;
            
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    red = image[i][j][0] / 255.0;
                    green = image[i][j][1] / 255.0;
                    blue = image[i][j][2] / 255.0;
                    
                    Y = red / 4 + green / 2 + blue / 4;
                    Co = red / 2 - blue / 2;
                    Cg = -red / 4 + green / 2 - blue / 4;
                    
                    image[i][j][0] = 255 * Y;
                    image[i][j][1] = 255 * (Co + 0.5);
                    image[i][j][2] = 255 * (Cg + 0.5);
                }
            }
            break;
        }
            
        case CMY: {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    red = image[i][j][0] / 255.0;
                    green= image[i][j][1] / 255.0;
                    blue = image[i][j][2] / 255.0;
                    
                    image[i][j][0] = 255 * (1 - red);
                    image[i][j][1] = 255 * (1 - green);
                    image[i][j][2] = 255 * (1 - blue);
                }
            }
            break;
        }
    }
}

void Image::output(std::ofstream & fout) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < 3; k++) {
                fout <<  image[i][j][k];
            }
        }
    }
}

void Image::output(std::ofstream & fout1, std::ofstream & fout2, std::ofstream & fout3) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fout1 << image[i][j][0];
            fout2 << image[i][j][1];
            fout3 << image[i][j][2];
        }
    }
}
