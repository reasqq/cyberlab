#include "common.hpp"

double piecewise(double x) {
    if (x >= 0.9) return 1 / ((0.1 + x) * (0.1 + x));
    if (x >= 0) return 0.2 * x + 0.1;
    return x * x + 0.2;
}

void lab22(std::istream& in, std::ostream& out) {
    double a = number(line(in), -1000000, 1000000);
    double b = number(line(in), -1000000, 1000000);
    double h = number(line(in), 0.000001, 1000000);
    require(a <= b, "Начало интервала должно быть не больше конца.");
    double steps = (b - a) / h;
    double tolerance = 1e-9;
    require(steps < 1000, "Таблица ограничена 1000 строками. Увеличьте шаг.");
    int count = static_cast<int>(std::floor(steps + tolerance)) + 1;
    require(count <= 1000, "Таблица ограничена 1000 строками.");
    out << "x\ty\n" << std::setprecision(12);
    for (int i = 0; i < count; ++i) {
        double x = a + i * h;
        if (x > b) x = b;
        if (std::abs(x) < 1e-12) x = 0;
        if (std::abs(x - 0.9) < 1e-12) x = 0.9;
        out << x << '\t' << piecewise(x) << '\n';
    }
}
