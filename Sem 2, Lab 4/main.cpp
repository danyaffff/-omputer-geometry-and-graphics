#include <regex>
#include "image.hpp"

typedef std::vector<std::pair<std::pair<std::string, int>, std::pair<int, int>>> metainformation_type;

void input(std::ifstream & fin, metainformation_type & metainformation, int numberOfFile) {
    fin >> metainformation[numberOfFile].first.first >> metainformation[numberOfFile].first.second >> metainformation[numberOfFile].second.first >> metainformation[numberOfFile].second.second;
}

bool checkMetainformation(metainformation_type & metainformation, int numberOfFile, bool count) {
    return metainformation[numberOfFile].first.first != (count == oneFile ? "P6" : "P5") || metainformation[numberOfFile].first.second < 0 || metainformation[numberOfFile].second.first < 0 || metainformation[numberOfFile].second.second < 0;
}

bool checkCoincidence(metainformation_type & metainformation, int numberOfFile) {
    return metainformation[0].first.first != metainformation[numberOfFile].first.first || metainformation[0].first.second != metainformation[numberOfFile].first.second || metainformation[0].second.first != metainformation[numberOfFile].second.first || metainformation[0].second.second != metainformation[1].second.second;
}

void output(std::ofstream & fout, metainformation_type & metainformation, bool outputCount) {
    outputCount == oneFile ? fout << "P6" << '\n' : fout << "P5" << '\n';
    fout << metainformation[0].first.second << " " << metainformation[0].second.first << '\n';
    fout << metainformation[0].second.second << '\n';
}

