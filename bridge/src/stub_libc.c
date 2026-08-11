typedef unsigned long size_t;
typedef long ssize_t;
struct utsname {
    char a[390];
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
ssize_t write(int a, const void*b, size_t c) {
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
int socket(int a, int b, int c) {
    return -1;
}
int bind(int a, const void*b, unsigned int c) {
    return -1;
}
int listen(int a, int b) {
    return -1;
}
int accept4(int a, void*b, void*c, int d) {
    return -1;
}
int setsockopt(int a, int b, int c, const void*d, unsigned int e) {
    return -1;
}
int fork(void) {
    return -1;
}
int waitpid(int a, int*b, int c) {
    return -1;
}
int pipe2(int a[2], int b) {
    return -1;
}
void _exit(int a) {
}
long syscall(long a, ...) {
    return -1;
}
int __system_property_get(const char*a, char*b) {
    return 0;
}
static int er;
int* __errno(void) {
    return &er;
}
char* strchr(const char*a, int b) {
    return(char*)0;
}
void __aeabi_memclr8(void* p, size_t n) {
}
void __aeabi_memclr4(void* p, size_t n) {
}
void __aeabi_memclr(void* p, size_t n) {
}
