//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int dmi_compat_errno;

/**
 * @internal
 * @brief Parse the magnitude of a number as `strtoull()` does, without
 * applying its sign.
 *
 * @param[in]  str      String to parse.
 * @param[out] end      Variable to store the pointer past the number in, or
 *                      `nullptr`.
 * @param[in]  base     Base of the number, or zero to detect it by prefix.
 * @param[out] negative Variable to store whether the number has a minus sign
 *                      in.
 * @param[out] overflow Variable to store whether the magnitude does not fit
 *                      into `unsigned long long` in.
 *
 * @return Magnitude of the number, or zero if the base is invalid.
 */
static unsigned long long dmi_compat_strtonum(
        const char  *str,
        char       **end,
        int          base,
        bool        *negative,
        bool        *overflow);

/**
 * @internal
 * @brief Get the value of a digit of any base up to 36.
 *
 * @param[in] c Character to convert.
 *
 * @return Value of the digit, or `INT_MAX` if the character is not a digit.
 */
static int dmi_compat_digit(int c);

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    if (str == NULL)
        str = *saveptr;

    // Leading delimiters are skipped, and nothing is left if there is
    // nothing but them
    str += strspn(str, delim);
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    char *end = str + strcspn(str, delim);
    if (*end != '\0')
        *end++ = '\0';

    *saveptr = end;

    return str;
}

unsigned long long strtoull(const char *str, char **end, int base)
{
    unsigned long long value;
    bool negative, overflow;

    value = dmi_compat_strtonum(str, end, base, &negative, &overflow);
    if (overflow) {
        errno = ERANGE;
        return ULLONG_MAX;
    }

    return negative ? -value : value;
}

unsigned long strtoul(const char *str, char **end, int base)
{
    unsigned long long value;
    bool negative, overflow;

    value = dmi_compat_strtonum(str, end, base, &negative, &overflow);
    if (overflow || (value > ULONG_MAX)) {
        errno = ERANGE;
        return ULONG_MAX;
    }

    return negative ? -(unsigned long)value : (unsigned long)value;
}

static int dmi_compat_digit(int c)
{
    if (isdigit(c))
        return c - '0';
    if (isalpha(c))
        return tolower(c) - 'a' + 10;

    return INT_MAX;
}

static unsigned long long dmi_compat_strtonum(
        const char  *str,
        char       **end,
        int          base,
        bool        *negative,
        bool        *overflow)
{
    const char *pos = str;
    unsigned long long value = 0;
    bool found = false;

    *negative = false;
    *overflow = false;

    if ((base < 0) || (base == 1) || (base > 36)) {
        errno = EINVAL;
        if (end != NULL)
            *end = (char *)str;
        return 0;
    }

    while (isspace((unsigned char)*pos))
        pos++;

    if ((*pos == '+') || (*pos == '-'))
        *negative = (*pos++ == '-');

    // The prefix only counts when a hexadecimal digit follows it, otherwise
    // the leading zero is the whole number
    if (((base == 0) || (base == 16)) && (pos[0] == '0') &&
        (tolower((unsigned char)pos[1]) == 'x') && isxdigit((unsigned char)pos[2])) {
        pos += 2;
        base = 16;
    } else if (base == 0) {
        base = (pos[0] == '0') ? 8 : 10;
    }

    for (;; pos++) {
        int digit = dmi_compat_digit((unsigned char)*pos);
        if (digit >= base)
            break;

        found = true;

        if (value > (ULLONG_MAX - (unsigned)digit) / (unsigned)base)
            *overflow = true;
        else
            value = value * (unsigned)base + (unsigned)digit;
    }

    if (end != NULL)
        *end = (char *)(found ? pos : str);

    return value;
}
