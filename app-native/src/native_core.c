// Zorin Trust Runtime v0.2.1 / Native Lab v4.1
// Native-first runtime. A tiny generated DEX hosts only TrustService lifecycle; app logic stays native.
// Interactive system/process/Binder/network/security/native probes rendered via ANativeWindow.

// ---- primitive types ----
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned long size_t;
typedef long ssize_t;
typedef long off_t;
typedef unsigned long pthread_t;

#include <jni.h>

// ---- minimal Android NDK ABI ----
typedef struct ANativeActivity ANativeActivity;
typedef struct ANativeWindow ANativeWindow;
typedef struct AInputQueue AInputQueue;
typedef struct AInputEvent AInputEvent;
typedef struct AAssetManager AAssetManager;
typedef struct AIBinder AIBinder;
typedef struct AIBinder_Class AIBinder_Class;
typedef struct AParcel AParcel;
typedef struct AStatus AStatus;

typedef struct ARect { int32_t left, top, right, bottom; } ARect;
typedef struct ANativeWindow_Buffer {
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t format;
    void* bits;
    uint32_t reserved[6];
} ANativeWindow_Buffer;

typedef struct ANativeActivityCallbacks {
    void (*onStart)(ANativeActivity* activity);
    void (*onResume)(ANativeActivity* activity);
    void* (*onSaveInstanceState)(ANativeActivity* activity, size_t* outSize);
    void (*onPause)(ANativeActivity* activity);
    void (*onStop)(ANativeActivity* activity);
    void (*onDestroy)(ANativeActivity* activity);
    void (*onWindowFocusChanged)(ANativeActivity* activity, int hasFocus);
    void (*onNativeWindowCreated)(ANativeActivity* activity, ANativeWindow* window);
    void (*onNativeWindowResized)(ANativeActivity* activity, ANativeWindow* window);
    void (*onNativeWindowRedrawNeeded)(ANativeActivity* activity, ANativeWindow* window);
    void (*onNativeWindowDestroyed)(ANativeActivity* activity, ANativeWindow* window);
    void (*onInputQueueCreated)(ANativeActivity* activity, AInputQueue* queue);
    void (*onInputQueueDestroyed)(ANativeActivity* activity, AInputQueue* queue);
    void (*onContentRectChanged)(ANativeActivity* activity, const ARect* rect);
    void (*onConfigurationChanged)(ANativeActivity* activity);
    void (*onLowMemory)(ANativeActivity* activity);
} ANativeActivityCallbacks;

struct ANativeActivity {
    ANativeActivityCallbacks* callbacks;
    JavaVM* vm;
    JNIEnv* env;
    jobject clazz;
    const char* internalDataPath;
    const char* externalDataPath;
    int32_t sdkVersion;
    void* instance;
    AAssetManager* assetManager;
    const char* obbPath;
};

// libandroid.so
extern int32_t ANativeWindow_setBuffersGeometry(ANativeWindow*, int32_t, int32_t, int32_t);
extern int32_t ANativeWindow_lock(ANativeWindow*, ANativeWindow_Buffer*, ARect*);
extern int32_t ANativeWindow_unlockAndPost(ANativeWindow*);
extern void ANativeActivity_setWindowFlags(ANativeActivity*, uint32_t, uint32_t);
extern int32_t AInputQueue_getEvent(AInputQueue*, AInputEvent**);
extern int32_t AInputQueue_preDispatchEvent(AInputQueue*, AInputEvent*);
extern void AInputQueue_finishEvent(AInputQueue*, AInputEvent*, int);
extern int32_t AInputEvent_getType(const AInputEvent*);
extern int32_t AMotionEvent_getAction(const AInputEvent*);
extern float AMotionEvent_getX(const AInputEvent*, size_t);
extern float AMotionEvent_getY(const AInputEvent*, size_t);

// ---- minimal bionic ABI ----
struct utsname { char sysname[65], nodename[65], release[65], version[65], machine[65], domainname[65]; };
struct timespec { long tv_sec; long tv_nsec; };
extern int snprintf(char*, size_t, const char*, ...);
extern int open(const char*, int, ...);
extern ssize_t read(int, void*, size_t);
extern int close(int);
extern int access(const char*, int);
extern int getpid(void);
extern unsigned int getuid(void);
extern unsigned int getgid(void);
extern int getpagesize(void);
extern int uname(struct utsname*);
extern size_t strlen(const char*);
extern void* memset(void*, int, size_t);
extern void* memcpy(void*, const void*, size_t);
extern char* strstr(const char*, const char*);
extern ssize_t readlink(const char*, char*, size_t);
extern int ioctl(int, unsigned long, ...);
extern int socket(int, int, int);
extern int connect(int, const void*, unsigned int);
extern ssize_t send(int, const void*, size_t, int);
extern ssize_t recv(int, void*, size_t, int);
extern void* mmap(void*, size_t, int, int, int, off_t);
extern int munmap(void*, size_t);
extern int mprotect(void*, size_t, int);
extern int epoll_create1(int);
extern int eventfd(unsigned int, int);
extern int pipe2(int[2], int);
extern long syscall(long, ...);
extern int clock_gettime(int, struct timespec*);
extern unsigned int usleep(unsigned int);
extern int pthread_create(pthread_t*, const void*, void* (*)(void*), void*);
extern int pthread_detach(pthread_t);
extern int __system_property_get(const char*, char*);
extern int* __errno(void);
extern int fork(void);
extern int waitpid(int, int*, int);
extern ssize_t write(int, const void*, size_t);
extern void _exit(int);
extern int dup2(int, int);
extern int execve(const char*, char* const[], char* const[]);
extern int unlink(const char*);

// Public NDK Binder API (libbinder_ndk.so, API 29+).
extern AIBinder* AServiceManager_checkService(const char*);
extern int AIBinder_ping(AIBinder*);
extern _Bool AIBinder_isRemote(const AIBinder*);
extern _Bool AIBinder_isAlive(const AIBinder*);
extern void AIBinder_decStrong(AIBinder*);
extern AIBinder_Class* AIBinder_Class_define(const char*, void* (*)(void*), void (*)(void*), int (*)(AIBinder*, uint32_t, const AParcel*, AParcel*));
extern _Bool AIBinder_associateClass(AIBinder*, const AIBinder_Class*);
extern int AIBinder_prepareTransaction(AIBinder*, AParcel**);
extern int AIBinder_transact(AIBinder*, uint32_t, AParcel**, AParcel**, uint32_t);
extern void AParcel_delete(AParcel*);
extern int AParcel_readStatusHeader(const AParcel*, AStatus**);
extern int AParcel_readString(const AParcel*, void*, _Bool (*)(void*, int32_t, char**));
extern int AParcel_readInt32(const AParcel*, int32_t*);
extern int AParcel_readInt64(const AParcel*, int64_t*);
extern int AParcel_writeString(AParcel*, const char*, int32_t);
extern _Bool AStatus_isOk(const AStatus*);
extern int AStatus_getStatus(const AStatus*);
extern const char* AStatus_getMessage(const AStatus*);
extern void AStatus_delete(AStatus*);

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 0x40
#define O_TRUNC 0x200
#define O_CLOEXEC 0x80000
#define F_OK 0
#define WINDOW_FORMAT_RGBA_8888 1
#define FLAG_FULLSCREEN 0x00000400u
#define FLAG_KEEP_SCREEN_ON 0x00000080u
#define FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS 0x80000000u
#define AINPUT_EVENT_TYPE_MOTION 2
#define AMOTION_EVENT_ACTION_MASK 0xff
#define AMOTION_EVENT_ACTION_UP 1
#define PROT_READ 1
#define PROT_WRITE 2
#define MAP_PRIVATE 2
#define MAP_ANONYMOUS 0x20
#define MAP_FAILED ((void*)(long)-1)
#define AF_UNIX 1
#define AF_INET 2
#define SOCK_STREAM 1
#define ZORIN_BRIDGE_PORT 47471

struct in_addr { uint32_t s_addr; };
struct sockaddr_in {
    uint16_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    uint8_t sin_zero[8];
};
#define CLOCK_MONOTONIC 1
#define PR_GET_SECCOMP 21
#define FUTEX_WAKE 1
#define PROP_VALUE_MAX 92
#define SIGSYS 31
#define EPERM 1
#define EACCES 13
#define ENOSYS 38
#define BINDER_VERSION_IOCTL 0xC0046209UL
#define IO_URING_SETUP_NR 425

#if defined(__aarch64__)
#define FUTEX_NR 98
#define GETTID_NR 178
#define BPF_NR 280
#define PERF_EVENT_OPEN_NR 241
#define USERFAULTFD_NR 282
#define CLONE3_NR 435
#define MOUNT_NR 40
#define SETNS_NR 268
#define PTRACE_NR 117
#elif defined(__x86_64__)
#define FUTEX_NR 202
#define GETTID_NR 186
#define BPF_NR 321
#define PERF_EVENT_OPEN_NR 298
#define USERFAULTFD_NR 323
#define CLONE3_NR 435
#define MOUNT_NR 165
#define SETNS_NR 308
#define PTRACE_NR 101
#elif defined(__arm__)
#define FUTEX_NR 240
#define GETTID_NR 224
#define BPF_NR 386
#define PERF_EVENT_OPEN_NR 364
#define USERFAULTFD_NR 388
#define CLONE3_NR 435
#define MOUNT_NR 21
#define SETNS_NR 375
#define PTRACE_NR 26
#else
#define FUTEX_NR 240
#define GETTID_NR 224
#define BPF_NR 357
#define PERF_EVENT_OPEN_NR 336
#define USERFAULTFD_NR 374
#define CLONE3_NR 435
#define MOUNT_NR 21
#define SETNS_NR 346
#define PTRACE_NR 26
#endif

static volatile ANativeWindow* g_window = 0;
static ANativeActivity* g_activity = 0;
static JavaVM* g_vm = 0;
static jobject g_app_context = 0;
static char g_files_dir[320];
static volatile int g_trust_service_alive = 0;
static jobject g_visual_view = 0;
static jobject g_visual_wm = 0;
static volatile AInputQueue* g_input_queue = 0;
static volatile int g_running = 1; // trust/runtime process lifetime
static volatile int g_ui_alive = 0; // NativeActivity instance lifetime
static volatile unsigned int g_ui_generation = 0;
static volatile int g_selected_tab = 2;
static volatile unsigned int g_run_counter = 1;
static volatile int g_width = 0;
static volatile int g_height = 0;
static int g_tab_rects[9][4];
static int g_run_rect[4];
static int g_copy_rect[4];
static char g_copy_status[64] = "COPY ALL";
static char g_bridge_token[96];
static int g_bridge_token_saved = 0;
static char g_report[65536];
static int g_report_len = 0;
static int g_collect_report = 0;

static int last_errno(void) { int* p = __errno(); return p ? *p : -1; }

static uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return 0xff000000u | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
}

static int mini_streq(const char* a, const char* b) {
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a == 0 && *b == 0;
}

static void trim_line(char* s) {
    int n = (int)strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || s[n-1] == ' ' || s[n-1] == '\t')) s[--n] = 0;
}

static uint8_t glyph_row(char c, int row) {
    if (c >= 'a' && c <= 'z') c = (char)(c - 32);
    static const uint8_t digits[10][7] = {
        {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},{2,6,10,18,31,2,2},
        {31,16,16,30,1,1,30},{14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,1,14}
    };
    if (c >= '0' && c <= '9') return digits[c-'0'][row];
    switch (c) {
        case 'A': { static const uint8_t g[7]={14,17,17,31,17,17,17}; return g[row]; }
        case 'B': { static const uint8_t g[7]={30,17,17,30,17,17,30}; return g[row]; }
        case 'C': { static const uint8_t g[7]={14,17,16,16,16,17,14}; return g[row]; }
        case 'D': { static const uint8_t g[7]={30,17,17,17,17,17,30}; return g[row]; }
        case 'E': { static const uint8_t g[7]={31,16,16,30,16,16,31}; return g[row]; }
        case 'F': { static const uint8_t g[7]={31,16,16,30,16,16,16}; return g[row]; }
        case 'G': { static const uint8_t g[7]={14,17,16,23,17,17,15}; return g[row]; }
        case 'H': { static const uint8_t g[7]={17,17,17,31,17,17,17}; return g[row]; }
        case 'I': { static const uint8_t g[7]={14,4,4,4,4,4,14}; return g[row]; }
        case 'J': { static const uint8_t g[7]={7,2,2,2,18,18,12}; return g[row]; }
        case 'K': { static const uint8_t g[7]={17,18,20,24,20,18,17}; return g[row]; }
        case 'L': { static const uint8_t g[7]={16,16,16,16,16,16,31}; return g[row]; }
        case 'M': { static const uint8_t g[7]={17,27,21,21,17,17,17}; return g[row]; }
        case 'N': { static const uint8_t g[7]={17,25,21,19,17,17,17}; return g[row]; }
        case 'O': { static const uint8_t g[7]={14,17,17,17,17,17,14}; return g[row]; }
        case 'P': { static const uint8_t g[7]={30,17,17,30,16,16,16}; return g[row]; }
        case 'Q': { static const uint8_t g[7]={14,17,17,17,21,18,13}; return g[row]; }
        case 'R': { static const uint8_t g[7]={30,17,17,30,20,18,17}; return g[row]; }
        case 'S': { static const uint8_t g[7]={15,16,16,14,1,1,30}; return g[row]; }
        case 'T': { static const uint8_t g[7]={31,4,4,4,4,4,4}; return g[row]; }
        case 'U': { static const uint8_t g[7]={17,17,17,17,17,17,14}; return g[row]; }
        case 'V': { static const uint8_t g[7]={17,17,17,17,17,10,4}; return g[row]; }
        case 'W': { static const uint8_t g[7]={17,17,17,21,21,21,10}; return g[row]; }
        case 'X': { static const uint8_t g[7]={17,17,10,4,10,17,17}; return g[row]; }
        case 'Y': { static const uint8_t g[7]={17,17,10,4,4,4,4}; return g[row]; }
        case 'Z': { static const uint8_t g[7]={31,1,2,4,8,16,31}; return g[row]; }
        case ':': { static const uint8_t g[7]={0,4,4,0,4,4,0}; return g[row]; }
        case '.': { static const uint8_t g[7]={0,0,0,0,0,6,6}; return g[row]; }
        case '/': { static const uint8_t g[7]={1,2,2,4,8,8,16}; return g[row]; }
        case '-': { static const uint8_t g[7]={0,0,0,31,0,0,0}; return g[row]; }
        case '_': { static const uint8_t g[7]={0,0,0,0,0,0,31}; return g[row]; }
        case '[': { static const uint8_t g[7]={14,8,8,8,8,8,14}; return g[row]; }
        case ']': { static const uint8_t g[7]={14,2,2,2,2,2,14}; return g[row]; }
        case '(': { static const uint8_t g[7]={2,4,8,8,8,4,2}; return g[row]; }
        case ')': { static const uint8_t g[7]={8,4,2,2,2,4,8}; return g[row]; }
        case '+': { static const uint8_t g[7]={0,4,4,31,4,4,0}; return g[row]; }
        case '=': { static const uint8_t g[7]={0,31,0,31,0,0,0}; return g[row]; }
        case '?': { static const uint8_t g[7]={14,17,1,2,4,0,4}; return g[row]; }
        case '#': { static const uint8_t g[7]={10,31,10,10,31,10,0}; return g[row]; }
        case '<': { static const uint8_t g[7]={2,4,8,16,8,4,2}; return g[row]; }
        case '>': { static const uint8_t g[7]={8,4,2,1,2,4,8}; return g[row]; }
        case '!': { static const uint8_t g[7]={4,4,4,4,4,0,4}; return g[row]; }
        case ' ': return 0;
        default:  { static const uint8_t g[7]={31,17,21,21,21,17,31}; return g[row]; }
    }
}

