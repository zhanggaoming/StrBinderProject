//
// Created by lbrd on 2024/3/7.
//

#ifndef STRBINDERPROJECT_ISTRSERVICE_H
#define STRBINDERPROJECT_ISTRSERVICE_H

#define STR_SERVICE_BINDER_MAP_SIZE 128*1024 //内存映射大小

#define STR_SERVICE_NAME "StrService"

enum FUN_CODE{
    FUNCTION_GET_SIZE,//获取字符串的空间占用大小
    FUNCTION_LOWERCASE//将字符串转成小写
};


#endif //STRBINDERPROJECT_ISTRSERVICE_H
