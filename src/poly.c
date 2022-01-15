/** @file
    Implementacja biblioteki podstawowych operacji na wielomianach rzadkich
    wielu zmiennych

    @author Grzegorz Nowakowski <gn429576@students.mimuw.edu.pl>
    @date 2021
*/

#include "poly.h"

void PolyDestroy(Poly *p) {
    if (p->arr != NULL) {
        for (size_t i = 0; i < p->size; i++) {
            MonoDestroy(&p->arr[i]);
        }
        free(p->arr);
    }
 }

Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }

    Poly q = (Poly) {.size = p->size, .arr = malloc(q.size * sizeof(Mono))};
    CHECK_PTR(&q.arr);

    for (size_t i = 0; i < q.size; i++) {
        q.arr[i] = MonoClone(&p->arr[i]);
    }

    return q;
}

/**
 * Porównuje dwie wartości.
 * @param[in] a
 * @param[in] b
 * @return Jeśli @f$a<b@f$ zwraca -1, jeśli @f$a=b@f$ zwraca 0,
 * jeśli @f$a=b@f$ zwraca -1.
 */
static int ExpComparator(const void *a, const void *b) {
    Mono x = *(Mono *) a;
    Mono y = *(Mono *) b;

    if (MonoGetExp(&x) < MonoGetExp(&y)) {
        return -1;
    }
    else if (MonoGetExp(&x) == MonoGetExp(&y)) {
        return 0;
    }
    else {
        return 1;
    }
}

/**
 * Sortuje listę jednomianów.
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba jednomianów
 */
static void SortMonos(Mono monos[], size_t count) {
    qsort(monos, count, sizeof(Mono), ExpComparator);
}

bool PolyIsDeepCoeff(Poly *p) {
    if (PolyIsCoeff(p)) {
        return true;
    }
    else if (p->size == 1 && p->arr[0].exp == 0) {
        return PolyIsDeepCoeff(&p->arr[0].p);
    }
    else {
        return false;
    }
}

poly_coeff_t PolyGetDeepCoeff(Poly *p) {
    if (PolyIsCoeff(p)) {
        return p->coeff;
    }
    else {
        return PolyGetDeepCoeff(&p->arr[0].p);
    }
}

/**
 * Upraszcza wielomian do współczynnika (jeśli jest zagłębionym współczynnikiem).
 * @param[in] p : wielomian
 * @return uproszczony wielomian
 */
static Poly PolyToCoeff(Poly *p) {
    if (PolyIsZero(p)) {
        PolyDestroy(p);
        return PolyZero();
    }
    else if (!PolyIsCoeff(p) && PolyIsDeepCoeff(p)) {
        poly_coeff_t coeff = PolyGetDeepCoeff(p);
        return PolyFromCoeff(coeff);
    }
    else {
        return *p;
    }
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsZero(p)) {
        Poly r = PolyClone(q);
        return r;
    }
    else if (PolyIsZero(q)) {
        Poly r = PolyClone(p);
        return r;
    }
    else if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return PolyFromCoeff(p->coeff + q->coeff);
    }
    else if (PolyIsCoeff(p)) { // wydzielić do innej funkcji
        Mono pom = (Mono) {.p = PolyClone(p), .exp = 0};
        Poly r = (Poly) {.size = 1, .arr = malloc(sizeof(Mono))};
        CHECK_PTR(&r.arr);
        r.arr[0] = pom;

        Poly result = PolyAdd(&r, q);
        PolyDestroy(&r);
        return result;
    }
    else if (PolyIsCoeff(q)) { // jak wyżej
        Mono pom = (Mono) {.p = PolyClone(q), .exp = 0};
        Poly r = (Poly) {.size = 1, .arr = malloc(sizeof(Mono))};
        CHECK_PTR(&r.arr);
        r.arr[0] = pom;

        Poly result = PolyAdd(p, &r);
        PolyDestroy(&r);
        return result;
    }
    else {
        Poly r = (Poly) {.size = p->size + q->size,
                         .arr = malloc(r.size * sizeof(Mono))};
        CHECK_PTR(&r.arr);
        size_t index_p = 0, index_q = 0, index_r = 0;

        while (index_p < p->size && index_q < q->size) {
            if (MonoGetExp(&p->arr[index_p]) < MonoGetExp(&q->arr[index_q])) {
                r.arr[index_r] = MonoClone(&p->arr[index_p]);
                index_p++;
                index_r++;
            }
            else if (MonoGetExp(&p->arr[index_p]) > MonoGetExp(&q->arr[index_q])) {
                r.arr[index_r] = MonoClone(&q->arr[index_q]);
                index_q++;
                index_r++;
            }
            else {
                r.arr[index_r] = (Mono) {.exp = MonoGetExp(&p->arr[index_p]),
                        .p = PolyAdd(&p->arr[index_p].p, &q->arr[index_q].p)};
                if (PolyIsZero(&r.arr[index_r].p)) {
                    PolyDestroy(&r.arr[index_r].p);
                }
                index_p++;
                index_q++;
                index_r++;
                r.size--;
            }
        }
        while (index_p < p->size) {
            r.arr[index_r] = MonoClone(&p->arr[index_p]);
            index_p++;
            index_r++;
        }
        while (index_q < q->size) {
            r.arr[index_r] = MonoClone(&q->arr[index_q]);
            index_q++;
            index_r++;
        }

        Poly result = (Poly) {.size = r.size,
                              .arr = malloc(r.size * sizeof(Mono))};
        CHECK_PTR(&result.arr);
        int index = 0;

        for (size_t i = 0; i < r.size; i++) {
            if (!PolyIsZero(&r.arr[i].p)) {
                result.arr[index] = MonoClone(&r.arr[i]);
                index++;
            }
        }

        PolyDestroy(&r);
        result.size = index;

        if (result.size == 0) {
            PolyDestroy(&result);
            return PolyZero();
        }
        else {
            result.arr = realloc(result.arr, result.size * sizeof(Mono));
            CHECK_PTR(&result.arr);
            return result;
        }
    }
}

