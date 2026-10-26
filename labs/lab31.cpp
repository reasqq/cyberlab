#include "common.hpp"

double minimal(double a, double b) {
    return a < b ? a : b;
}

void lab31(std::istream& in, std::ostream& out) {
    double x = number(line(in), -1000000, 1000000);
    double y = number(line(in), -1000000, 1000000);
    double z = minimal(3 * x, 2 * y) + minimal(x - y, x + y);
    out << "z = " << std::setprecision(12) << z << '\n';
}
