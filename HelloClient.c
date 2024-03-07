//
// Created by lbrd on 2024/1/25.
//

#include "binder.h"
#include "HelloInterface.h"
#include "malloc.h"
#include "string.h"

struct binder_state *bs;
uint32_t handle;

uint32_t svcmgr_lookup(struct binder_state *bs, uint32_t target, const char *name) {
    uint32_t handle;
    unsigned iodata[512 / 4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_string16_x(&msg, SVC_MGR_NAME);
    bio_put_string16_x(&msg, name);

    if (binder_call(bs, &msg, &reply, target, SVC_MGR_CHECK_SERVICE))
        return 0;

    handle = bio_get_ref(&reply);

    if (handle)
        binder_acquire(bs, handle);

    binder_done(bs, &msg, &reply);

    return handle;
}


char *svcmgr_list_services(struct binder_state *bs, uint32_t target, int index) {

    unsigned int iodata[1024 / 4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_string16_x(&msg, SVC_MGR_NAME);
    bio_put_uint32(&msg, index);//长度
    bio_put_uint32(&msg, 1);//优先级

    if (binder_call(bs, &msg, &reply, target, SVC_MGR_LIST_SERVICES))
        return NULL;

    size_t str_len;

    uint16_t *str16 = bio_get_string16(&reply, &str_len);

    printf("str_len->%d\n", str_len);


    char *str = NULL;

    if (str_len > 0) {

        str = malloc(10 * 1024);

        for (int i = 0; i < str_len; ++i) {
            str[i] = *(str16 + i);
        }

        str[str_len] = '\0';
    }

    printf("before done\n");

    binder_done(bs, &msg, &reply);


    return str;


}


int add(int x, int y) {
    //调用服务
    unsigned iodata[512 / 4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_uint32(&msg, 2);
    bio_put_uint32(&msg, 5);

    if (binder_call(bs, &msg, &reply, handle, ADD))
        return 0;

    uint32_t ret = bio_get_uint32(&reply);

    binder_done(bs, &msg, &reply);

    return ret;
}


int createPerson(int age, char *name,Person *person) {
    //调用服务
    unsigned iodata[512 / 4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_uint32(&msg, age);

//    uint16_t uint16_name[nameLen+1];

    //charStrToUint16Str(uint16_name,name,nameLen);

    bio_put_string16_x(&msg, name);

    if (binder_call(bs, &msg, &reply, handle, ADD))
        return 0;

    uint32_t person_ptr = bio_get_ref(&reply);


    Person *reply_person=(Person*)person_ptr;

    person->age=reply_person->age;
    strcpy(person->name,reply_person->name);

    binder_done(bs, &msg, &reply);

    return 1;
}


int main() {

    char *driver = "/dev/binder";

    bs = binder_open(driver, 128 * 1024);

    if (!bs) {
        ALOGE("failed to open binder driver %s\n", driver);
        return -1;
    }

//    for (int i = 0; i < 6; ++i) {
//
//        char *service = svcmgr_list_services(bs, SERVER_MANAGER_HANDLER, i);
//
//        if (service) {
//            printf("services[%d]->%s\n", i, service);
//            free(service);
//        } else{
//            break;
//        }
//    }


    printf("getService start\n");

    //获取服务
    handle = svcmgr_lookup(bs, SERVER_MANAGER_HANDLER, HELLO_SEVER_NAME);

    int add_ret = add(5, 6);
    printf("add_ret->%d\n", add_ret);


    Person person;
    createPerson(20,"zgm",&person);

    printf("person age->%d,name->%d\n",person.age,person.name);

    return 0;

}