Poly PolyOwnMonos(size_t count, Mono *monos) {
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }

    SortMonos(monos, count);

    Poly result = (Poly) {.size = count, .arr = malloc(count * sizeof(Mono))};
    CHECK_PTR(&result.arr);
    result.arr[0] = MonoClone(&monos[0]);
    int index = 0;

    for (size_t i = 1; i < count; i++) {
        if (MonoGetExp(&result.arr[index]) == MonoGetExp(&monos[i])) {
            Poly pom = PolyAdd(&result.arr[index].p, &monos[i].p);
            PolyDestroy(&result.arr[index].p);
            result.arr[index].p = pom;
            result.size--;
        }
        else if (PolyIsZero(&result.arr[index].p)) {
            PolyDestroy(&result.arr[index].p);
            result.arr[index] = MonoClone(&monos[i]);
            result.size--;
        }
        else {
            index++;
            result.arr[index] = MonoClone(&monos[i]);
        }
    }

    if (PolyIsZero(&result.arr[index].p)) {
        MonoDestroy(&result.arr[index]);
        result.size--;
    }

    for (size_t i = 0; i < count; i++) {
        MonoDestroy(&monos[i]);
    }
    free(monos);

    if (result.size == 0) {
        PolyDestroy(&result);
        return PolyZero();
    }
    else {
        result.arr = realloc(result.arr, result.size * sizeof(Mono));
        CHECK_PTR(&result.arr);
        return result;
    }
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }

    Mono *copy = malloc(count * sizeof(Mono));
    CHECK_PTR(&copy);
    for (size_t i = 0; i < count; i++) {
        copy[i] = monos[i];
    }

    return PolyOwnMonos(count, copy);
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }

    Mono *copy = malloc(count * sizeof(Mono));
    CHECK_PTR(&copy);
    for (size_t i = 0; i < count; i++) {
        copy[i] = MonoClone(&monos[i]);
    }

    return PolyOwnMonos(count, copy);
}

/**
 * Mnoży wielomian przez stałą.
 * @param[in] p : wielomian
 * @param[in] coeff : stała
 * @return @f$p * coeff@f$
 */
