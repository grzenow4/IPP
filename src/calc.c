/** @file
    Implementacja kalkulatora działającego na wielomianach rzadkich wielu
    zmiennych, stosujący odwrotną notację polską

    @author Grzegorz Nowakowski <gn429576@students.mimuw.edu.pl>
    @date 2021
*/

#define _GNU_SOURCE

#include "instructions.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define DEC 10

/**
 * Konwertuje napis @p arg na argument do instrukcji @p DegBy.
 * @param[in] arg : argument jako napis
 * @return argument jako liczba
 */
unsigned long ConvertDegByOrComposeArg(char *arg) {
    char *remainder;
    unsigned long result = strtoul(arg, &remainder, DEC);
    if (!isdigit(arg[0]) || remainder[0] != '\0' || errno == ERANGE) {
        errno = 0;
        return 0;
    }
    else {
        return result;
    }
}

/**
 * Konwertuje napis @p arg na argument do instrukcji @p At.
 * @param[in] arg : argument jako napis
 * @return argument jako liczba
 */
long ConvertAtArg(char *arg) {
    char *remainder;
    long result = strtol(arg, &remainder, DEC);
    if (arg[0] == '+' || arg[0] == ' ' || arg[0] == '\t' ||
        remainder[0] != '\0' || errno == ERANGE) {
        errno = 0;
        return 0;
    }
    else {
        return result;
    }
}

/**
 * Sprawdza czy napis reprezentuje wielomian, który jest współczynnikiem.
 * @param[in] str : wielomian jako napis
 * @param[in] size : długość napisu
 * @return Czy napis reprezentuje wielomian, który jest współczynnikiem?
 */
bool IsPolyCoeff(const char *str, size_t size) {
    if ((str[0] == '-' && size < 3) || (!isdigit(str[0]) && str[0] != '-')) {
        return false;
    }

    for (size_t i = 1; i < size - 1; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }

    return true;
}

/**
 * Sprawdza czy napis reprezentuje wielomian.
 * @param[in] str : wielomian jako napis
 * @param[in] size : długość napisu
 * @return Czy napis reprezentuje wielomian?
 */
bool IsPolyCorrect(const char *str, size_t size) {
    for (size_t i = 0; i < size - 1; i++) {
        if (str[i] == '(' && !isdigit(str[i + 1]) &&
            str[i + 1] != '-' && str[i + 1] != '(') {
            return false;
        }
        else if (str[i] == ')' && str[i + 1] != '+' &&
                str[i + 1] != ',' && str[i + 1] != '\0') {
            return false;
        }
        else if (str[i] == '+' && str[i + 1] != '(') {
            return false;
        }
        else if (str[i] == '-' && !isdigit(str[i + 1])) {
            return false;
        }
        else if (str[i] == ',' && !isdigit(str[i + 1])) {
            return false;
        }
        else if (isdigit(str[i]) && !isdigit(str[i + 1]) &&
                            str[i + 1] != ',' && str[i + 1] != ')') {
            return false;
        }
        else if (str[i] == '\0') {
            return false;
        }
    }

    int brackets = 0;

    for (size_t i = 0; i < size - 1; i++) {
        switch (str[i]) {
            case '(':
                brackets++;
                break;
            case ')':
                brackets--;
                if (brackets < 0) {
                    return false;
                }
                break;
        }
    }

    return true;
}

/**
 * Zlicza z ilu jednomianów składa się wielomian na obecnym poziomie rekurencji.
 * @param[in] str : wielomian jako napis
 * @return liczba jednomianów składających się w wielomian na obecnej głębokości
 */
size_t NumberOfMonos(const char *str) {
    size_t size = strlen(str);
    int cur_brackets = 0;
    size_t monos = 1;

    for (size_t i = 0; i < size; i++) {
        switch (str[i]) {
            case '(':
                cur_brackets++;
                break;
            case ')':
                cur_brackets--;
                if (cur_brackets < 0) {
                    return monos;
                }
                break;
            case '+':
                if (cur_brackets == 0) {
                    monos++;
                }
                break;
        }
    }

    return monos;
}

/**
 * Konwertuje część napisu jako współczynnik jednomianu
 * i nadpisuje do *str resztę.
 * @param[in] str : napis
 * @return współczynnik jednomianu
 */
