#ifndef refc_h
#define refc_h
#include <stddef.h>

#define NEW(type, finalizer, destructor) ((type*)rc_alloc(sizeof(type), finalizer, destructor));
#define HOLD(x) retain((void*)x)
#define DONE(x) release((void*)x)

typedef void (*finalizer_t)(void*);
typedef void (*destructor_t)(void*);
typedef struct rc {
    int count;
    finalizer_t finalizer;
    destructor_t destructor;
} rc_t;

void* rc_alloc(size_t size, finalizer_t finalizer, destructor_t destructor);
void retain(void* ptr);
void release(void* ptr);
void set_destructor(void* ptr, destructor_t destructor);
void set_finalizer(void* ptr, finalizer_t finalizer);

#endif
