#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "exception.hpp"
#include "image.hpp"
#include "matrices.hpp"

Image::Image(int width, int height, int color): width(width), height(height), color(color) {
    image.resize(height);
    error.resize(height);
    for (int i = 0; i < height; i++) {
        image[i].resize(width);
        error[i].resize(width);
    }
}

Image::~Image() {
    for (int i = 0; i < height; i++) {
        image[i].clear();
        error[i].clear();
    }
    image.clear();
    error.clear();
}

void Image::input(std::ifstream & fin) {
    fin.get();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            !fin.eof() ? image[i][j] = fin.get() : throw Exception("Встречен неожиданный конец файла!");
        }
    }
}

void Image::print(std::ofstream & fout) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fout << image[i][j];
        }
    }
}

void Image::setGamma (double gamma) {
    this->gamma = gamma;
}

void Image::makeGradient() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = gammaCorrection(static_cast<double>(i) / height * color);
        }
    }
}

double Image::gammaCorrection(double brightness) {
    brightness /= color;
    if (gamma == 0) {
        if (brightness <= 0.0031308) {
            return color * brightness * 12.92;
        } else {
            return color * (1.055 * pow(brightness, 1 / 2.4) - 0.055);
        }
    } else {
        return color * std::pow(brightness, gamma);
    }
}

double Image::reverseGammaCorrection(double brightness) {
    brightness /= color;
    if (gamma == 0) {
        if (brightness <= 0.04045) {
            return color * brightness / 12.92;
        } else {
            return color * pow((brightness + 0.055) / 1.055, 2.4);
        }
    } else {
        return color * std::pow(brightness, 1 / gamma);
    }
}

