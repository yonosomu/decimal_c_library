#include "check_compat.h"

struct TCase {
  const char *name;
  int count;
  void (*tests[128])(void);
};

struct Suite {
  const char *name;
  int count;
  TCase *cases[8];
};

struct SRunner {
  int count;
  Suite *suites[16];
  int passed;
};

Suite *suite_create(const char *name) {
  Suite *suite = calloc(1, sizeof(*suite));
  suite->name = name;
  return suite;
}

TCase *tcase_create(const char *name) {
  TCase *tc = calloc(1, sizeof(*tc));
  tc->name = name;
  return tc;
}

void add_shim_test(TCase *tc, const char *name, void (*fn)(void)) {
  (void)name;
  tc->tests[tc->count] = fn;
  ++tc->count;
}

void suite_add_tcase(Suite *suite, TCase *tc) {
  suite->cases[suite->count] = tc;
  ++suite->count;
}

SRunner *srunner_create(Suite *suite) {
  SRunner *runner = calloc(1, sizeof(*runner));
  runner->suites[runner->count] = suite;
  ++runner->count;
  return runner;
}

void srunner_add_suite(SRunner *runner, Suite *suite) {
  runner->suites[runner->count] = suite;
  ++runner->count;
}

void srunner_run_all(SRunner *runner, int mode) {
  (void)mode;
  for (int i = 0; i < runner->count; ++i) {
    Suite *suite = runner->suites[i];
    for (int j = 0; j < suite->count; ++j) {
      TCase *tc = suite->cases[j];
      for (int k = 0; k < tc->count; ++k) {
        tc->tests[k]();
        ++runner->passed;
      }
    }
  }
  printf("Local Check-compatible tests: %d passed, 0 failed\n", runner->passed);
}

int srunner_ntests_failed(SRunner *runner) {
  (void)runner;
  return 0;
}

void srunner_free(SRunner *runner) {
  for (int i = 0; i < runner->count; ++i) {
    Suite *suite = runner->suites[i];
    for (int j = 0; j < suite->count; ++j) {
      free(suite->cases[j]);
    }
    free(suite);
  }
  free(runner);
}
