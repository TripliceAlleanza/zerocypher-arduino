#include <crypthograpy.h>
#include <Arduino.h>
#include "debug.h"

String ceasarEncrypt(String s, int key) {
    auto newString = String();

    for(char c : s) {

        if(c == ' ') {
            newString.concat(c);
            continue;
        }

        int encl = (c + key);

        if (encl > 'z')
            encl = ((encl - 'z') % 24) + 'a';

        newString.concat((char)encl);
    }
    
    return newString;
}

String ceasarDecrypt(String e, int key) {
    auto newString = String();
    for(char c : e) {
        
        if(c == ' ') {
            newString.concat(c);
            continue;
        }

        int encl = (int)c;

        encl = encl - ((key % 24) - 1); 

        newString.concat((char)(encl));
    }

    return newString;
}

char** allocMemoryForMatrix(int rows, int columns);
void printMatrix(char** matrix, int rows, int columns);
String readMatrix(char** matrix, int rows, int columns);
char** populateMatrixEncryption(char** matrix, int rows, int columns, String data);
char** populateMatrixDecryption(char** matrix, int rows, int columns, String data);

template<typename T> void swap(T& t1, T& t2) {
    T temp(t1); // or T temp(std::move(t1));
    t1 = t2;
    t2 = temp;
}

String transpositionEncrypt(String d, String key) {
    const char* cstrpointer = key.c_str();
    
    int addChars = ceil((float)d.length() / key.length()) * key.length() - d.length();
    
    for(size_t i = 0; i < addChars; ++i)
        d.concat('*');

    DEBUG_PRINTLN(d);
    uint32_t columns = d.length() / key.length(), rows = key.length();

    // crea matrice
    char** matrix = allocMemoryForMatrix(rows, columns);
    populateMatrixEncryption(matrix, rows, columns, d);

    char ckey[key.length()];
    memcpy(ckey, cstrpointer, strlen(cstrpointer) + 1);
    DEBUG_PRINTLN(ckey);

    // ordina arrays in base a ckey
    for (size_t i = 0; i < rows - 1; i++)  // rows = lunghezza array (punt) ckey
        for (size_t j = 0; j < rows - i - 1; j++)  // rows = lunghezza array (punt) ckey
            if (ckey[j] > ckey[j+1]) {
                swap(ckey[j], ckey[j+1]);
                swap(matrix[j], matrix[j+1]);
            }

    String finalString = readMatrix(matrix, rows, columns);
    DEBUG_PRINTLN(finalString);

    // dealloc data
    for(size_t i = 0; i<rows; ++i) {
        free(matrix[i]);
    }

    free(matrix);

    return finalString;
}

char** allocMemoryForMatrix(int rows, int columns) {
    char** matrix;
    matrix = (char**)malloc(sizeof(char*[rows]));
    for(int i = 0; i<rows; ++i) {
        matrix[i] = (char*)malloc(sizeof(char[columns]));
    }
    return matrix;
}

char** populateMatrixEncryption(char** matrix, int rows, int columns, String data) {
    for(int i = 0; i<columns; i++) {
        for(int j = 0; j<rows; j++) {
            char cdata = data[i * rows + j];
            matrix[j][i] = cdata;
        }
    }
}

char** populateMatrixDecryption(char** matrix, int rows, int columns, String data) {
    for(int i = 0; i<rows; i++) {
        for(int j = 0; j<columns; j++) {
            char cdata = data[i * columns + j];
            matrix[i][j] = cdata;
        }
    }
}

String readMatrix(char** matrix, int rows, int columns) {
    String str = "";
    for(int i = 0; i<rows; i++) {
        for(int j = 0; j<columns; j++) {
            str.concat(matrix[i][j]);
        }
    }
    return str;
}

void printMatrix(char** matrix, int rows, int columns) {
    for(int i = 0; i<rows; i++) {
        for(int j = 0; j<columns; j++) {
            Serial.print(matrix[i][j]);
            Serial.print(' ');
        }
        Serial.println();
    }
}

transpCouple* getFirstCoupleWithLetter(transpCouple couples[], char lett, int length, int startFrom) {
    for(int i = startFrom; i<length;++i) {
        if(couples[i].letter == lett) return &(couples[i]);
    }

    return nullptr;
}

String transpositionDecrypt(String e, String key) {
    int rows = key.length(), columns = e.length() / key.length();

    auto ckey = key.c_str();
    char alphabeticallyOrderedKey[strlen(ckey) + 1];

    memcpy(alphabeticallyOrderedKey, ckey, strlen(ckey) + 1);
    qsort(alphabeticallyOrderedKey, (size_t) strlen(alphabeticallyOrderedKey), (size_t) sizeof(char), 
    [](const void* a, const void* b) -> int {
        return *(char*)a - *(char*)b;
    });
    
    auto matrix = allocMemoryForMatrix(rows, columns);
    populateMatrixDecryption(matrix, rows, columns, e);
    
    transpCouple couples[rows];

    for(int i = 0; i < rows; ++i)
        couples[i] = {matrix[i],alphabeticallyOrderedKey[i]};
    
    size_t coupleSize = sizeof(transpCouple);

    for(int i = 0; i<rows; ++i) {
        char lett = key[i];
        auto couple = getFirstCoupleWithLetter(couples, lett, rows, i);
        if(couple == nullptr) continue;
        swap(couples[i], *couple);
    }

    String finalString = "";

    for(int i = 0; i < rows; ++i) {
        matrix[i] = couples[i].row;
    }
    
    for(size_t i = 0; i < columns; i++)
        for(size_t j = 0; j < rows; j++)
            finalString.concat(matrix[j][i]);
    
    DEBUG_PRINTLN(finalString.c_str());

    // finalString.replace("*","");
    return finalString;
}