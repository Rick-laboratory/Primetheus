#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <thread>
#include <mutex>

constexpr int NUM_THREADS = 32; // make this match with your threadcount of your cpu
constexpr uint64_t SEGMENT_SIZE = 2500000; // make this match with your max L1,2,3 cachesize of your cpu

std::mutex count_mutex;
uint64_t global_prime_count = 0;

inline bool passes_wheel30(uint64_t n) {
    if (n < 2) return false;
    if (n <= 7) return n == 2 || n == 3 || n == 5 || n == 7;
    return n % 2 && n % 3 && n % 5;
}

inline uint64_t isqrt(uint64_t x) {
    uint64_t res = 0;
    uint64_t bit = 1ULL << 62;

    while (bit > x) bit >>= 2;

    while (bit != 0) {
        if (x >= res + bit) {
            x -= res + bit;
            res = (res >> 1) + bit;
        }
        else {
            res >>= 1;
        }
        bit >>= 2;
    }

    return res;
}

std::vector<uint64_t> simple_sieve(uint64_t max) {
    std::vector<bool> is_prime(max + 1, true);
    is_prime[0] = is_prime[1] = false;
    for (uint64_t i = 2; i * i <= max; ++i) {
        if (is_prime[i]) {
            for (uint64_t j = i * i; j <= max; j += i) {
                is_prime[j] = false;
            }
        }
    }
    std::vector<uint64_t> primes;
    for (uint64_t i = 2; i <= max; ++i) {
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }
    return primes;
}

void sieve_range(uint64_t start, uint64_t end, const std::vector<uint64_t>& base_primes) {
    std::vector<uint8_t> bitset((SEGMENT_SIZE + 7) / 8, 0xFF);

    uint64_t local_count = 0;
    uint64_t low = start;
    uint64_t high = std::min(low + SEGMENT_SIZE - 1, end);

    auto clear_bit = [](std::vector<uint8_t>& b, uint64_t i) {
        b[i >> 3] &= ~(1 << (i & 7));
        };

    auto get_bit = [](const std::vector<uint8_t>& b, uint64_t i) -> bool {
        return b[i >> 3] & (1 << (i & 7));
        };

    while (low <= end) {
        std::fill(bitset.begin(), bitset.end(), 0xFF);

        for (uint64_t prime : base_primes) {
            uint64_t first_multiple = std::max(prime * prime, ((low + prime - 1) / prime) * prime);
            for (uint64_t j = first_multiple; j <= high; j += prime) {
                clear_bit(bitset, j - low);
            }
        }

        for (uint64_t i = 0; i < SEGMENT_SIZE && low + i <= end; ++i) {
            uint64_t n = low + i;
            if (get_bit(bitset, i) && passes_wheel30(n)) {
                local_count++;
            }
        }

        low += SEGMENT_SIZE;
        high = std::min(high + SEGMENT_SIZE, end);
    }

    std::lock_guard<std::mutex> guard(count_mutex);
    global_prime_count += local_count;
}

int main() {
    const uint64_t MAX = 1000000000000ULL; // Für Tests auf 10^12 beschränkt
    const uint64_t TRUE_COUNT = 37607912018; // True count für 10^12

    std::vector<uint64_t> segment_sizes = { SEGMENT_SIZE };

    for (uint64_t test_segment_size : segment_sizes) {
        global_prime_count = 0;

        uint64_t limit = isqrt(MAX);
        std::vector<uint64_t> base_primes = simple_sieve(limit);

        clock_t start = clock();

        std::vector<std::thread> threads;
        uint64_t range_per_thread = MAX / NUM_THREADS;

        for (int i = 0; i < NUM_THREADS; ++i) {
            uint64_t range_start = i * range_per_thread + (i == 0 ? 2 : 0);
            uint64_t range_end = (i == NUM_THREADS - 1) ? MAX : (i + 1) * range_per_thread - 1;
            threads.emplace_back(sieve_range, range_start, range_end, std::cref(base_primes));
        }

        for (auto& t : threads) {
            t.join();
        }

        clock_t end = clock();

        std::cout << "\n=== Segment Size: " << SEGMENT_SIZE << " (bitpacked + isqrt) ===\n";
        std::cout << "\u23F1 Time: " << std::fixed << std::setprecision(2)
            << double(end - start) / CLOCKS_PER_SEC << "s\n";
        std::cout << "\u2705 Primes Found: " << global_prime_count << "\n";
        std::cout << "\U0001F4CA Accuracy: " << (global_prime_count * 100.0 / TRUE_COUNT) << "%\n";
        std::cout << "   (True count for 10^12: " << TRUE_COUNT << ")\n";
        std::cout << "   (Difference: " << (global_prime_count > TRUE_COUNT
            ? global_prime_count - TRUE_COUNT
            : TRUE_COUNT - global_prime_count)
            << " primes)\n";
    }
    return 0;
}
