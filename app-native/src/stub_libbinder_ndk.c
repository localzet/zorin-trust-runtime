typedef unsigned int uint32_t;
typedef int int32_t;
typedef long long int64_t;
typedef struct AIBinder AIBinder;
typedef struct AIBinder_Class AIBinder_Class;
typedef struct AParcel AParcel;
typedef struct AStatus AStatus;
AIBinder* AServiceManager_checkService(const char* instance) {
    (void)instance;
    return(AIBinder*)0;
}
int AIBinder_ping(AIBinder* binder) {
    (void)binder;
    return -1;
}
_Bool AIBinder_isRemote(const AIBinder* binder) {
    (void)binder;
    return 0;
}
_Bool AIBinder_isAlive(const AIBinder* binder) {
    (void)binder;
    return 0;
}
void AIBinder_decStrong(AIBinder* binder) {
    (void)binder;
}
    AIBinder_Class* AIBinder_Class_define(const char*a, void*(*b)(void*), void(*c)(void*), int(*d)(AIBinder*, uint32_t,
const AParcel*, AParcel*)) {
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    return(AIBinder_Class*)1;
}
_Bool AIBinder_associateClass(AIBinder*a, const AIBinder_Class*b) {
    (void)a;
    (void)b;
    return 1;
}
int AIBinder_prepareTransaction(AIBinder*a, AParcel**b) {
    (void)a;
    (void)b;
    return -1;
}
int AIBinder_transact(AIBinder*a, uint32_t b, AParcel**c, AParcel**d, uint32_t e) {
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    (void)e;
    return -1;
}
void AParcel_delete(AParcel*a) {
    (void)a;
}
int AParcel_readStatusHeader(const AParcel*a, AStatus**b) {
    (void)a;
    (void)b;
    return -1;
}
int AParcel_readString(const AParcel*a, void*b, _Bool(*c)(void*, int32_t, char**)) {
    (void)a;
    (void)b;
    (void)c;
    return -1;
}
int AParcel_readInt32(const AParcel*a, int32_t*b) {
    (void)a;
    (void)b;
    return -1;
}
int AParcel_readInt64(const AParcel*a, int64_t*b) {
    (void)a;
    (void)b;
    return -1;
}
int AParcel_writeString(AParcel*a, const char*b, int32_t c) {
    (void)a;
    (void)b;
    (void)c;
    return -1;
}
_Bool AStatus_isOk(const AStatus*a) {
    (void)a;
    return 0;
}
int AStatus_getStatus(const AStatus*a) {
    (void)a;
    return -1;
}
const char* AStatus_getMessage(const AStatus*a) {
    (void)a;
    return "";
}
void AStatus_delete(AStatus*a) {
    (void)a;
}