static void fill_rect(ANativeWindow_Buffer* b, int x, int y, int w, int h, uint32_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > b->width) w = b->width - x;
    if (y + h > b->height) h = b->height - y;
    if (w <= 0 || h <= 0) return;
    uint32_t* p = (uint32_t*)b->bits;
    for (int yy=0; yy<h; ++yy) {
        uint32_t* row = p + (y+yy)*b->stride + x;
        for (int xx=0; xx<w; ++xx) row[xx] = color;
    }
}

static void outline_rect(ANativeWindow_Buffer* b, int x, int y, int w, int h, int t, uint32_t c) {
    fill_rect(b, x, y, w, t, c); fill_rect(b, x, y+h-t, w, t, c);
    fill_rect(b, x, y, t, h, c); fill_rect(b, x+w-t, y, t, h, c);
}

static void fill_circle(ANativeWindow_Buffer* b, int cx, int cy, int r, uint32_t color) {
    if(!b||r<=0)return;
    uint32_t* p=(uint32_t*)b->bits;
    int r2=r*r;
    int y0=cy-r,y1=cy+r; if(y0<0)y0=0;if(y1>=b->height)y1=b->height-1;
    int x0=cx-r,x1=cx+r;if(x0<0)x0=0;if(x1>=b->width)x1=b->width-1;
    for(int y=y0;y<=y1;++y){int dy=y-cy;for(int x=x0;x<=x1;++x){int dx=x-cx;if(dx*dx+dy*dy<=r2)p[y*b->stride+x]=color;}}
}

static void draw_ring(ANativeWindow_Buffer* b,int cx,int cy,int r,int thickness,uint32_t color,uint32_t inner){
    if(thickness<1)thickness=1;fill_circle(b,cx,cy,r,color);fill_circle(b,cx,cy,r-thickness,inner);
}

static void draw_char(ANativeWindow_Buffer* b, int x, int y, char c, int scale, uint32_t color) {
    for (int row=0; row<7; ++row) {
        uint8_t bits = glyph_row(c, row);
        for (int col=0; col<5; ++col)
            if (bits & (1u << (4-col))) fill_rect(b, x + col*scale, y + row*scale, scale, scale, color);
    }
}

static void draw_text(ANativeWindow_Buffer* b, int x, int y, const char* s, int scale, uint32_t color) {
    int start = x;
    for (; *s; ++s) {
        if (*s == '\n') { y += 9*scale; x = start; continue; }
        draw_char(b, x, y, *s, scale, color);
        x += 6*scale;
        if (x + 6*scale >= b->width - 16) { y += 9*scale; x = start; }
    }
}

static int text_width(const char* s, int scale) { return (int)strlen(s) * 6 * scale; }

static int read_small(const char* path, char* out, int cap) {
    if (cap <= 1) return -1;
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) { out[0]=0; return -1; }
    ssize_t n = read(fd, out, (size_t)(cap-1));
    close(fd);
    if (n < 0) { out[0]=0; return -1; }
    out[n] = 0;
    return (int)n;
}

static int readlink_small(const char* path, char* out, int cap) {
    if (cap <= 1) return -1;
    ssize_t n = readlink(path, out, (size_t)(cap-1));
    if (n < 0) { out[0]=0; return -1; }
    out[n] = 0;
    return (int)n;
}

static int prop_get(const char* name, char* out, int cap) {
    char tmp[PROP_VALUE_MAX];
    int n = __system_property_get(name, tmp);
    if (n <= 0) { if (cap) out[0]=0; return 0; }
    int copy = n < cap-1 ? n : cap-1;
    if (copy < 0) copy = 0;
    for (int i=0; i<copy; ++i) out[i]=tmp[i];
    out[copy]=0;
    return copy;
}

static int extract_status(const char* key, char* out, int cap) {
    char b[8192];
    if (read_small("/proc/self/status", b, (int)sizeof(b)) <= 0) return -1;
    char* p = strstr(b, key);
    if (!p) return -1;
    p += strlen(key);
    while (*p == ' ' || *p == '\t') ++p;
    int n=0;
    while (*p && *p != '\n' && n < cap-1) out[n++]=*p++;
    out[n]=0;
    trim_line(out);
    return n;
}

static int count_file_lines_ex(const char* path, int* out_errno) {
    if (out_errno) *out_errno = 0;
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) { if (out_errno) *out_errno = last_errno(); return -1; }
    char b[4096]; int lines=0; ssize_t n;
    while ((n=read(fd,b,sizeof(b))) > 0) for (ssize_t i=0;i<n;++i) if (b[i]=='\n') ++lines;
    if (n < 0 && out_errno) *out_errno = last_errno();
    close(fd); return n < 0 ? -1 : lines;
}

static int count_file_lines(const char* path) {
    return count_file_lines_ex(path, 0);
}

static int approx_fd_count(void) {
    int n=0; char p[64], v[8];
    for (int i=0;i<512;++i) {
        snprintf(p,sizeof(p),"/proc/self/fd/%d",i);
        if (readlink_small(p,v,sizeof(v)) >= 0) ++n;
    }
    return n;
}

static const char* selinux_mode(char* scratch, int cap) {
    int e = last_errno(); (void)e;
    char b[16];
    if (read_small("/sys/fs/selinux/enforce", b, sizeof(b)) > 0) {
        if (b[0]=='1') return "ENFORCING";
        if (b[0]=='0') return "PERMISSIVE";
    }
    snprintf(scratch, (size_t)cap, "UNKNOWN ERRNO %d", last_errno());
    return scratch;
}

static void report_reset(void) {
    g_report_len = 0;
    g_report[0] = 0;
}

static void report_append(const char* s) {
    if (!s) return;
    int n = (int)strlen(s);
    int left = (int)sizeof(g_report) - 1 - g_report_len;
    if (left <= 0) return;
    if (n > left) n = left;
    memcpy(g_report + g_report_len, s, (size_t)n);
    g_report_len += n;
    g_report[g_report_len] = 0;
}

static void report_kv_line(const char* label, const char* value) {
    char line[512];
    snprintf(line, sizeof(line), "%-16s %s\n", label, value && *value ? value : "<N/A>");
    report_append(line);
}

static jobject runtime_context(void) {
    if (g_app_context) return g_app_context;
    return (g_activity && g_activity->clazz) ? g_activity->clazz : 0;
}

static int init_runtime_context_from(JNIEnv* env, jobject obj) {
    if (!env || !obj) return -1;
    if (!g_app_context) {
        jclass cls = (*env)->GetObjectClass(env, obj);
        jmethodID mid = cls ? (*env)->GetMethodID(env, cls, "getApplicationContext", "()Landroid/content/Context;") : 0;
        jobject ctx = mid ? (*env)->CallObjectMethod(env, obj, mid) : 0;
        if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); ctx = 0; }
        jobject base = ctx ? ctx : obj;
        g_app_context = (*env)->NewGlobalRef(env, base);
        if (ctx) (*env)->DeleteLocalRef(env, ctx);
        if (cls) (*env)->DeleteLocalRef(env, cls);
    }
    if (g_app_context && !g_files_dir[0]) {
        jclass cc = (*env)->GetObjectClass(env, g_app_context);
        jmethodID gfd = cc ? (*env)->GetMethodID(env, cc, "getFilesDir", "()Ljava/io/File;") : 0;
        jobject f = gfd ? (*env)->CallObjectMethod(env, g_app_context, gfd) : 0;
        jclass fc = f ? (*env)->GetObjectClass(env, f) : 0;
        jmethodID gap = fc ? (*env)->GetMethodID(env, fc, "getAbsolutePath", "()Ljava/lang/String;") : 0;
        jstring ps = gap ? (jstring)(*env)->CallObjectMethod(env, f, gap) : 0;
        if (ps) {
            const char* c = (*env)->GetStringUTFChars(env, ps, 0);
            if (c) { snprintf(g_files_dir, sizeof(g_files_dir), "%s", c); (*env)->ReleaseStringUTFChars(env, ps, c); }
        }
        if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); g_files_dir[0]=0; }
        if (ps) (*env)->DeleteLocalRef(env, ps);
        if (fc) (*env)->DeleteLocalRef(env, fc);
        if (f) (*env)->DeleteLocalRef(env, f);
        if (cc) (*env)->DeleteLocalRef(env, cc);
    }
    return g_app_context ? 0 : -2;
}

static int init_runtime_context(ANativeActivity* activity) {
    if (!activity || !activity->vm || !activity->env || !activity->clazz) return -1;
    g_vm = activity->vm;
    return init_runtime_context_from(activity->env, activity->clazz);
}

static void move_activity_to_back(ANativeActivity* activity) {
    if (!activity || !activity->env || !activity->clazz) return;
    JNIEnv* env = activity->env;
    jclass cls = (*env)->GetObjectClass(env, activity->clazz);
    jmethodID mid = cls ? (*env)->GetMethodID(env, cls, "moveTaskToBack", "(Z)Z") : 0;
    if (mid) (void)(*env)->CallBooleanMethod(env, activity->clazz, mid, JNI_TRUE);
    if ((*env)->ExceptionCheck(env)) (*env)->ExceptionClear(env);
    if (cls) (*env)->DeleteLocalRef(env, cls);
}

static int copy_text_to_clipboard(const char* text) {
    if (!g_activity || !g_activity->vm || !g_activity->clazz || !text) return -1;
    JNIEnv* env = 0;
    int attached = 0;
    JavaVM* vm = g_activity->vm;
    jint ge = (*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6);
    if (ge != JNI_OK) {
        if ((*vm)->AttachCurrentThread(vm, (void**)&env, 0) != JNI_OK || !env) return -2;
        attached = 1;
    }

    int rc = 0;
    jclass activity_cls = (*env)->GetObjectClass(env, g_activity->clazz);
    jmethodID get_service = activity_cls ? (*env)->GetMethodID(env, activity_cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;") : 0;
    jstring service_name = (*env)->NewStringUTF(env, "clipboard");
    jobject clipboard = (get_service && service_name) ? (*env)->CallObjectMethod(env, g_activity->clazz, get_service, service_name) : 0;

    jclass clipdata_cls = (*env)->FindClass(env, "android/content/ClipData");
    jmethodID new_plain = clipdata_cls ? (*env)->GetStaticMethodID(env, clipdata_cls, "newPlainText", "(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Landroid/content/ClipData;") : 0;
    jstring label = (*env)->NewStringUTF(env, "Zorin Trust Runtime diagnostics");
    jstring body = (*env)->NewStringUTF(env, text);
    jobject clip = (new_plain && label && body) ? (*env)->CallStaticObjectMethod(env, clipdata_cls, new_plain, label, body) : 0;

    jclass clipboard_cls = clipboard ? (*env)->GetObjectClass(env, clipboard) : 0;
    jmethodID set_clip = clipboard_cls ? (*env)->GetMethodID(env, clipboard_cls, "setPrimaryClip", "(Landroid/content/ClipData;)V") : 0;
    if (set_clip && clip) (*env)->CallVoidMethod(env, clipboard, set_clip, clip);
    else rc = -3;

    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionClear(env);
        rc = -4;
    }

    if (clip) (*env)->DeleteLocalRef(env, clip);
    if (body) (*env)->DeleteLocalRef(env, body);
    if (label) (*env)->DeleteLocalRef(env, label);
    if (clipdata_cls) (*env)->DeleteLocalRef(env, clipdata_cls);
    if (clipboard_cls) (*env)->DeleteLocalRef(env, clipboard_cls);
    if (clipboard) (*env)->DeleteLocalRef(env, clipboard);
    if (service_name) (*env)->DeleteLocalRef(env, service_name);
    if (activity_cls) (*env)->DeleteLocalRef(env, activity_cls);
    if (attached) (*vm)->DetachCurrentThread(vm);
    return rc;
}

#include "trust_runtime.inc"

// ---- TrustService lifecycle + Trust Visual Channel ---------------------------------
// TrustService is a deliberately tiny DEX shim. Everything below stays native.
#define ZTRUST_SERVICE_CLASS "dev.zorin.trustruntime.TrustService"
#define ZTRUST_PKG "dev.zorin.trustruntime"
static int g_trust_visual_rect[4];

static int trust_intent_bool_obj(JNIEnv* env, jobject intent, const char* key) {
    if (!env || !intent || !key) return 0;
    jclass ic = (*env)->GetObjectClass(env, intent);
    jmethodID gb = ic ? (*env)->GetMethodID(env, ic, "getBooleanExtra", "(Ljava/lang/String;Z)Z") : 0;
    jstring jk = (*env)->NewStringUTF(env, key);
    int v = (gb && jk) ? ((*env)->CallBooleanMethod(env, intent, gb, jk, JNI_FALSE) ? 1 : 0) : 0;
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); v = 0; }
    if (jk) (*env)->DeleteLocalRef(env, jk);
    if (ic) (*env)->DeleteLocalRef(env, ic);
    return v;
}

static int trust_overlay_allowed(void) {
    jobject ctx = runtime_context();
    if (!ctx) return 0;
    JNIEnv* env = 0; int attached = 0;
    if (trust_attach(&env, &attached) != 0) return 0;
    jclass sc = (*env)->FindClass(env, "android/provider/Settings");
    jmethodID mid = sc ? (*env)->GetStaticMethodID(env, sc, "canDrawOverlays", "(Landroid/content/Context;)Z") : 0;
    int ok = (mid && sc) ? ((*env)->CallStaticBooleanMethod(env, sc, mid, ctx) ? 1 : 0) : 0;
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); ok = 0; }
    if (sc) (*env)->DeleteLocalRef(env, sc);
    trust_detach(attached);
    return ok;
}

