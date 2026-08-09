#ifndef ZORIN_MINIMAL_STDIO_H
#define ZORIN_MINIMAL_STDIO_H
/* JNI's public header only needs FILE to be declared for function signatures.
   Android I/O in the app itself goes through bionic symbols declared in native_core.c. */
typedef struct ZorinFakeFILE FILE;
#endif
