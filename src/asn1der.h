#i#include <iostream>
#include "string.h"

#define ENDOFARRAY -1

#define INTEGER 0x02
#define OCTET_STRING 0x04
#define OBJECT_IDENTIFIER 0x06
#define SEQUENCE 0x30

#define CHECKBIT(number,bit) ((number) & (1 << (bit)))
 
class decoder{
    public:
        decoder();

        ~decoder();

        char * decode(const unsigned char * ,char * cipherT = nullptr, char * Other = nullptr);

        void decode64(const char * ,char * cipherT = nullptr, char * Other = nullptr);

    private:
        //Calculates number in power
        int pow(int,int);

        //Calculates all decimal values of indentifier
        void CalculateIndentifier(int);

        //Calculates first byte of indentifier which is decoded to two values
        void CalculateIndentifierFirstByte();

        //Convert byte array to decimal value
        int CalculateBytes(int);

        //Decoding encoded ans1.der string
        void ParseData();

        //Appends passed array with new array
        char * Append(char *,char *);

        //Decodes length of front bytes
        int ParseLength(int);

        //Handles Object Indentifier
        void ObjectIndentifierHandler();

        //Handles Integer
        void IntegerHandler();

        //Handles Sequance
        void SequanceHandler();

        //Converts char array to int array
        void ToIntArray();

        //Copies value of passed array and returns new created one with same values
        const unsigned char *  NewArr(const unsigned char* );

        //Byte array as integers
        int * IntByteArray = nullptr;

        //Pointer to IntByteArray which is used to move through array
        int * pArr = nullptr;

        //Byte array as char
        const unsigned char * CharByteArray = nullptr;

        //The result decoded string
        char * Result = new char[1]{};

        //Size of IntByteArray
        int size = 0;
};
