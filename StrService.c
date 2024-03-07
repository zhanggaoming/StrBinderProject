#include <stdio.h>
#include "binder.h"
#include "IStrService.h"
#include <string.h>
#include <ctype.h>
#include <stdint.h>


void uint16StrToCharStr(uint16_t * uint16_str,char * char_str,int len){

    for (int i = 0; i < len; ++i){
        char_str[i]=uint16_str[i];
    }

    char_str[len+1]='\0';
}

// 将字符串转换为小写
void str_to_lower(char *str) {
    while (*str != '\0') {
        *str = tolower(*str);
        str++;
    }
}

//处理函数
int handle_func(struct binder_state *bs,
                struct binder_transaction_data *txn,
                struct binder_io *msg,
                struct binder_io *reply) {


    printf("code->%d\n",txn->code);

    switch (txn->code) {

        case FUNCTION_GET_SIZE: {
            size_t str_len;
            uint16_t *str16 = bio_get_string16(msg, &str_len);
            bio_put_uint32(reply,str_len);
            break;
        }
        case FUNCTION_LOWERCASE:{
            size_t len;
            uint16_t *str16 = bio_get_string16(msg, &len);
            char temp[len+1];
            uint16StrToCharStr(str16,temp,len);
            str_to_lower(temp);
            bio_put_string16_x(reply,temp);
            break;
        }

        default:{
            printf("unknown code %d\n", txn->code);
            return -1;
        }

    }

    bio_put_uint32(reply, 0);
    return 0;
}


int svcmgr_publish(struct binder_state *bs, uint32_t target, const char *name, void *ptr) {
    int status;
    unsigned iodata[512 / 4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_string16_x(&msg, SVC_MGR_NAME);
    bio_put_string16_x(&msg, name);
    bio_put_obj(&msg, ptr);

    if (binder_call(bs, &msg, &reply, target, SVC_MGR_ADD_SERVICE))
        return -1;

    status = bio_get_uint32(&reply);

    binder_done(bs, &msg, &reply);

    return status;
}


unsigned token;


int main() {

    struct binder_state *bs = binder_open("/dev/binder", STR_SERVICE_BINDER_MAP_SIZE);

    //1.向service_manager注册服务
    int register_ret=svcmgr_publish(bs, BINDER_SERVICE_MANAGER, STR_SERVICE_NAME, &token);

    printf("register_ret->%d\n",register_ret);

    //2.监听客户端调用
    binder_loop(bs, handle_func);

    binder_close(bs);

    return 0;
}
