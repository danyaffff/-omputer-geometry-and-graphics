#include "image.hpp"
using namespace std;

Image::Image(int height, int width, int color, string format): height(height), width(width), color(color), format(format) {
    for (int i = 0; i < this->height; i++) {
        image[i] = new int * [this->width];
    }
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            image[i][j] = new int [grayscale];
        }
    }
}

Image::~Image() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            delete [] image[i][j];
        }
    }
    for (int i = 0; i < height; i++) {
        delete [] image[i];
    }
    delete [] image;
}

void Image::input(ifstream &fin) {
    fin.get();
    cout << "Запись в массив..." << endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (fin.eof()) {
                throw Exception("Встречен неожиданный конец файла!");
            }
            if (format == "P5") {
                image[i][j][0] = fin.get();
            } else {
                for (int k = 0; k < grayscale; k++) {
                    image[i][j][k] = fin.get();
                     
                }
            }
        }
    }
}

void Image::operate(int operation) {
    switch (operation) {
        case inversion:
            cout << "Инверсия..." << endl;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (format == "P5") {
                        image[i][j][0] = color - image[i][j][0];
                    } else {
                        for (int k = 0; k < grayscale; k++) {
                            image[i][j][k] = color - image[i][j][k];
                        }
                    }
                }
            }
            break;
            
        case horizontalFlip:
            cout << "Отражение по горизонтали..." << endl;
            for (int i = 0; i < height / 2; i++) {
                for (int j = 0; j < width; j++) {
                    swap(image[i][j], image[height - 1 - i][j]);
                }
            }
            break;
            
        case verticalFlip:
            cout << "Отражение по вертикали..." << endl;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width / 2; j++) {
                    swap(image[i][j], image[i][width - 1 - j]);
                }
            }
            break;
            
        case clockwiseRotation:
            cout << "Вращение на 90º по часовой стрелке..." << endl;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < i; j++) {
                    swap(image[i][width - 1 - j], image[j][height - 1 - i]);
                }
            }
            for (int i = 0; i < height / 2; i++) {
                for (int j = 0; j < width; j++) {
                    swap(image[i][j], image[height - 1 - i][j]);
                }
            }
            break;
            
        case anticlockwisseRotation:
            cout << "Вращение на 90º против часовой стрелки..." << endl;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < i; j++) {
                    swap(image[i][j], image[j][i]);
                }
            }
            for (int i = 0; i < height / 2; i++) {
                for (int j = 0; j < width; j++) {
                    swap(image[i][j], image[height - 1 - i][j]);
                }
            }
            break;
            
        default:
            throw Exception("Что-то пошло не так!");
            break;
    }
}

void Image::print(ofstream &fout) {
    cout << "Создание картинки..." << endl;
    fout << format << '\n';
    fout << width << " " << height << " " << '\n';
    fout << color << '\n';
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (format == "P5") {
                fout << (char)image[i][j][0];
            } else {
                for (int k = 0; k < grayscale; k++) {
                    fout << (char)image[i][j][k];
                }
            }
        }
    }
    cout << "Готово!" << endl;
}
