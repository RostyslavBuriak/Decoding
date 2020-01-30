#include "asn1der.h"
#include <iostream>
#include <cstring>

#define SETBIT(number,bit) ((number) |= (pow(2,bit)))
#define CHECKBIT(number,bit) ((number) & (1 << (bit)))

decoder::decoder(){
}

decoder::decoder(const decoder& obj){
    this->size = obj.size;
    unsigned char * NewChar = new unsigned char[this->size+1];
    memcpy(NewChar,obj.CharByteArray,size+1);
    CharByteArray = NewChar;
    memcpy(this->IntByteArray,obj.IntByteArray,size);
    this->pArr = this->IntByteArray;
}


decoder::decoder(decoder && obj){
    this->size = obj.size;
    this->IntByteArray = obj.IntByteArray;
    this->CharByteArray = obj.CharByteArray;
    this->pArr = obj.IntByteArray;
}


decoder::~decoder(){
    if(CharByteArray)
        delete[] CharByteArray;
    if(IntByteArray)
        delete[] IntByteArray;
}


decoder& decoder::operator=(decoder && obj){
    this->size = obj.size;
    this->IntByteArray = obj.IntByteArray;
    this->CharByteArray = obj.CharByteArray;
    this->pArr = obj.IntByteArray;
    return *this;
}


decoder& decoder::operator=(const decoder& obj){
    this->size = obj.size;
    unsigned char * NewChar = new unsigned char[this->size+1];
    memcpy(NewChar,obj.CharByteArray,size+1);
    CharByteArray = NewChar;
    memcpy(this->IntByteArray,obj.IntByteArray,size);
    this->pArr = this->IntByteArray;
    return *this;
}

char * decoder::Append(char * dst,const char * src){  
     unsigned long dstlen = strlen(dst),srclen =  strlen(src);
     char* temp = new char[dstlen+srclen+1];
     memcpy(temp, dst, dstlen);
     memcpy(temp + dstlen, src, srclen + 1);
     delete[] dst;
     return temp;
}


const unsigned char *  decoder::NewArr(const unsigned char* arr) {
    unsigned char * temp = new unsigned char[size+1]{};
    memcpy(temp,arr,size+1);
    return temp;
}


void decoder::ToIntArray(){
    IntByteArray = new int[size+1]{};
    for(unsigned int i = 0;i<size;++i){
        IntByteArray[i] = static_cast<unsigned int>(CharByteArray[i]);
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
            ParseData();
            break;
        case OCTET_STRING:
            OctetStringHandler();
            ParseData();
            break;
        case OBJECT_IDENTIFIER:
            ObjectIdentifierHandler();
            ParseData();
            Result = Append(Result,("}"));
            break;
        case SEQUENCE:
            SequenceHandler();
            ParseData();
            Result = Append(Result,("\n}"));
            break;
        default:
            break;
    }
}


char * decoder::decode(const unsigned char * Text,long unsigned length){
      size = length;
      if(CharByteArray)
        delete[] CharByteArray;
      CharByteArray = NewArr(Text);
      ToIntArray();
      try{
          ParseData();
      }
      catch(const char* exception){
         std::cout << exception;
          return nullptr;
      }
      return Result;
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

        Result = Append(Result,(std::to_string(1) + ".").c_str());
        Result = Append(Result,(std::to_string(*pArr-40)+ ".").c_str()); //if first node is 1 the second node is : number - (firstnode * 40)
        return;
    }else if (*pArr < 120) //means that first node is 2
    {
        Result = Append(Result,(std::to_string(2) + ".").c_str());
        Result = Append(Result,(std::to_string(*pArr-80)+ ".").c_str()); //if first node is 1 the second node is : number - (firstnode * 40)
        return;
    }else // first node is 3
    {
        Result = Append(Result,(std::to_string(3) + ".").c_str());
        Result = Append(Result,(std::to_string(*pArr-120)+ ".").c_str());
        return;
    }
}


int decoder::CalculateLongSID(int& size) {
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
  size += length-2;
  return SID;
 
}


void decoder::CalculateIdentifier(int len){
    CalculateIdentifierFirstByte();
    for(int i = 0; i<len && ++pArr ; ++i){
        if(*pArr == -1 || *(pArr+1) == -1)
            throw "CalculateIndentifier error";
        if(!CHECKBIT(*pArr,7)){ //if decimal number consists of only one byte
            Result = Append(Result,(std::to_string(*pArr) + ".").c_str());
        }else //if bit 7 is set it means that length of decimal number in bytes is bigger than one byte
        {
            Result = Append(Result,(std::to_string(CalculateLongSID(i)) + ".").c_str());
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
         Result = Append(Result,p);
         delete[] p;
     }
}


void decoder::OctetStringHandler(){
    if(!CHECKBIT(*++pArr,7)){
        if(*pArr == -1 )
            throw "IntegerHandler error";

        Result = Append(Result," Octet String : { ");
        AppendOctetString(ParseLength(*pArr));
        Result = Append(Result,"}");
    }
    ++pArr;
}


void decoder::ObjectIdentifierHandler(){
    Result = Append(Result," OBJECT INDENTIFIED WITH VALUE: { ");
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

        Result = Append(Result," INTEGER WITH VALUE: { ");
        Result = Append(Result, ((std::to_string(CalculateBytes(ParseLength(*pArr)))+ " }").c_str()));
    }
    else
    {
        Result = Append(Result," INTEGER WITH VALUE: { ");
        Result = Append(Result, (std::to_string(CalculateBytes(CalculateBytes(ParseLength(*pArr))))+ " }").c_str());
    }
    ++pArr;
}


void decoder::SequenceHandler(){
    if(!CHECKBIT(*++pArr,7)){

            if(*pArr == -1 )
                throw "SequenceHandler error";

            Result = Append(Result," SEQUENCE WITH LENGTH: ");
            Result = Append(Result, (std::to_string(*pArr) + "{ \n\t").c_str());
      }
    else
    {
        Result = Append(Result," SEQUENCE WITH LENGTH: ");
        Result = Append(Result, (std::to_string(CalculateBytes(ParseLength(*pArr)))+ "\n\t").c_str());
    }
    ++pArr;
}