Poly ConvertCoeff(char **str) {
    char *remainder;
    long result = strtol((*str) + 1, &remainder, DEC);

    if (remainder[0] != ',' || errno == ERANGE) {
        return PolyZero();
    }
    else {
        *str = ++remainder;
        return PolyFromCoeff(result);
    }
}

/**
 * Konwetuje część napisu jako wykładnik jednomianu i nadpisuje do *str resztę.
 * @param[in] str : napis
 * @return wykładnik jednomianu
 */
poly_exp_t ConvertExp(char **str) {
    char *remainder;
    long result = strtol(*str, &remainder, DEC);

    if (remainder[0] != ')' || result > INT_MAX ||
        result < 0 || errno == ERANGE) {
        errno = ERANGE;
        return 0;
    }
    else {
        *str = ++remainder;
        return (poly_exp_t) result;
    }
}

static inline Poly ConvertPoly(char **str);

/**
 * Konwertuje napis do jednomianu.
 * @param[in] str : wielomian jako napis
 * @return jednomian
 */
Mono ConvertMono(char **str) {
    if ((*str)[0] == '+') {
        (*str)++;
    }

    if (isdigit((*str)[1]) || (*str)[1] == '-') {
        Mono m;
        m.p = ConvertCoeff(str);
        m.exp = ConvertExp(str);
        return m;
    }
    else if ((*str)[1] == '(') {
        Mono m;
        (*str)++;
        m.p = ConvertPoly(str);
        m.exp = ConvertExp(str);
        return m;
    }

    errno = ERANGE;
    return (Mono) {.p = PolyZero(), .exp = 0};
}

/**
 * Konwetuje napis do wielomianu.
 * @param[in] str : wielomian jako napis
 * @return wielomian
 */
Poly ConvertPoly(char **str) {
    size_t monos_size = NumberOfMonos(*str);
    Mono *monos = malloc(monos_size * sizeof(Mono));
    CHECK_PTR(monos);
    for (size_t i = 0; i < monos_size; i++) {
        monos[i] = ConvertMono(str);
    }
    (*str)++;

    Poly p = PolyAddMonos(monos_size, monos);
    free(monos);
    return p;
}

/**
 * Konwertuje napis na instrukcję i ją wykonuje, jeśli jest to możliwe. Jeśli
 * wykryje jakiś błąd w trakcie konwersji to go wypisuje.
 * @param[in] s : stos
 * @param[in] buffer : napis
 * @param[in] line_length : długość wiersza
 * @param[in] line_number : numer wiersza
 */
void ParseInstruction(Stack *s, char *buffer, ssize_t line_length, int line_number) {
    if (line_length == 5 && strncmp(buffer, "ZERO", 4) == 0) {
        ExecZero(s);
    }
    else if (line_length == 9 && strncmp(buffer, "IS_COEFF", 8) == 0) {
        ExecIsCoeff(s, line_number);
    }
    else if (line_length == 8 && strncmp(buffer, "IS_ZERO", 7) == 0) {
        ExecIsZero(s, line_number);
    }
    else if (line_length == 6 && strncmp(buffer, "CLONE", 5) == 0) {
        ExecClone(s, line_number);
    }
    else if (line_length == 4 && strncmp(buffer, "ADD", 3) == 0) {
        ExecAdd(s, line_number);
    }
    else if (line_length == 4 && strncmp(buffer, "MUL", 3) == 0) {
        ExecMul(s, line_number);
    }
    else if (line_length == 4 && strncmp(buffer, "NEG", 3) == 0) {
        ExecNeg(s, line_number);
    }
    else if (line_length == 4 && strncmp(buffer, "SUB", 3) == 0) {
        ExecSub(s, line_number);
    }
    else if (line_length == 6 && strncmp(buffer, "IS_EQ", 5) == 0) {
        ExecIsEq(s, line_number);
    }
    else if (line_length == 4 && strncmp(buffer, "DEG", 3) == 0) {
        ExecDeg(s, line_number);
    }
    else if (strncmp(buffer, "DEG_BY", 6) == 0) {
        if (line_length > 7 && buffer[6] != ' ') {
            CommandErr(line_number);
        }
        else if (line_length < 9 || buffer[6] != ' ') {
            DegByErr(line_number);
        }
        else if (line_length == 9 && buffer[7] == '0') {
            ExecDegBy(s, 0, line_number);
        }
        else if (line_length - 1 != (ssize_t) strlen(buffer)) {
            DegByErr(line_number);
        }
        else {
            unsigned long idx = ConvertDegByOrComposeArg(&buffer[7]);
            if (idx == 0) {
                DegByErr(line_number);
            }
            else {
                ExecDegBy(s, idx, line_number);
            }
        }
    }
    else if (strncmp(buffer, "AT", 2) == 0) {
        if (line_length > 3 && buffer[2] != ' ') {
            CommandErr(line_number);
        }
        else if (line_length < 5 || buffer[2] != ' ') {
            AtErr(line_number);
        }
        else if (line_length == 5 && buffer[3] == '0') {
            ExecAt(s, 0, line_number);
        }
        else if (line_length - 1 != (ssize_t) strlen(buffer)) {
            AtErr(line_number);
        }
        else {
            long x = ConvertAtArg(&buffer[3]);
            if (x == 0) {
                AtErr(line_number);
            }
            else {
                ExecAt(s, x, line_number);
            }
        }
    }
    else if (line_length == 6 && strncmp(buffer, "PRINT", 5) == 0) {
        ExecPrint(s, line_number);
    }
    else if (line_length == 4 && strncmp(buffer, "POP", 3) == 0) {
        ExecPop(s, line_number);
    }
    else if (strncmp(buffer, "COMPOSE", 7) == 0) {
        if (line_length > 8 && buffer[7] != ' ') {
            CommandErr(line_number);
        }
        else if (line_length < 10 || buffer[7] != ' ') {
            ComposeErr(line_number);
        }
        else if (line_length == 10 && buffer[8] == '0') {
            ExecCompose(s, 0, line_number);
        }
        else if (line_length - 1 != (ssize_t) strlen(buffer)) {
            ComposeErr(line_number);
        }
        else {
            unsigned long k = ConvertDegByOrComposeArg(&buffer[8]);
            if (k == 0) {
                ComposeErr(line_number);
            }
            else {
                ExecCompose(s, k, line_number);
            }
        }
    }
    else {
        CommandErr(line_number);
    }
}

