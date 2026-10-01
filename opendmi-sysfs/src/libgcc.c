//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <linux/math64.h>

// 32-bit architectures divide 64-bit numbers by calling helpers of libgcc,
// which the kernel is not linked with, and divides them by explicit calls of
// div64_*() instead. The helpers the compiler calls for the library are made
// of these calls, so that the library is left as it is. They are called with
// the calling convention the module is built with, e.g. -mregparm=3 on x86.
#if BITS_PER_LONG == 32

// Declared to keep -Wmissing-prototypes quiet, the compiler knows them itself
u64 __udivmoddi4(u64 dividend, u64 divisor, u64 *remainder);
u64 __udivdi3(u64 dividend, u64 divisor);
u64 __umoddi3(u64 dividend, u64 divisor);
s64 __divmoddi4(s64 dividend, s64 divisor, s64 *remainder);
s64 __divdi3(s64 dividend, s64 divisor);
s64 __moddi3(s64 dividend, s64 divisor);

u64 __udivmoddi4(u64 dividend, u64 divisor, u64 *remainder)
{
    u64 rest;
    u64 quotient = div64_u64_rem(dividend, divisor, &rest);

    if (remainder != NULL)
        *remainder = rest;

    return quotient;
}

u64 __udivdi3(u64 dividend, u64 divisor)
{
    return div64_u64(dividend, divisor);
}

u64 __umoddi3(u64 dividend, u64 divisor)
{
    u64 rest;

    div64_u64_rem(dividend, divisor, &rest);

    return rest;
}

// Quotients of signed numbers are truncated toward zero, the same way C
// division does, and remainders take the sign of the dividend
s64 __divmoddi4(s64 dividend, s64 divisor, s64 *remainder)
{
    s64 quotient = div64_s64(dividend, divisor);

    if (remainder != NULL)
        *remainder = dividend - quotient * divisor;

    return quotient;
}

s64 __divdi3(s64 dividend, s64 divisor)
{
    return div64_s64(dividend, divisor);
}

s64 __moddi3(s64 dividend, s64 divisor)
{
    return dividend - div64_s64(dividend, divisor) * divisor;
}

#endif // BITS_PER_LONG == 32
