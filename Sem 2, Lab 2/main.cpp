#include <regex>
#include "Wu.hpp"
#include "exceptions.hpp"

int main(int argc, char * argv[]) {
    string version;
    int width;
    int height;
    short color;
    
    ifstream fin;
    ofstream fout;
    fin.exceptions(ifstream::badbit | ifstream::failbit);
    fout.exceptions(ofstream::badbit | ofstream:: failbit);
    
    regex regular("[^\\.]+[^\\>]*\\.(pgm|ppm){1}$");
    cmatch checker1;
    cmatch checker2;
    
    try {
        if (argc < 9) {
            throw Exception("Введено недостаточно аргументов!");
        }
        if (argc > 10) {
            throw Exception("Введены лишние аргументы!");
        }
        
        if (regex_match(argv[1], checker1, regular) == false || regex_match(argv[2], checker2, regular) == false) {
            throw Exception("Введено некорректное имя файла!");
        }
        if (checker1[1].compare(checker2[1]) != 0) {
            throw Exception("Разрешения файлов не совпадают!");
        }
        
        fin.open(argv[1]);
        fout.open(argv[2]);
        
        fin >> version >> height >> width >> color;
        
        if (version != "P5" || height <= 0 || width <= 0 || color < 0 || color > 255) {
            throw Exception("Метаинформация неверна!");
        }
        
        fout << version << endl;
        
        Image image(height, width, color);
        
        image.setBrightness(atoi(argv[3]));
        image.setThickness(atoi(argv[4]));
        image.setCoordinates(atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atof(argv[7]));
        
        if (argv[9] != NULL) {
            image.setGamma(atoi(argv[9]));
        }
        
        image.input(fin);
        
        fin.close();
        
        image.drawWuLine();
        image.print(fout);
        
        fout.close();
    } catch (ifstream::failure) {
        cerr << "Произошла ошибка при открытии входного файла!" << endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        remove(argv[2]);
        return 1;
    } catch (ofstream::failure) {
        cerr << "Произошла ошибка при открытии выходного файла!" << endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        remove(argv[2]);
        return 1;
    } catch (const bad_alloc) {
        cerr << "Не удалось выделить память!" << endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        remove(argv[2]);
        return 1;
    } catch (Exception &exception) {
        cerr << exception.what() << endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        remove(argv[2]);
        return 1;
    } catch (exception &exception) {
        cerr << "Поймана непредвиденная ошибка: " << exception.what() << endl;
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
