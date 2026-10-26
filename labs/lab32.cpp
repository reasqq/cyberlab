#include "common.hpp"

long long sequence(int n, long long first = -10, long long second = 2) {
    if (n == 1) return first;
    if (n == 2) return second;
    return sequence(n - 1, second, std::abs(first) - 6 * second);
}

void lab32(std::istream& in, std::ostream& out) {
    int n = integer(line(in), 1, 24);
    out << "b[" << n << "] = " << sequence(n) << '\n';
}
