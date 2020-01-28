#include <iostream>
#include "string.h"

#define ENDOFARRAY -1

#define INTEGER 0x02
#define OCTET_STRING 0x04
#define OBJECT_IDENTIFIER 0x06
#define SEQUENCE 0x30

#define SETBIT(number,bit) ((number) |= (pow(2,bit)))
#define CHECKBIT(number,bit) ((number) & (1 << (bit)))
 
class decoder{
    public:
        decoder();

        ~decoder();

        char * decode(const unsigned char * ,int,char * cipherT = nullptr, char * Other = nullptr);

        void decode64(const char * ,char * cipherT = nullptr, char * Other = nullptr);

    private:
        //Calculates number in power
        int pow(int,int);

        char* IntToHexChar(int);

        void AppendOctetString(int);

        int CalculateLongSID(int&);

        //Calculates all decimal values of indentifier
        void CalculateIdentifier(int);

        //Calculates first byte of indentifier which is decoded to two values
        void CalculateIdentifierFirstByte();

        //Convert byte array to decimal value
        int CalculateBytes(int);

        //Decoding encoded ans1.der string
        void ParseData();

        //Appends passed array with new array
        char * Append(char *,char *);

        //Decodes length of front bytes
        int ParseLength(int);

        //Handles Octet String
        void OctetStringHandler();

        //Handles Object Indentifier
        void ObjectIdentifierHandler();

        //Handles Integer
        void IntegerHandler();

        //Handles Sequance
        void SequenceHandler();

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
