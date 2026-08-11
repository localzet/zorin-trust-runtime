#ifndef ZORIN_MINIMAL_STDIO_H
#define ZORIN_MINIMAL_STDIO_H
/* Публичному JNI header достаточно объявления FILE для сигнатур функций.
Само Android I/O в приложении идёт через bionic symbols, объявленные в native_core.c. */
typedef struct ZorinFakeFILE FILE;
#endif
