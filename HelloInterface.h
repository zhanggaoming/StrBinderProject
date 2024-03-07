//
// Created by lbrd on 2024/1/25.
//

#ifndef BINDERC_HELLOINTERFACE_H
#define BINDERC_HELLOINTERFACE_H


enum Function {
    ADD, PRINT,CREATE_PERSON
};

struct _Person {
    int age;
    char name[20];
};

typedef struct _Person Person;


void uint16StrToCharStr(uint16_t * uint16_str,char * char_str,int len){

    for (int i = 0; i < len; ++i){
        char_str[i]=uint16_str[i];
    }

    char_str[len+1]='\0';
}

void charStrToUint16Str(uint16_t * uint16_str,char * char_str,int len){

    for (int i = 0; i < len; ++i){
        uint16_str[i]=char_str[i];
    }

    uint16_str[len+1]='\0';
}



#define SERVER_MANAGER_HANDLER 0
#define HELLO_SEVER_NAME "service.hello"

#endif //BINDERC_HELLOINTERFACE_H