static int trust_request_overlay_permission(void) {
    jobject ctx = runtime_context();
    if (!ctx) return -1;
    JNIEnv* env = 0; int attached = 0;
    if (trust_attach(&env, &attached) != 0) return -2;
    int rc = -3;
    jclass ic = (*env)->FindClass(env, "android/content/Intent");
    jmethodID ctor = ic ? (*env)->GetMethodID(env, ic, "<init>", "(Ljava/lang/String;)V") : 0;
    jstring action = (*env)->NewStringUTF(env, "android.settings.action.MANAGE_OVERLAY_PERMISSION");
    jobject in = (ctor && action) ? (*env)->NewObject(env, ic, ctor, action) : 0;
    jclass uc = (*env)->FindClass(env, "android/net/Uri");
    jmethodID parse = uc ? (*env)->GetStaticMethodID(env, uc, "parse", "(Ljava/lang/String;)Landroid/net/Uri;") : 0;
    jstring uri_s = (*env)->NewStringUTF(env, "package:" ZTRUST_PKG);
    jobject uri = (parse && uri_s) ? (*env)->CallStaticObjectMethod(env, uc, parse, uri_s) : 0;
    jmethodID setData = ic ? (*env)->GetMethodID(env, ic, "setData", "(Landroid/net/Uri;)Landroid/content/Intent;") : 0;
    jmethodID addFlags = ic ? (*env)->GetMethodID(env, ic, "addFlags", "(I)Landroid/content/Intent;") : 0;
    if (in && uri && setData) { jobject tmp = (*env)->CallObjectMethod(env, in, setData, uri); if (tmp) (*env)->DeleteLocalRef(env, tmp); }
    if (in && addFlags) { jobject tmp = (*env)->CallObjectMethod(env, in, addFlags, 0x10000000); if (tmp) (*env)->DeleteLocalRef(env, tmp); }
    jclass cc = (*env)->GetObjectClass(env, ctx);
    jmethodID start = cc ? (*env)->GetMethodID(env, cc, "startActivity", "(Landroid/content/Intent;)V") : 0;
    if (start && in) { (*env)->CallVoidMethod(env, ctx, start, in); rc = 0; }
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); rc = -4; }
    if (cc) (*env)->DeleteLocalRef(env, cc);
    if (uri) (*env)->DeleteLocalRef(env, uri);
    if (uri_s) (*env)->DeleteLocalRef(env, uri_s);
    if (uc) (*env)->DeleteLocalRef(env, uc);
    if (in) (*env)->DeleteLocalRef(env, in);
    if (action) (*env)->DeleteLocalRef(env, action);
    if (ic) (*env)->DeleteLocalRef(env, ic);
    trust_detach(attached);
    return rc;
}

static int trust_start_service_from_context(jobject ctx, int pulse) {
    if (!ctx) return -1;
    JNIEnv* env = 0; int attached = 0;
    if (trust_attach(&env, &attached) != 0) return -2;
    int rc = -3;
    jclass ic = (*env)->FindClass(env, "android/content/Intent");
    jmethodID ctor = ic ? (*env)->GetMethodID(env, ic, "<init>", "()V") : 0;
    jobject in = ctor ? (*env)->NewObject(env, ic, ctor) : 0;
    jmethodID scn = ic ? (*env)->GetMethodID(env, ic, "setClassName", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;") : 0;
    jmethodID pe = ic ? (*env)->GetMethodID(env, ic, "putExtra", "(Ljava/lang/String;Z)Landroid/content/Intent;") : 0;
    jstring pkg = (*env)->NewStringUTF(env, ZTRUST_PKG);
    jstring cls = (*env)->NewStringUTF(env, ZTRUST_SERVICE_CLASS);
    if (in && scn && pkg && cls) { jobject t = (*env)->CallObjectMethod(env, in, scn, pkg, cls); if (t) (*env)->DeleteLocalRef(env, t); }
    jstring ensurek = (*env)->NewStringUTF(env, "dev.zorin.trust.ensure");
    if (in && pe && ensurek) { jobject t = (*env)->CallObjectMethod(env, in, pe, ensurek, JNI_TRUE); if (t) (*env)->DeleteLocalRef(env, t); }
    if (pulse) {
        jstring pulsek = (*env)->NewStringUTF(env, "dev.zorin.trust.pulse");
        if (in && pe && pulsek) { jobject t = (*env)->CallObjectMethod(env, in, pe, pulsek, JNI_TRUE); if (t) (*env)->DeleteLocalRef(env, t); }
        if (pulsek) (*env)->DeleteLocalRef(env, pulsek);
    }
    jclass cc = (*env)->GetObjectClass(env, ctx);
    jmethodID sf = cc ? (*env)->GetMethodID(env, cc, "startForegroundService", "(Landroid/content/Intent;)Landroid/content/ComponentName;") : 0;
    jobject comp = (sf && in) ? (*env)->CallObjectMethod(env, ctx, sf, in) : 0;
    if (comp) { rc = 0; (*env)->DeleteLocalRef(env, comp); }
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); rc = -4; }
    if (cc) (*env)->DeleteLocalRef(env, cc);
    if (ensurek) (*env)->DeleteLocalRef(env, ensurek);
    if (cls) (*env)->DeleteLocalRef(env, cls);
    if (pkg) (*env)->DeleteLocalRef(env, pkg);
    if (in) (*env)->DeleteLocalRef(env, in);
    if (ic) (*env)->DeleteLocalRef(env, ic);
    trust_detach(attached);
    return rc;
}

static int trust_start_foreground_notification(JNIEnv* env, jobject service) {
    if (!env || !service) return -1;
    int rc = -2;
    jclass sc = (*env)->GetObjectClass(env, service);
    jmethodID gss = sc ? (*env)->GetMethodID(env, sc, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;") : 0;
    jstring ns = (*env)->NewStringUTF(env, "notification");
    jobject nm = (gss && ns) ? (*env)->CallObjectMethod(env, service, gss, ns) : 0;

    jstring channel_id = (*env)->NewStringUTF(env, "zorin_trust");
    jstring channel_name = (*env)->NewStringUTF(env, "Zorin Trust");
    jclass ncc = (*env)->FindClass(env, "android/app/NotificationChannel");
    jmethodID ncctor = ncc ? (*env)->GetMethodID(env, ncc, "<init>", "(Ljava/lang/String;Ljava/lang/CharSequence;I)V") : 0;
    jobject channel = (ncctor && channel_id && channel_name) ? (*env)->NewObject(env, ncc, ncctor, channel_id, channel_name, 2) : 0;
    jclass nmc = nm ? (*env)->GetObjectClass(env, nm) : 0;
    jmethodID cnc = nmc ? (*env)->GetMethodID(env, nmc, "createNotificationChannel", "(Landroid/app/NotificationChannel;)V") : 0;
    if (cnc && channel) (*env)->CallVoidMethod(env, nm, cnc, channel);

    // Use a stable public framework drawable directly. The previous implementation
    // queried OEM/system resources by name; on some HyperOS builds that can yield 0,
    // producing an invalid foreground notification and causing Android to kill the
    // service shortly after start. android.R.drawable.ic_lock_lock = 0x0108002f.
    int icon = 0x0108002f;
    jclass rccls = 0;
    jobject res = 0;
    jstring iname = 0;
    jstring dtype = 0;
    jstring apkg = 0;

    jclass bc = (*env)->FindClass(env, "android/app/Notification$Builder");
    jmethodID bctor = bc ? (*env)->GetMethodID(env, bc, "<init>", "(Landroid/content/Context;Ljava/lang/String;)V") : 0;
    jobject b = (bctor && channel_id) ? (*env)->NewObject(env, bc, bctor, service, channel_id) : 0;
    jmethodID ssi = bc ? (*env)->GetMethodID(env, bc, "setSmallIcon", "(I)Landroid/app/Notification$Builder;") : 0;
    jmethodID sct = bc ? (*env)->GetMethodID(env, bc, "setContentTitle", "(Ljava/lang/CharSequence;)Landroid/app/Notification$Builder;") : 0;
    jmethodID scx = bc ? (*env)->GetMethodID(env, bc, "setContentText", "(Ljava/lang/CharSequence;)Landroid/app/Notification$Builder;") : 0;
    jmethodID song = bc ? (*env)->GetMethodID(env, bc, "setOngoing", "(Z)Landroid/app/Notification$Builder;") : 0;
    jmethodID build = bc ? (*env)->GetMethodID(env, bc, "build", "()Landroid/app/Notification;") : 0;
    jstring title = (*env)->NewStringUTF(env, "Zorin Trust");
    jstring text = (*env)->NewStringUTF(env, "Owner-device trust runtime active");
    if (b && ssi && icon) { jobject t=(*env)->CallObjectMethod(env,b,ssi,icon); if(t)(*env)->DeleteLocalRef(env,t); }
    if (b && sct && title) { jobject t=(*env)->CallObjectMethod(env,b,sct,title); if(t)(*env)->DeleteLocalRef(env,t); }
    if (b && scx && text) { jobject t=(*env)->CallObjectMethod(env,b,scx,text); if(t)(*env)->DeleteLocalRef(env,t); }
    if (b && song) { jobject t=(*env)->CallObjectMethod(env,b,song,JNI_TRUE); if(t)(*env)->DeleteLocalRef(env,t); }
    jobject notif = (b && build) ? (*env)->CallObjectMethod(env,b,build) : 0;
    jmethodID sf = sc ? (*env)->GetMethodID(env,sc,"startForeground","(ILandroid/app/Notification;)V") : 0;
    if (sf && notif) { (*env)->CallVoidMethod(env, service, sf, 7301, notif); rc=0; }
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); rc=-3; }

    if(notif)(*env)->DeleteLocalRef(env,notif); if(text)(*env)->DeleteLocalRef(env,text); if(title)(*env)->DeleteLocalRef(env,title);
    if(b)(*env)->DeleteLocalRef(env,b); if(bc)(*env)->DeleteLocalRef(env,bc);
    if(apkg)(*env)->DeleteLocalRef(env,apkg); if(dtype)(*env)->DeleteLocalRef(env,dtype); if(iname)(*env)->DeleteLocalRef(env,iname);
    if(res)(*env)->DeleteLocalRef(env,res); if(rccls)(*env)->DeleteLocalRef(env,rccls);
    if(nmc)(*env)->DeleteLocalRef(env,nmc); if(channel)(*env)->DeleteLocalRef(env,channel); if(ncc)(*env)->DeleteLocalRef(env,ncc);
    if(channel_name)(*env)->DeleteLocalRef(env,channel_name); if(channel_id)(*env)->DeleteLocalRef(env,channel_id);
    if(nm)(*env)->DeleteLocalRef(env,nm); if(ns)(*env)->DeleteLocalRef(env,ns); if(sc)(*env)->DeleteLocalRef(env,sc);
    return rc;
}

