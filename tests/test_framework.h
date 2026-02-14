#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>

static int _tests_run = 0;
static int _tests_passed = 0;
static int _tests_failed = 0;

#define ASSERT_TRUE(expr) do { \
    _tests_run++; \
    if (expr) { _tests_passed++; } \
    else { _tests_failed++; \
        printf("  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #expr); } \
} while(0)

#define ASSERT_EQ(a, b) do { \
    _tests_run++; \
    if ((a) == (b)) { _tests_passed++; } \
    else { _tests_failed++; \
        printf("  FAIL: %s:%d: %s == %s (%d != %d)\n", \
               __FILE__, __LINE__, #a, #b, (int)(a), (int)(b)); } \
} while(0)

#define ASSERT_NEQ(a, b) do { \
    _tests_run++; \
    if ((a) != (b)) { _tests_passed++; } \
    else { _tests_failed++; \
        printf("  FAIL: %s:%d: %s != %s (both %d)\n", \
               __FILE__, __LINE__, #a, #b, (int)(a)); } \
} while(0)

#define ASSERT_LT(a, b) do { \
    _tests_run++; \
    if ((a) < (b)) { _tests_passed++; } \
    else { _tests_failed++; \
        printf("  FAIL: %s:%d: %s < %s (%d >= %d)\n", \
               __FILE__, __LINE__, #a, #b, (int)(a), (int)(b)); } \
} while(0)

#define ASSERT_NOT_NULL(ptr) do { \
    _tests_run++; \
    if ((ptr) != NULL) { _tests_passed++; } \
    else { _tests_failed++; \
        printf("  FAIL: %s:%d: %s is NULL\n", __FILE__, __LINE__, #ptr); } \
} while(0)

#define RUN_TEST(fn) do { \
    printf("  [TEST] %s ... ", #fn); \
    fn(); \
    printf("OK\n"); \
} while(0)

#define TEST_SUMMARY() do { \
    printf("\n========================================\n"); \
    printf("Tests: %d | Passed: %d | Failed: %d\n", \
           _tests_run, _tests_passed, _tests_failed); \
    printf("========================================\n"); \
} while(0)

#define TEST_EXIT() (_tests_failed > 0 ? 1 : 0)

#endif
