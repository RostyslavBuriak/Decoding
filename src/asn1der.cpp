#include "asn1der.h"
#include <iostream>
#include <cstring>

#define SETBIT(number,bit) ((number) |= (pow(2,bit)))
#define CHECKBIT(number,bit) ((number) & (1 << (bit)))

decoder::decoder(){
    Result = new char[1]{};
}

decoder::decoder(const decoder& obj){
    sizeofascii = obj.sizeofascii;
    sizeof64 = obj.sizeof64;
    unsigned char * NewChar = new unsigned char[sizeofascii+1];
    memcpy(NewChar,obj.CharByteArray,sizeofascii+1);
    CharByteArray = NewChar;
    IntByteArray = new int[sizeofascii];
    memcpy(IntByteArray,obj.IntByteArray,sizeofascii);
    pArr = IntByteArray;
    Result = new char[1]{};
    Append(&Result,obj.Result);
}


decoder::decoder(decoder && obj){
    sizeofascii = obj.sizeofascii;
    sizeof64 = obj.sizeof64;
    IntByteArray = obj.IntByteArray;
    obj.IntByteArray = nullptr;
    CharByteArray = obj.CharByteArray;
    obj.CharByteArray = nullptr;
    pArr = IntByteArray;
    obj.pArr = nullptr;
    Result = obj.Result;
    obj.Result = nullptr;
}


decoder::~decoder(){
    if(CharByteArray)
        delete[] CharByteArray;
    if(IntByteArray)
        delete[] IntByteArray;
    if(Result)
        delete[] Result;
}


decoder& decoder::operator=(decoder && obj){
    sizeofascii = obj.sizeofascii;
    sizeof64 = obj.sizeof64;
    IntByteArray = obj.IntByteArray;
    obj.IntByteArray = nullptr;
    CharByteArray = obj.CharByteArray;
    obj.CharByteArray = nullptr;
    pArr = IntByteArray;
    obj.pArr = nullptr;
    Result = obj.Result;
    obj.Result = nullptr;
    return *this;
}


decoder& decoder::operator=(const decoder& obj){
    sizeofascii = obj.sizeofascii;
    sizeof64 = obj.sizeof64;
    unsigned char * NewChar = new unsigned char[sizeofascii+1];
    memcpy(NewChar,obj.CharByteArray,sizeofascii+1);
    CharByteArray = NewChar;
    IntByteArray = new int[sizeofascii];
    memcpy(IntByteArray,obj.IntByteArray,sizeofascii);
    pArr = IntByteArray;
    Result = new char[1]{};
    Append(&Result,obj.Result);
    return *this;
}

void decoder::Append(char ** dst,const char * src){  
     unsigned long dstlen = strlen(*dst),srclen =  strlen(src);
     char* temp = new char[dstlen+srclen+1];
     memcpy(temp, *dst, dstlen);
     memcpy(temp + dstlen, src, srclen + 1);
     delete[] *dst;
     *dst = temp;
}


char *  decoder::NewArr(const void* arr,int length) {
    char * temp = new char[length+1]{};
    memcpy(temp,arr,length+1);
    return temp;
}


void decoder::ToIntArray(){
    IntByteArray = new int[sizeofascii+1]{};
    for(unsigned int i = 0;i<sizeofascii;++i){
        IntByteArray[i] = static_cast<unsigned int>(CharByteArray[i]);
    }
    IntByteArray[sizeofascii] = -1;
    pArr = IntByteArray;
}

unsigned long decoder::CalculateCharArrayLength(const char * array){
    unsigned long length = 0;
    while(*array != '\0'){
        ++length;
        ++array;
    }
    return length;
}

void decoder::ParseData(){
    switch (*pArr)
    {
        case ENDOFARRAY:
            break;
        case INTEGER:
            IntegerHandler();
            ParseData();
            break;
        case OCTET_STRING:
            OctetStringHandler();
            Append(&Result,("}\n"));
            ParseData();
            break;
        case OBJECT_IDENTIFIER:
            ObjectIdentifierHandler();
            Append(&Result,("}\n"));
            ParseData();
            break;
        case SEQUENCE:
            SequenceHandler();
            ParseData();
            Append(&Result,("\n}"));
            break;
        default:
            break;
    }
}


