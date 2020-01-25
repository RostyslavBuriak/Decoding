#include "asn1der.h"

decoder::decoder(){
}


decoder::~decoder(){
    if(CharByteArray)
        delete[] CharByteArray;
    if(IntByteArray)
        delete[] IntByteArray;
}


char * decoder::Append(char * dst,char * src){
     int dstlen = strlen(dst),srclen =  strlen(src);
     char* temp = new char[dstlen+srclen+1]{};
     memcpy(temp, dst, dstlen);
     strcat(temp,src);
     delete[] dst;
     return temp;
}


const unsigned char *  decoder::NewArr(const unsigned char* arr) {
    const unsigned char * pointer = arr;
        for(;*pointer != '\0';++pointer){
            ++size;
        }
    unsigned char * temp = new unsigned char[size+1]{};
    memcpy(temp,arr,size);
    return temp;
}


void decoder::ToIntArray(){
    const unsigned char * tempchar = CharByteArray;
    IntByteArray = new int[size+1]{};
    int * tempint = IntByteArray;
        for(;*tempchar != '\0';++tempchar && ++tempint){
            *tempint = (int)*tempchar;
        }
    IntByteArray[size] = -1;
    pArr = IntByteArray;
}


void decoder::ParseData(){
    switch (*pArr)
    {
        case ENDOFARRAY:
            break;
        case INTEGER:
            IntegerHandler();
            Result = Append(Result,const_cast<char *>("}"));
            break;
        case SEQUENCE:
            SequanceHandler();
            ParseData();
            Result = Append(Result,const_cast<char *>("\n}"));
            break;
        default:
            break;
    }
     
}


char * decoder::decode(const unsigned char * Text,char * cipherT, char * Other){
    CharByteArray = NewArr(Text);
    ToIntArray();
    ParseData();
    return Result;
}


int decoder::ParseLength(int num){
    num &= 63;
    return num;
}


int decoder::quad(int num, int times){
    if(times == 0)
        return 1;
    if(times == 1)
        return 2;
    if(times > 1)
        return num*quad(num,--times);
}


int decoder::CalculateBytes(int len){
    int calculated = 0,power = 0;
    for(int i = 0;i < len && ++pArr;i++){
        for(int q = 0;q < 8;q++){
            if(CHECKBIT(*pArr,q)){
                calculated += quad(2,power);
            }
            ++power;
        }
    }
    return calculated;
}


void decoder::IntegerHandler(){
     if(!CHECKBIT(*++pArr,7)){
        Result = Append(Result,const_cast<char *>("INTEGER WITH VALUE: { "));
        Result = Append(Result, const_cast<char *>((std::to_string(CalculateBytes(ParseLength(*++pArr)))+ " ").c_str()));
    }
    else
    {
        Result = Append(Result,const_cast<char *>("INTEGER WITH VALUE: { "));
        Result = Append(Result, const_cast<char *>((std::to_string(CalculateBytes(CalculateBytes(ParseLength(*pArr))))+ " ").c_str()));
    }
    ++pArr;
}


void decoder::SequanceHandler(){
    if(!CHECKBIT(*++pArr,7)){
            Result = Append(Result,const_cast<char *>("SEQUANCE WITH LENGTH: "));
            Result = Append(Result, const_cast<char *>((std::to_string(*pArr) + "{ \n").c_str()));
      }
    else
    {
        Result = Append(Result,const_cast<char *>("SEQUANCE WITH LENGTH: "));
        Result = Append(Result, const_cast<char *>((std::to_string(CalculateBytes(ParseLength(*pArr)))+ "\n").c_str()));
    }
    ++pArr;
}


void decoder::decode64(const char * Text,char * cipherT, char * Other){
    
}