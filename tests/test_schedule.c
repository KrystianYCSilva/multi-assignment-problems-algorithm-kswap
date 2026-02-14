#include "test_framework.h"
#include "../src/core/schedule.h"
#include "../src/core/cost.h"

static void test_schedule_create(void) {
    Schedule *s = schedule_create(10, 7);
    ASSERT_NOT_NULL(s);
    ASSERT_EQ(s->size.n_nurses, 10);
    ASSERT_EQ(s->size.n_days, 7);
    for (int i = 0; i < 10; i++)
        for (int d = 0; d < 7; d++)
            ASSERT_EQ(schedule_get(s, i, d), SHIFT_FREE);
    schedule_free(s);
}

static void test_schedule_copy(void) {
    Schedule *s = schedule_create(5, 3);
    schedule_set(s, 0, 0, SHIFT_MORNING);
    schedule_set(s, 1, 1, SHIFT_NIGHT);
    Schedule *copy = schedule_copy(s);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(schedule_get(copy, 0, 0), SHIFT_MORNING);
    ASSERT_EQ(schedule_get(copy, 1, 1), SHIFT_NIGHT);
    ASSERT_EQ(schedule_get(copy, 2, 2), SHIFT_FREE);
    schedule_set(s, 0, 0, SHIFT_EVENING);
    ASSERT_EQ(schedule_get(copy, 0, 0), SHIFT_MORNING);
    schedule_free(s);
    schedule_free(copy);
}

static void test_schedule_swap_block(void) {
    Schedule *s = schedule_create(3, 5);
    schedule_set(s, 0, 1, SHIFT_MORNING);
    schedule_set(s, 0, 2, SHIFT_MORNING);
    schedule_set(s, 1, 1, SHIFT_NIGHT);
    schedule_set(s, 1, 2, SHIFT_NIGHT);
    schedule_swap_block(s, 0, 1, 1, 2);
    ASSERT_EQ(schedule_get(s, 0, 1), SHIFT_NIGHT);
    ASSERT_EQ(schedule_get(s, 0, 2), SHIFT_NIGHT);
    ASSERT_EQ(schedule_get(s, 1, 1), SHIFT_MORNING);
    ASSERT_EQ(schedule_get(s, 1, 2), SHIFT_MORNING);
    schedule_free(s);
}

static void test_schedule_reverse(void) {
    Schedule *s = schedule_create(2, 4);
    schedule_set(s, 0, 0, SHIFT_MORNING);
    schedule_set(s, 0, 1, SHIFT_EVENING);
    schedule_set(s, 0, 2, SHIFT_NIGHT);
    schedule_set(s, 0, 3, SHIFT_FREE);
    schedule_reverse_days(s);
    ASSERT_EQ(schedule_get(s, 0, 0), SHIFT_FREE);
    ASSERT_EQ(schedule_get(s, 0, 1), SHIFT_NIGHT);
    ASSERT_EQ(schedule_get(s, 0, 2), SHIFT_EVENING);
    ASSERT_EQ(schedule_get(s, 0, 3), SHIFT_MORNING);
    schedule_reverse_days(s);
    ASSERT_EQ(schedule_get(s, 0, 0), SHIFT_MORNING);
    schedule_free(s);
}

static void test_count_shift_on_day(void) {
    Schedule *s = schedule_create(5, 3);
    schedule_set(s, 0, 0, SHIFT_MORNING);
    schedule_set(s, 1, 0, SHIFT_MORNING);
    schedule_set(s, 2, 0, SHIFT_EVENING);
    ASSERT_EQ(schedule_count_shift_on_day(s, 0, SHIFT_MORNING), 2);
    ASSERT_EQ(schedule_count_shift_on_day(s, 0, SHIFT_EVENING), 1);
    ASSERT_EQ(schedule_count_shift_on_day(s, 0, SHIFT_FREE), 2);
    schedule_free(s);
}

static void test_forbidden_sequence(void) {
    ASSERT_TRUE(is_forbidden_sequence(SHIFT_EVENING, SHIFT_MORNING));
    ASSERT_TRUE(is_forbidden_sequence(SHIFT_NIGHT, SHIFT_MORNING));
    ASSERT_TRUE(is_forbidden_sequence(SHIFT_NIGHT, SHIFT_EVENING));
    ASSERT_TRUE(!is_forbidden_sequence(SHIFT_MORNING, SHIFT_EVENING));
    ASSERT_TRUE(!is_forbidden_sequence(SHIFT_MORNING, SHIFT_MORNING));
    ASSERT_TRUE(!is_forbidden_sequence(SHIFT_FREE, SHIFT_MORNING));
}

static void test_consecutive_work(void) {
    Schedule *s = schedule_create(1, 7);
    schedule_set(s, 0, 0, SHIFT_MORNING);
    schedule_set(s, 0, 1, SHIFT_EVENING);
    schedule_set(s, 0, 2, SHIFT_NIGHT);
    schedule_set(s, 0, 4, SHIFT_MORNING);
    schedule_set(s, 0, 5, SHIFT_MORNING);
    ASSERT_EQ(count_consecutive_work(s, 0), 3);
    schedule_free(s);
}

static void test_consecutive_same_shift(void) {
    Schedule *s = schedule_create(1, 7);
    schedule_set(s, 0, 0, SHIFT_MORNING);
    schedule_set(s, 0, 1, SHIFT_MORNING);
    schedule_set(s, 0, 2, SHIFT_MORNING);
    schedule_set(s, 0, 4, SHIFT_EVENING);
    schedule_set(s, 0, 5, SHIFT_EVENING);
    ASSERT_EQ(count_consecutive_same_shift(s, 0, SHIFT_MORNING), 3);
    ASSERT_EQ(count_consecutive_same_shift(s, 0, SHIFT_EVENING), 2);
    schedule_free(s);
}

int main(void) {
    printf("=== Schedule Tests ===\n");
    RUN_TEST(test_schedule_create);
    RUN_TEST(test_schedule_copy);
    RUN_TEST(test_schedule_swap_block);
    RUN_TEST(test_schedule_reverse);
    RUN_TEST(test_count_shift_on_day);
    RUN_TEST(test_forbidden_sequence);
    RUN_TEST(test_consecutive_work);
    RUN_TEST(test_consecutive_same_shift);
    TEST_SUMMARY();
    return TEST_EXIT();
}
