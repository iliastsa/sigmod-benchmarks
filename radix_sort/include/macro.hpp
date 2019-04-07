
#pragma once

#if ! defined (__RNJESUS__)
    #define RANDOM_SEED  (0U)
#else
    #include <ctime>
    #define RANDOM_SEED  (static_cast<unsigned>(std::time(nullptr)))
#endif

#define print128_hex(var)                                                       \
do                                                                              \
{                                                                               \
    const uint16_t * p = reinterpret_cast<const uint16_t *>(&var);              \
                                                                                \
    std::printf                                                                 \
    (                                                                           \
        "%s: %.4x%.4x %.4x%.4x %.4x%.4x %.4x%.4x\n", (#var),                    \
        p[0UL], p[1UL], p[2UL], p[3UL], p[4UL], p[5UL], p[6UL], p[7UL]          \
    );                                                                          \
} while (false)                                                                 \

#define print128_dec(var)                                                       \
do                                                                              \
{                                                                               \
    const int32_t * p = reinterpret_cast<const int32_t *>(&var);                \
                                                                                \
    std::printf                                                                 \
    (                                                                           \
        "%s: %d %d %d %d\n", (#var),                                            \
        p[0UL], p[1UL], p[2UL], p[3UL]                                          \
    );                                                                          \
} while (false)                                                                 \

#define __SIZE_WIDE(size_narrow, narrow_t, wide_t)                              \
(                                                                               \
    ((size_narrow * sizeof(narrow_t)) % sizeof(wide_t))       ?                 \
    ((size_narrow * sizeof(narrow_t)) / sizeof(wide_t) + 1UL) :                 \
    ((size_narrow * sizeof(narrow_t)) / sizeof(wide_t))                         \
)                                                                               \

#define KEYS_32_MIN   (-500)
#define KEYS_32_MAX   (+500)
#define KEYS_32_SIZE  (16UL)

#define KEYS_128_SIZE __SIZE_WIDE(KEYS_32_SIZE, key_t, key_wide_t)
#define DELS_128_SIZE __SIZE_WIDE(DELS_32_SIZE, del_t, del_wide_t)
