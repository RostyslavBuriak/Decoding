#include <iostream>
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

        char * Append(char *,char *);

        int ParseLength(int);

        void ObjectIndentifierHandler();

        void IntegerHandler();

        void SequanceHandler();

        void ToIntArray();

        const unsigned char *  NewArr(const unsigned char* );

        int * IntByteArray = nullptr;

        int * pArr = nullptr;

        const unsigned char * CharByteArray = nullptr;

        char * Result = new char[1]{};

        char * cipherT = nullptr;

        char * Other = nullptr;

        int size = 0;
};