char * decoder::decode(const unsigned char * Text,long unsigned length){
    sizeofascii = length;
    if(CharByteArray) //clear 
        delete[] CharByteArray;
    if(IntByteArray == nullptr){
        CharByteArray = reinterpret_cast<unsigned char*>(NewArr(Text,sizeofascii));
        ToIntArray();
    }
    try{
        ParseData();
    }
    catch(const char* exception){
        std::cout << exception;
        return nullptr;
    }
    return Result;
}


char * decoder::decode64(const char * Text){
    sizeof64 = CalculateCharArrayLength(Text);
    sizeofascii = sizeof64-(sizeof64/4);
    if(CharByteArray) //clear Base64CharArray if it exists
        delete[] CharByteArray;
    char * Base64Char = NewArr(Text,sizeof64);
    CharByteArray = new unsigned char[sizeofascii]{};
    Base64ToASCII(Base64Char);
     if(IntByteArray == nullptr){
         ToIntArray();
     }
    try{
        ParseData();
    }
    catch(const char* exception){
        std::cout << exception;
        return nullptr;
    }
    return Result;
}


int decoder::Base64CharToDec(char base64){
    for(int i = 0;i<65;++i){
        if(Base64Chars[i] == base64)
            return i;
    }
    return -1;
}


void decoder::Base64ToASCII(char * base64){
    int ch = 0,offset = 2;
    unsigned char * pCharByteArray = CharByteArray; //pointer to CharByteArray to move through this array
    for(unsigned long i = 0;i < sizeof64*8-1;++i && ++pCharByteArray){
        for(int q = 0; q < 8-offset;++i && ++q){
            if(CHECKBIT(Base64CharToDec(base64[i/8]),i%8))
                SETBIT(ch,q);
        }
        ch = ch << offset;
        for(int q = 0;q < offset;++i && ++q){
            if(CHECKBIT(Base64CharToDec(base64[(i+6)/8]),(i+6)%8))
                SETBIT(ch,q);
        }
        --i;
        offset == 2? offset = 4 : offset ==4? offset = 6 :offset == 6? offset = 2,i+=8: throw("Base64ToASCII error");
        *pCharByteArray = static_cast<unsigned char>(ch);
        ch = 0;
    }
}


int decoder::ParseLength(int num){
    num &= 63;
    return num;
}


int decoder::pow(int num, int times){
    if(times == 0)
        return 1;
    if(times == 1)
        return num;
    if(times > 1)
        return num*pow(num,--times);
    return -1;
}


int decoder::CalculateBytes(int len){
    int calculated = 0,power = len*8-1;
    for(int i = 0;i < len && ++pArr;i++){

        if(*pArr == -1 )
            throw "CalculateBytes error";

        for(int q = 7;q >= 0;--q){
            if(CHECKBIT(*pArr,q)){
                calculated += pow(2,power);
            }
            --power;
        }
    }
    return calculated;
}


void decoder::CalculateIdentifierFirstByte(){ //there are only three types of first node 1,2,3 , first byte is calculated with (firstnode*40 + secondnode)
    if(*++pArr < 80){ //if first byte is less than 80 it means that first node is 1

        if(*pArr == -1 )
            throw "CalculateIndentifierFirstByte error";

        Append(&Result,(std::to_string(1) + ".").c_str());
        Append(&Result,(std::to_string(*pArr-40)+ ".").c_str()); //if first node is 1 the second node is : number - (firstnode * 40)
        return;
    }else if (*pArr < 120) //means that first node is 2
    {
        Append(&Result,(std::to_string(2) + ".").c_str());
        Append(&Result,(std::to_string(*pArr-80)+ ".").c_str()); //if first node is 1 the second node is : number - (firstnode * 40)
        return;
    }else // first node is 3
    {
        Append(&Result,(std::to_string(3) + ".").c_str());
        Append(&Result,(std::to_string(*pArr-120)+ ".").c_str());
        return;
    }
}