static Poly PolyMulByCoeff(const Poly *p, poly_coeff_t coeff) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff * coeff);
    }
    else {
        Poly q = (Poly) {.size = p->size, .arr = malloc(q.size * sizeof(Mono))};
        CHECK_PTR(&q.arr);

        for (size_t i = 0; i < p->size; i++) {
            Poly pom = PolyMulByCoeff(&p->arr[i].p, coeff);

            if (PolyIsZero(&pom)) {
                q.size--;
            }
            else {
                q.arr[i].p = pom;
                q.arr[i].exp = MonoGetExp(&p->arr[i]);
            }
        }

        if (q.size == 0) {
            PolyDestroy(&q);
            return PolyZero();
        }
        else {
            q.arr = realloc(q.arr, q.size * sizeof(Mono));
            CHECK_PTR(&q.arr);
            return q;
        }
    }
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsZero(p) || PolyIsZero(q)) {
        return PolyZero();
    }
    else if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return PolyFromCoeff(p->coeff * q->coeff);
    }
    else if (PolyIsCoeff(p)) {
        return PolyMulByCoeff(q, p->coeff);
    }
    else if (PolyIsCoeff(q)) {
        return PolyMulByCoeff(p, q->coeff);
    }
    else {
        size_t monos_size = p->size * q->size;
        Mono monos[monos_size];
        int index = 0;

        for (size_t i = 0; i < p->size; i++) {
            for (size_t j = 0; j < q->size; j++) {
                monos[index] = (Mono) {.p = PolyMul(&p->arr[i].p, &q->arr[j].p),
                       .exp = MonoGetExp(&p->arr[i]) + MonoGetExp(&q->arr[j])};
                index++;
            }
        }

        return PolyAddMonos(monos_size, monos);
    }
}

Poly PolyNeg(const Poly *p) {
    return PolyMulByCoeff(p, -1);
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly r = PolyNeg(q);
    Poly result = PolyAdd(p, &r);
    PolyDestroy(&r);
    return result;
}

/**
 * Schodzi w rekurencji do interesującego nas miejsca (czyli aż @p var_idx = 0),
 * po czym szuka największego wykładnika spośród wszystkich jednomianów na tej
 * głębokości.
 * @param[in] p : wielomian
 * @param[in] var_idx : głębokość rekurencji
 * @param[in] max : maksymalny znaleziony wykładnik
 * @param[in] tmp : aktualny znaleziony wykładnik
 */
static void PolyDegByHelp(const Poly *p, size_t *var_idx, poly_exp_t *max, poly_exp_t *tmp) {
    if (PolyIsZero(p)) {
        *tmp = -1;
    }
    else if (PolyIsCoeff(p)) {
        *tmp = 0;
    }
    else if (*var_idx == 0) {
        for (size_t i = 0; i < p->size; i++) {
            poly_exp_t exp = MonoGetExp(&p->arr[i]);
            if (exp > *tmp) {
                *tmp = exp;
            }
        }

        if (*tmp > *max) {
            *max = *tmp;
        }
    }
    else {
        (*var_idx)--;
        for (size_t i = 0; i < p->size; i++) {
            PolyDegByHelp(&p->arr[i].p, var_idx, max, tmp);
        }
        (*var_idx)++;
    }
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsZero(p)) {
        return -1;
    }
    else if (PolyIsCoeff(p)) {
        return 0;
    }
    else if (var_idx == 0) {
        poly_exp_t max = 0;

        for (size_t i = 0; i < p->size; i++) {
            poly_exp_t exp = MonoGetExp(&p->arr[i]);
            if (exp > max) {
                max = exp;
            }
        }

        return max;
    }

    poly_exp_t max = 0;

    for (size_t i = 0; i < p->size; i++) {
        poly_exp_t tmp = 0;
        size_t var_idx_pom = var_idx - 1;
        PolyDegByHelp(&p->arr[i].p, &var_idx_pom, &max, &tmp);

        if (tmp > max) {
            max = tmp;
        }
    }

    return max;
}

/**
 * Schodzi w rekurencji najniżej jak się da aktualizując przy tym @p tmp,
 * a na końcu aktualizuje @p max jeśli znaleźliśmy większy stopień.
 * @param[in] p : wielomian
 * @param[in] max : maksymalna suma wykładników
 * @param[in] tmp : aktualna suma wykładników
 */
static void PolyDegHelp(const Poly *p, poly_exp_t *max, poly_exp_t *tmp) {
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < p->size; i++) {
            *tmp += MonoGetExp(&p->arr[i]);

            if (*tmp > *max) {
                *max = *tmp;
            }

            PolyDegHelp(&p->arr[i].p, max, tmp);
            *tmp -= MonoGetExp(&p->arr[i]);
        }
    }
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p)) {
        return -1;
    }
    else if (PolyIsCoeff(p)) {
        return 0;
    }
    else {
        poly_exp_t max = 0;

        for (size_t i = 0; i < p->size; i++) {
            poly_exp_t tmp = MonoGetExp(&p->arr[i]);
            PolyDegHelp(&p->arr[i].p, &max, &tmp);

            if (tmp > max) {
                max = tmp;
            }
        }

        return max;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    Poly x = PolyToCoeff((Poly*)p);
    Poly y = PolyToCoeff((Poly*)q);

    if (PolyIsCoeff(&x) && PolyIsCoeff(&y))
        return x.coeff == y.coeff;

    if (PolyIsCoeff(&x) || PolyIsCoeff(&y))
        return false;

    if (x.size != y.size)
        return false;

    for (size_t i = 0; i < x.size; i++) {
        if (MonoGetExp(&x.arr[i]) != MonoGetExp(&y.arr[i]))
            return false;

        if (!PolyIsEq(&x.arr[i].p, &y.arr[i].p))
            return false;
    }

    return true;
}

