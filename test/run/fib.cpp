#include "fib.hpp"
#include <utility>


unsigned fib(unsigned n) {
    if (n < 2) {
        return n;
    } else {
        unsigned a = 1, b = 1;
        while (n-- > 2) { a = std::exchange(b, a + b); }
        return b;
    }
}