int decoder::CalculateLongSID(int& sizeofascii) {
  int length = 1, value = 0, SID = 0;
  for (;; ++pArr) {
    if (!CHECKBIT(*pArr, 7)) {
      ++length;
      *pArr &= 127;
      value |= *pArr;
      break;
    }
    else
    {
      ++length;
      *pArr &= 127;
      value |= *pArr;
      value = value << 8;
    }
  }
  for (int i = 0; i < length*8-1; ++i) {
    if(CHECKBIT(value,i)){
        SETBIT(SID,i-i/8);
    }
  }
  sizeofascii += length-2;
  return SID;
 
}


void decoder::CalculateIdentifier(int len){
    CalculateIdentifierFirstByte();
    for(int i = 0; i<len && ++pArr ; ++i){
        if(*pArr == -1 || *(pArr+1) == -1)
            throw "CalculateIndentifier error";
        if(!CHECKBIT(*pArr,7)){ //if decimal number consists of only one byte
            Append(&Result,(std::to_string(*pArr) + ".").c_str());
        }else //if bit 7 is set it means that length of decimal number in bytes is bigger than one byte
        {
            Append(&Result,(std::to_string(CalculateLongSID(i)) + ".").c_str());
        }
    }
}


char * decoder::IntToHexChar(int number){
    char * hex = new char[3]{};
    char hexarr[] = "0123456789ABCDEF";
    int num = number/16;
    hex[0] = hexarr[num];
    num = number%16;
    hex[1] = hexarr[num];
    return hex;
}


void decoder::AppendOctetString(int length){
     for(int i = 0; i < length && ++pArr;++i){
         char * p = IntToHexChar(*pArr);
         Append(&Result,p);
         delete[] p;
     }
}


void decoder::OctetStringHandler(){
    if(!CHECKBIT(*++pArr,7)){
        if(*pArr == -1 )
            throw "IntegerHandler error";

        Append(&Result," Octet String : { ");
        AppendOctetString(ParseLength(*pArr));
        Append(&Result,"}");
    }
    ++pArr;
}


void decoder::ObjectIdentifierHandler(){
    Append(&Result," OBJECT INDENTIFIED WITH VALUE: { ");
    if(!CHECKBIT(*++pArr,7)){ //if length in one byte

        if(*pArr == -1 )
            throw "IntegerHandler error";

        CalculateIdentifier(*pArr-1);
    }
    else //behaviour not described
    {
        CalculateIdentifierFirstByte();
        CalculateIdentifier(ParseLength(*pArr));
    }
    ++pArr;
}


void decoder::IntegerHandler(){
    if(!CHECKBIT(*++pArr,7)){

        if(*pArr == -1 )
            throw "IntegerHandler error";

        Append(&Result," INTEGER WITH VALUE: { ");
        Append(&Result, ((std::to_string(CalculateBytes(ParseLength(*pArr)))+ " }").c_str()));
    }
    else
    {
        Append(&Result," INTEGER WITH VALUE: { ");
        Append(&Result, (std::to_string(CalculateBytes(CalculateBytes(ParseLength(*pArr))))+ " }").c_str());
    }
    ++pArr;
}


void decoder::SequenceHandler(){
    if(!CHECKBIT(*++pArr,7)){

        if(*pArr == -1 )
            throw "SequenceHandler error";

        Append(&Result," SEQUENCE WITH LENGTH: ");
        Append(&Result, (std::to_string(*pArr) + "{ \n\t").c_str());
      }
    else
    {
         Append(&Result," SEQUENCE WITH LENGTH: ");
         Append(&Result, (std::to_string(CalculateBytes(ParseLength(*pArr)))+ "\n\t").c_str());
    }
    ++pArr;
}

