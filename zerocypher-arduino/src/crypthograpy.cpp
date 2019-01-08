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
    Serial.println("decipt");
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

String transpositionEncrypt(String d, String key) {
    return "notimpl";

}

String transpositionDecrypt(String e, String key) {
    return "notimpl";
}