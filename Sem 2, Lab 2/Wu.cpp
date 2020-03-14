#include "Wu.hpp"

Image::Image(int height, int width, int color): height(height), width(width), color(color) {
    for (int i = 0; i < this->height; i++) {
        image[i] = new int[this->width];
    }
}

Image::~Image() {
    for (int i = 0; i < height; i++) {
        delete [] image[i];
    }
    delete [] image;
}

void Image::setBrightness(short int brightness) {
    if (brightness < 0 || brightness > color) {
        throw Exception("Задано неверное значение яркости!");
    } else {
        this->brightness = brightness;
    }
}

void Image::setThickness(double thickness) {
    if (thickness < 0) {
        throw Exception("Задано неверное значние толщины!");
    } else {
        this->thickness = thickness;
    }
}

void Image::setCoordinates(double x0, double y0, double x1, double y1) {
    if ((x0 < 0) && (y0 < 0) && (x1 < 0) && (y1 < 0)) {
        throw Exception("Введены неверные координаты!");
    } else {
        start.x = x0;
        start.y = y0;
        end.x = x1;
        end.y = y1;
    }
}

void Image::setGamma(double gamma) {
    if (gamma <= 0) {
        throw Exception("Введено неверное значнеие гаммы!");
    } else {
        this->gamma = gamma;
    }
}

int Image::doGammaDecorrection(int pixel) {
    int decoded = round((double)this->color * pow((double)pixel / (double)this->color, 1 / (double)this->gamma));
    return decoded;
}

int Image::doGammaCorrection(int pixel) {
    int encoded = round((double)this->color * pow((double)pixel / (double)this->color, (double)this->gamma));
    return encoded;
}

void Image::input(ifstream &fin) {
    fin.get();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (fin.eof()) {
                throw Exception("Встречен неожиданный конец файла!");
            }
            image[i][j] = doGammaDecorrection(fin.get());
        }
    }
}

void Image::putPoint(double x, double y, int brightness) {
    if ((int)y < 0 || (int)y >= height) {
        return;
    }
    if (x >= 0 && y >= 0 && x < width && y < height) {
        image[(int)y][(int)x] = brightness;
    }
}

void Image::putPoint(int x, int y, bool swapped) {
    for (int i = 0; i < thickness - 2; i++) {
        if (swapped) {
            if (x + i >= 0 && y >= 0 && x + i < width && y < height) {
                image[y][x + i] = this->brightness;
            }
        } else {
            if (x >= 0 && y + i >= 0 && x < width && y + i < height) {
                image[y + i][x] = this->brightness;
            }
        }
    }
}

void Image::doAntiAliasing(bool swapped, double x, double y, double brightness) {
    int b = round(this->brightness * brightness + (1 - brightness) * (double)image[(int)y][(int)x]);
    putPoint(swapped ? y : x, swapped ? x : y, b);
}

void Image::drawLine(bool swapped, double x, double y) {
    x = floor(x);
    y = floor(y);
    putPoint(swapped ? (int)y : (int)x, swapped ? (int)x : (int)y, swapped);
}

void Image::drawWuLine() {
    bool swapped = abs(end.y - start.y) > abs(end.x - start.x);
    
    if (swapped) {
        swap(start.x, start.y);
        swap(end.x, end.y);
    }
    
    if (start.x > end.x) {
        swap(start.x, end.x);
        swap(start.y, end.y);
    }
    
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double gradient = dy / dx;
    double x = start.x;
    double y = start.y + gradient;
    
    while (x <= end.x) {
        double brightness = y - (int)y;
        if (y < 0 || x < 0 || y >= height || x >= width) {
            x++;
            y += gradient;
            continue;
        }
        
        drawLine(swapped, x, y - round(thickness / 2) + 1);
        doAntiAliasing(swapped, x, y - round(thickness / 2), 1 - brightness);
        doAntiAliasing(swapped, x, y + round(thickness / 2) - 1, brightness);
        
        x++;
        y += gradient;
    }
}

void Image::print(ofstream &fout) {
    fout << width << " " << height << endl;
    fout << color << endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fout << (char)doGammaCorrection(image[i][j]);
        }
    }
}
