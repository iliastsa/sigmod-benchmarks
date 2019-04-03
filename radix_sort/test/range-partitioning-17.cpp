
#include <immintrin.h>
#include <emmintrin.h>
#include <x86intrin.h>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <benchmark/benchmark.hpp>

#if ! defined (__RNJESUS__)
    #define RANDOM_SEED  (0U)
#else
    #include <ctime>
    #define RANDOM_SEED  (static_cast<unsigned>(std::time(nullptr)))
#endif

using key_t      = int32_t;
using key_wide_t = __m128i; 

#define KEYS_32_SIZE  (16UL)
#define KEYS_32_MAX   (+500)
#define KEYS_32_MIN   (-500)

#define KEYS_128_SIZE ((KEYS_32_SIZE * sizeof(key_t)) / sizeof(key_wide_t))

using del_t      = int32_t;
using del_wide_t = __m128i;

#define DELS_32_SIZE  (16UL)

#define DELS_128_SIZE ((DELS_32_SIZE * sizeof(del_t)) / sizeof(del_wide_t))

template <std::size_t N>
void range_17_way
(
    const del_t (&dels_32)[DELS_32_SIZE],
    const key_t (&keys_32)[N],
    std::size_t (&indices)[N]
);

int main()
{
    std::srand(RANDOM_SEED);

    key_t keys_32[KEYS_32_SIZE];

    for (std::size_t i = 0UL; i < KEYS_32_SIZE; i++)
        keys_32[i] = std::rand() % (KEYS_32_MAX - KEYS_32_MIN + 1) + KEYS_32_MIN;

    const del_t dels_32[DELS_32_SIZE] = {
        -400, -350, -300, -250, -200, -150, -100, -50,
        +50, +100, +150, +200, +250, +300, +350, +400
    };

    std::size_t indices[KEYS_32_SIZE];

    double ms = utility::benchmark(range_17_way<KEYS_32_SIZE>, dels_32, keys_32, indices);

    for (std::size_t i = 0UL; i < KEYS_32_SIZE; i++)
    {
        if (indices[i] > DELS_32_SIZE - 1UL)
        {
            if (keys_32[i] < dels_32[DELS_32_SIZE - 1UL])
                std::fprintf(stderr, "{ key-position: %2lu, delimeter-index: %2lu }\t%+4d >= %+4d\n", i, indices[i], keys_32[i], dels_32[DELS_32_SIZE - 1UL]);
        }
        else if (indices[i] == 0UL)
        {
            if (keys_32[i] > dels_32[0UL])
                std::fprintf(stderr, "{ key-position: %2lu, delimeter-index: %2lu }\t%+4d <= %+4d\n", i, indices[i], keys_32[i], dels_32[0UL]);
        }
        else
        {
            const key_t lower = dels_32[indices[i] - 1UL];
            const key_t upper = dels_32[indices[i]];

            if (lower > keys_32[i] || keys_32[i] > upper)
                std::fprintf(stderr, "{ key-position: %2lu, delimeter-index: %2lu }\t%+4d <= %+4d <= %+4d\n", i, indices[i], lower, keys_32[i], upper);
        }
    }

    std::fprintf(stdout, "\nRange partitioned %lu keys in %.6lf milliseconds\n", KEYS_32_SIZE, ms);

    return 0;
}

template <std::size_t N>
void range_17_way
(
    const del_t (&dels_32)[DELS_32_SIZE],
    const key_t (&keys_32)[N],
    std::size_t (&indices)[N]
)
{
    // Currently, the keys must be aligned on a 16-byte boundary, otherwise
    // some of them are being skipped
    constexpr std::size_t skipped = N % (sizeof(key_wide_t) / sizeof(key_t));

    if constexpr (skipped)
        std::fprintf(stderr, "Warning: Skipping %lu/%lu keys]\n\n", skipped, N);

    // The 16 delimeters are loaded into an array, whose entries
    // are able to store 4 x 32-bit delimeters each
    const del_wide_t * dels_128_ptr = reinterpret_cast<const del_wide_t *>(dels_32);

    del_wide_t dels_128[DELS_128_SIZE];

    dels_128[0UL] = _mm_load_si128(dels_128_ptr++);
    dels_128[1UL] = _mm_load_si128(dels_128_ptr++);
    dels_128[2UL] = _mm_load_si128(dels_128_ptr++);
    dels_128[3UL] = _mm_load_si128(dels_128_ptr++);

    std::size_t at = 0UL;

    const key_wide_t * keys_128 = reinterpret_cast<const key_wide_t *>(keys_32);

    key_wide_t key_128[4UL], result[4UL];

    for (const key_wide_t * beg = keys_128, * end = &keys_128[KEYS_128_SIZE]; beg < end; ++beg)
    {
        // We load 4 keys at a time
        key_128[3UL] = _mm_load_si128(beg);

        // Each key is being broadcasted to all the lanes of a 128-bit register
        key_128[0UL] = _mm_shuffle_epi32(key_128[3UL], _MM_SHUFFLE(0, 0, 0, 0));
        key_128[1UL] = _mm_shuffle_epi32(key_128[3UL], _MM_SHUFFLE(1, 1, 1, 1));
        key_128[2UL] = _mm_shuffle_epi32(key_128[3UL], _MM_SHUFFLE(2, 2, 2, 2));
        key_128[3UL] = _mm_shuffle_epi32(key_128[3UL], _MM_SHUFFLE(3, 3, 3, 3));

        for (std::size_t i = 0UL; i < sizeof(key_128) / sizeof(key_128[0UL]); i++)
        {
            // Compare the current key with every delimeter, 4 delimeters at a time
            // _mm_cmpgt_epi32: true -> 0xFFFFFFFF, false -> 0x00000000 
            result[0UL] = _mm_cmpgt_epi32(dels_128[0UL], key_128[i]);
            result[1UL] = _mm_cmpgt_epi32(dels_128[1UL], key_128[i]);
            result[2UL] = _mm_cmpgt_epi32(dels_128[2UL], key_128[i]);
            result[3UL] = _mm_cmpgt_epi32(dels_128[3UL], key_128[i]);

            // We pack the 32-bit integers stored in result[0/2] and result[1/3]
            // in 16-bit integers using signed saturation
            result[0UL] = _mm_packs_epi32(result[0UL], result[1UL]);
            result[1UL] = _mm_packs_epi32(result[2UL], result[3UL]);

            // We pack the 16-bit integers stored in result[0] and result[1]
            // in 8-bit integers using signed saturation
            result[0UL] = _mm_packs_epi16(result[0UL], result[1UL]);
            
            // _mm_movemask_epi8 creates a mask from the most-significant-bit
            // of every 8-bit element in our 8-bit packed comparison results
            // We use _bit_scan_forward to find the least-significant-set-bit
            // which indicates the index of the delimeter that is
            // greater than the current key. 
            // We logically or our packed comparison results with 0x10000,
            // so that, if no such delimeter exists, 16 will be returned
            indices[at++] = _bit_scan_forward(_mm_movemask_epi8(result[0UL]) | 0x10000);
        }
    }
}
