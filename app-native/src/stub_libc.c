typedef unsigned long size_t;
typedef long ssize_t;
typedef long off_t;
typedef unsigned long pthread_t;
struct utsname {
    char a[390];
};
struct timespec {
    long a, b;
};
int snprintf(char*a, size_t b, const char*c, ...) {
    return 0;
}
int open(const char*a, int b, ...) {
    return -1;
}
ssize_t read(int a, void*b, size_t c) {
    return -1;
}
int close(int a) {
    return 0;
}
int access(const char*a, int b) {
    return -1;
}
int getpid(void) {
    return 1;
}
unsigned int getuid(void) {
    return 1;
}
unsigned int getgid(void) {
    return 1;
}
int getpagesize(void) {
    return 4096;
}
int uname(struct utsname*a) {
    return -1;
}
size_t strlen(const char*a) {
    return 0;
}
void* memset(void*a, int b, size_t c) {
    return a;
}
void* memcpy(void*a, const void*b, size_t c) {
    return a;
}
char* strstr(const char*a, const char*b) {
    return 0;
}
ssize_t readlink(const char*a, char*b, size_t c) {
    return -1;
}
int ioctl(int a, unsigned long b, ...) {
    return -1;
}
int socket(int a, int b, int c) {
    return -1;
}
int connect(int a, const void*b, unsigned int c) {
    return -1;
}
ssize_t send(int a, const void*b, size_t c, int d) {
    return -1;
}
ssize_t recv(int a, void*b, size_t c, int d) {
    return -1;
}
void* mmap(void*a, size_t b, int c, int d, int e, off_t f) {
    return(void*)-1;
}
int munmap(void*a, size_t b) {
    return -1;
}
int mprotect(void*a, size_t b, int c) {
    return -1;
}
int epoll_create1(int a) {
    return -1;
}
int eventfd(unsigned int a, int b) {
    return -1;
}
int pipe2(int a[2], int b) {
    return -1;
}
long syscall(long a, ...) {
    return -1;
}
int clock_gettime(int a, struct timespec*b) {
    return -1;
}
unsigned int usleep(unsigned int a) {
    return 0;
}
int pthread_create(pthread_t*a, const void*b, void*(*c)(void*), void*d) {
    return -1;
}
int pthread_detach(pthread_t a) {
    return 0;
}
int __system_property_get(const char*a, char*b) {
    return 0;
}
static int e;
int* __errno(void) {
    return &e;
}
int fork(void) {
    return -1;
}
int waitpid(int a, int*b, int c) {
    return -1;
}
ssize_t write(int a, const void*b, size_t c) {
    return -1;
}
void _exit(int a) {
    for(;;) {
    }
}
int dup2(int a, int b) {
    return -1;
}
int execve(const char*a, char*const b[], char*const c[]) {
    return -1;
}
