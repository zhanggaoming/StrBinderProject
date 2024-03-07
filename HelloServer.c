//
// Created by lbrd on 2024/1/25.
//

#include "binder.h"
#include "HelloInterface.h"


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


int svcmgr_handler(struct binder_state *bs,
                   struct binder_transaction_data *txn,
                   struct binder_io *msg,
                   struct binder_io *reply) {
    struct svcinfo *si;
    uint16_t *s;
    size_t len;
    uint32_t handle;
    uint32_t strict_policy;
    int allow_isolated;
    uint32_t dumpsys_priority;

    //ALOGI("target=%p code=%d pid=%d uid=%d\n",
    //      (void*) txn->target.ptr, txn->code, txn->sender_pid, txn->sender_euid);

    if (txn->code == PING_TRANSACTION)
        return 0;

    // Equivalent to Parcel::enforceInterface(), reading the RPC
    // header with the strict mode policy mask and the interface name.
    // Note that we ignore the strict_policy and don't propagate it
    // further (since we do no outbound RPCs anyway).
    strict_policy = bio_get_uint32(msg);
//    s = bio_get_string16(msg, &len);
//    if (s == NULL) {
//        return -1;
//    }

    switch (txn->code) {
        uint32_t x;
        uint32_t age;
        case ADD:

            x = bio_get_uint32(msg);

            uint32_t y = bio_get_uint32(msg);

            uint32_t addRet = x + y;

            bio_put_uint32(reply, addRet);
            return 0;


        case CREATE_PERSON:

            age = bio_get_uint32(msg);

            char name[20];
            uint16StrToCharStr(bio_get_string16(msg, &len), name, len);

            Person person = {
                    .name=name,
                    .age=age
            };


            bio_put_obj(reply,&person);


//        case SVC_MGR_ADD_SERVICE:
//            s = bio_get_string16(msg, &len);
//            if (s == NULL) {
//                return -1;
//            }
//            handle = bio_get_ref(msg);
//            allow_isolated = bio_get_uint32(msg) ? 1 : 0;
//            dumpsys_priority = bio_get_uint32(msg);
//            if (do_add_service(bs, s, len, handle, txn->sender_euid, allow_isolated, dumpsys_priority,
//                               txn->sender_pid))
//                return -1;
//            break;

        default:
            ALOGE("unknown code %d\n", txn->code);
            return -1;
    }

    bio_put_uint32(reply, 0);
    return 0;
}


unsigned token;

int main() {

    char *driver = "/dev/binder";

    struct binder_state *bs = binder_open(driver, 128 * 1024);

    if (!bs) {
        ALOGE("failed to open binder driver %s\n", driver);
        return -1;
    }

    //注册服务
    int register_ret = svcmgr_publish(bs, SERVER_MANAGER_HANDLER, HELLO_SEVER_NAME, &token);
    ALOGE("register_ret->%d\n", register_ret);


    //监听客户端发过来的数据
    binder_loop(bs, svcmgr_handler);


}
