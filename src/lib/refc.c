#include <refc.h>
#include <stdio.h>
#include <stdlib.h>

/** 3
 * rc_alloc - reference counted allocator
 * rc_alloc defines a size bytes long region of memory with book-keeping data
 * such that the number of references can be tallied manually during runtime
 * using retain(void*) and release(void*).
 *
 * size is the number of bytes to allocate for the memory object. finalizer is
 * a function (or NULL) that will be run on the object before it is collected.
 * destructor is the collection function, which will default to free() if NULL.
 * 
 * MACROS
 * You may opt to use NEW(type, finalizer, destructor), which performs same
 * action as rc_alloc, but infers the size automatically from the type.
 *
 * WARNINGS
 * Do not attempt to free(3) a pointer returned from rc_alloc. Instead, call
 * release(3).
 *
 * SEE ALSO
 * retain(3), release(3), set_destructor(3), set_finalizer(3) 
 *
 * AUTHORS
 * james lay <james@jlmgtech.com>
 * */
void* rc_alloc(size_t size, finalizer_t finalizer, destructor_t destructor)
{
    rc_t* rc = malloc(sizeof(rc_t) + size);
    rc->count = 1;
    rc->finalizer = finalizer;
    rc->destructor = destructor ? destructor : free;
    return (void*)(rc+1);
}

/** 3
 * retain - add reference to rc object
 * ptr is the pointer of the object returned by rc_alloc(3)
 *
 * WARNINGS
 * Do not attempt to free(3) a pointer returned from rc_alloc. Instead, call
 * release(3).
 *
 * MACROS
 * You may use HOLD(ptr) instead of retain.
 *
 * SEE ALSO
 * rc_alloc(3), release(3), set_destructor(3), set_finalizer(3) 
 *
 * AUTHORS
 * james lay <james@jlmgtech.com>
 */
void retain(void* ptr)
{
    rc_t* rc = (rc_t*)ptr - 1;
    rc->count += 1;
}


/** 3
 * release - decrement number of references on an rc object
 * ptr is the pointer of the object returned by rc_alloc(3)
 *
 * NOTES
 * release should be called in place of free(3). If you never call retain(3),
 * you should be able to use release much like you'd use free(3), but for every
 * time you retain(3) an object, you must call release on it, plus one
 * additional time to reach 0 (since the count starts at 1 when allocated by
 * rc_alloc(3)).
 *
 * MACROS
 * You may use DONE(ptr) in place of release.
 *
 * SEE ALSO
 * rc_alloc(3), retain(3), set_destructor(3), set_finalizer(3) 
 *
 * AUTHORS
 * james lay <james@jlmgtech.com>
 */
void release(void* ptr)
{
    rc_t* rc = (rc_t*)ptr - 1;
    rc->count -= 1;
    if (rc->count < 1) {
        if (rc->finalizer) {
            rc->finalizer(rc+1);
        }
        rc->destructor(rc);
    }
}

/** 3
 * set_destructor - dynamically change the destructor invoked when an rc object has no more references.
 * The destructor is a function that will receive a void pointer that is
 * guaranteed to be the beginning of a block allocated by malloc. This is why
 * free(3) may be specified as a valid destructor.
 *
 * SEE ALSO
 * rc_alloc(3), retain(3), release(3), set_finalizer(3)
 *
 * AUTHORS
 * james lay <james@jlmgtech.com>
 */
void set_destructor(void* ptr, destructor_t destructor)
{
    rc_t* rc = (rc_t*)ptr - 1;
    rc->destructor = destructor;
}

/** 3
 * set_finalizer - dynamically change the finalizer invoked when an rc object has no more references.
 * The finalizer is a function that will also receive void* ptr (the rc object)
 * just before it is freed.
 *
 * SEE ALSO
 * rc_alloc(3), retain(3), release(3), set_destructor(3)
 *
 * AUTHORS
 * james lay <james@jlmgtech.com>
 */
void set_finalizer(void* ptr, finalizer_t finalizer)
{
    rc_t* rc = (rc_t*)ptr - 1;
    rc->finalizer = finalizer;
}

inline void* get_freeable_ptr(void* ptr)
{
    return (rc_t*)ptr - 1;
}
