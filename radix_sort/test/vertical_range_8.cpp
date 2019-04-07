
#include <immintrin.h>
#include <emmintrin.h>
#include <x86intrin.h>
#include <cstdio>
#include <cstdlib>

#include <macro.hpp>
#include <types.hpp>
#include <utility/benchmark.hpp>

#define DELS_32_SIZE (7UL)

template <unarrow_t N>
void range_8_way
(
    const del_t (&dels_32)[DELS_32_SIZE],
    const key_t (&keys_32)[N],
    unarrow_t (&indices)[N]
);

int main()
{
    std::srand(RANDOM_SEED);

    key_t keys_32[KEYS_32_SIZE] = { 0 };

    for (unarrow_t i = 0UL; i < KEYS_32_SIZE; i++)
        keys_32[i] = std::rand() % (KEYS_32_MAX - KEYS_32_MIN + 1) + KEYS_32_MIN;

    const del_t dels_32[] = { -200, -150, -100, 0, +100, +150, +200 };

    unarrow_t indices[KEYS_32_SIZE] = { 0UL };

    double ms = massiva::benchmark(range_8_way<KEYS_32_SIZE>, dels_32, keys_32, indices);

    for (unarrow_t i = 0UL; i < KEYS_32_SIZE; i++)
    {
        if (indices[i] > DELS_32_SIZE - 1UL)
        {
            if (keys_32[i] < dels_32[DELS_32_SIZE - 1UL])
                std::fprintf(stderr, "{ key-position: %2u, delimeter-index: %2u }\t%+4d >= %+4d\n", i, indices[i], keys_32[i], dels_32[DELS_32_SIZE - 1UL]);
        }
        else if (indices[i] == 0UL)
        {
            if (keys_32[i] > dels_32[0UL])
                std::fprintf(stderr, "{ key-position: %2u, delimeter-index: %2u }\t%+4d <= %+4d\n", i, indices[i], keys_32[i], dels_32[0UL]);
        }
        else
        {
            const del_t lower = dels_32[indices[i] - 1UL];
            const del_t upper = dels_32[indices[i]];

            if (lower > keys_32[i] || keys_32[i] > upper)
                std::fprintf(stderr, "{ key-position: %2u, delimeter-index: %2u }\t%+4d <= %+4d <= %+4d\n", i, indices[i], lower, keys_32[i], upper);
        }
    }

    std::printf("\nRange partitioned %lu keys in %.6lf milliseconds\n", KEYS_32_SIZE, ms);

    return 0;
}

template <unarrow_t N>
void range_8_way
(
    const del_t (&dels_32)[DELS_32_SIZE],
    const key_t (&keys_32)[N],
    unarrow_t (&indices)[N]
)
{
    // (TODO)
    static_assert
    (
        N % (sizeof(key_wide_t) / sizeof(char)) == 0UL,
        "The keys must be aligned on a 16-byte boundary"
    );

    del_wide_t dels_128[DELS_32_SIZE];

    const del_wide_t * dels_128_ptr = reinterpret_cast<const del_wide_t *>(dels_32);

    // We load 4 delimeters at a time into a single register
    dels_128[3UL] = _mm_load_si128(dels_128_ptr++);

    dels_128[6UL] = _mm_load_si128(dels_128_ptr++);

    // Broadcast each one to all the lanes of a different 128-bit register
    dels_128[0UL] = _mm_shuffle_epi32(dels_128[3UL], _MM_SHUFFLE(0, 0, 0, 0));
    dels_128[1UL] = _mm_shuffle_epi32(dels_128[3UL], _MM_SHUFFLE(1, 1, 1, 1));
    dels_128[2UL] = _mm_shuffle_epi32(dels_128[3UL], _MM_SHUFFLE(2, 2, 2, 2));
    
    dels_128[3UL] = _mm_shuffle_epi32(dels_128[3UL], _MM_SHUFFLE(3, 3, 3, 3));

    dels_128[4UL] = _mm_shuffle_epi32(dels_128[6UL], _MM_SHUFFLE(0, 0, 0, 0));
    dels_128[5UL] = _mm_shuffle_epi32(dels_128[6UL], _MM_SHUFFLE(1, 1, 1, 1));
    dels_128[6UL] = _mm_shuffle_epi32(dels_128[6UL], _MM_SHUFFLE(2, 2, 2, 2));

    unarrow_t at = 0UL;

    const key_wide_t * keys_128 = reinterpret_cast<const key_wide_t *>(keys_32);

    del_wide_t merged[3UL]; wide_t result[3UL], index; key_wide_t key_128;

    for (const key_wide_t * beg = keys_128, * end = &keys_128[KEYS_128_SIZE]; beg < end; ++beg)
    {
        // We load 4 keys at a time
        key_128 = _mm_load_si128(beg);

        // Compare each key with the root
        result[0UL] = _mm_cmpgt_epi32(key_128, dels_128[3UL]);

        // If the key in lane x of the register is greater than the root
        // then we store the right child of the root in the x lane of the 
        // merged delimeter, otherwise we store the left child. We repeat
        // this procedure, which resembles traversing a 3-lvl tree, until
        // we reach the "leaves"
        merged[0UL] = _mm_blendv_epi8(dels_128[0UL], dels_128[4UL], result[0UL]);
        merged[1UL] = _mm_blendv_epi8(dels_128[1UL], dels_128[5UL], result[0UL]);
        merged[2UL] = _mm_blendv_epi8(dels_128[2UL], dels_128[6UL], result[0UL]);

        result[1UL] = _mm_cmpgt_epi32(key_128, merged[1UL]);

        merged[1UL] = _mm_blendv_epi8(merged[0UL], merged[2UL], result[1UL]);

        result[2UL] = _mm_cmpgt_epi32(key_128, merged[1UL]);

        // Finally we bit interleave the comparison results, which
        // indicate what actions we took to reach the "leaves", and
        // extract the delimeter indices
        index = _mm_sub_epi32(_mm_setzero_si128(),         result[0UL]);
        index = _mm_sub_epi32(_mm_add_epi32(index, index), result[1UL]);
        index = _mm_sub_epi32(_mm_add_epi32(index, index), result[2UL]);

        const unarrow_t * index_ptr = reinterpret_cast<const unarrow_t *>(&index);

        indices[at++] = index_ptr[0UL];
        indices[at++] = index_ptr[1UL];
        indices[at++] = index_ptr[2UL];
        indices[at++] = index_ptr[3UL];
    }
}
