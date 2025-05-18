#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <ctime>
#include <iomanip>
#include <immintrin.h>
#include <cstdint>

constexpr int NUM_THREADS = 32;
constexpr uint64_t SEGMENT_SIZE = 2'500'000;
constexpr uint64_t MAX = 1'000'000'000'000ULL;
constexpr uint64_t TRUE_COUNT = 37'607'912'018ULL;

std::atomic<uint64_t> global_prime_count{0};

inline uint64_t isqrt(uint64_t x) {
    uint64_t res = 0, bit = 1ULL << 62;
    while (bit > x) bit >>= 2;
    while (bit) {
        if (x >= res + bit) {
            x -= res + bit;
            res = (res >> 1) + bit;
        } else res >>= 1;
        bit >>= 2;
    }
    return res;
}

std::vector<uint64_t> simple_sieve(uint64_t max) {
    std::vector<bool> is_prime(max + 1, true);
    is_prime[0] = is_prime[1] = false;
    for (uint64_t i = 2; i * i <= max; ++i)
        if (is_prime[i])
            for (uint64_t j = i * i; j <= max; j += i)
                is_prime[j] = false;

    std::vector<uint64_t> primes;
    for (uint64_t i = 2; i <= max; ++i)
        if (is_prime[i]) primes.push_back(i);

    return primes;
}

void sieve_segment(uint64_t low, uint64_t high, const std::vector<uint64_t>& base_primes) {
    uint64_t count = 0;
    size_t segment_bytes = (SEGMENT_SIZE + 7) / 8;
    std::vector<uint8_t> bitset(segment_bytes, 0xFF);

    for (uint64_t prime : base_primes) {
        if (prime * prime > high) break;
        uint64_t start = std::max(prime * prime, ((low + prime - 1) / prime) * prime);
        for (uint64_t j = start; j <= high; j += prime) {
            uint64_t idx = j - low;
            bitset[idx >> 3] &= ~(1 << (idx & 7));
        }
    }

    // AVX2 processa 256 bits por rodada (32 bytes)
    size_t i = 0;
    size_t limit = (SEGMENT_SIZE >> 5) << 5;

    for (; i < limit; i += 32) {
        __m256i vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&bitset[i >> 3]));
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&vec);

        for (int j = 0; j < 32; ++j) {
            uint8_t byte = ptr[j >> 3];
            if ((byte >> (j & 7)) & 1) {
                ++count;
            }
        }
    }

    for (; i < SEGMENT_SIZE && low + i <= high; ++i) {
        if ((bitset[i >> 3] >> (i & 7)) & 1) {
            ++count;
        }
    }

    global_prime_count += count;
}

int main() {
    std::cout << "🚀 UltraFast Crivo | Threads: " << NUM_THREADS << ", Segment size: " << SEGMENT_SIZE << "\n";

    uint64_t sqrt_limit = isqrt(MAX);
    std::vector<uint64_t> base_primes = simple_sieve(sqrt_limit);

    clock_t start_time = clock();

    std::vector<std::thread> threads;
    uint64_t range_per_thread = MAX / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; ++i) {
        uint64_t range_start = i * range_per_thread + (i == 0 ? 2 : 0);
        uint64_t range_end = (i == NUM_THREADS - 1) ? MAX : (i + 1) * range_per_thread - 1;

        threads.emplace_back([range_start, range_end, &base_primes]() {
            uint64_t low = range_start;
            uint64_t high = std::min(low + SEGMENT_SIZE - 1, range_end);

            while (low <= range_end) {
                sieve_segment(low, high, base_primes);
                low += SEGMENT_SIZE;
                high = std::min(high + SEGMENT_SIZE, range_end);
            }
        });
    }

    for (auto& t : threads) t.join();

    clock_t end_time = clock();
    double elapsed = double(end_time - start_time) / CLOCKS_PER_SEC;

    std::cout << "\n⏱ Tempo: " << std::fixed << std::setprecision(2) << elapsed << "s\n";
    std::cout << "🔢 Primos encontrados: " << global_prime_count << "\n";
    std::cout << "📈 Precisão: " << (global_prime_count * 100.0 / TRUE_COUNT) << "%\n";
    std::cout << "Diferença: " << std::llabs((int64_t)global_prime_count - (int64_t)TRUE_COUNT) << "\n";

    return 0;
}