void Image::makeDither(Dithering type, int colorDepth) {
    int brightness = std::pow(2, colorDepth);
    switch (type) {
        case noDithering: {
            for (int i = 0 ; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = reverseGammaCorrection(static_cast<double>(image[i][j])) / color;
                    buffer *= brightness - 1;
                    buffer = std::round(buffer);
                    image[i][j] = std::round(gammaCorrection(buffer * (static_cast<double>(color) / (brightness - 1))));
                }
            }
            break;
        }
        case orderedDithering: {
            double shift = colorDepth == 4 ? 0.1 : 0;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = (reverseGammaCorrection(image[i][j]) + (static_cast<double>(color) / colorDepth) * (orderedMatrix[i % 8][j % 8] / 64 - (0.5 + colorDepth / 20.0 + shift))) / color;
                    if (buffer < 0) {
                        buffer = 0;
                    }
                    buffer *= brightness - 1;
                    buffer = std::round(buffer);
                    image[i][j] = std::round(gammaCorrection(buffer * (static_cast<double>(color) / (brightness - 1))));
                }
            }
            break;
        }
        case randomDithering: {
            double shift = 0;
            switch (colorDepth) {
                case 1:
                    shift = 0.5;
                    break;
                    
                case 2:
                    shift = 0.65;
                    break;
                    
                case 3:
                    shift = 0.8;
                    break;
                    
                case 4:
                    shift = 0.85;
                    break;
                    
                case 5:
                case 6:
                    shift = 0.9;
                    break;
                    
                case 7:
                    shift = 0.95;
                    break;
                    
                case 8:
                    shift = 1;
                    break;
            }
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = (reverseGammaCorrection(image[i][j]) + (static_cast<double>(color) / colorDepth) * (static_cast<double>(std::rand()) / RAND_MAX - shift)) / color;
                    if (buffer < 0) {
                        buffer = 0;
                    }
                    buffer *= brightness - 1;
                    buffer = std::round(buffer);
                    if (std::round(static_cast<int>(gammaCorrection(buffer * (static_cast<double>(color) / (brightness - 1))))) > color) {
                        std::cout << std::round(gammaCorrection(buffer * (static_cast<double>(color) / (brightness - 1)))) << std::endl;
                    }
                    image[i][j] = std::round(gammaCorrection(buffer * (static_cast<double>(color) / (brightness - 1))));
                }
            }
            break;
        }
        case fsDithering: {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = static_cast<double>(reverseGammaCorrection(image[i][j]) + error[i][j]) / color;
                    buffer *= brightness - 1;
                    buffer = std::round(buffer);
                    buffer *= static_cast<double>(color) / (brightness - 1);
                    int currentError = image[i][j] + error[i][j] - static_cast<int>(buffer);
                    image[i][j] = static_cast<int>(buffer);
                    if (static_cast<int>(buffer) > 255) {
                        image[i][j] = color;
                    }
                    if (j + 1 < width) {
                        error[i][j + 1] += currentError * (7.0 / 16);
                    }
                    if (j - 1 >= 0 && i + 1 < height) {
                        error[i + 1][j - 1] += currentError * (3.0 / 16);
                    }
                    if (i + 1 < height) {
                        error[i + 1][j] += currentError * (5.0 / 16);
                    }
                    if (i + 1 < height && j + 1 < width) {
                        error[i + 1][j + 1] += currentError * (1.0 / 16);
                    }
                }
            }
            break;
        }
        case jjnDithering: {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = static_cast<double>(reverseGammaCorrection(image[i][j]) + error[i][j]) / color;
                    buffer *= brightness - 1;
                    buffer = std::round(buffer);
                    buffer *= static_cast<double>(color) / (brightness - 1);
                    int currentError = image[i][j] + error[i][j] - static_cast<int>(buffer);
                    image[i][j] = static_cast<int>(buffer);
                    if (static_cast<int>(buffer) > 255) {
                        image[i][j] = color;
                    }
                    for (int mi = 0; mi <= 2; mi++) {
                        for (int mj = -2; mj <= 2; mj++) {
                            if (!(mi == 0 && mj <= 0) && j + mj < width && j + mj >= 0 && i + mi < height) {
                                error[i + mi][j + mj] += currentError * jjnMatrix[mi][2 + mj] / 48;
                            }
                        }
                    }
                }
            }
            break;
        }
        case sierra: {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = static_cast<double>(reverseGammaCorrection(image[i][j]) + error[i][j]) / color;
                    buffer *= brightness - 1;
                    buffer = std::round(buffer);
                    buffer *= static_cast<double>(color) / (brightness - 1);
                    int currentError = image[i][j] + error[i][j] - static_cast<int>(buffer);
                    image[i][j] = static_cast<int>(buffer);
                    if (static_cast<int>(buffer) > 255) {
                        image[i][j] = color;
                    }
                    for (int mi = 0; mi <= 2; mi++) {
                        for (int mj = -2; mj <= 2; mj++) {
                            if (!(mi == 0 && mj <= 0) && j + mj < width && j + mj >= 0 && i + mi < height && i + mi < height) {
                                error[i + mi][j + mj] += currentError * sierraMatrix[mi][2 + mj] / 32;
                            }
                        }
                    }
                }
            }
            break;
        }
        case atkinson: {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = static_cast<double>(reverseGammaCorrection(image[i][j]) + error[i][j]) / color;
                    buffer *= brightness - 1;
                    buffer = std::round(buffer);
                    buffer *= static_cast<double>(color) / (brightness - 1);
                    int currentErrorValue = image[i][j] + error[i][j] - static_cast<int>(buffer);
                    image[i][j] = static_cast<int>(buffer);
                    if (static_cast<int>(buffer) > 255) {
                        image[i][j] = color;
                    }
                    for (int mi = 0; mi <= 2; mi++) {
                        for (int mj = -2; mj <= 2; mj++) {
                            if (!(mi == 0 && mj <= 0) && j + mj < width && j + mj >= 0 && i + mi < height) {
                                error[i + mi][j + mj] += currentErrorValue * atkinsonMatrix[mi][2 + mj] / 8.0;
                            }
                        }
                    }
                }
            }
            break;
        }
        case halftone: {
            double shift = 0;
            switch (colorDepth) {
                case 1:
                    shift = 0.5;
                    break;
                    
                case 2:
                    shift = 0.6;
                    break;
                
                case 3:
                    shift = 0.65;
                    break;
                    
                case 4:
                    shift = 0.7;
                    break;
                    
                case 5:
                    shift = 0.75;
                    break;
                    
                case 6:
                case 7:
                    shift = 0.8;
                    break;
                    
                case 8:
                    shift = 0.85;
                    break;
            }
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    double buffer = static_cast<double>(reverseGammaCorrection(image[i][j]) + (static_cast<double>(color) / colorDepth) * (halftoneMatrix[i % 4][j % 4] / 16 - shift)) / color;
                    if (buffer < 0) {
                        buffer = 0;
                    }
                    buffer *= brightness - 1;
                    buffer = std::round (buffer);
                    image[i][j] = static_cast<unsigned char>(std::round(gammaCorrection(buffer * (static_cast<double>(color) / (brightness - 1)))));
                }
            }
            break;
        }
    }
}