/**
 * Konwertuje napis na wielomian i wrzuca go na stos. Jeśli wykryje jakiś błąd
 * podczas konwersji to go wypisuje.
 * @param[in] s : stos
 * @param[in] buffer : napis
 * @param[in] line_length : długość wiersza
 * @param[in] line_number : numer wiersza
 */
void ParsePoly(Stack *s, char *buffer, ssize_t line_length, int line_number) {
    if (IsPolyCoeff(buffer, line_length)) {
        char *remainder;
        long coeff = strtol(buffer, &remainder, DEC);
        if (errno == ERANGE || remainder[0] != '\0') {
            errno = 0;
            PolyErr(line_number);
        }
        else {
            Poly p = PolyFromCoeff(coeff);
            StackPush(&p, s);
        }
    }
    else if (buffer[0] == '(' && IsPolyCorrect(buffer, line_length)) {
        char *str = buffer;
        Poly p = ConvertPoly(&str);

        if (errno == ERANGE) {
            errno = 0;
            PolyDestroy(&p);
            PolyErr(line_number);
        }
        else {
            StackPush(&p, s);
        }
    }
    else {
        PolyErr(line_number);
    }
}

/**
 * Funkcja wykonująca program.
 * @return kod wyjścia programu
 */
int main() {
    int line_number = 1;
    ssize_t line_length;
    size_t buffer_size = INIT_SIZE;
    char *buffer = malloc(INIT_SIZE * sizeof(char));
    CHECK_PTR(buffer);
    Stack stack = InitStack();

    while ((line_length = getline(&buffer, &buffer_size, stdin)) !=  -1) {
        char first_char = buffer[0];

        if (buffer[line_length - 1] != '\n' && buffer[line_length - 1] != '\0') {
            line_length++;
            if (buffer_size < (size_t) line_length) {
                buffer_size++;
                buffer = realloc(buffer, buffer_size * sizeof(char));
                CHECK_PTR(buffer);
            }
        }
        buffer[line_length - 1] = '\0';

        if (first_char == '#' || first_char == '\n' || first_char == EOF) {
            line_number++;
            continue;
        }
        else if (isalpha(first_char)) { // instruction expected
            ParseInstruction(&stack, buffer, line_length, line_number);
        }
        else { // poly expected
            ParsePoly(&stack, buffer, line_length, line_number);
        }

        line_number++;
    }

    free(buffer);
    RemoveStack(&stack);
}