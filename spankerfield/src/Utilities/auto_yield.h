#ifndef AUTO_YIELD_H
#define AUTO_YIELD_H

#include <chrono>
#include <thread>

// Universal auto_yield function declaration and definition
template<typename Rep, typename Period>
void auto_yield(const std::chrono::duration<Rep, Period>& duration)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start_time < duration)
    {
        std::this_thread::yield(); // Yield to allow other threads to run
    }
}

#endif // AUTO_YIELD_H