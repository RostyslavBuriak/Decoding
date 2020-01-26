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
            ParseData();
            Result = Append(Result,const_cast<char *>("}"));
            break;
        case OBJECT_IDENTIFIER:
            ObjectIndentifierHandler();
            ParseData();
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
    int calculated = 0,power = len*8;
    for(int i = 0;i < len && ++pArr;i++){
        for(int q = 7;q >= 0;--q){
            if(CHECKBIT(*pArr,q)){
                calculated += quad(2,power-1);
            }
            --power;
        }
    }
    return calculated;
}


void decoder::CalculateIndentifierFirstByte(){ //there are only three types of first node 1,2,3 , first byte is calculated with (firstnode*40 + secondnode)
    if(*++pArr < 80){ //if first byte is less than 80 it means that first node is 1
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


void decoder::CalculateIndentifierByte(){

}


void decoder::CalculateIndentifier(int len){
    for(int i = 0; i<len && ++pArr ; i++){
        if(!CHECKBIT(*pArr,7)){ //if decimal number consists of only one byte
            Result = Append(Result,const_cast<char *>((std::to_string(*pArr) + ".").c_str()));
        }else //if bit 7 is set it means that length of decimal number in bytes is bigger than one byte
        {
            *pArr &= 31; //set all bits of leftmost nibble of the leftmost byte to 0
            if(CHECKBIT(*pArr,0)){ //we have to miss leftmsot bit of rightmost byte and replace it with righmost bit of leftmost byte, if righmost bit of leftmost byte is set we have to set leftmost bit of righmost byte 
                *pArr >>=1;
                *++pArr |= 128;
            }else
            {
                *pArr >>=1;
                *++pArr |= 0;
            }
            ++i; //because we will pass 2 bytes
            ----pArr; //return to length
            Result = Append(Result,const_cast<char *>((std::to_string(CalculateBytes(2)) + ".").c_str()));
        }
    }
}


void decoder::ObjectIndentifierHandler(){
    Result = Append(Result,const_cast<char *>("OBJECT INDENTIFIED WITH VALUE: { "));
    if(!CHECKBIT(*++pArr,7)){ //if length in one byte
        int length = *pArr;
        CalculateIndentifierFirstByte();
        CalculateIndentifier(length-1);
    }
    else //behaviour not described
    {
        CalculateIndentifierFirstByte();
        CalculateIndentifier(ParseLength(*pArr));
    }
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
            Result = Append(Result, const_cast<char *>((std::to_string(*pArr) + "{ \n\t").c_str()));
      }
    else
    {
        Result = Append(Result,const_cast<char *>("SEQUANCE WITH LENGTH: "));
        Result = Append(Result, const_cast<char *>((std::to_string(CalculateBytes(ParseLength(*pArr)))+ "\n\t").c_str()));
    }
    ++pArr;
}


void decoder::decode64(const char * Text,char * cipherT, char * Other){
    
}