#include <crypthograpy.h>
#include <Arduino.h>

String ceasarEncrypt(String s, int key) {
    auto newString = String();

    for(char c : s) {

        if(c == ' ') {
            newString.concat(c);
            continue;
        }

        int encl = (c + key);

        if (encl > 'z')
            encl = ((encl - 'z') % 25) + 'a' - 1;

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

        encl = encl - ((key % 25) - 1); 

        newString.concat((char)encl);
    }

    return newString;
}

char** allocMemoryForMatrix(int rows, int columns, String data);
void DEBUG_MATRIX(char** matrix, int rows, int columns);
String readMatrix(char** matrix, int rows, int columns);

template<typename T> void swap(T& t1, T& t2) {
    T temp(t1); // or T temp(std::move(t1));
    t1 = t2;
    t2 = temp;
}

String transpositionEncrypt(String d, String key) {
    const char* cstrpointer = key.c_str();
    
    int addChars = ceil((float)d.length() / key.length()) * key.length() - d.length();
    
    for(size_t i = 0; i< addChars; ++i)
        d.concat('*');

    uint32_t columns = d.length() / key.length(), rows = key.length();

    char** matrix = allocMemoryForMatrix(rows, columns, d);
    char ckey[key.length()];
    memcpy(ckey, cstrpointer, strlen(cstrpointer) + 1);
    
    // ordina arrays in base a ckey
    for (size_t i = 0; i < rows - 1; i++)  // rows = lunghezza array (punt) ckey
        for (size_t j = 0; j < rows - i - 1; j++)  // rows = lunghezza array (punt) ckey
            if (ckey[j] > ckey[j+1]) {
                swap(ckey[j], ckey[j+1]);
                swap(matrix[j], matrix[j+1]);
            }
    
    // todo fixare memory leak su matrix (delete non fatto)
    String finalString = readMatrix(matrix, rows, columns);
    Serial.println(finalString);
    return finalString;   
}

char** allocMemoryForMatrix(int rows, int columns, String data) {
    char** matrix;
    matrix = (char**)malloc(sizeof(char*[rows]));
    for(int i = 0; i<columns; ++i) {
        matrix[i] = (char*)malloc(sizeof(char[columns]));
    }

    for(int i = 0; i<columns; i++) {
        for(int j = 0; j<rows; j++) {
            char cdata = data[i * rows + j];
            matrix[j][i] = cdata;
        }
    }
    return matrix;
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

void DEBUG_MATRIX(char** matrix, int rows, int columns) {
    for(int i = 0; i<rows; i++) {
        for(int j = 0; j<columns; j++) {
            Serial.print(matrix[i][j]);
            Serial.print(' ');
        }
        Serial.println();
    }
}

String transpositionDecrypt(String e, String key) {
    return "notimpl";
}