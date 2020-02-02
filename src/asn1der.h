#ifndef DECODER_H
#define DECODER_H
 class decoder{
    public:
        decoder();

        decoder(const decoder&);

        decoder(decoder&&);

        ~decoder();

        decoder& operator=(const decoder&);

        decoder& operator=(decoder&&);

        char * decode(const unsigned char * ,unsigned long);

        char * decode64(const char * );
    private:
        enum TYPES{
            ENDOFARRAY = -1,
            INTEGER = 0x02,
            OCTET_STRING = 0x04,
            OBJECT_IDENTIFIER = 0x06,
            SEQUENCE = 0x30
        };
        //Calculates number in power
        int pow(int,int);

        char* IntToHexChar(int);

        void AppendOctetString(int);

        int CalculateLongSID(int&);

        //Calculates length of char array which ends with '\0'
        unsigned long CalculateCharArrayLength(const char *);

        //Converts Base64 char to decimal number
        int Base64CharToDec(char );

        //Convert base64 to ASCII
        void Base64ToASCII(char *);

        //Calculates all decimal values of indentifier
        void CalculateIdentifier(int);

        //Calculates first byte of indentifier which is decoded to two values
        void CalculateIdentifierFirstByte();

        //Convert byte array to decimal value
        int CalculateBytes(int);

        //Decoding encoded ans1.der string
        void ParseData();

        //Appends passed array with new array
        void Append(char **,const char *);

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
        char *  NewArr(const void* ,int );

        //Byte array as integers
        int * IntByteArray = nullptr;

        //Pointer to IntByteArray which is used to move through array
        int * pArr = nullptr;

        //Byte array as char
        unsigned char * CharByteArray = nullptr;

        //The result decoded string
        char * Result = nullptr;

        //Size of CharByteArray
        unsigned long sizeofascii = 0;

        //Size of Base64CharArray
        unsigned long sizeof64 = 0;

        //base64 array of chars
        char Base64Chars[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
};
#endif
