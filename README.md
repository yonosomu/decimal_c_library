# decimal_c_library

A C11 static library for decimal arithmetic with a 96-bit coefficient, an explicit sign, and a decimal scale from 0 to 28.

The library provides arithmetic, comparisons, integer and floating-point conversions, and integer-rounding operations through `s21_decimal.h`. Intermediate calculations use a fixed-size 256-bit representation instead of `__int128`.

## Features

- Addition, subtraction, multiplication, and division.
- Six numerical comparison operations, including comparison of equivalent values with different scales.
- Conversions between decimal and `int`, and between decimal and binary32 `float`.
- Negation, truncation, floor, and rounding to the nearest integer.
- Multiword integer operations with carry, borrow, shifts, multiplication, and quotient/remainder division.
- Round-half-to-even when reducing arithmetic precision and when rounding float conversions to at most seven significant decimal digits.
- Check-based unit and integration tests, HTML coverage reports, and separate sanitizer builds.

## Representation

```c
typedef struct {
  int bits[4];
} s21_decimal;
```

- `bits[0]`, `bits[1]`, and `bits[2]` hold the low, middle, and high words of an unsigned 96-bit coefficient.
- Bit 31 of `bits[3]` stores the sign.
- Bits 16 through 23 of `bits[3]` store the scale.
- All remaining bits of `bits[3]` are reserved and must be zero.

The numeric value is `(-1)^sign * coefficient / 10^scale`. The maximum coefficient is `2^96 - 1`. The internal header checks that `int` is 32 bits.

Different representations can describe the same value. For example, coefficients 15 and 150 with scales 1 and 2 both represent 1.5. Comparisons treat positive and negative zero as numerically equal.

## Public API

| Category | Functions |
| --- | --- |
| Arithmetic | `s21_add`, `s21_sub`, `s21_mul`, `s21_div` |
| Comparison | `s21_is_less`, `s21_is_less_or_equal`, `s21_is_greater`, `s21_is_greater_or_equal`, `s21_is_equal`, `s21_is_not_equal` |
| Conversion | `s21_from_int_to_decimal`, `s21_from_float_to_decimal`, `s21_from_decimal_to_int`, `s21_from_decimal_to_float` |
| Other operations | `s21_negate`, `s21_truncate`, `s21_floor`, `s21_round` |

Arithmetic functions return a status code and write the computed value through an output pointer:

| Status | Meaning |
| --- | --- |
| `0` | Success |
| `1` | Positive overflow or another rejected arithmetic input |
| `2` | Negative overflow |
| `3` | Division by zero |

Conversion and other-operation functions return `0` on success and `1` on failure. Comparisons return `1` for true and `0` for false.

### Rounding and conversion behavior

- `s21_truncate` discards fractional digits toward zero.
- `s21_floor` rounds toward negative infinity.
- `s21_round` rounds to the nearest integer, with exact halves away from zero. This policy is distinct from arithmetic half-to-even rounding.
- `s21_negate` preserves the coefficient and scale and flips the sign, including the sign of zero.
- Integer-rounding operations return scale zero and normalize a zero result to positive zero.
- Decimal-to-int conversion discards the fractional part and rejects results outside the `int` range.
- Float-to-decimal conversion rejects nonfinite values, nonzero magnitudes below `1e-28`, and magnitudes outside the representable range. It preserves signed zero.
- Float conversion uses exact integer quotient/remainder calculations for half-to-even rounding. Its significand extraction assumes binary32 `float`.

Comparison inputs must contain valid decimal metadata. Invalid reserved bits or an invalid scale are outside the supported comparison contract.

## Requirements

For the static library:

- A C11 compiler, `make`, and `ar`.
- The math library when linking an executable that uses float conversions (`-lm` on platforms that require it).

For tests and optional analysis:

- The Check unit-testing library, discoverable through `pkg-config`.
- `gcovr` and a compiler-compatible `gcov` implementation for coverage.
- `clang-format` 18.1.8 for the checked formatting configuration.
- Valgrind on Linux, or `leaks` on macOS, for a separate memory-leak check.
- Appropriate compiler runtime libraries for the requested sanitizers.

