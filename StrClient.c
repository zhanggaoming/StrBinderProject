//
// Created by lbrd on 2024/3/7.
//
#include "binder.h"
#include "IStrService.h"
#include <stdio.h>


//像servicemanager查询服务
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

static uint32_t strServiceHandle=-1;

int main() {

    //打开binder驱动
    struct binder_state *bs = binder_open("/dev/binder", STR_SERVICE_BINDER_MAP_SIZE);

    //向servicemanager查询StrService服务
    strServiceHandle = svcmgr_lookup(bs, BINDER_SERVICE_MANAGER, STR_SERVICE_NAME);

    printf("the Handle->%d\n",strServiceHandle);

    //调用计算字符串长度并打印
    unsigned iodata[512 / 4];
    struct binder_io msg, reply;
    bio_init(&msg, iodata, sizeof(iodata), 4);
    char *test_str = "我是一个binder测试客户端";
    bio_put_string16_x(&msg, test_str);
    binder_call(bs, &msg, &reply, strServiceHandle, FUNCTION_GET_SIZE);

    uint32_t size = bio_get_uint32(&reply);

    printf("字符串 %s 的长度是%d\n", test_str, size);

    binder_done(bs, &msg, &reply);

    //调用字符串转小写接口并打印
    unsigned iodata1[512 / 4];
    struct binder_io msg1, reply1;
    bio_init(&msg1, iodata1, sizeof(iodata1), 4);
    char *test_str1 = "KERNEL BINDER";
    bio_put_string16_x(&msg1, test_str1);
    binder_call(bs, &msg1, &reply1, strServiceHandle, FUNCTION_LOWERCASE);

    size_t str_len;
    uint16_t *str_16 = bio_get_string16(&reply1, &str_len);

    printf("转成小写的字符串为：");

    for (int i = 0; i<str_len; i++) {
        printf("%lc", str_16[i]);
    }
    printf("\n");

    binder_done(bs, &msg1, &reply1);


    //退出
    binder_close(bs);


    return 0;

}