static int trust_visual_show(JNIEnv* env, jobject service) {
    if (!env || !service) return -1;
    if (!trust_overlay_allowed()) return -2;
    // Re-use one transparent overlay view for the process lifetime; each pulse animates
    // from a compact bright red ring to a larger transparent ring.
    jobject view = g_visual_view;
    if (!view) {
        jclass sc = (*env)->GetObjectClass(env, service);
        jmethodID gss = sc ? (*env)->GetMethodID(env, sc, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;") : 0;
        jstring wn = (*env)->NewStringUTF(env, "window");
        jobject wm = (gss && wn) ? (*env)->CallObjectMethod(env, service, gss, wn) : 0;
        jclass tc = (*env)->FindClass(env, "android/widget/TextView");
        jmethodID tctor = tc ? (*env)->GetMethodID(env, tc, "<init>", "(Landroid/content/Context;)V") : 0;
        jobject tv = tctor ? (*env)->NewObject(env, tc, tctor, service) : 0;
        jstring txt = (*env)->NewStringUTF(env, "ZORIN\nOWNER LINKED");
        jmethodID st = tc ? (*env)->GetMethodID(env, tc, "setText", "(Ljava/lang/CharSequence;)V") : 0;
        jmethodID stc = tc ? (*env)->GetMethodID(env, tc, "setTextColor", "(I)V") : 0;
        jmethodID sts = tc ? (*env)->GetMethodID(env, tc, "setTextSize", "(F)V") : 0;
        jmethodID sg = tc ? (*env)->GetMethodID(env, tc, "setGravity", "(I)V") : 0;
        if (tv && st && txt) (*env)->CallVoidMethod(env, tv, st, txt);
        if (tv && stc) (*env)->CallVoidMethod(env, tv, stc, (jint)0xffffffffu);
        if (tv && sts) (*env)->CallVoidMethod(env, tv, sts, (jfloat)18.0f);
        if (tv && sg) (*env)->CallVoidMethod(env, tv, sg, 17);

        float density=1.0f;
        jclass cc=(*env)->GetObjectClass(env,service); jmethodID gr=cc?(*env)->GetMethodID(env,cc,"getResources","()Landroid/content/res/Resources;"):0;
        jobject r=gr?(*env)->CallObjectMethod(env,service,gr):0; jclass rc=r?(*env)->GetObjectClass(env,r):0;
        jmethodID gdm=rc?(*env)->GetMethodID(env,rc,"getDisplayMetrics","()Landroid/util/DisplayMetrics;"):0; jobject dm=gdm?(*env)->CallObjectMethod(env,r,gdm):0;
        jclass dmc=dm?(*env)->GetObjectClass(env,dm):0; jfieldID df=dmc?(*env)->GetFieldID(env,dmc,"density","F"):0; if(dm&&df)density=(*env)->GetFloatField(env,dm,df);
        int px=(int)(236.0f*density); if(px<180)px=180; int stroke=(int)(7.0f*density); if(stroke<5)stroke=5;

        jclass gc = (*env)->FindClass(env, "android/graphics/drawable/GradientDrawable");
        jmethodID gctor = gc ? (*env)->GetMethodID(env, gc, "<init>", "()V") : 0;
        jobject gd = gctor ? (*env)->NewObject(env, gc, gctor) : 0;
        jmethodID shape = gc ? (*env)->GetMethodID(env,gc,"setShape","(I)V") : 0;
        jmethodID color = gc ? (*env)->GetMethodID(env,gc,"setColor","(I)V") : 0;
        jmethodID strk = gc ? (*env)->GetMethodID(env,gc,"setStroke","(II)V") : 0;
        if(gd&&shape)(*env)->CallVoidMethod(env,gd,shape,1);
        if(gd&&color)(*env)->CallVoidMethod(env,gd,color,(jint)0x28ff1010u);
        if(gd&&strk)(*env)->CallVoidMethod(env,gd,strk,stroke,(jint)0xffff2020u);
        jmethodID sbg = tc ? (*env)->GetMethodID(env,tc,"setBackground","(Landroid/graphics/drawable/Drawable;)V") : 0;
        if(tv&&sbg&&gd)(*env)->CallVoidMethod(env,tv,sbg,gd);

        jclass lpc = (*env)->FindClass(env, "android/view/WindowManager$LayoutParams");
        jmethodID lpctor = lpc ? (*env)->GetMethodID(env,lpc,"<init>","(IIIII)V") : 0;
        jobject lp = lpctor ? (*env)->NewObject(env,lpc,lpctor,px,px,2038,0x118,-3) : 0; // OVERLAY, NOT_FOCUSABLE|NOT_TOUCHABLE|LAYOUT_IN_SCREEN, TRANSLUCENT
        jfieldID grav = lpc ? (*env)->GetFieldID(env,lpc,"gravity","I") : 0;
        if(lp&&grav)(*env)->SetIntField(env,lp,grav,17);
        jclass wmc = wm ? (*env)->GetObjectClass(env,wm) : 0;
        jmethodID add = wmc ? (*env)->GetMethodID(env,wmc,"addView","(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V") : 0;
        if(wm&&tv&&lp&&add)(*env)->CallVoidMethod(env,wm,add,tv,lp);
        if(!(*env)->ExceptionCheck(env) && wm && tv){g_visual_wm=(*env)->NewGlobalRef(env,wm);g_visual_view=(*env)->NewGlobalRef(env,tv);view=g_visual_view;}
        if((*env)->ExceptionCheck(env))(*env)->ExceptionClear(env);
        if(wmc)(*env)->DeleteLocalRef(env,wmc); if(lp)(*env)->DeleteLocalRef(env,lp); if(lpc)(*env)->DeleteLocalRef(env,lpc);
        if(gd)(*env)->DeleteLocalRef(env,gd); if(gc)(*env)->DeleteLocalRef(env,gc);
        if(dmc)(*env)->DeleteLocalRef(env,dmc); if(dm)(*env)->DeleteLocalRef(env,dm); if(rc)(*env)->DeleteLocalRef(env,rc); if(r)(*env)->DeleteLocalRef(env,r); if(cc)(*env)->DeleteLocalRef(env,cc);
        if(txt)(*env)->DeleteLocalRef(env,txt); if(tv)(*env)->DeleteLocalRef(env,tv); if(tc)(*env)->DeleteLocalRef(env,tc);
        if(wm)(*env)->DeleteLocalRef(env,wm); if(wn)(*env)->DeleteLocalRef(env,wn); if(sc)(*env)->DeleteLocalRef(env,sc);
    }
    if (!view) return -3;
    jclass vc=(*env)->GetObjectClass(env,view);
    jmethodID sa=vc?(*env)->GetMethodID(env,vc,"setAlpha","(F)V"):0;
    jmethodID ssx=vc?(*env)->GetMethodID(env,vc,"setScaleX","(F)V"):0;
    jmethodID ssy=vc?(*env)->GetMethodID(env,vc,"setScaleY","(F)V"):0;
    jmethodID anim=vc?(*env)->GetMethodID(env,vc,"animate","()Landroid/view/ViewPropertyAnimator;"):0;
    if(sa)(*env)->CallVoidMethod(env,view,sa,(jfloat)1.0f); if(ssx)(*env)->CallVoidMethod(env,view,ssx,(jfloat)0.72f); if(ssy)(*env)->CallVoidMethod(env,view,ssy,(jfloat)0.72f);
    jobject a=anim?(*env)->CallObjectMethod(env,view,anim):0; jclass ac=a?(*env)->GetObjectClass(env,a):0;
    jmethodID cancel=ac?(*env)->GetMethodID(env,ac,"cancel","()V"):0; if(a&&cancel)(*env)->CallVoidMethod(env,a,cancel);
    // Get a fresh animator after cancel and configure expand+fade.
    if(a)(*env)->DeleteLocalRef(env,a); if(ac)(*env)->DeleteLocalRef(env,ac); a=anim?(*env)->CallObjectMethod(env,view,anim):0; ac=a?(*env)->GetObjectClass(env,a):0;
    jmethodID aa=ac?(*env)->GetMethodID(env,ac,"alpha","(F)Landroid/view/ViewPropertyAnimator;"):0;
    jmethodID ax=ac?(*env)->GetMethodID(env,ac,"scaleX","(F)Landroid/view/ViewPropertyAnimator;"):0;
    jmethodID ay=ac?(*env)->GetMethodID(env,ac,"scaleY","(F)Landroid/view/ViewPropertyAnimator;"):0;
    jmethodID delay=ac?(*env)->GetMethodID(env,ac,"setStartDelay","(J)Landroid/view/ViewPropertyAnimator;"):0;
    jmethodID dur=ac?(*env)->GetMethodID(env,ac,"setDuration","(J)Landroid/view/ViewPropertyAnimator;"):0;
    jmethodID start=ac?(*env)->GetMethodID(env,ac,"start","()V"):0;
    if(a&&aa){jobject q=(*env)->CallObjectMethod(env,a,aa,(jfloat)0.0f);if(q)(*env)->DeleteLocalRef(env,q);} if(a&&ax){jobject q=(*env)->CallObjectMethod(env,a,ax,(jfloat)1.18f);if(q)(*env)->DeleteLocalRef(env,q);} if(a&&ay){jobject q=(*env)->CallObjectMethod(env,a,ay,(jfloat)1.18f);if(q)(*env)->DeleteLocalRef(env,q);}
    if(a&&delay){jobject q=(*env)->CallObjectMethod(env,a,delay,(jlong)550);if(q)(*env)->DeleteLocalRef(env,q);} if(a&&dur){jobject q=(*env)->CallObjectMethod(env,a,dur,(jlong)950);if(q)(*env)->DeleteLocalRef(env,q);} if(a&&start)(*env)->CallVoidMethod(env,a,start);
    int ok=1;if((*env)->ExceptionCheck(env)){(*env)->ExceptionClear(env);ok=0;}
    if(ac)(*env)->DeleteLocalRef(env,ac);if(a)(*env)->DeleteLocalRef(env,a);if(vc)(*env)->DeleteLocalRef(env,vc);
    return ok?0:-4;
}

static void trust_visual_remove(JNIEnv* env) {
    if(!env||!g_visual_view||!g_visual_wm)return;
    jclass wc=(*env)->GetObjectClass(env,g_visual_wm);
    jmethodID rm=wc?(*env)->GetMethodID(env,wc,"removeViewImmediate","(Landroid/view/View;)V"):0;
    if(rm)(*env)->CallVoidMethod(env,g_visual_wm,rm,g_visual_view);
    if((*env)->ExceptionCheck(env))(*env)->ExceptionClear(env);
    if(wc)(*env)->DeleteLocalRef(env,wc);
    (*env)->DeleteGlobalRef(env,g_visual_view);g_visual_view=0;
    (*env)->DeleteGlobalRef(env,g_visual_wm);g_visual_wm=0;
}

__attribute__((visibility("default")))
JNIEXPORT void JNICALL Java_dev_zorin_trustruntime_TrustService_nativeOnCreate(JNIEnv* env, jclass cls, jobject service) {
    (void)cls;
    if(!env||!service)return;
    JavaVM* vm=0;if((*env)->GetJavaVM(env,&vm)==JNI_OK&&vm)g_vm=vm;
    (void)init_runtime_context_from(env,service);
    g_running=1;g_trust_service_alive=1;
    (void)trust_start_foreground_notification(env,service);
    trust_ui_publish_state();
    trust_start_worker();
}

__attribute__((visibility("default")))
JNIEXPORT void JNICALL Java_dev_zorin_trustruntime_TrustService_nativeOnStart(JNIEnv* env, jclass cls, jobject service, jobject intent) {
    (void)cls;
    if(!env||!service)return;
    g_running=1;g_trust_service_alive=1;trust_start_worker();
    if(intent&&trust_intent_bool_obj(env,intent,"dev.zorin.trust.pulse")) (void)trust_visual_show(env,service);
}

__attribute__((visibility("default")))
JNIEXPORT void JNICALL Java_dev_zorin_trustruntime_TrustService_nativeOnDestroy(JNIEnv* env, jclass cls, jobject service) {
    (void)cls;(void)service;
    if(env)trust_visual_remove(env);
    g_trust_service_alive=0; trust_ui_publish_state(); g_running=0;
}

static int in_rect(int x, int y, const int r[4]) { return x>=r[0] && y>=r[1] && x<r[0]+r[2] && y<r[1]+r[3]; }

static void kv(ANativeWindow_Buffer* b, int* y, int x, const char* label, const char* value, int state, int scale) {
    if (g_collect_report) report_kv_line(label, value);
    if (!b) return;
    uint32_t fg = rgb(216,226,235), good = rgb(86,232,157), warn = rgb(255,190,82), bad = rgb(255,105,120), dim=rgb(129,148,164);
    uint32_t c = state > 0 ? good : (state < 0 ? bad : warn);
    char line[256];
    snprintf(line,sizeof(line),"%-13s %s",label,value && *value ? value : "<N/A>");
    draw_text(b,x,*y,line,scale,c);
    int maxchars=(b->width-x-18)/(6*scale); if(maxchars<8)maxchars=8;
    int rows=((int)strlen(line)+maxchars-1)/maxchars; if(rows<1)rows=1;
    *y += rows*9*scale + 3*scale;
    (void)fg; (void)dim;
}

static void section_note(ANativeWindow_Buffer* b, int* y, int x, const char* s, int scale) {
    if (g_collect_report) { report_append("# "); report_append(s); report_append("\n"); }
    if (!b) return;
    draw_text(b,x,*y,s,scale,rgb(126,146,164));
    int maxchars=(b->width-x-18)/(6*scale); if(maxchars<8)maxchars=8;
    int rows=((int)strlen(s)+maxchars-1)/maxchars; if(rows<1)rows=1;
    *y += rows*9*scale + 3*scale;
}

static void fmt_int(char* out, int cap, long v) { snprintf(out,(size_t)cap,"%ld",v); }

static void trust_draw_action(ANativeWindow_Buffer* b,int x,int y,int w,int h,const char* label,int rect[4],int scale,int danger) {
    if(!b)return;uint32_t accent=danger?rgb(255,105,120):rgb(238,72,88),panel=danger?rgb(40,20,27):rgb(37,17,23);fill_rect(b,x,y,w,h,panel);outline_rect(b,x,y,w,h,2,accent);int tw=text_width(label,scale);draw_text(b,x+(w-tw)/2,y+(h-7*scale)/2,label,scale,accent);rect[0]=x;rect[1]=y;rect[2]=w;rect[3]=h;
}

static void trust_pair_code(const char* fp,char* out,int cap){
    static const char* words[16]={"EMBER","FALCON","NOVA","WOLF","ORBIT","ONYX","PIXEL","RAVEN","SOLAR","TITAN","VECTOR","COMET","PULSE","ATLAS","NEXUS","VAULT"};
    int n[8],k=0;for(int i=0;fp&&fp[i]&&k<8;++i){int v=trust_hexval(fp[i]);if(v>=0)n[k++]=v;}if(k<8){snprintf(out,(size_t)cap,"UNAVAILABLE");return;}
    int b0=(n[0]<<4)|n[1],b1=(n[2]<<4)|n[3],b2=(n[4]<<4)|n[5],b3=(n[6]<<4)|n[7];snprintf(out,(size_t)cap,"%s-%s %02d",words[b0&15],words[b1&15],((b2<<8)|b3)%100);
}

static void trust_state_card(ANativeWindow_Buffer* b,int x,int y,int w,int h,const char* label,const char* value,int state,int scale){
    if(!b)return;uint32_t good=rgb(238,72,88),warn=rgb(245,183,77),bad=rgb(255,90,110),dim=rgb(92,111,128),panel=rgb(14,20,28),fg=rgb(228,234,240);uint32_t c=state>0?good:(state<0?bad:warn);
    fill_rect(b,x,y,w,h,panel);outline_rect(b,x,y,w,h,1,rgb(44,54,66));fill_rect(b,x,y,5,h,c);draw_text(b,x+14,y+10*scale,label,scale,dim);draw_text(b,x+14,y+h-13*scale,value,scale,state==0?fg:c);
}

static void render_trust(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    (void)trust_ui_sync_from_service();
    int locked=trust_device_locked();int trusted=g_trust_state==3;int pending=g_trust_state==1;int overlay=trust_overlay_allowed();
    if(g_collect_report){
        report_kv_line("DEVICE TRUST",trusted?"ACTIVE":"INACTIVE");report_kv_line("OWNER PRESENCE",locked?"LOCKED":"PRESENT");report_kv_line("OWNER ACTIONS",trusted&&!locked?"ALLOWED":"DENIED");report_kv_line("HOST",g_trust_host_name);report_kv_line("HOST FP",g_trust_host_fp);report_kv_line("HOST KEY",g_trust_host_identity);report_kv_line("PHONE FP",g_trust_phone_fp);return;
    }
    if(!b)return;
    uint32_t bg=rgb(10,17,24),red=rgb(238,72,88),redDim=rgb(87,36,45),amber=rgb(245,183,77),dim=rgb(120,139,156),fg=rgb(232,238,244);
    int avail=b->width-x-18;int cx=x+avail/2;int r=52*scale;if(r>avail/4)r=avail/4;int cy=*y+r+8*scale;
    draw_ring(b,cx,cy,r,7*scale,trusted?red:(pending?amber:redDim),bg);
    if(locked&&trusted){for(int i=-r+8*scale;i<r-8*scale;i+=14*scale)fill_rect(b,cx+i,cy-r-4*scale,7*scale,4*scale,bg);}
    const char* center=trusted?(locked?"DEVICE TRUST":"OWNER LINKED"):(pending?"PAIRING":(g_trust_state==2?"AUTHENTICATING":(g_trust_state<0?"ERROR":"OFFLINE")));
    int tw=text_width(center,scale);draw_text(b,cx-tw/2,cy-4*scale,center,scale,trusted?red:(pending?amber:fg));
    *y=cy+r+18*scale;

    int gap=8;int cw=(avail-gap)/2;int ch=34*scale;
    trust_state_card(b,x,*y,cw,ch,"DEVICE",trusted?"TRUSTED":"OFFLINE",trusted?1:0,scale);
    trust_state_card(b,x+cw+gap,*y,cw,ch,"OWNER",locked?"LOCKED":"PRESENT",locked?0:1,scale);*y+=ch+gap;
    trust_state_card(b,x,*y,cw,ch,"AUTHORITY",trusted&&!locked?"ENABLED":"SUSPENDED",trusted&&!locked?1:0,scale);
    trust_state_card(b,x+cw+gap,*y,cw,ch,"TRANSPORT",g_trust_service_alive?"SERVICE ACTIVE":"STARTING",g_trust_service_alive?1:0,scale);*y+=ch+14;

    char line[300];snprintf(line,sizeof(line),"HOST  %s",g_trust_host_name);draw_text(b,x,*y,line,scale,fg);*y+=11*scale;
    snprintf(line,sizeof(line),"FP    %s",g_trust_host_fp);draw_text(b,x,*y,line,scale,dim);*y+=11*scale;
    snprintf(line,sizeof(line),"KEY   %s",g_trust_host_identity);draw_text(b,x,*y,line,scale,dim);*y+=11*scale;
    snprintf(line,sizeof(line),"POLICY %s",g_trust_policy);draw_text(b,x,*y,line,scale,dim);*y+=14*scale;

    if(pending){char code[96];trust_pair_code(g_trust_host_fp,code,sizeof(code));fill_rect(b,x,*y,avail,44*scale,rgb(35,25,18));outline_rect(b,x,*y,avail,44*scale,2,amber);draw_text(b,x+12,*y+8*scale,"PAIR VERIFICATION",scale,amber);int ctw=text_width(code,scale+1);draw_text(b,cx-ctw/2,*y+24*scale,code,scale+1,fg);*y+=52*scale;}

    snprintf(line,sizeof(line),"PROOF BROKER  ZOWNER/1 / %u ISSUED",g_trust_proof_count);draw_text(b,x,*y,line,scale,dim);*y+=11*scale;
    draw_text(b,x,*y,locked?"SCREEN LOCKED: DEVICE TRUST STAYS; OWNER ACTIONS ARE BLOCKED.":"OWNER PRESENT: SENSITIVE ACTIONS MAY REQUEST A SIGNED PROOF.",scale,locked?amber:dim);*y+=17*scale;

    int w=(avail-gap)/2,h=28*scale;trust_draw_action(b,x,*y,w,h,pending?"TRUST THIS WORKSTATION":"APPROVE",g_trust_approve_rect,scale,0);trust_draw_action(b,x+w+gap,*y,w,h,"REVOKE HOST",g_trust_forget_rect,scale,1);*y+=h+8;
    trust_draw_action(b,x,*y,avail,h,overlay?"TEST OWNER LINK PULSE":"ENABLE TRUST PULSE",g_trust_visual_rect,scale,0);*y+=h+8;
}

static void render_system(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[256], v2[128], se[64];
    snprintf(v,sizeof(v),"SDK %d / RUN #%u",(int)g_activity->sdkVersion,g_run_counter); kv(b,y,x,"RUNTIME",v,1,scale);
    struct utsname u;
    if (uname(&u)==0) { snprintf(v,sizeof(v),"%s %s",u.sysname,u.release); kv(b,y,x,"KERNEL",v,1,scale); kv(b,y,x,"MACHINE",u.machine,1,scale); }
    snprintf(v,sizeof(v),"%d BYTES",getpagesize()); kv(b,y,x,"PAGE SIZE",v,1,scale);
    if (prop_get("ro.build.version.release",v,sizeof(v))) kv(b,y,x,"ANDROID",v,1,scale);
    if (prop_get("ro.build.type",v,sizeof(v))) kv(b,y,x,"BUILD TYPE",v, mini_streq(v,"user")?1:0,scale);
    kv(b,y,x,"SELINUX",selinux_mode(se,sizeof(se)), mini_streq(selinux_mode(se,sizeof(se)),"ENFORCING")?1:0,scale);
    if (read_small("/proc/self/attr/current",v,sizeof(v))>0) { trim_line(v); kv(b,y,x,"SECONTEXT",v,1,scale); }
    if (read_small("/proc/self/cgroup",v,sizeof(v))>0) { trim_line(v); char* nl=strstr(v,"\n"); if(nl)*nl=0; kv(b,y,x,"CGROUP",v,1,scale); }
    if (readlink_small("/proc/self/ns/mnt",v,sizeof(v))>=0) kv(b,y,x,"NS MOUNT",v,1,scale);
    if (readlink_small("/proc/self/ns/pid",v,sizeof(v))>=0) kv(b,y,x,"NS PID",v,1,scale);
    if (readlink_small("/proc/self/ns/net",v,sizeof(v))>=0) kv(b,y,x,"NS NET",v,1,scale);
    snprintf(v,sizeof(v),"B:%s H:%s V:%s",access("/dev/binder",F_OK)==0?"YES":"NO",access("/dev/hwbinder",F_OK)==0?"YES":"NO",access("/dev/vndbinder",F_OK)==0?"YES":"NO");
    kv(b,y,x,"BINDER NODES",v,access("/dev/binder",F_OK)==0?1:0,scale);
    if (prop_get("ro.product.cpu.abi",v2,sizeof(v2))) kv(b,y,x,"PRIMARY ABI",v2,1,scale);
}

static void render_process(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[256];
    snprintf(v,sizeof(v),"PID %d / TID %ld",getpid(),syscall(GETTID_NR)); kv(b,y,x,"IDENTITY",v,1,scale);
    snprintf(v,sizeof(v),"UID %u / GID %u",getuid(),getgid()); kv(b,y,x,"CREDENTIALS",v,getuid()==0?0:1,scale);
    if (extract_status("Threads:",v,sizeof(v))>=0) kv(b,y,x,"THREADS",v,1,scale);
    if (extract_status("VmRSS:",v,sizeof(v))>=0) kv(b,y,x,"RSS",v,1,scale);
    if (extract_status("VmSize:",v,sizeof(v))>=0) kv(b,y,x,"VM SIZE",v,1,scale);
    if (extract_status("TracerPid:",v,sizeof(v))>=0) kv(b,y,x,"TRACER PID",v,mini_streq(v,"0")?1:-1,scale);
    if (extract_status("Seccomp:",v,sizeof(v))>=0) kv(b,y,x,"SECCOMP",v,mini_streq(v,"2")?1:0,scale);
    if (extract_status("NoNewPrivs:",v,sizeof(v))>=0) kv(b,y,x,"NO NEW PRIV",v,mini_streq(v,"1")?1:0,scale);
    if (extract_status("CapEff:",v,sizeof(v))>=0) kv(b,y,x,"CAP EFF",v,mini_streq(v,"0000000000000000")?1:0,scale);
    snprintf(v,sizeof(v),"%d (SCAN 0..511)",approx_fd_count()); kv(b,y,x,"OPEN FDS",v,1,scale);
    int maps=count_file_lines("/proc/self/maps"); snprintf(v,sizeof(v),"%d",maps); kv(b,y,x,"MEM MAPS",v,maps>0?1:-1,scale);
    if (readlink_small("/proc/self/exe",v,sizeof(v))>=0) kv(b,y,x,"EXECUTABLE",v,1,scale);
    if (readlink_small("/proc/self/ns/user",v,sizeof(v))>=0) kv(b,y,x,"NS USER",v,1,scale);
}

static uint16_t net_u16(uint16_t v) { return (uint16_t)((v << 8) | (v >> 8)); }

static int bridge_token_from_intent(void) {
    g_bridge_token[0] = 0; g_bridge_token_saved = 0;
    if (!g_activity || !g_activity->env || !g_activity->clazz) return -1;
    JNIEnv* env = g_activity->env;
    jclass activity_cls = (*env)->GetObjectClass(env, g_activity->clazz);
    jmethodID get_intent = activity_cls ? (*env)->GetMethodID(env, activity_cls, "getIntent", "()Landroid/content/Intent;") : 0;
    jobject intent = get_intent ? (*env)->CallObjectMethod(env, g_activity->clazz, get_intent) : 0;
    jclass intent_cls = intent ? (*env)->GetObjectClass(env, intent) : 0;
    jmethodID get_extra = intent_cls ? (*env)->GetMethodID(env, intent_cls, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;") : 0;
    jstring extra_key = (*env)->NewStringUTF(env, "dev.zorin.bridge.token");
    jstring value = (get_extra && extra_key) ? (jstring)(*env)->CallObjectMethod(env, intent, get_extra, extra_key) : 0;
    if (value) {
        const char* utf = (*env)->GetStringUTFChars(env, value, 0);
        if (utf) { int i=0;while(utf[i]&&i<(int)sizeof(g_bridge_token)-1){g_bridge_token[i]=utf[i];++i;}g_bridge_token[i]=0;(*env)->ReleaseStringUTFChars(env,value,utf); }
    }
    if(strlen(g_bridge_token)<16){
        // No usable Intent token: recover the last token that previously authenticated successfully.
        jmethodID get_prefs = activity_cls ? (*env)->GetMethodID(env, activity_cls, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;") : 0;
        jstring prefs_name = (*env)->NewStringUTF(env, "zorin_native_lab");
        jobject prefs = (get_prefs && prefs_name) ? (*env)->CallObjectMethod(env, g_activity->clazz, get_prefs, prefs_name, 0) : 0;
        jclass prefs_cls = prefs ? (*env)->GetObjectClass(env,prefs) : 0;
        jmethodID get_string = prefs_cls ? (*env)->GetMethodID(env,prefs_cls,"getString","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;") : 0;
        jstring pref_key = (*env)->NewStringUTF(env,"bridge_token");
        jstring saved = (get_string&&pref_key)?(jstring)(*env)->CallObjectMethod(env,prefs,get_string,pref_key,0):0;
        if(saved){const char* utf=(*env)->GetStringUTFChars(env,saved,0);if(utf){int i=0;while(utf[i]&&i<(int)sizeof(g_bridge_token)-1){g_bridge_token[i]=utf[i];++i;}g_bridge_token[i]=0;(*env)->ReleaseStringUTFChars(env,saved,utf);g_bridge_token_saved=(i>=16);}(*env)->DeleteLocalRef(env,saved);}
        if(pref_key)(*env)->DeleteLocalRef(env,pref_key);if(prefs_cls)(*env)->DeleteLocalRef(env,prefs_cls);if(prefs)(*env)->DeleteLocalRef(env,prefs);if(prefs_name)(*env)->DeleteLocalRef(env,prefs_name);
    }
    int rc = strlen(g_bridge_token)>=16 ? 0 : -2;
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); rc=-4; }
    if(value)(*env)->DeleteLocalRef(env,value);if(extra_key)(*env)->DeleteLocalRef(env,extra_key);if(intent_cls)(*env)->DeleteLocalRef(env,intent_cls);if(intent)(*env)->DeleteLocalRef(env,intent);if(activity_cls)(*env)->DeleteLocalRef(env,activity_cls);
    return rc;
}

static int bridge_save_authenticated_token(void){
    if(g_bridge_token_saved || strlen(g_bridge_token)<16 || !g_activity || !g_activity->vm || !g_activity->clazz)return 0;
    JNIEnv* env=0; int attached=0; JavaVM* vm=g_activity->vm;
    if((*vm)->GetEnv(vm,(void**)&env,JNI_VERSION_1_6)!=JNI_OK){if((*vm)->AttachCurrentThread(vm,(void**)&env,0)!=JNI_OK||!env)return -3;attached=1;}
    int rc=0;
    jclass activity_cls=(*env)->GetObjectClass(env,g_activity->clazz);
    jmethodID get_prefs=activity_cls?(*env)->GetMethodID(env,activity_cls,"getSharedPreferences","(Ljava/lang/String;I)Landroid/content/SharedPreferences;"):0;
    jstring prefs_name=(*env)->NewStringUTF(env,"zorin_native_lab");
    jobject prefs=(get_prefs&&prefs_name)?(*env)->CallObjectMethod(env,g_activity->clazz,get_prefs,prefs_name,0):0;
    jclass prefs_cls=prefs?(*env)->GetObjectClass(env,prefs):0;
    jmethodID edit=prefs_cls?(*env)->GetMethodID(env,prefs_cls,"edit","()Landroid/content/SharedPreferences$Editor;"):0;
    jobject editor=edit?(*env)->CallObjectMethod(env,prefs,edit):0;
    jclass editor_cls=editor?(*env)->GetObjectClass(env,editor):0;
    jmethodID put=editor_cls?(*env)->GetMethodID(env,editor_cls,"putString","(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;"):0;
    jmethodID apply=editor_cls?(*env)->GetMethodID(env,editor_cls,"apply","()V"):0;
    jstring key=(*env)->NewStringUTF(env,"bridge_token");jstring val=(*env)->NewStringUTF(env,g_bridge_token);
    if(put&&key&&val&&editor){jobject ignored=(*env)->CallObjectMethod(env,editor,put,key,val);if(ignored)(*env)->DeleteLocalRef(env,ignored);if(apply)(*env)->CallVoidMethod(env,editor,apply);g_bridge_token_saved=1;}else rc=-1;
    if((*env)->ExceptionCheck(env)){(*env)->ExceptionClear(env);rc=-2;}
    if(val)(*env)->DeleteLocalRef(env,val);if(key)(*env)->DeleteLocalRef(env,key);if(editor_cls)(*env)->DeleteLocalRef(env,editor_cls);if(editor)(*env)->DeleteLocalRef(env,editor);if(prefs_cls)(*env)->DeleteLocalRef(env,prefs_cls);if(prefs)(*env)->DeleteLocalRef(env,prefs);if(prefs_name)(*env)->DeleteLocalRef(env,prefs_name);if(activity_cls)(*env)->DeleteLocalRef(env,activity_cls);
    if(attached)(*vm)->DetachCurrentThread(vm);
    return rc;
}

static int bridge_request(const char* command, char* out, int cap) {
    if (!out || cap < 2) return -1;
    out[0] = 0;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return last_errno();
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = net_u16((uint16_t)ZORIN_BRIDGE_PORT);
    sa.sin_addr.s_addr = 0x0100007fU;
    if (connect(fd, &sa, (unsigned int)sizeof(sa)) != 0) {
        int e = last_errno(); close(fd); return e ? e : -2;
    }
    char wire[384];
    if (mini_streq(command,"HELLO")) snprintf(wire,sizeof(wire),"HELLO\n");
    else {
        if (g_bridge_token[0] == 0) { close(fd); return 401; }
        snprintf(wire,sizeof(wire),"AUTH %s %s\n",g_bridge_token,command);
    }
    size_t ncmd = strlen(wire);
    if (send(fd, wire, ncmd, 0) != (ssize_t)ncmd) { int e=last_errno(); close(fd); return e?e:-3; }
    int pos=0; int e=0;
    while(pos<cap-1){
        ssize_t n=recv(fd,out+pos,(size_t)(cap-1-pos),0);
        if(n==0)break;
        if(n<0){e=last_errno(); if(pos==0){close(fd);return e?e:-4;} break;}
        pos += (int)n;
    }
    close(fd); out[pos]=0; trim_line(out);
    if(pos<=0)return e?e:-4;
    if(strstr(out,"ERR auth-required-or-failed")) return 403;
    return 0;
}

static int bridge_field(const char* text,const char* key,char* out,int cap){
    if(!text||!key||!out||cap<2)return -1; char pat[96];snprintf(pat,sizeof(pat),"%s=",key);char* p=strstr(text,pat);if(!p){out[0]=0;return -1;}p+=strlen(pat);int i=0;while(*p&&*p!='\n'&&i<cap-1)out[i++]=*p++;out[i]=0;return i;
}

static AIBinder_Class* g_core_class = 0;
static void* core_class_create(void* args) { return args; }
static void core_class_destroy(void* user) { (void)user; }
static int core_class_transact(AIBinder* binder, uint32_t code, const AParcel* in, AParcel* out) {
    (void)binder; (void)code; (void)in; (void)out; return -1;
}
static AIBinder_Class* get_core_class(void) {
    if (!g_core_class) g_core_class = AIBinder_Class_define(
        "dev.zorin.core.IDeviceCore", core_class_create, core_class_destroy, core_class_transact);
    return g_core_class;
}

struct core_string_alloc { char* out; int cap; };
static _Bool core_string_allocator(void* data, int32_t length, char** buffer) {
    struct core_string_alloc* a = (struct core_string_alloc*)data;
    if (!a || !a->out || a->cap < 1) return 0;
    if (length == -1) { a->out[0] = 0; *buffer = 0; return 1; }
    if (length <= 0 || length > a->cap) return 0;
    *buffer = a->out;
    return 1;
}

static int core_prepare(AIBinder* core, AParcel** in) {
    AIBinder_Class* clazz = get_core_class();
    if (!clazz || !AIBinder_associateClass(core, clazz)) return -1001;
    return AIBinder_prepareTransaction(core, in);
}

static int core_read_status(AParcel* reply, char* err, int errcap) {
    AStatus* status = 0;
    int rc = AParcel_readStatusHeader(reply, &status);
    if (rc != 0) { if(err)snprintf(err,(size_t)errcap,"STATUS_HEADER %d",rc); return rc; }
    if (!status) { if(err)snprintf(err,(size_t)errcap,"NO STATUS"); return -1002; }
    if (!AStatus_isOk(status)) {
        const char* msg = AStatus_getMessage(status);
        int st = AStatus_getStatus(status);
        if(err)snprintf(err,(size_t)errcap,"AIDL ERROR %d %s",st,msg?msg:"");
        AStatus_delete(status);
        return st ? st : -1003;
    }
    AStatus_delete(status);
    return 0;
}

static int core_call_string(AIBinder* core, uint32_t code, char* out, int cap) {
    AParcel* in=0; AParcel* reply=0; char err[128];
    int rc=core_prepare(core,&in); if(rc!=0){snprintf(out,(size_t)cap,"PREP %d",rc);return rc;}
    rc=AIBinder_transact(core,code,&in,&reply,0); if(rc!=0){if(reply)AParcel_delete(reply);snprintf(out,(size_t)cap,"TX %d",rc);return rc;}
    rc=core_read_status(reply,err,sizeof(err)); if(rc!=0){snprintf(out,(size_t)cap,"%s",err);AParcel_delete(reply);return rc;}
    struct core_string_alloc a={out,cap}; out[0]=0;
    rc=AParcel_readString(reply,&a,core_string_allocator); AParcel_delete(reply);
    if(rc!=0)snprintf(out,(size_t)cap,"READ %d",rc); return rc;
}

static int core_call_i32(AIBinder* core, uint32_t code, int32_t* value) {
    AParcel* in=0; AParcel* reply=0; char err[64]; int rc=core_prepare(core,&in); if(rc!=0)return rc;
    rc=AIBinder_transact(core,code,&in,&reply,0); if(rc!=0){if(reply)AParcel_delete(reply);return rc;}
    rc=core_read_status(reply,err,sizeof(err)); if(rc==0)rc=AParcel_readInt32(reply,value); AParcel_delete(reply); return rc;
}
static int core_call_i64(AIBinder* core, uint32_t code, int64_t* value) {
    AParcel* in=0; AParcel* reply=0; char err[64]; int rc=core_prepare(core,&in); if(rc!=0)return rc;
    rc=AIBinder_transact(core,code,&in,&reply,0); if(rc!=0){if(reply)AParcel_delete(reply);return rc;}
    rc=core_read_status(reply,err,sizeof(err)); if(rc==0)rc=AParcel_readInt64(reply,value); AParcel_delete(reply); return rc;
}

static int core_call_policy(AIBinder* core, const char* principal, const char* action, const char* resource, char* out, int cap) {
    AParcel* in=0; AParcel* reply=0; char err[128];
    int rc=core_prepare(core,&in); if(rc!=0){snprintf(out,(size_t)cap,"PREP %d",rc);return rc;}
    rc=AParcel_writeString(in,principal,(int32_t)strlen(principal));
    if(rc==0)rc=AParcel_writeString(in,action,(int32_t)strlen(action));
    if(rc==0)rc=AParcel_writeString(in,resource,(int32_t)strlen(resource));
    if(rc!=0){AParcel_delete(in);snprintf(out,(size_t)cap,"WRITE %d",rc);return rc;}
    rc=AIBinder_transact(core,10,&in,&reply,0);
    if(rc!=0){if(reply)AParcel_delete(reply);snprintf(out,(size_t)cap,"TX %d",rc);return rc;}
    rc=core_read_status(reply,err,sizeof(err));
    if(rc!=0){snprintf(out,(size_t)cap,"%s",err);AParcel_delete(reply);return rc;}
    struct core_string_alloc a={out,cap}; out[0]=0;
    rc=AParcel_readString(reply,&a,core_string_allocator); AParcel_delete(reply);
    if(rc!=0)snprintf(out,(size_t)cap,"READ %d",rc);
    return rc;
}

static void render_core(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[4096], status[2048], services[4096], tmp[256], appsec[64];
    const char* service_name = "dev.zorin.core.IDeviceCore/default";
    int core_ok = 0;
    AIBinder* core = AServiceManager_checkService(service_name);
    if (core) {
        int pr = AIBinder_ping(core);
        snprintf(v, sizeof(v), "FOUND PING %d %s/%s", pr,
                 AIBinder_isRemote(core) ? "REMOTE" : "LOCAL",
                 AIBinder_isAlive(core) ? "ALIVE" : "DEAD");
        kv(b, y, x, "DEVICE CORE", v, pr == 0 ? 1 : -1, scale);
        core_ok = (pr == 0);
    } else kv(b, y, x, "DEVICE CORE", "NOT INSTALLED / NOT VISIBLE", 0, scale);
    kv(b, y, x, "CORE SERVICE", service_name, core_ok ? 1 : 0, scale);

    if (core_ok) {
        int32_t proto=0; int64_t caps=0;
        int rc=core_call_i32(core,2,&proto); if(rc==0){snprintf(v,sizeof(v),"%d",proto);kv(b,y,x,"CORE PROTOCOL",v,1,scale);} else {snprintf(v,sizeof(v),"ERR %d",rc);kv(b,y,x,"CORE PROTOCOL",v,-1,scale);}
        rc=core_call_i64(core,3,&caps); if(rc==0){snprintf(v,sizeof(v),"0x%llx",(unsigned long long)caps);kv(b,y,x,"CORE CAPS",v,1,scale);} else {snprintf(v,sizeof(v),"ERR %d",rc);kv(b,y,x,"CORE CAPS",v,-1,scale);}
        rc=core_call_string(core,4,v,sizeof(v)); kv(b,y,x,"CORE CALLER",v,rc==0?1:-1,scale);
        rc=core_call_string(core,5,v,sizeof(v)); kv(b,y,x,"CORE RUNTIME",v,rc==0?1:-1,scale);
        rc=core_call_string(core,6,v,sizeof(v)); kv(b,y,x,"CORE KERNEL",v,rc==0?1:-1,scale);
        rc=core_call_string(core,7,v,sizeof(v)); kv(b,y,x,"CORE PROCESS",v,rc==0?1:-1,scale);
        rc=core_call_string(core,8,v,sizeof(v)); kv(b,y,x,"CORE NETWORK",v,rc==0?1:-1,scale);
        rc=core_call_string(core,9,v,sizeof(v)); kv(b,y,x,"CORE SECURITY",v,rc==0?1:-1,scale);
        rc=core_call_policy(core,"app","read","device.status",v,sizeof(v)); kv(b,y,x,"CORE POLICY",v,rc==0?1:-1,scale);
        if(proto>=2){
            rc=core_call_string(core,11,v,sizeof(v)); kv(b,y,x,"CORE FILESYSTEM",v,rc==0?1:-1,scale);
            rc=core_call_string(core,12,v,sizeof(v)); kv(b,y,x,"CORE SYSCTL",v,rc==0?1:-1,scale);
            rc=core_call_string(core,13,v,sizeof(v)); kv(b,y,x,"CORE BINDER",v,rc==0?1:-1,scale);
            rc=core_call_string(core,14,v,sizeof(v)); kv(b,y,x,"CORE MATRIX",v,rc==0?1:-1,scale);
        }
    }
    if(core)AIBinder_decStrong(core);

    int hello=bridge_request("HELLO",v,sizeof(v));
    if(hello==0) kv(b,y,x,"ADB BRIDGE",v,1,scale); else {snprintf(v,sizeof(v),"OFFLINE ERRNO %d",hello);kv(b,y,x,"ADB BRIDGE",v,0,scale);}
    if(hello==0 && strstr(v,"auth=required") && g_bridge_token[0]==0) {
        kv(b,y,x,"BRIDGE AUTH","TOKEN MISSING - RUN START SCRIPT",0,scale);
    } else if(g_bridge_token[0]) {
        snprintf(tmp,sizeof(tmp),"AUTHENTICATED SESSION / TOKEN %d CHARS",(int)strlen(g_bridge_token));kv(b,y,x,"BRIDGE AUTH",tmp,1,scale);
    }

    int bridge_ok=0; status[0]=0; services[0]=0;
    if(hello==0){
        int br=bridge_request("STATUS",status,sizeof(status));
        if(br==0){bridge_ok=1;bridge_save_authenticated_token();char ctx[192],sec[32],enf[64];bridge_field(status,"context",ctx,sizeof(ctx));bridge_field(status,"seccomp",sec,sizeof(sec));bridge_field(status,"selinux_enforce",enf,sizeof(enf));snprintf(v,sizeof(v),"%s / SECCOMP %s / SELINUX %s",ctx,sec,enf);kv(b,y,x,"SHELL CORE",v,1,scale);}else{snprintf(v,sizeof(v),br==401?"AUTH TOKEN MISSING":"QUERY ERRNO %d",br);kv(b,y,x,"SHELL CORE",v,0,scale);}
        br=bridge_request("SERVICES",services,sizeof(services));
        if(br==0){char count[32],tested[32];bridge_field(services,"count",count,sizeof(count));bridge_field(services,"tested",tested,sizeof(tested));snprintf(v,sizeof(v),"%s/%s KNOWN SERVICES VISIBLE",count,tested);kv(b,y,x,"SHELL BINDER",v,1,scale);}
    }
    if(bridge_ok){
        char shellsec[32]="?",enf[64]="?",pnet[32]="?",ctx[192]="?";bridge_field(status,"seccomp",shellsec,sizeof(shellsec));bridge_field(status,"selinux_enforce",enf,sizeof(enf));bridge_field(status,"proc_net",pnet,sizeof(pnet));bridge_field(status,"context",ctx,sizeof(ctx));
        if(extract_status("Seccomp:",appsec,sizeof(appsec))<0)snprintf(appsec,sizeof(appsec),"?");
        snprintf(v,sizeof(v),"SECCOMP %s -> %s",appsec,shellsec);kv(b,y,x,"APP -> SHELL",v,mini_streq(shellsec,"0")?1:0,scale);
        snprintf(v,sizeof(v),"PROC_NET DENIED -> %s / SELINUX READ -> %s",mini_streq(pnet,"1")?"VISIBLE":"DENIED",enf);kv(b,y,x,"BOUNDARY",v,1,scale);
        if(!core_ok)kv(b,y,x,"ACTIVE MODE","ADB SHELL CORE / AUTH LOOPBACK",1,scale);
    } else if(core_ok)kv(b,y,x,"ACTIVE MODE","SYSTEM DEVICE CORE / BINDER",1,scale);
    else kv(b,y,x,"ACTIVE MODE","APPLICATION SANDBOX",0,scale);
    snprintf(v,sizeof(v),"127.0.0.1:%d",ZORIN_BRIDGE_PORT);kv(b,y,x,"BRIDGE ENDPOINT",v,bridge_ok?1:0,scale);
    section_note(b,y,x,"PRIORITY: SYSTEM BINDER CORE > AUTHENTICATED ADB SHELL CORE > APP SANDBOX.",scale);
    section_note(b,y,x,"BRIDGE v3 ADDS VALIDATED KERNEL/PROCFS PROBES; STILL FIXED-COMMAND + READ-ONLY.",scale);
}

static const char* g_known_services[]={
    "activity","package","window","power","display","SurfaceFlinger","sensorservice","connectivity","netd","vold",
    "media.audio_flinger","permission","mount","keystore2","stats","incident","gatekeeper","gpu","input","batterystats",
    "deviceidle","thermalservice","storaged","usagestats","jobscheduler","appops","role","shortcut","notification","location",
    "wifi","bluetooth_manager","media.extractor","media.metrics","drm.drmManager","android.security.authorization.IKeystoreAuthorization/default"
};
#define KNOWN_SERVICE_COUNT ((int)(sizeof(g_known_services)/sizeof(g_known_services[0])))

static int app_service_map(char* list,int cap,int* ping_ok){
    int pos=0,found=0,pok=0;if(list&&cap>0)list[0]=0;
    for(int i=0;i<KNOWN_SERVICE_COUNT;++i){AIBinder* svc=AServiceManager_checkService(g_known_services[i]);if(!svc)continue;int pr=AIBinder_ping(svc);AIBinder_decStrong(svc);if(pr!=0)continue;++found;++pok;if(list&&cap>1){if(pos&&pos<cap-1)list[pos++]=',';const char*n=g_known_services[i];while(*n&&pos<cap-1)list[pos++]=*n++;list[pos]=0;}}
    if(ping_ok)*ping_ok=pok;return found;
}

static int csv_has_token(const char* csv,const char* token){
    if(!csv||!token||!*token)return 0;int tl=(int)strlen(token);const char*p=csv;
    while(*p){const char*s=p;while(*p&&*p!=',')++p;int n=(int)(p-s);if(n==tl){int same=1;for(int i=0;i<n;++i)if(s[i]!=token[i]){same=0;break;}if(same)return 1;}if(*p==',')++p;}
    return 0;
}

struct binder_version { int32_t protocol_version; };
static void render_binder(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[256];
    const char* paths[3]={"/dev/binder","/dev/hwbinder","/dev/vndbinder"};
    const char* names[3]={"BINDER","HWBINDER","VNDBINDER"};
    for(int i=0;i<3;++i) {
        int fd=open(paths[i],O_RDWR|O_CLOEXEC);
        if(fd>=0) { snprintf(v,sizeof(v),"OPEN OK FD %d",fd); kv(b,y,x,names[i],v,1,scale); close(fd); }
        else { snprintf(v,sizeof(v),"OPEN FAIL ERRNO %d",last_errno()); kv(b,y,x,names[i],v,-1,scale); }
    }
    int fd=open("/dev/binder",O_RDWR|O_CLOEXEC);
    if(fd>=0) {
        struct binder_version bv; bv.protocol_version=0;
        int rc=ioctl(fd,BINDER_VERSION_IOCTL,&bv);
        if(rc==0) { snprintf(v,sizeof(v),"PROTOCOL %d",bv.protocol_version); kv(b,y,x,"VERSION IOCTL",v,1,scale); }
        else { snprintf(v,sizeof(v),"FAIL ERRNO %d",last_errno()); kv(b,y,x,"VERSION IOCTL",v,-1,scale); }
        close(fd);
    } else kv(b,y,x,"VERSION IOCTL","SKIPPED",-1,scale);

    // Direct public libbinder_ndk ServiceManager queries. Screen keeps the compact classic 12;
    // COPY ALL records the same 36-name map used by the shell bridge for an exact privilege diff.
    int found=0, ping_ok=0;
    for(int i=0;i<KNOWN_SERVICE_COUNT;++i) {
        AIBinder* svc=AServiceManager_checkService(g_known_services[i]);
        if(svc) {
            ++found; int pr=AIBinder_ping(svc); if(pr==0) ++ping_ok;
            if(!b || i<12){snprintf(v,sizeof(v),"FOUND PING %d %s/%s",pr,AIBinder_isRemote(svc)?"REMOTE":"LOCAL",AIBinder_isAlive(svc)?"ALIVE":"DEAD");kv(b,y,x,g_known_services[i],v,pr==0?1:0,scale);}
            AIBinder_decStrong(svc);
        } else if(!b || i<12) kv(b,y,x,g_known_services[i],"NOT VISIBLE",0,scale);
    }
    snprintf(v,sizeof(v),"%d/%d FOUND, %d PING OK",found,KNOWN_SERVICE_COUNT,ping_ok); kv(b,y,x,"SM SUMMARY",v,found>0?1:0,scale);
    section_note(b,y,x,"APP AND SHELL NOW PROBE THE SAME 36-SERVICE BINDER MAP.",scale);
}

static void render_network(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[256]; int n, e=0;
    n=count_file_lines_ex("/proc/net/dev",&e); if(n>=2)snprintf(v,sizeof(v),"%d",n-2);else snprintf(v,sizeof(v),"DENIED ERRNO %d",e); kv(b,y,x,"INTERFACES",v,n>=2?1:0,scale);
    n=count_file_lines_ex("/proc/net/route",&e); if(n>=1)snprintf(v,sizeof(v),"%d",n-1);else snprintf(v,sizeof(v),"DENIED ERRNO %d",e); kv(b,y,x,"IPV4 ROUTES",v,n>=1?1:0,scale);
    n=count_file_lines_ex("/proc/net/tcp",&e); if(n>=1)snprintf(v,sizeof(v),"%d",n-1);else snprintf(v,sizeof(v),"DENIED ERRNO %d",e); kv(b,y,x,"TCP ENTRIES",v,n>=1?1:0,scale);
    n=count_file_lines_ex("/proc/net/udp",&e); if(n>=1)snprintf(v,sizeof(v),"%d",n-1);else snprintf(v,sizeof(v),"DENIED ERRNO %d",e); kv(b,y,x,"UDP ENTRIES",v,n>=1?1:0,scale);
    int fd=socket(AF_INET,SOCK_STREAM,0); if(fd>=0){snprintf(v,sizeof(v),"CREATE OK FD %d",fd);kv(b,y,x,"INET SOCKET",v,1,scale);close(fd);}else{snprintf(v,sizeof(v),"FAIL ERRNO %d",last_errno());kv(b,y,x,"INET SOCKET",v,-1,scale);}
    fd=socket(AF_UNIX,SOCK_STREAM,0); if(fd>=0){snprintf(v,sizeof(v),"CREATE OK FD %d",fd);kv(b,y,x,"UNIX SOCKET",v,1,scale);close(fd);}else{snprintf(v,sizeof(v),"FAIL ERRNO %d",last_errno());kv(b,y,x,"UNIX SOCKET",v,-1,scale);}
    fd=open("/dev/tun",O_RDWR|O_CLOEXEC); if(fd>=0){snprintf(v,sizeof(v),"OPEN OK FD %d",fd);kv(b,y,x,"TUN DEVICE",v,1,scale);close(fd);}else{snprintf(v,sizeof(v),"NO ACCESS ERRNO %d",last_errno());kv(b,y,x,"TUN DEVICE",v,0,scale);}
    if (prop_get("net.dns1",v,sizeof(v))) kv(b,y,x,"DNS PROP",v,1,scale); else kv(b,y,x,"DNS PROP","NOT EXPOSED",0,scale);
    section_note(b,y,x,"INTERNET IS A NORMAL (NON-DANGEROUS) PERMISSION.",scale);
}

static int su_visible(void) {
    const char* p[]={"/system/bin/su","/system/xbin/su","/sbin/su","/su/bin/su","/debug_ramdisk/su"};
    for(int i=0;i<5;++i) if(access(p[i],F_OK)==0) return 1; return 0;
}

static void render_security(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[256], se[64];
    const char* sm=selinux_mode(se,sizeof(se)); kv(b,y,x,"SELINUX",sm,mini_streq(sm,"ENFORCING")?1:0,scale);
    if(read_small("/proc/self/attr/current",v,sizeof(v))>0){trim_line(v);kv(b,y,x,"SECONTEXT",v,1,scale);}
    kv(b,y,x,"SU PATH",su_visible()?"VISIBLE":"NOT VISIBLE",su_visible()?0:1,scale);
    snprintf(v,sizeof(v),"UID %u",getuid()); kv(b,y,x,"ROOT UID",v,getuid()==0?0:1,scale);
    if(prop_get("ro.boot.verifiedbootstate",v,sizeof(v))) kv(b,y,x,"VERIFIED BOOT",v,mini_streq(v,"green")?1:0,scale); else kv(b,y,x,"VERIFIED BOOT","NOT EXPOSED",0,scale);
    if(prop_get("ro.boot.vbmeta.device_state",v,sizeof(v))) kv(b,y,x,"VBMETA STATE",v,mini_streq(v,"locked")?1:0,scale);
    if(prop_get("ro.boot.flash.locked",v,sizeof(v))) kv(b,y,x,"FLASH LOCK",v,mini_streq(v,"1")?1:0,scale);
    if(prop_get("ro.debuggable",v,sizeof(v))) kv(b,y,x,"DEBUGGABLE",v,mini_streq(v,"0")?1:0,scale);
    if(prop_get("ro.secure",v,sizeof(v))) kv(b,y,x,"RO SECURE",v,mini_streq(v,"1")?1:0,scale);
    if(extract_status("TracerPid:",v,sizeof(v))>=0) kv(b,y,x,"TRACER PID",v,mini_streq(v,"0")?1:-1,scale);
    if(extract_status("Seccomp:",v,sizeof(v))>=0) kv(b,y,x,"SECCOMP",v,mini_streq(v,"2")?1:0,scale);
    if(extract_status("NoNewPrivs:",v,sizeof(v))>=0) kv(b,y,x,"NO NEW PRIV",v,mini_streq(v,"1")?1:0,scale);
    section_note(b,y,x,"ROOT/BOOT CHECKS ARE SIGNALS, NOT A TRUST VERDICT.",scale);
}

static void render_native(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[256]; int ps=getpagesize();
    snprintf(v,sizeof(v),"%d BYTES",ps); kv(b,y,x,"PAGE SIZE",v,1,scale);
    void* p=mmap(0,(size_t)ps,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    if(p!=MAP_FAILED) {
        ((volatile char*)p)[0]=0x5a; kv(b,y,x,"MMAP RW","OK",1,scale);
        int rc=mprotect(p,(size_t)ps,PROT_READ); snprintf(v,sizeof(v),"%s",rc==0?"OK":"FAIL"); kv(b,y,x,"MPROTECT R",v,rc==0?1:-1,scale);
        munmap(p,(size_t)ps);
    } else { snprintf(v,sizeof(v),"FAIL ERRNO %d",last_errno()); kv(b,y,x,"MMAP RW",v,-1,scale); }
    int fd=epoll_create1(O_CLOEXEC); if(fd>=0){snprintf(v,sizeof(v),"FD %d",fd);kv(b,y,x,"EPOLL",v,1,scale);close(fd);}else{snprintf(v,sizeof(v),"ERRNO %d",last_errno());kv(b,y,x,"EPOLL",v,-1,scale);}
    fd=eventfd(0,O_CLOEXEC); if(fd>=0){snprintf(v,sizeof(v),"FD %d",fd);kv(b,y,x,"EVENTFD",v,1,scale);close(fd);}else{snprintf(v,sizeof(v),"ERRNO %d",last_errno());kv(b,y,x,"EVENTFD",v,-1,scale);}
    int pipefd[2]={-1,-1}; int rc=pipe2(pipefd,O_CLOEXEC); if(rc==0){snprintf(v,sizeof(v),"FDS %d/%d",pipefd[0],pipefd[1]);kv(b,y,x,"PIPE2",v,1,scale);close(pipefd[0]);close(pipefd[1]);}else{snprintf(v,sizeof(v),"ERRNO %d",last_errno());kv(b,y,x,"PIPE2",v,-1,scale);}
    int word=0; long fr=syscall(FUTEX_NR,&word,FUTEX_WAKE,1,0,0,0); snprintf(v,sizeof(v),"RC %ld",fr); kv(b,y,x,"FUTEX WAKE",v,fr>=0?1:-1,scale);
    char sc[32];
    if (extract_status("Seccomp:",sc,sizeof(sc))>=0 && mini_streq(sc,"2")) {
        kv(b,y,x,"IO_URING","SKIPPED: SECCOMP FILTER",0,scale);
        section_note(b,y,x,"V0.2 PROVED IO_URING_SETUP(425) IS SIGSYS-BLOCKED IN THIS APP SANDBOX.",scale);
    } else {
        kv(b,y,x,"IO_URING","NOT EXECUTED: RISKY RAW SYSCALL",0,scale);
    }
    struct timespec ts; if(clock_gettime(CLOCK_MONOTONIC,&ts)==0){snprintf(v,sizeof(v),"%ld.%09ld",ts.tv_sec,ts.tv_nsec);kv(b,y,x,"MONOTONIC",v,1,scale);}
    section_note(b,y,x,"SAFE PROBES RUN IN-PROCESS; KILLING SYSCALLS ARE NOT RETRIED.",scale);
}

struct child_probe_result { long rc; int err; };

static void run_child_syscall(long nr, long a0, long a1, long a2, long a3, long a4, long a5, char* out, int cap, int* state) {
    int pfd[2]={-1,-1};
    if(pipe2(pfd,O_CLOEXEC)!=0){snprintf(out,(size_t)cap,"PIPE ERRNO %d",last_errno());*state=-1;return;}
    int pid=fork();
    if(pid==0){
        close(pfd[0]);
        struct child_probe_result r;
        r.rc=syscall(nr,a0,a1,a2,a3,a4,a5);
        r.err=(r.rc<0)?last_errno():0;
        write(pfd[1],&r,sizeof(r));
        close(pfd[1]);
        _exit(0);
    }
    close(pfd[1]);
    if(pid<0){snprintf(out,(size_t)cap,"FORK ERRNO %d",last_errno());*state=-1;close(pfd[0]);return;}
    struct child_probe_result r; memset(&r,0,sizeof(r));
    ssize_t n=read(pfd[0],&r,sizeof(r)); close(pfd[0]);
    int st=0; int wr=waitpid(pid,&st,0);
    if(wr<0){snprintf(out,(size_t)cap,"WAIT ERRNO %d",last_errno());*state=-1;return;}
    int sig=st & 0x7f;
    if(sig!=0){snprintf(out,(size_t)cap,sig==SIGSYS?"SIGSYS / SECCOMP":"SIGNAL %d",sig);*state=-1;return;}
    if(n==(ssize_t)sizeof(r)){
        if(r.rc>=0){snprintf(out,(size_t)cap,"ALLOWED RC %ld",r.rc);*state=1;}
        else {snprintf(out,(size_t)cap,"ERRNO %d",r.err);*state=(r.err==EPERM||r.err==EACCES||r.err==ENOSYS)?0:1;}
    } else {snprintf(out,(size_t)cap,"NO RESULT");*state=0;}
}

static void render_sandbox(ANativeWindow_Buffer* b, int* y, int x, int scale) {
    char v[128]; int st=0;
    run_child_syscall(IO_URING_SETUP_NR,0,0,0,0,0,0,v,sizeof(v),&st); kv(b,y,x,"IO_URING",v,st,scale);
    run_child_syscall(BPF_NR,-1,0,0,0,0,0,v,sizeof(v),&st); kv(b,y,x,"BPF",v,st,scale);
    run_child_syscall(PERF_EVENT_OPEN_NR,0,0,-1,-1,0,0,v,sizeof(v),&st); kv(b,y,x,"PERF EVENT",v,st,scale);
    run_child_syscall(USERFAULTFD_NR,0,0,0,0,0,0,v,sizeof(v),&st); kv(b,y,x,"USERFAULTFD",v,st,scale);
    run_child_syscall(CLONE3_NR,0,0,0,0,0,0,v,sizeof(v),&st); kv(b,y,x,"CLONE3",v,st,scale);
    run_child_syscall(MOUNT_NR,0,0,0,0,0,0,v,sizeof(v),&st); kv(b,y,x,"MOUNT",v,st,scale);
    run_child_syscall(SETNS_NR,-1,0,0,0,0,0,v,sizeof(v),&st); kv(b,y,x,"SETNS",v,st,scale);
    run_child_syscall(PTRACE_NR,-1,0,0,0,0,0,v,sizeof(v),&st); kv(b,y,x,"PTRACE",v,st,scale);
    section_note(b,y,x,"EACH RISKY SYSCALL RUNS IN A SHORT-LIVED CHILD PROCESS.",scale);
    section_note(b,y,x,"SIGSYS = SECCOMP KILL/TRAP; ERRNO = SYSCALL REACHED A KERNEL POLICY/CHECK.",scale);
}

static void report_bridge_detail(const char* title,const char* command){
    char b[8192]; int rc=bridge_request(command,b,sizeof(b));
    report_append("\n[SHELL CORE / ");report_append(title);report_append("]\n");
    if(rc==0){report_append(b);report_append("\n");}
    else {char e[96];snprintf(e,sizeof(e),"ERROR=%d\n",rc);report_append(e);}
}

static void build_full_report(void) {
    int y = 0;
    report_reset();
    char h[256];
    snprintf(h, sizeof(h), "ZORIN TRUST RUNTIME v0.2.1 / LAB v4.1\nSDK %d | PID %d | UID %u | RUN #%u\n", (int)g_activity->sdkVersion, getpid(), getuid(), g_run_counter);
    report_append(h); report_append("========================================\n"); g_collect_report = 1;
    report_append("\n[SYSTEM]\n"); render_system(0, &y, 0, 1);
    report_append("\n[PROCESS]\n"); render_process(0, &y, 0, 1);
    report_append("\n[TRUST]\n"); render_trust(0, &y, 0, 1);
    report_append("\n[CORE]\n"); render_core(0, &y, 0, 1);
    report_append("\n[BINDER]\n"); render_binder(0, &y, 0, 1);
    report_append("\n[NETWORK]\n"); render_network(0, &y, 0, 1);
    report_append("\n[SECURITY]\n"); render_security(0, &y, 0, 1);
    report_append("\n[NATIVE]\n"); render_native(0, &y, 0, 1);
    report_append("\n[SANDBOX]\n"); render_sandbox(0, &y, 0, 1);
    g_collect_report = 0;
    if(g_bridge_token[0]){
        report_bridge_detail("STATUS","STATUS");
        report_bridge_detail("SERVICES","SERVICES");
        report_bridge_detail("KERNEL","KERNEL");
        report_bridge_detail("PROCESS","PROCESS");
        report_bridge_detail("NETWORK","NETWORK");
        report_bridge_detail("SECURITY","SECURITY");
        report_bridge_detail("FILESYSTEM","FILESYSTEM");
        report_bridge_detail("PROCFS","PROCFS");
        report_bridge_detail("VALIDATED PROBES","PROBES");
    }
    report_append("\n[SUMMARY]\n");
    {
        char v[128],ssec[32]="offline",senf[32]="offline",bs[2048],ss[4096],shell_count[32]="offline",shell_found[3072]="",app_found[3072]="";
        if(extract_status("Seccomp:",v,sizeof(v))>=0){report_append("app.seccomp=");report_append(v);report_append("\n");}
        if(bridge_request("STATUS",bs,sizeof(bs))==0){bridge_field(bs,"seccomp",ssec,sizeof(ssec));bridge_field(bs,"selinux_enforce",senf,sizeof(senf));}
        int app_ping=0,app_count=app_service_map(app_found,sizeof(app_found),&app_ping);
        if(bridge_request("SERVICES",ss,sizeof(ss))==0){bridge_field(ss,"count",shell_count,sizeof(shell_count));bridge_field(ss,"found",shell_found,sizeof(shell_found));}
        char nbuf[64];snprintf(nbuf,sizeof(nbuf),"%d",app_count);report_append("binder.app_visible=");report_append(nbuf);report_append("/36\n");
        report_append("binder.shell_visible=");report_append(shell_count);report_append("/36\n");
        report_append("binder.shell_only=");int any=0;for(int i=0;i<KNOWN_SERVICE_COUNT;++i){if(csv_has_token(shell_found,g_known_services[i])&&!csv_has_token(app_found,g_known_services[i])){if(any++)report_append(",");report_append(g_known_services[i]);}}if(!any)report_append("none");report_append("\n");
        report_append("shell.seccomp=");report_append(ssec);report_append("\n");report_append("shell.selinux_enforce=");report_append(senf);report_append("\n");report_append(g_bridge_token[0]?"bridge.auth=token\n":"bridge.auth=none\n");
    }
    report_append("\n--- END ZORIN TRUST RUNTIME DIAGNOSTICS ---\n");
}

static const char* tab_names[9]={"SYSTEM","PROCESS","CENTER","CORE","BINDER","NETWORK","SECURITY","NATIVE","SANDBOX"};

static void draw_button(ANativeWindow_Buffer* b, int idx, int x, int y, int w, int h, int selected, int scale) {
    uint32_t accent=rgb(238,72,88), panel=rgb(18,28,39), panel2=rgb(25,39,52), fg=rgb(222,232,240), dim=rgb(130,151,169);
    fill_rect(b,x,y,w,h,selected?panel2:panel); outline_rect(b,x,y,w,h,selected?3:1,selected?accent:rgb(48,67,82));
    int tw=text_width(tab_names[idx],scale); int tx=x+(w-tw)/2; int ty=y+(h-7*scale)/2;
    draw_text(b,tx,ty,tab_names[idx],scale,selected?accent:(selected?fg:dim));
    g_tab_rects[idx][0]=x;g_tab_rects[idx][1]=y;g_tab_rects[idx][2]=w;g_tab_rects[idx][3]=h;
}

static void render(void) {
    ANativeWindow* win=(ANativeWindow*)g_window;
    if(!win || !g_activity) return;
    ANativeWindow_setBuffersGeometry(win,0,0,WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer b; if(ANativeWindow_lock(win,&b,0)!=0) return;
    g_width=b.width; g_height=b.height;
    uint32_t bg=rgb(7,12,18), panel=rgb(14,22,31), panel2=rgb(10,17,24), fg=rgb(226,234,241), dim=rgb(125,146,164), accent=rgb(238,72,88);
    fill_rect(&b,0,0,b.width,b.height,bg);
    int margin=b.width/45; if(margin<16)margin=16; if(margin>32)margin=32;
    int scale=b.width>=650?2:1; int title_scale=b.width>=650?3:2;
    int y=26;
    draw_text(&b,margin,y,"ZORIN TRUST RUNTIME",title_scale,fg);
    char header[128]; snprintf(header,sizeof(header),"V0.4 / TRUST CENTER / OWNER LINK / ZTRUST2 / NATIVE-FIRST");
    y += 11*title_scale; draw_text(&b,margin,y,header,scale,dim); y += 15*scale;
    fill_rect(&b,margin,y,b.width-2*margin,2,accent); y += 12;

    int gap=8, bh=26*scale;
    int bw3=(b.width-2*margin-2*gap)/3;
    for(int row=0;row<3;++row){for(int col=0;col<3;++col){int i=row*3+col;draw_button(&b,i,margin+col*(bw3+gap),y,bw3,bh,g_selected_tab==i,scale);}y += bh + (row<2?gap:14);}

    int runh=28*scale;
    int action_gap=8;
    int action_total=b.width-2*margin;
    int runw=(action_total-action_gap)*3/5;
    int copyw=action_total-action_gap-runw;
    fill_rect(&b,margin,y,runw,runh,rgb(17,35,34)); outline_rect(&b,margin,y,runw,runh,2,accent);
    char rb[64]; snprintf(rb,sizeof(rb),"RUN / REFRESH  #%u",g_run_counter); int rtw=text_width(rb,scale);
    draw_text(&b,margin+(runw-rtw)/2,y+(runh-7*scale)/2,rb,scale,accent);
    g_run_rect[0]=margin;g_run_rect[1]=y;g_run_rect[2]=runw;g_run_rect[3]=runh;

    int copyx=margin+runw+action_gap;
    fill_rect(&b,copyx,y,copyw,runh,rgb(23,34,45)); outline_rect(&b,copyx,y,copyw,runh,2,accent);
    int ctw=text_width(g_copy_status,scale);
    int ctx=copyx+(copyw-ctw)/2; if(ctx<copyx+4)ctx=copyx+4;
    draw_text(&b,ctx,y+(runh-7*scale)/2,g_copy_status,scale,accent);
    g_copy_rect[0]=copyx;g_copy_rect[1]=y;g_copy_rect[2]=copyw;g_copy_rect[3]=runh;
    y += runh+14;

    fill_rect(&b,margin,y,b.width-2*margin,b.height-y-24,panel2); fill_rect(&b,margin,y,5,b.height-y-24,accent);
    int cx=margin+18, cy=y+16;
    char cap[64]; snprintf(cap,sizeof(cap),"[%s PROBE]",tab_names[g_selected_tab]); draw_text(&b,cx,cy,cap,scale,fg); cy += 15*scale;
    if(g_selected_tab==0) render_system(&b,&cy,cx,scale);
    else if(g_selected_tab==1) render_process(&b,&cy,cx,scale);
    else if(g_selected_tab==2) render_trust(&b,&cy,cx,scale);
    else if(g_selected_tab==3) render_core(&b,&cy,cx,scale);
    else if(g_selected_tab==4) render_binder(&b,&cy,cx,scale);
    else if(g_selected_tab==5) render_network(&b,&cy,cx,scale);
    else if(g_selected_tab==6) render_security(&b,&cy,cx,scale);
    else if(g_selected_tab==7) render_native(&b,&cy,cx,scale);
    else render_sandbox(&b,&cy,cx,scale);

    int foot=b.height-38*scale; if(foot>cy+10) draw_text(&b,cx,foot,"DEVICE TRUST / OWNER PRESENCE / AUTHORITY / TRANSPORT ARE SEPARATE SECURITY STATES.",scale,dim);
    ANativeWindow_unlockAndPost(win);
}

static void handle_touch(int x,int y) {
    (void)trust_ui_sync_from_service();
    for(int i=0;i<9;++i) if(in_rect(x,y,g_tab_rects[i])) { g_selected_tab=i; ++g_run_counter; render(); return; }
    if(g_selected_tab==2 && in_rect(x,y,g_trust_approve_rect)) { if(g_trust_state==1){ if(trust_ui_send_command("APPROVE",g_trust_host_pub_pending)==0) snprintf(g_trust_status,sizeof(g_trust_status),"APPROVAL SENT TO TRUST SERVICE"); else snprintf(g_trust_status,sizeof(g_trust_status),"APPROVAL IPC FAILED"); } ++g_run_counter; render(); return; }
    if(g_selected_tab==2 && in_rect(x,y,g_trust_forget_rect)) { (void)trust_ui_send_command("FORGET",""); ++g_run_counter; render(); return; }
    if(g_selected_tab==2 && in_rect(x,y,g_trust_visual_rect)) { if(trust_overlay_allowed()) (void)trust_start_service_from_context(runtime_context(),1); else (void)trust_request_overlay_permission(); ++g_run_counter; render(); return; }
    if(in_rect(x,y,g_run_rect)) { ++g_run_counter; snprintf(g_copy_status,sizeof(g_copy_status),"COPY ALL"); render(); return; }
    if(in_rect(x,y,g_copy_rect)) {
        ++g_run_counter;
        snprintf(g_copy_status,sizeof(g_copy_status),"COLLECTING...");
        render();
        build_full_report();
        int rc=copy_text_to_clipboard(g_report);
        if(rc==0) snprintf(g_copy_status,sizeof(g_copy_status),"COPIED %d B",g_report_len);
        else snprintf(g_copy_status,sizeof(g_copy_status),"COPY ERR %d",rc);
        render();
        return;
    }
}

static void* trust_ui_refresh_thread(void* arg) {
    unsigned int generation=(unsigned int)(unsigned long)arg;
    while(g_running && g_ui_alive && generation==g_ui_generation) {
        int changed=trust_ui_sync_from_service();
        if(changed>0 && g_selected_tab==2 && g_window && g_activity) render();
        usleep(250000);
    }
    return 0;
}

static void* input_thread(void* arg) {
    unsigned int generation=(unsigned int)(unsigned long)arg;
    while(g_running && g_ui_alive && generation==g_ui_generation) {
        AInputQueue* q=(AInputQueue*)g_input_queue;
        if(!q) { usleep(12000); continue; }
        AInputEvent* ev=0;
        if(AInputQueue_getEvent(q,&ev)>=0 && ev) {
            if(AInputQueue_preDispatchEvent(q,ev)) continue;
            int handled=0;
            if(AInputEvent_getType(ev)==AINPUT_EVENT_TYPE_MOTION) {
                int action=AMotionEvent_getAction(ev)&AMOTION_EVENT_ACTION_MASK;
                if(action==AMOTION_EVENT_ACTION_UP) {
                    int x=(int)AMotionEvent_getX(ev,0), y=(int)AMotionEvent_getY(ev,0);
                    handle_touch(x,y); handled=1;
                }
            }
            AInputQueue_finishEvent(q,ev,handled);
        } else usleep(5000);
    }
    return 0;
}

static void on_window_created(ANativeActivity* a, ANativeWindow* w){(void)a;g_window=w;render();}
static void on_window_resized(ANativeActivity* a, ANativeWindow* w){(void)a;g_window=w;render();}
static void on_window_redraw(ANativeActivity* a, ANativeWindow* w){(void)a;g_window=w;render();}
static void on_window_destroyed(ANativeActivity* a, ANativeWindow* w){(void)a;if(g_window==w)g_window=0;}
static void on_focus(ANativeActivity* a,int focus){if(focus){ANativeActivity_setWindowFlags(a,FLAG_FULLSCREEN|FLAG_KEEP_SCREEN_ON|FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS,0);render();}}
static void on_input_created(ANativeActivity* a,AInputQueue* q){(void)a;g_input_queue=q;}
static void on_input_destroyed(ANativeActivity* a,AInputQueue* q){(void)a;if(g_input_queue==q)g_input_queue=0;}
static void on_destroy(ANativeActivity* a){
    (void)a;
    // The UI is disposable. Keep the trust worker and its application Context alive so
    // swiping the task away does not revoke an otherwise valid USB owner session.
    g_ui_alive=0; ++g_ui_generation; g_input_queue=0; g_window=0; g_activity=0;
}

__attribute__((visibility("default")))
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    (void)savedState;(void)savedStateSize;
    g_activity=activity;
    g_vm=activity->vm;
    (void)init_runtime_context(activity);
    int headless=trust_intent_headless();
    // Hide an automatic trusted-host bootstrap before installing window callbacks/drawing.
    if(headless) move_activity_to_back(activity);
    if(trust_intent_autoconnect()) g_selected_tab=2;
    g_running=1; g_ui_alive=1; unsigned int ui_generation=++g_ui_generation;
    // The foreground TrustService owns the long-lived worker. Activity is now UI only.
    // Fall back to the in-process worker if OEM policy unexpectedly rejects service startup.
    if(trust_start_service_from_context(activity->clazz,0)!=0) trust_start_worker();
    activity->instance=activity; bridge_token_from_intent();
    activity->callbacks->onNativeWindowCreated=on_window_created;
    activity->callbacks->onNativeWindowResized=on_window_resized;
    activity->callbacks->onNativeWindowRedrawNeeded=on_window_redraw;
    activity->callbacks->onNativeWindowDestroyed=on_window_destroyed;
    activity->callbacks->onWindowFocusChanged=on_focus;
    activity->callbacks->onInputQueueCreated=on_input_created;
    activity->callbacks->onInputQueueDestroyed=on_input_destroyed;
    activity->callbacks->onDestroy=on_destroy;
    ANativeActivity_setWindowFlags(activity,FLAG_FULLSCREEN|FLAG_KEEP_SCREEN_ON|FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS,0);
    pthread_t t; if(pthread_create(&t,0,input_thread,(void*)(unsigned long)ui_generation)==0) pthread_detach(t);
    pthread_t rt; if(pthread_create(&rt,0,trust_ui_refresh_thread,(void*)(unsigned long)ui_generation)==0) pthread_detach(rt);
}
