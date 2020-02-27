#include <iostream>
#include <fstream>
#include <regex>
using namespace std;

int main(int argc, char * argv[]) {
    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL) {
        cerr << "Введены не все аргументы!" << endl;
        return 1;
    }
    if (argv[4] != NULL) {
        cerr << "Введено слишком много аргументов!" << endl;
        return 1;
    }
    regex regular("[^\\.]+[^\\>]*\\.(pgm|ppm){1}$");
    cmatch checker1;
    cmatch checker2;
    if (regex_match(argv[1], checker1, regular) == false || regex_match(argv[2], checker2, regular) == false) {
        cerr << "Введено некорректное имя файла!" << endl;
        return 1;
    }
    if (checker1[1].compare(checker2[1]) != 0) {
        cerr << "Разрешения файлов не совпадают!" << endl;
        return 1;
    }
    string output = argv[2];
    ifstream fin;
    ofstream fout;
    int operation = -1;
    fin.exceptions(ifstream::badbit | ifstream::failbit);
    fout.exceptions(ofstream::badbit | ofstream::failbit);
    try {
        cout << "Открываем входной файл..." << endl;
        fin.open(argv[1]);
        cout << "Входной файл открыт..." << endl;
        cout << "Открытие выходного файла..." << endl;
        fout.open(argv[2]);
        cout << "Выходной файл открыт..." << endl;
        operation = atoi(argv[3]);
        if (operation < 0 || operation > 4) {
            throw 1;
        }
    } catch (const ifstream::failure) {
        cerr << "Ошибка открытия входного файла!" << endl;
        return 1;
    } catch (const ofstream::failure) {
        cerr << "Ошибка открытия выходного файла!" << endl;
        fin.close();
        return 1;
    } catch (const int) {
        cerr << "Неправильная операция!" << endl;
        fin.close();
        fout.close();
        remove(argv[2]);
        return 1;
    } catch (...) {
        cerr << "Поймана неизвестная ошибка!" << endl;
        fin.close();
        fout.close();
        remove(argv[2]);
        return 1;
    }
    string version;
    fin >> version;
    int col;
    int row;
    int color;
    fin >> col >> row >> color;
    if (version == "P5") {
        int **image;
        try {
            image = new int*[row];
            for (int i = 0; i < row; i++) {
                image[i] = new int[col];
            }
        } catch (const bad_alloc) {
            cerr << "Не удалось выделить память!" << endl;
            fin.close();
            fout.close();
            remove(argv[2]);
            for (int i = 0; i < col; i++) {
                delete []image[i];
            }
            delete []image;
            return 1;
        } catch (...) {
            cerr << "Поймана неизвестная ошибка!" << endl;
            fin.close();
            fout.close();
            remove(argv[2]);
            for (int i = 0; i < col; i++) {
                delete []image[i];
            }
            delete []image;
            return 1;
        }
        char symbol;
        fin.get(symbol);
        cout << "Запись в массив..." << endl;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (fin.eof()) {
                    cerr << "Встречен неожиданный конец файла!" << endl;
                    fin.close();
                    fout.close();
                    remove(argv[2]);
                    for (int i = 0; i < col; i++) {
                        delete []image[i];
                    }
                    delete []image;
                    return 1;
                }
                fin.get(symbol);
                image[i][j] = symbol;
            }
        }
        fin.close();
        switch (operation) {
            case 0:
                cout << "Инверсия..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col; j++) {
                        image[i][j] = color - image[i][j];
                    }
                }
                break;
            case 1:
                cout << "Отражение по горизонтали..." << endl;
                for (int i = 0; i < row / 2; i++) {
                    for (int j = 0; j < col; j++) {
                        swap(image[i][j], image[row - 1 - i][j]);
                    }
                }
                break;
            case 2:
                cout << "Отражение по вертикали..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col / 2; j++) {
                        swap(image[i][j], image[i][col - 1 - j]);
                    }
                }
                break;
            case 3:
                cout << "Вращение на 90º по часовой стрелке..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < i; j++) {
                        swap(image[i][col - 1 - j], image[j][row - 1 - i]);
                    }
                }
                for (int i = 0; i < row / 2; i++) {
                    for (int j = 0; j < col; j++) {
                        swap(image[i][j], image[row - 1 - i][j]);
                    }
                }
                break;
            case 4:
                cout << "Вращение на 90º против часовой стрелки..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < i; j++) {
                        swap(image[i][j], image[j][i]);
                    }
                }
                for (int i = 0; i < row / 2; i++) {
                    for (int j = 0; j < col; j++) {
                        swap(image[i][j], image[row - 1 - i][j]);
                    }
                }
                break;
                
        }
        cout << "Создание картинки..." << endl;
        fout << version << endl << col << " " << row << " " << endl << color << endl;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                symbol = image[i][j];
                fout << symbol;
            }
        }
        cout << "Готово!" << endl;
        fout.close();
        for (int i = 0; i < col; i++) {
            delete []image[i];
        }
        delete []image;
    } else if (version == "P6") {
        int colors = 3;
        int ***image;
        try {
            image = new int**[row];
            for (int i = 0; i < row; i++) {
                image[i] = new int*[col];
            }
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    image[i][j] = new int[colors];
                }
            }
        } catch (const bad_alloc) {
            cerr << "Не удалось выделить память!" << endl;
            fin.close();
            fout.close();
            remove(argv[2]);
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    delete []image[i][j];
                }
            }
            for (int i = 0; i < row; i++) {
                delete []image[i];
            }
            delete []image;
            return 1;
        } catch (...) {
            cerr << "Поймана неизвестная ошибка!" << endl;
            fin.close();
            fout.close();
            remove(argv[2]);
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    delete []image[i][j];
                }
            }
            for (int i = 0; i < row; i++) {
                delete []image[i];
            }
            delete []image;
            return 1;
        }
        char symbol;
        fin.get(symbol);
        cout << "Запись в массив..." << endl;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                for (int k = 0; k < colors; k++) {
                    if (fin.eof()) {
                        cerr << "Встречен неожиданный конец файла!" << endl;
                        fin.close();
                        fout.close();
                        remove(argv[2]);
                        for (int i = 0; i < row; i++) {
                            for (int j = 0; j < col; j++) {
                                delete []image[i][j];
                            }
                        }
                        for (int i = 0; i < row; i++) {
                            delete []image[i];
                        }
                        delete []image;
                        return 1;
                    }
                    fin.get(symbol);
                    image[i][j][k] = symbol;
                }
            }
        }
        fin.close();
        switch (operation) {
            case 0:
                cout << "Инверсия..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col; j++) {
                        for (int k = 0; k < colors; k++) {
                            image[i][j][k] = color - image[i][j][k];
                        }
                    }
                }
                break;
            case 1:
                cout << "Отражение по горизонтали..." << endl;
                for (int i = 0; i < row / 2; i++) {
                    for (int j = 0; j < col; j++) {
                        swap(image[i][j], image[row - 1 - i][j]);
                    }
                }
                break;
            case 2:
                cout << "Отражение по вертикали..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col / 2; j++) {
                        swap(image[i][j], image[i][col - 1 - j]);
                    }
                }
                break;
            case 3:
                cout << "Вращение на 90º по часовой стрелке..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < i; j++) {
                        swap(image[i][col - 1 - j], image[j][row - 1 - i]);
                    }
                }
                for (int i = 0; i < row / 2; i++) {
                    for (int j = 0; j < col; j++) {
                        swap(image[i][j], image[row - 1 - i][j]);
                    }
                }
                break;
            case 4:
                cout << "Вращение на 90º против часовой стрелки..." << endl;
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < i; j++) {
                        swap(image[i][j], image[j][i]);
                    }
                }
                for (int i = 0; i < row / 2; i++) {
                    for (int j = 0; j < col; j++) {
                        swap(image[i][j], image[row - 1 - i][j]);
                    }
                }
                break;

        }
        cout << "Создание картинки..." << endl;
        fout << version << endl << col << " " << row << endl << color << endl;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                for (int k = 0; k < colors; k++) {
                    symbol = image[i][j][k];
                    fout << symbol;
                }
            }
        }
        fout.close();
        cout << "Готово!" << endl;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                delete []image[i][j];
            }
        }
        for (int i = 0; i < row; i++) {
            delete []image[i];
        }
        delete []image;
    } else {
        cerr << "Что-то не так с файлом!" << endl;
        fin.close();
        fout.close();
        remove(argv[2]);
        return 1;
    }
}
