#ifndef LOCAL_CHECK_SHIM_H_
#define LOCAL_CHECK_SHIM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Suite Suite;
typedef struct TCase TCase;
typedef struct SRunner SRunner;

#define CK_NORMAL 0
#define START_TEST(name) static void name(void)
#define END_TEST

Suite *suite_create(const char *name);
TCase *tcase_create(const char *name);
void add_shim_test(TCase *tc, const char *name, void (*fn)(void));
#define tcase_add_test(tc, fn) add_shim_test((tc), #fn, (fn))
void suite_add_tcase(Suite *suite, TCase *tc);
SRunner *srunner_create(Suite *suite);
void srunner_add_suite(SRunner *runner, Suite *suite);
void srunner_run_all(SRunner *runner, int mode);
int srunner_ntests_failed(SRunner *runner);
void srunner_free(SRunner *runner);

#define ck_assert_int_eq(a, b)                                             \
  do {                                                                     \
    long long lhs = (long long)(a);                                        \
    long long rhs = (long long)(b);                                        \
    if (lhs != rhs) {                                                      \
      fprintf(stderr, "%s:%d: %s=%lld, %s=%lld\n", __FILE__, __LINE__, #a, \
              lhs, #b, rhs);                                               \
      exit(1);                                                             \
    }                                                                      \
  } while (0)

#define ck_assert_uint_eq(a, b)                                            \
  do {                                                                     \
    unsigned long long lhs = (unsigned long long)(a);                      \
    unsigned long long rhs = (unsigned long long)(b);                      \
    if (lhs != rhs) {                                                      \
      fprintf(stderr, "%s:%d: %s=%llu, %s=%llu\n", __FILE__, __LINE__, #a, \
              lhs, #b, rhs);                                               \
      exit(1);                                                             \
    }                                                                      \
  } while (0)

#define ck_assert_ptr_eq(a, b)                                              \
  do {                                                                      \
    const void *lhs = (a);                                                  \
    const void *rhs = (b);                                                  \
    if (lhs != rhs) {                                                       \
      fprintf(stderr, "%s:%d: %s=%p, %s=%p\n", __FILE__, __LINE__, #a, lhs, \
              #b, rhs);                                                     \
      exit(1);                                                              \
    }                                                                       \
  } while (0)

#endif
