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
    unsigned char * temp = new unsigned char[size+1]{};
    for(int i = 0;i < size;++i){
        temp[i] = arr[i];
    }
    return temp;
}


void decoder::ToIntArray(){
    IntByteArray = new int[size+1]{};
    for(int i = 0;i<size;++i){
        IntByteArray[i] = (unsigned int)CharByteArray[i];
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
            Result = Append(Result,const_cast<char *>("}"));
            break;
        case SEQUENCE:
            SequenceHandler();
            ParseData();
            Result = Append(Result,const_cast<char *>("\n}"));
            break;
        default:
            break;
    }
}


char * decoder::decode(const unsigned char * Text,int length,char * cipherT, char * Other){
    size = length;
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
        return 2;
    if(times > 1)
        return num*pow(num,--times);
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

        Result = Append(Result,const_cast<char *>((std::to_string(1) + ".").c_str()));
        Result = Append(Result,const_cast<char *>((std::to_string(*pArr-40)+ ".").c_str())); //if first node is 1 the second node is : number - (firstnode * 40)
        return;
    }else if (*pArr < 120) //means that first node is 2
    {
        Result = Append(Result,const_cast<char *>((std::to_string(2) + ".").c_str()));
        Result = Append(Result,const_cast<char *>((std::to_string(*pArr-80)+ ".").c_str())); //if first node is 1 the second node is : number - (firstnode * 40)
        return;
    }else // first node is 3
    {
        Result = Append(Result,const_cast<char *>((std::to_string(3) + ".").c_str()));
        Result = Append(Result,const_cast<char *>((std::to_string(*pArr-120)+ ".").c_str()));
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
            Result = Append(Result,const_cast<char *>((std::to_string(*pArr) + ".").c_str()));
        }else //if bit 7 is set it means that length of decimal number in bytes is bigger than one byte
        {
            Result = Append(Result,const_cast<char *>((std::to_string(CalculateLongSID(i)) + ".").c_str()));
        }
    }
}


char * decoder::IntToHexChar(int number){
    char * hex = new char[3]{};
    int num = number/16;
    for(int i = 0;i < 2;++i)
    switch (num)
    {
    case 15:
        hex[i] = 'F';
        num = number%16;
        break;
    case 14:
        hex[i] = 'E';
        num = number%16;
        break;
    case 13:
        hex[i] = 'D';
        num = number%16;
        break;
    case 12:
        hex[i] = 'C';
        num = number%16;
        break;
    case 11:
        hex[i] = 'B';
        num = number%16;
        break;
    case 10:
        hex[i] = 'A';
        num = number%16;
        break;
    case 9:
        hex[i] = '9';
        num = number%16;
        break;
    case 8:
        hex[i] = '8';
        num = number%16;
        break;
    case 7:
        hex[i] = '7';
        num = number%16;
        break;
    case 6:
        hex[i] = '6';
        num = number%16;
        break;
    case 5:
        hex[i] = '5';
        num = number%16;
        break;
    case 4:
        hex[i] = '4';
        num = number%16;
        break;
    case 3:
        hex[i] = '3';
        num = number%16;
        break;
    case 2:
        hex[i] = '2';
        num = number%16;
        break;
    case 1:
        hex[i] = '1';
        num = number%16;
        break;
    case 0:
        hex[i] = '0';
        num = number%16;
        break;
    default:
        break;
    }
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

        Result = Append(Result,const_cast<char *>(" Octet String : { "));
        AppendOctetString(ParseLength(*pArr));
        Result = Append(Result,const_cast<char *>("}"));
    }
    ++pArr;
}


void decoder::ObjectIdentifierHandler(){
    Result = Append(Result,const_cast<char *>(" OBJECT INDENTIFIED WITH VALUE: { "));
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
    std::cout << "IT IS :" <<  *pArr;
    ++pArr;
}


void decoder::IntegerHandler(){
    if(!CHECKBIT(*++pArr,7)){

        if(*pArr == -1 )
            throw "IntegerHandler error";

        Result = Append(Result,const_cast<char *>(" INTEGER WITH VALUE: { "));
        Result = Append(Result, const_cast<char *>((std::to_string(CalculateBytes(ParseLength(*pArr)))+ " }").c_str()));
    }
    else
    {
        Result = Append(Result,const_cast<char *>(" INTEGER WITH VALUE: { "));
        Result = Append(Result, const_cast<char *>((std::to_string(CalculateBytes(CalculateBytes(ParseLength(*pArr))))+ " }").c_str()));
    }
    ++pArr;
}


void decoder::SequenceHandler(){
    if(!CHECKBIT(*++pArr,7)){

            if(*pArr == -1 )
                throw "SequenceHandler error";

            Result = Append(Result,const_cast<char *>(" SEQUENCE WITH LENGTH: "));
            Result = Append(Result, const_cast<char *>((std::to_string(*pArr) + "{ \n\t").c_str()));
      }
    else
    {
        Result = Append(Result,const_cast<char *>(" SEQUENCE WITH LENGTH: "));
        Result = Append(Result, const_cast<char *>((std::to_string(CalculateBytes(ParseLength(*pArr)))+ "\n\t").c_str()));
    }
    ++pArr;
}


void decoder::decode64(const char * Text,char * cipherT, char * Other){
    
}
