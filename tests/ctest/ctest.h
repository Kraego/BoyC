#ifndef CTEST_H
#define CTEST_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ctest_func)(void);

void ctest_register(const char *name, ctest_func func);
int ctest_run_all(void);
int ctest_run_by_name(const char *name);
extern int ctest_current_failed;

#define TEST(suite, name) \
    static void suite##_##name(void); \
    static void register_##suite##_##name(void) __attribute__((constructor)); \
    static void register_##suite##_##name(void) { ctest_register(#suite "." #name, suite##_##name); } \
    static void suite##_##name(void)

#define EXPECT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            fprintf(stderr, "%s:%d: EXPECT_EQ failed: %lld != %lld\n", __FILE__, __LINE__, (long long)(expected), (long long)(actual)); \
            ctest_current_failed = 1; \
        } \
    } while(0)

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "%s:%d: EXPECT_TRUE failed\n", __FILE__, __LINE__); \
            ctest_current_failed = 1; \
        } \
    } while(0)

#define SUCCEED() do { } while(0)
#define GTEST_SKIP() do { return; } while(0)

#ifdef __cplusplus
}
#endif

#endif // CTEST_H
