#include <stdio.h>
#include <stdbool.h>
#include <async/rc.h>
#include <async/test.h>

// used during tests to ensure proper functions have run
static int num_freed = 0;
static int num_finalized = 0;

typedef struct node {
    char* name;
    struct node* next;
} node_t;

void node_init(node_t* node, char* name) {
    node->next = NULL;
    node->name = name;
}

void node_finalize(void* ptr) {
    node_t* node = (node_t*)ptr;
    num_finalized++;
    if (node->next) {
        DONE(node->next);
        node->next = NULL;
    }
}

void node_free(void* ptr) {
    num_freed++;
    free(ptr);
}

void node_append(node_t* this, node_t* next) {
    this->next = next;
}

void reset_test() {
    num_freed = 0;
    num_finalized = 0;
}

bool test_single() {
    printf("collect a single reference");
    reset_test();

    node_t* node = NEW(node_t, node_finalize, node_free);
    node_init(node, "single node");
    DONE(node);
    return num_finalized == 1 && num_freed == 1;
}

bool test_linked_list() {
    printf("collect a tree");
    reset_test();

    node_t* root = NEW(node_t, node_finalize, node_free);
    node_init(root, "root node");
    
    node_t* next = NEW(node_t, node_finalize, node_free);
    node_init(next, "child node");

    node_append(root, next);
    HOLD(root->next);

    DONE(next);
    DONE(root);
    return num_finalized == 2 && num_freed == 2;
}

void debug_finalize(void* ptr) {
    node_t* node = (node_t*)ptr;
    num_finalized++;
}

bool test_cyclic_list() {
    printf("collect cyclic references");

    reset_test();

    node_t* l = NEW(node_t, debug_finalize, node_free);
    node_t* r = NEW(node_t, debug_finalize, node_free);

    node_init(l, "left");
    node_init(r, "right");

    node_append(l, r);
    node_append(r, l);

    DONE(r);
    DONE(l);

    return num_finalized == 2 && num_freed == 2;
}

int main() {
    test_preamble();
    test(test_single);
    test(test_linked_list);
    test(test_cyclic_list);
    return 0;
}