/**
 * Podnosi stałą @p x do potęgi @p exp.
 * @param[in] x : stała
 * @param[in] exp : wykładnik
 * @return @f$x^{exp}@f$
 */
static poly_coeff_t Power(poly_coeff_t x, poly_exp_t exp) {
    if (exp == 0) {
        return 1;
    }
    else if (exp % 2 == 0) {
        poly_coeff_t y = Power(x, exp / 2);
        return y * y;
    }
    else {
        poly_coeff_t y = Power(x, exp - 1);
        return x * y;
    }
}

/**
 * Sumuje listę wielomianów.
 * @param[in] count : liczba wielomianów
 * @param[in] polys : tablica wielomianów
 * @return wielomian będący sumą wielomianów
 */
static Poly PolyAddPolys(size_t count, Poly polys[]) {
    Poly result = PolyZero();

    for (size_t i = 0; i < count; i++) {
        Poly pom = PolyAdd(&result, &polys[i]);
        PolyDestroy(&result);
        PolyDestroy(&polys[i]);
        result = pom;
    }

    free(polys);

    return result;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }

    Poly *polys = malloc(p->size * sizeof(Poly));
    CHECK_PTR(polys);

    for (size_t i = 0; i < p->size; i++) {
        Mono tmp = p->arr[i];
        if (PolyIsCoeff(&p->arr[i].p)) {
            polys[i] = PolyFromCoeff(tmp.p.coeff * Power(x, tmp.exp));
        }
        else {
            polys[i] = PolyMulByCoeff(&tmp.p, Power(x, tmp.exp));
        }
    }

    return PolyAddPolys(p->size, polys);
}

/**
 * Podnosi wielomian @p p do potęgi @p exp.
 * @param[in] p : wielomian
 * @param[in] exp : wykładnik
 * @return @f$p^{exp}@f$
 */
static Poly PolyPower(const Poly *p, poly_exp_t exp) {
    if (exp == 0) {
        return PolyFromCoeff(1);
    }
    else if (exp % 2 == 0) {
        Poly q = PolyPower(p, exp / 2);
        Poly result = PolyMul(&q, &q);
        PolyDestroy(&q);
        return result;
    }
    else {
        Poly q = PolyPower(p, exp - 1);
        Poly result = PolyMul(p, &q);
        PolyDestroy(&q);
        return result;
    }
}

/**
 * PolyCompose z dodatkowym argumentem określającym, który wielomian z tablicy
 * aktualnie wstawiamy.
 * @param[in] p : wielomian
 * @param[in] k : liczba wielomianów
 * @param[in] idx : numer wielomianu z tablicy, który podstawiamy do @p p
 * @param[in] q : tablica wielomianów
 * @return @f$p(q_0, q_1, q_2, \ldots)@f$
 */
static Poly PolyComposeHelp(const Poly *p, size_t k, size_t idx, const Poly q[]) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }
    else {
        Poly *polys = malloc(p->size * sizeof(Poly));
        CHECK_PTR(polys);
        Poly arg_first;
        Poly arg_second;

        for (size_t i = 0; i < p->size; i++) {
            arg_first = PolyComposeHelp(&p->arr[i].p, k, idx + 1, q);

            if (idx < k) {
                arg_second = PolyPower(&q[idx], p->arr[i].exp);
            }
            else if (MonoGetExp(&p->arr[i]) == 0) {
                arg_second = PolyFromCoeff(1);
            }
            else {
                arg_second = PolyZero();
            }

            polys[i] = PolyMul(&arg_first, &arg_second);
            PolyDestroy(&arg_first);
            PolyDestroy(&arg_second);
        }

        return PolyAddPolys(p->size, polys);
    }
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
    return PolyComposeHelp(p, k, 0, q);
}