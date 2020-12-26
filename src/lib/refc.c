#include <refc.h>
#include <stdio.h>
#include <stdlib.h>

void* rc_alloc(size_t size, finalizer_t finalizer, destructor_t destructor) {
    rc_t* rc = malloc(sizeof(rc_t) + size);
    rc->count = 1;
    rc->finalizer = finalizer;
    rc->destructor = destructor ? destructor : free;
    return (void*)(rc+1);
}

void retain(void* ptr) {
    rc_t* rc = (rc_t*)ptr - 1;
    rc->count += 1;
}

void release(void* ptr) {
    rc_t* rc = (rc_t*)ptr - 1;
    rc->count -= 1;
    if (rc->count < 1) {
        if (rc->finalizer) {
            rc->finalizer(rc+1);
        }
        rc->destructor(rc);
    }
}

void set_destructor(void* ptr, destructor_t destructor) {
    rc_t* rc = (rc_t*)ptr - 1;
    rc->destructor = destructor;
}

void set_finalizer(void* ptr, finalizer_t finalizer) {
    rc_t* rc = (rc_t*)ptr - 1;
    rc->finalizer = finalizer;
}

inline void* get_freeable_ptr(void* ptr) {
    return (rc_t*)ptr - 1;
}
