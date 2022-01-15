/** @file
    Implementacja biblioteki stosu

    @author Grzegorz Nowakowski <gn429576@students.mimuw.edu.pl>
    @date 2021
 */

#include "stack.h"

Stack InitStack() {
    Stack s;
    s.pick = 0;
    s.size = INIT_SIZE;
    s.array = malloc(s.size * sizeof(Poly));
    CHECK_PTR(s.array);
    return s;
}

void ExtendStack(Stack *s) {
    s->size *= EXTEND_FACTOR;
    s->array = realloc(s->array, s->size * sizeof(Poly));
    CHECK_PTR(s->array);
}

void RemoveStack(Stack *s) {
    for (size_t i = 0; i < s->pick; i++) {
        PolyDestroy(&s->array[i]);
    }
    free(s->array);
}

bool StackIsFull(Stack *s) {
    return s->pick == s->size;
}

bool StackIsEmpty(Stack *s) {
    return s->pick == 0;
}

void StackPush(const Poly *p, Stack *s) {
    s->array[s->pick] = *p;
    s->pick++;
    if (StackIsFull(s)) {
        ExtendStack(s);
    }
}

Poly StackPop(Stack *s) {
    s->pick--;
    Poly p = PolyClone(&s->array[s->pick]);
    PolyDestroy(&s->array[s->pick]);
    return p;
}

Poly StackTop(Stack *s) {
    return s->array[s->pick - 1];
}