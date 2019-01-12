class String;

extern String ceasarEncrypt(String s, int key);
extern String ceasarDecrypt(String e, int key);
extern String transpositionEncrypt(String d, String key);
extern String transpositionDecrypt(String e, String key);

struct transpCouple {
    char* row;
    char letter;
};