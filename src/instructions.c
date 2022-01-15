/** @file
    Implementacja biblioteki instrukcji kalkulatora di wielomianów rzadkich
    wielu zmiennych oraz funkcji wypisujących błedy

    @author Grzegorz Nowakowski <gn429576@students.mimuw.edu.pl>
    @date 2021
 */

#include "instructions.h"

void CommandErr(int line_number) {
    fprintf(stderr, "ERROR %d WRONG COMMAND\n", line_number);
}

void DegByErr(int line_number) {
    fprintf(stderr, "ERROR %d DEG BY WRONG VARIABLE\n", line_number);
}

void AtErr(int line_number) {
    fprintf(stderr, "ERROR %d AT WRONG VALUE\n", line_number);
}

void UnderflowErr(int line_number) {
    fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", line_number);
}

void PolyErr(int line_number) {
    fprintf(stderr, "ERROR %d WRONG POLY\n", line_number);
}

void ComposeErr(int line_number) {
    fprintf(stderr, "ERROR %d COMPOSE WRONG PARAMETER\n", line_number);
}

void Zero(Stack *s) {
    Poly p = PolyZero();
    StackPush(&p, s);
}

void IsCoeff(Stack *s) {
    Poly p = StackTop(s);
    printf("%d\n", PolyIsDeepCoeff(&p));
}

void IsZero(Stack *s) {
    Poly p = StackTop(s);
    printf("%d\n", PolyIsZero(&p));
}

void Clone(Stack *s) {
    Poly p = StackTop(s);
    Poly q = PolyClone(&p);
    StackPush(&q, s);
}

void Add(Stack *s) {
    Poly p = StackPop(s);
    Poly q = StackPop(s);
    Poly r = PolyAdd(&p, &q);
    StackPush(&r, s);
    PolyDestroy(&p);
    PolyDestroy(&q);
}

void Mul(Stack *s) {
    Poly p = StackPop(s);
    Poly q = StackPop(s);
    Poly r = PolyMul(&p, &q);
    StackPush(&r, s);
    PolyDestroy(&p);
    PolyDestroy(&q);
}

void Neg(Stack *s) {
    Poly p = StackPop(s);
    Poly q = PolyNeg(&p);
    StackPush(&q, s);
    PolyDestroy(&p);
}

void Sub(Stack *s) {
    Poly p = StackPop(s);
    Poly q = StackPop(s);
    Poly r = PolySub(&p, &q);
    StackPush(&r, s);
    PolyDestroy(&p);
    PolyDestroy(&q);
}

void IsEq(Stack *s) {
    Poly p = StackTop(s);
    Poly q = s->array[s->pick - 2];
    printf("%d\n", PolyIsEq(&p, &q));
}

void Deg(Stack *s) {
    Poly p = StackTop(s);
    printf("%d\n", PolyDeg(&p));
}

void DegBy(Stack *s, size_t idx) {
    Poly p = StackTop(s);
    printf("%d\n", PolyDegBy(&p, idx));
}

void At(Stack *s, poly_coeff_t x) {
    Poly p = StackPop(s);
    Poly q = PolyAt(&p, x);
    StackPush(&q, s);
    PolyDestroy(&p);
}

/**
 * Wypisuje wielomian na standardowe wyjście.
 * @param[in] p : wielomian
 */
static void PrintPoly(Poly *p) {
    if (PolyIsDeepCoeff(p)) {
        poly_coeff_t coeff = PolyGetDeepCoeff(p);
        printf("%ld", coeff);
    }
    else {
        for (size_t i = 0; i < p->size; i++) {
            putchar('(');
            PrintPoly(&p->arr[i].p);
            putchar(',');
            printf("%d", p->arr[i].exp);
            putchar(')');
            if (i + 1 < p->size) {
                putchar('+');
            }
        }
    }
}

void Print(Stack *s) {
    Poly p = StackTop(s);
    PrintPoly(&p);
    putchar('\n');
}

void Pop(Stack *s) {
    s->pick--;
    PolyDestroy(&s->array[s->pick]);
}

void Compose(Stack *s, size_t k) {
    Poly p = StackPop(s);
    Poly *q = malloc(k * sizeof(Poly));
    CHECK_PTR(&q);
    for (size_t i = 0; i < k; i++) {
        q[k - 1 - i] = StackPop(s);
    }

    Poly r = PolyCompose(&p, k, q);
    StackPush(&r, s);
    PolyDestroy(&p);
    for (size_t i = 0; i < k; i++) {
        PolyDestroy(&q[i]);
    }
    free(q);
}

void ExecZero(Stack *s) {
    if (StackIsFull(s)) {
        ExtendStack(s);
    }
    else {
        Zero(s);
    }
}

void ExecIsCoeff(Stack *s, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        IsCoeff(s);
    }
}

void ExecIsZero(Stack *s, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        IsZero(s);
    }
}

void ExecClone(Stack *s, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        Clone(s);
    }
}

void ExecAdd(Stack *s, int line_number) {
    if (s->pick < 2) {
        UnderflowErr(line_number);
    }
    else {
        Add(s);
    }
}

void ExecMul(Stack *s, int line_number) {
    if (s->pick < 2) {
        UnderflowErr(line_number);
    }
    else {
        Mul(s);
    }
}

void ExecNeg(Stack *s, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        Neg(s);
    }
}

void ExecSub(Stack *s, int line_number) {
    if (s->pick < 2) {
        UnderflowErr(line_number);
    }
    else {
        Sub(s);
    }
}

void ExecIsEq(Stack *s, int line_number) {
    if (s->pick < 2) {
        UnderflowErr(line_number);
    }
    else {
        IsEq(s);
    }
}

void ExecDeg(Stack *s, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        Deg(s);
    }
}

void ExecDegBy(Stack *s, size_t idx, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        DegBy(s, idx);
    }
}

void ExecAt(Stack *s, poly_coeff_t x, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        At(s, x);
    }
}

void ExecPrint(Stack *s, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        Print(s);
    }
}

void ExecPop(Stack *s, int line_number) {
    if (StackIsEmpty(s)) {
        UnderflowErr(line_number);
    }
    else {
        Pop(s);
    }
}

void ExecCompose(Stack *s, size_t k, int line_number) {
    if (s->pick == 0 || s->pick - 1 < k) {
        UnderflowErr(line_number);
    }
    else {
        Compose(s, k);
    }
}