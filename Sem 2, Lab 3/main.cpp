#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "image.hpp"
#include "exception.hpp"

int main (int argc, char * argv[]) {
    std::regex regular("[^\\.]+[^\\>]*\\.(pgm|ppm){1}$");
    std::cmatch inputChecker;
    std::cmatch outputChecker;
    
    std::ifstream fin;
    std::ofstream fout;
    fin.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    fout.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    
    std::string version;
    int height;
    int width;
    int color;
    
    int colorDepth;
    
    bool isGradient = false;
    
    Dithering type;
    
    try {
        if (argc < 7) {
            throw Exception("Введено недостаточно аргументов!");
        } else if (argc > 7) {
            throw Exception("Введены лишние аргументы!");
        }
        
        if (regex_match(argv[1], inputChecker, regular) == false || regex_match(argv[2], outputChecker, regular) == false) {
            throw Exception("Введено некорректное имя файла!");
        }
        if (inputChecker[1].compare(outputChecker[1]) != 0) {
            throw Exception("Разрешения файлов не совпадают!");
        }
        
        fin.open(argv[1], std::ifstream::binary);
        fout.open(argv[2], std::ifstream::binary);
        
        fin >> version;
        fin >> width >> height >> color;
        fout << version << '\n';
        fout << width << ' ' << height << '\n';
        fout << color << '\n';
        
        if (version != "P5" || width <= 0 || height <= 0 || color <= 0 || color > 255) {
            throw Exception("Метаинформация неверна!");
        }
        
        Image image(width, height, color);
        
        if (atoi(argv[3]) == 1) {
            isGradient = true;
        } else if (atoi(argv[3]) != 0) {
            throw Exception("Неверное значение градиента!");
        }
        if (isGradient) {
            image.makeGradient();
        }
        (atoi(argv[4]) >= 0 && atoi(argv[4]) <= 7) ? type = static_cast<Dithering>(atoi(argv[4])) : throw Exception("Неверный номер алгоритма!");
        (atoi(argv[5]) >= 1 && atoi(argv[5]) <= 8) ? colorDepth = atoi(argv[5]) : throw Exception("Неверное значение глубины цвета!");
        atof(argv[6]) >= 0 ? image.setGamma(atof(argv[6])) : throw Exception("Неверное значение гаммы!");
        
        if (!isGradient) {
            image.input(fin);
        }
        image.makeDither(type, colorDepth);
        image.print(fout);
        
        fin.close();
        fout.close();
    } catch (std::bad_alloc) {
        std::cerr << "Не удалось выделить память!" << std::endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        remove(argv[2]);
        return 1;
    } catch (std::bad_cast) {
        std::cerr << "Не удалось преобразовать!" << std::endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        remove(argv[2]);
        return 1;
    } catch (std::ifstream::failure) {
        std::cerr << "Не удалось открыть входной файл!" << std::endl;
        return 1;
    } catch (std::ofstream::failure) {
        std::cerr << "Не удалось открыть выходной файл!" << std::endl;
        return 1;
    } catch (Exception & exception) {
        std::cerr << exception.what() << std::endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        remove(argv[2]);
        return 1;
    }
}
