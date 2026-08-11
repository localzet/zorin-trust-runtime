typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long size_t;
typedef void ANativeWindow;
typedef void ANativeActivity;
typedef void AInputQueue;
typedef void AInputEvent;
typedef struct {
    int32_t width, height, stride, format;
    void*bits;
    uint32_t reserved[6];
}
B;
typedef struct {
    int32_t left, top, right, bottom;
}
R;
int32_t ANativeWindow_setBuffersGeometry(ANativeWindow*a, int32_t b, int32_t c, int32_t d) {
    return 0;
}
int32_t ANativeWindow_lock(ANativeWindow*a, B*b, R*c) {
    return -1;
}
int32_t ANativeWindow_unlockAndPost(ANativeWindow*a) {
    return 0;
}
void ANativeActivity_setWindowFlags(ANativeActivity*a, uint32_t b, uint32_t c) {
}
int32_t AInputQueue_getEvent(AInputQueue*a, AInputEvent**b) {
    return -1;
}
int32_t AInputQueue_preDispatchEvent(AInputQueue*a, AInputEvent*b) {
    return 0;
}
void AInputQueue_finishEvent(AInputQueue*a, AInputEvent*b, int c) {
}
int32_t AInputEvent_getType(const AInputEvent*a) {
    return 0;
}
int32_t AMotionEvent_getAction(const AInputEvent*a) {
    return 0;
}
float AMotionEvent_getX(const AInputEvent*a, size_t b) {
    return 0;
}
float AMotionEvent_getY(const AInputEvent*a, size_t b) {
    return 0;
}
