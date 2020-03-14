#include <regex>
#include "image.hpp"

int main(int argc, char * argv[]) {
    regex regular("[^\\.]+[^\\>]*\\.(pgm|ppm){1}$");
    cmatch checker1;
    cmatch checker2;
    
    ifstream fin;
    ofstream fout;
    fin.exceptions(ifstream::badbit | ifstream::failbit);
    fout.exceptions(ofstream::badbit | ofstream::failbit);
    
    int operation;
    
    string format;
    int height;
    int width;
    int color;
    
    try {
        if (argc < 4) {
            throw Exception("Введено недостаточно аргуменнтов!");
        }
        
        if (argc > 4) {
            throw Exception("Введено слишком много аргментов!");
        }
        
        if (regex_match(argv[1], checker1, regular) == false || regex_match(argv[2], checker2, regular) == false) {
            throw Exception("Введено неверное имя файла!");
        }
        
        if (checker1[1].compare(checker2[1]) != 0) {
            throw Exception("Расширения файлов не совпадают!");
        }
        
        cout << "Открываем входной файл..." << endl;
        fin.open(argv[1], ifstream::binary);
        cout << "Входной файл открыт..." << endl;
        cout << "Открываем выходной файл..." << endl;
        fout.open(argv[2], ofstream::binary);
        cout << "Выходной файл открыт..." << endl;
        operation = atoi(argv[3]);
        if (operation < 0 || operation > 4) {
            throw Exception("Введен неверный код операции!");
        }
        
        fin >> format;
        fin >> width >> height >> color;
        
        Image image(height, width, color, format);
        
        image.input(fin);
        
        fin.close();
        
        image.operate(operation);
        image.print(fout);
        
        fout.close();
    } catch (ifstream::failure) {
        cerr << "Ошибка при открытии входного файла!" << endl;
        return 1;
    } catch (ofstream::failure) {
        cerr << "Ошибка при открытии выходного файла!" << endl;
        fin.close();
        return 1;
    } catch(bad_alloc) {
        cerr << "Не удалось выделить память!" << endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        return 1;
    } catch (Exception &exception) {
        cerr << exception.what() << endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        return 1;
    } catch (exception &exception) {
        cerr << "Поймана неизвестная ошибка: " << exception.what() << endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout.is_open()) {
            fout.close();
        }
        return 1;
    }
}
