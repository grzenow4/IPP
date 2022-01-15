/** @file
    Interfejs instrukcji wykonywanych przez kalkulator do wielomianów rzadkich
    wielu zmiennych oraz funkcji do wypisywania błedów

    @author Grzegorz Nowakowski <gn429576@students.mimuw.edu.pl>
    @date 2021
 */

#ifndef __INSTR_H__
#define __INSTR_H__

#include "stack.h"
#include <stdio.h>

/**
 * Jeśli program wykryje niepoprawną nazwę polecenia, wypisuje błąd.
 * @param[in] line_number : numer wiersza
 */
void CommandErr(int line_number);

/**
 * Jeśli w poleceniu DEG_BY nie podano parametru lub jest on niepoprawny,
 * program wypisuje błąd.
 * @param[in] line_number : numer wiersza
 */
void DegByErr(int line_number);

/**
 * Jeśli w poleceniu AT nie podano parametru lub jest on niepoprawny,
 * program wypisuje błąd.
 * @param[in] line_number : numer wiersza
 */
void AtErr(int line_number);

/**
 * Jeśli na stosie jest za mało wielomianów, aby wykonać polecenie, program
 * wypisuje błąd.
 * @param[in] line_number : numer wiersza
 */
void UnderflowErr(int line_number);

/**
 * Jeśli program wykryje błąd podczas parsowania wielomianu, wypisuje błąd.
 * @param[in] line_number : numer wiersza
 */
void PolyErr(int line_number);

/**
 * Jeśli program wykryje błąd podczas parsowania wielomianu, wypisuje błąd.
 * @param[in] line_number : numer wiersza
 */
void ComposeErr(int line_number);

/**
 * Wstawia na wierzchołek stosu wielomian tożsamościowo równy zeru
 * @param[in] s : stos
 */
void Zero(Stack *s);

/**
 * Sprawdza czy wielomian na wierzchołku stosu jest współczynnikiem - wypisuje
 * na standardowe wyjście 0 lub 1.
 * @param[in] s : stos
 */
void IsCoeff(Stack *s);

/**
 * Sprawdza czy wielomian na wierzchołku stosu jest tożsamościowo równy zeru –
 * wypisuje na standardowe wyjście 0 lub 1.
 * @param[in] s : stos
 */
void IsZero(Stack *s);

/**
 * Wstawia na stos kopię wielomianu z wierzchołka.
 * @param[in] s : stos
 */
void Clone(Stack *s);

/**
 * Dodaje dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek
 * stosu ich sumę.
 * @param[in] s : stos
 */
void Add(Stack *s);

/**
 * Mnoży dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek
 * stosu ich iloczyn.
 * @param[in] s : stos
 */
void Mul(Stack *s);

/**
 * Neguje wielomian na wierzchołku stosu.
 * @param[in] s : stos
 */
void Neg(Stack *s);

/**
 * Odejmuje od wielomianu z wierzchołka wielomian pod wierzchołkiem, usuwa je
 * i wstawia na wierzchołek stosu różnicę.
 * @param[in] s : stos
 */
void Sub(Stack *s);

/**
 * Sprawdza czy dwa wielomiany na wierzchu stosu są równe – wypisuje na
 * standardowe wyjście 0 lub 1.
 * @param[in] s : stos
 */
void IsEq(Stack *s);

/**
 * Wypisuje na standardowe wyjście stopień wielomianu (−1 dla wielomianu
 * tożsamościowo równego zeru).
 * @param[in] s : stos
 */
void Deg(Stack *s);

/**
 * Wypisuje na standardowe wyjście stopień wielomianu ze względu na zmienną
 * o numerze @p idx (−1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] s : stos
 * @param[in] idx : indeks zmiennej
 */
void DegBy(Stack *s, size_t idx);

/**
 * Wylicza wartość wielomianu w punkcie @p x, usuwa wielomian z wierzchołka
 * i wstawia na stos wynik operacji.
 * @param[in] s : stos
 * @param[in] x : wartość argumentu
 */
void At(Stack *s, poly_coeff_t x);

/**
 * Wypisuje na standardowe wyjście wielomian z wierzchołka stosu.
 * @param[in] s : stos
 */
void Print(Stack *s);

/**
 * Usuwa wielomian z wierzchołka stosu.
 * @param[in] s : stos
 */
void Pop(Stack *s);

/**
 * Zdejmuje wielomian z wierzchołka stosu, a następnie @p k kolejnych
 * wielomianów i wstawia na stos wynik operacji złożenia tych wielomianów.
 * @param[in] s : stos
 * @param[in] k : parametr
 */
void Compose(Stack *s, size_t k);

/**
 * Wywołuje funkcję Zero.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecZero(Stack *s);

/**
 * Wywołuje funkcję IsCoeff, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecIsCoeff(Stack *s, int line_number);

/**
 * Wywołuje funkcję IsZero, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecIsZero(Stack *s, int line_number);

/**
 * Wywołuje funkcję Clone, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecClone(Stack *s, int line_number);

/**
 * Wywołuje funkcję Add, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecAdd(Stack *s, int line_number);

/**
 * Wywołuje funkcję Mul, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecMul(Stack *s, int line_number);

/**
 * Wywołuje funkcję Neg, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecNeg(Stack *s, int line_number);

/**
 * Wywołuje funkcję Sub, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecSub(Stack *s, int line_number);

/**
 * Wywołuje funkcję IsEq, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecIsEq(Stack *s, int line_number);

/**
 * Wywołuje funkcję Deg, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecDeg(Stack *s, int line_number);

/**
 * Wywołuje funkcję DegBy, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] idx : indeks zmiennej
 * @param[in] line_number : numer wiersza
 */
void ExecDegBy(Stack *s, size_t idx, int line_number);

/**
 * Wywołuje funkcję At, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] x : wartość argumentu
 * @param[in] line_number : numer wiersza
 */
void ExecAt(Stack *s, poly_coeff_t x, int line_number);

/**
 * Wywołuje funkcję Print, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecPrint(Stack *s, int line_number);

/**
 * Wywołuje funkcję Pop, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] line_number : numer wiersza
 */
void ExecPop(Stack *s, int line_number);

/**
 * Wywołuje funkcję Compose, jeśli jest to możliwe.
 * @param[in] s : stos
 * @param[in] k : parametr
 * @param[in] line_number : numer wiersza
 */
void ExecCompose(Stack *s, size_t k, int line_number);

#endif