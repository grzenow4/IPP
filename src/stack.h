/** @file
    Interfejs stosu

    @author Grzegorz Nowakowski <gn429576@students.mimuw.edu.pl>
    @date 2021
 */

#ifndef __STACK_H__
#define __STACK_H__

#include "poly.h"
#include <stdlib.h>

#define EXTEND_FACTOR 2
#define INIT_SIZE 32

/**
 * To jest struktura przechowująca stos.
 */
typedef struct Stack {
    Poly *array; ///< tablica reprezentująca stos
    size_t pick; ///< indeks pierwszego wolnego miejsca w tablicy
    size_t size; ///< pojemność stosu
} Stack;

/**
 * Tworzy stos.
 * @return stos
 */
Stack InitStack();

/**
 * Rozszerza pojemność stosu zwiększając jego rozmiar dwukrotnie.
 * @param[in] s : stos
 */
void ExtendStack(Stack *s);

/**
 * Usuwa stos z pamięci wraz z wielomianami, które się w nim znajdują.
 * @param[in] s : stos
 */
void RemoveStack(Stack *s);

/**
 * Sprawdza czy stos jest pełny.
 * @param[in] s : stos
 * @return Czy stos jest pełny?
 */
bool StackIsFull(Stack *s);

/**
 * Sprawdza czy stos jest pusty.
 * @param[in] s : stos
 * @return Czy stos jest pusty?
 */
bool StackIsEmpty(Stack *s);

/**
 * Wrzuca wielomian na stos.
 * @param[in] p : wielomian
 * @param[in] s : stos
 */
void StackPush(const Poly *p, Stack *s);

/**
 * Zwraca wielomian z wierzchołka stosu jednocześnie go zdejmując.
 * @param[in] s : stos
 * @return wielomian z wierzchołka stosu
 */
Poly StackPop(Stack *s);

/**
 * Zwraca wielomian z wierzchołka stosu.
 * @param[in] s : stos
 * @return wielomian z wierzchołka stosu
 */
Poly StackTop(Stack *s);

#endif