Dependencies are not downloaded or installed by the Makefile. On macOS, `/usr/bin/gcc` may invoke Apple Clang rather than GNU GCC.

## Build

Run build commands from `src`:

```sh
cd src
make all
```

The output is `src/s21_decimal.a`. `make s21_decimal.a` builds the same archive directly.

### Example

Save the following as `example.c` in the repository root:

```c
#include <stdio.h>

#include "s21_decimal.h"

int main(void) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal sum;
  float result = 0.0f;

  if (s21_from_float_to_decimal(12.5f, &a) != 0 ||
      s21_from_float_to_decimal(0.75f, &b) != 0 ||
      s21_add(a, b, &sum) != 0 ||
      s21_from_decimal_to_float(sum, &result) != 0) {
    return 1;
  }

  printf("%.2f\n", (double)result);
  return 0;
}
```

Build and run the example from the repository root after building the archive:

```sh
cc -std=c11 -Wall -Wextra -Werror -pedantic -I src \
  example.c src/s21_decimal.a -lm -o example.out
./example.out
```

Expected output:

```text
13.25
```

## Tests

From `src`:

```sh
make test
```

Tests link against the built archive and the real Check library. The current suite registers 64 tests; several tests iterate over tables of independent expected values.

Coverage includes word-level arithmetic, carry and borrow, signs and scales, overflow, division by zero, signed zero, integer boundaries, float rounding, invalid metadata where validation is implemented, null output pointers, and in-place unary calls.

The integer-rounding tables contain 112 vectors each for floor, round, and truncate. An additional integration suite exercises cross-module behavior.

### Sanitizers

```sh
make test-sanitize
```

This target defaults to fail-fast UndefinedBehaviorSanitizer and disables Check process forking. To request AddressSanitizer as well:

```sh
make test-sanitize SANITIZERS=address,undefined
```

The selected compiler must provide the requested sanitizer runtimes. The ASan leak-detection option defaults off on macOS and on elsewhere. A successful UBSan run is not a leak-check result.

### Memory checks

Build the normal tests before running these commands from `src`.

Linux:

```sh
CK_FORK=no valgrind --tool=memcheck --leak-check=full \
  --show-leak-kinds=all --errors-for-leak-kinds=all \
  --error-exitcode=99 ./.build/normal/s21_test
```

macOS:

```sh
CK_FORK=no MallocStackLogging=1 \
  leaks --atExit -- ./.build/normal/s21_test
```

Leak-check results apply to the executed scenarios, not to every possible input.

## Coverage

```sh
make gcov_report
```

Generated reports:

- HTML: `src/.build/coverage/index.html`
- JSON: `src/.build/coverage/coverage.json`

The Makefile selects `gcov` for GNU GCC, `xcrun llvm-cov gcov` for Clang on macOS, and `llvm-cov gcov` for Clang on other platforms. Override `GCOV` if needed:

```sh
make gcov_report GCOV='xcrun llvm-cov gcov'
```

Normal, sanitizer, and coverage builds use separate object directories. Coverage does not replace the normal public archive. A high coverage percentage does not establish correctness by itself.

## Formatting

From `src`:

```sh
find . -type f \( -name '*.c' -o -name '*.h' \) \
  -not -path './.build/*' \
  -exec clang-format --dry-run --Werror {} +
```

The configuration in `src/.clang-format` is based on Google Style.

## Core layout

```text
src/
  s21_decimal.h
  s21_big_decimal.h
  Makefile
  common/
  arithmetic/
  comparison/
  conversion/
  other/
  tests/
```

`common` contains multiword integer operations and representation conversion. Public operations are grouped by purpose. Tests share a single runner and common fixture helpers. The legacy `check_compat` files are retained but are not included or linked by the active build.

## Clean

```sh
make clean
```

This removes the normal archive, test binaries, and generated build directories.

## License

The existing `LICENSE` file is preserved. This README does not add or replace its licensing terms.