int main(int argc, char * argv[]) {
    std::regex regular("[^\\.]+[^\\>]*\\.(pgm|ppm){1}$");
    std::cmatch inputFile;
    std::cmatch outputFile;
    
    int inputColorSpace = 0;
    int outputColorSpace = 0;
    
    std::ifstream fin;
    fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::vector<std::string> inputFiles;
    metainformation_type inputFilesMetainformation;
    
    std::ofstream fout1;
    std::ofstream fout2;
    std::ofstream fout3;
    fout1.exceptions(std::ofstream::badbit | std::ofstream::badbit);
    fout2.exceptions(std::ofstream::badbit | std::ofstream::badbit);
    fout3.exceptions(std::ofstream::badbit | std::ofstream::badbit);
    std::vector<std::string> outputFiles;
    
    bool inputCount = oneFile;
    bool outputCount = oneFile;
    
    bool fflag = false;
    bool tflag = false;
    bool iflag = false;
    bool oflag = false;
    
    try {
        if (argc < 11) {
            throw Exception("Введено недостаточно аргументов!");
        } else if (argc > 11) {
            throw Exception("Введено слишком много аргументов!");
        }
        
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-f") == 0) {
                if (strcmp(argv[i + 1], "RGB") == 0) {
                    inputColorSpace = RGB;
                } else if (strcmp(argv[i + 1], "HSL") == 0) {
                    inputColorSpace = HSL;
                } else if (strcmp(argv[i + 1], "HSV") == 0) {
                    inputColorSpace = HSV;
                } else if (strcmp(argv[i + 1], "YCbCr.601") == 0 || strcmp(argv[i + 1], "YUV.601") == 0) {
                    inputColorSpace = YCbCr601;
                } else if (strcmp(argv[i + 1], "YCbCr.709") == 0 || strcmp(argv[i + 1], "YUV.709") == 0) {
                    inputColorSpace = YCbCr709;
                } else if (strcmp(argv[i + 1], "YCoCg") == 0) {
                    inputColorSpace = YCoCg;
                } else if (strcmp(argv[i + 1], "CMY") == 0) {
                    inputColorSpace = CMY;
                } else {
                    throw Exception("Встречено неизвестное цветовое пространство входного файла!");
                }
                
                fflag = true;
                continue;
            }
            
            if (strcmp(argv[i], "-t") == 0) {
                if (strcmp(argv[i + 1], "RGB") == 0) {
                    outputColorSpace = RGB;
                } else if (strcmp(argv[i + 1], "HSL") == 0) {
                    outputColorSpace = HSL;
                } else if (strcmp(argv[i + 1], "HSV") == 0) {
                    outputColorSpace = HSV;
                } else if (strcmp(argv[i + 1], "YCbCr.601") == 0) {
                    outputColorSpace = YCbCr601;
                } else if (strcmp(argv[i + 1], "YCbCr.709") == 0) {
                    outputColorSpace = YCbCr709;
                } else if (strcmp(argv[i + 1], "YCoCg") == 0) {
                    outputColorSpace = YCoCg;
                } else if (strcmp(argv[i + 1], "CMY") == 0) {
                    outputColorSpace = CMY;
                } else {
                    throw Exception("Встречено неизвестное цветовое пространство входного файла!");
                }
                
                tflag = true;
                continue;
            }
            
            if (strcmp(argv[i], "-i") == 0) {
                if (atoi(argv[i + 1]) == 1) {
                    inputCount = oneFile;
                    
                    inputFiles.push_back(argv[i + 2]);
                    inputFiles.shrink_to_fit();
                    
                    inputFilesMetainformation.resize(1);
                } else if (atoi(argv[i + 1]) == 3) {
                    inputCount = threeFiles;
                    
                    for (int j = 1; j <= 3; j++) {
                        std::string inputFileName = argv[i + 2];
                        inputFileName.insert(inputFileName.find("."), "_");
                        inputFileName.insert(inputFileName.find("."), std::to_string(j));
                        inputFiles.push_back(inputFileName);
                    }
                    inputFiles.shrink_to_fit();
                    
                    inputFilesMetainformation.resize(3);
                } else {
                    throw Exception("Введено неверное количество входных файлов!");
                }
                
                iflag = true;
                continue;
            }
            
            if (strcmp(argv[i], "-o") == 0) {
                if (atoi(argv[i + 1]) == 1) {
                    outputCount = oneFile;
                    
                    outputFiles.push_back(argv[i + 2]);
                    outputFiles.shrink_to_fit();
                } else if (atoi(argv[i + 1]) == 3) {
                    outputCount = threeFiles;
                    
                    outputFiles.push_back(argv[i + 2]);
                    outputFiles.push_back(argv[i + 2]);
                    outputFiles.push_back(argv[i + 2]);
                    outputFiles.shrink_to_fit();
                } else {
                    throw Exception("Введено неверное количество выходных файлов");
                }
                
                oflag = true;
                continue;
            }
        }
        
        if (!fflag) {
            throw Exception("Не введена команда для инициализации цветового пространства входного файла!");
        } else if (!tflag) {
            throw Exception("Не введена команда для инициализации цветового пространства выходного файла!");
        } else if (!iflag) {
            throw Exception("Не введена команда для инициализации количества и названия входных файлов!");
        } else if (!tflag) {
            throw Exception("Не введена команда для инициализации количества и названия выходных файлов!");
        }
        
        if (regex_match(inputFiles[0].c_str(), inputFile, regular) == false) {
            throw Exception("Введено некорректное имя входного файла!");
        } else if (regex_match(outputFiles[0].c_str(), outputFile, regular) == false) {
            throw Exception("Введено некорректное имя выходного файла!");
        } else if (inputCount == oneFile ? inputFile[1] != "ppm" : inputFile[1] != "pgm") {
            throw Exception("Введено неверное расширение входного файла!");
        } else if (outputCount == oneFile ? outputFile[1] != "ppm" : outputFile[1] != "pgm") {
            throw Exception("Введено неверное расширение выходного файла!");
        }
        
        if (outputCount == threeFiles) {
            for (int i = 2; i >= 0; i--) {
                outputFiles[i].insert(outputFiles[i].find("."), "_");
                outputFiles[i] = outputFiles[i].insert(outputFiles[i].find("."), std::to_string(i + 1));
            }
        }
        
        fin.open(inputFiles[0]);
        input(fin, inputFilesMetainformation, 0);
        
        if (checkMetainformation(inputFilesMetainformation, 0, inputCount)) {
            throw inputCount == oneFile ? Exception("Ошибка метаинформации входящего файла!") : Exception("Ошибка метаинформации первого входящего файла!");
        }
        
        Image image(inputFilesMetainformation[0].first.second, inputFilesMetainformation[0].second.first, inputCount, outputCount, inputColorSpace, outputColorSpace);
        
        image.input(fin);
        
        fin.close();
        
        if (inputCount == threeFiles) {
            fin.open(inputFiles[1]);
            input(fin, inputFilesMetainformation, 1);
            
            if (checkMetainformation(inputFilesMetainformation, 1, inputCount)) {
                throw Exception("Ошибка метаинформации второго входящего файла!");
            }
            
            if (checkCoincidence(inputFilesMetainformation, 1)) {
                throw Exception("Метаинформация первого и второго файлов не совпадает!");
            }
            
            image.input(fin);
            
            fin.close();
            
            fin.open(inputFiles[2]);
            input(fin, inputFilesMetainformation, 2);
            
            if (checkMetainformation(inputFilesMetainformation, 2, inputCount)) {
                throw Exception("Ошибка метаинформации третьего входящего файла!");
            }
            
            image.input(fin);
            
            fin.close();
        }
        
        image.convertToRGB();
        image.convertFromRGB();
        
        fout1.open(outputFiles[0]);
        output(fout1, inputFilesMetainformation, outputCount);
        
        if (outputCount == oneFile) {
            image.output(fout1);
            
            fout1.close();
        } else {
            fout2.open(outputFiles[1]);
            fout3.open(outputFiles[2]);
            
            output(fout2, inputFilesMetainformation, outputCount);
            output(fout3, inputFilesMetainformation, outputCount);
            
            image.output(fout1, fout2, fout3);
            
            fout1.close();
            fout2.close();
            fout3.close();
        }
    } catch (std::bad_cast) {
        std::cerr << "Не удалось преобразовать!" << std::endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout1.is_open()) {
            fout1.close();
        }
        if (fout2.is_open()) {
            fout2.close();
        }
        if (fout3.is_open()) {
            fout3.close();
        }
        return 1;
    } catch (std::bad_alloc) {
        std::cerr << "Не удалось выделить память!" << std::endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout1.is_open()) {
            fout1.close();
        }
        if (fout2.is_open()) {
            fout2.close();
        }
        if (fout3.is_open()) {
            fout3.close();
        }
        return 1;
    } catch (std::ifstream::failure) {
        std::cerr << "Ошибка при открытии входного файла!" << std::endl;
        if (fout1.is_open()) {
            fout1.close();
        }
        if (fout2.is_open()) {
            fout2.close();
        }
        if (fout3.is_open()) {
            fout3.close();
        }
        return 1;
    } catch (std::ofstream::failure) {
        std::cerr << "Ошибка при открытии выходного файла!" << std::endl;
        if (fin.is_open()) {
            fin.close();
        }
        return 1;
    } catch (Exception & exception) {
        std::cerr << exception.what() << std::endl;
        if (fin.is_open()) {
            fin.close();
        }
        if (fout1.is_open()) {
            fout1.close();
        }
        if (fout2.is_open()) {
            fout2.close();
        }
        if (fout3.is_open()) {
            fout3.close();
        }
        return 1;
    }
}
