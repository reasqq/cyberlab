#include "common.hpp"

std::string pointRegion(double x, double y) {
    double radius = x * x + y * y;
    const double eps = 1e-10;
    if (y < -eps || radius > 9 + eps) return "Нет";
    if (std::abs(y) <= eps || std::abs(radius - 9) <= eps) return "На границе";
    return "Да";
}

void lab11(std::istream& in, std::ostream& out) {
    double x = number(line(in), -1000000, 1000000);
    double y = number(line(in), -1000000, 1000000);
    out << pointRegion(x, y) << '\n';
}
