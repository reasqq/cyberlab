#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <random>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <locale>
#include <iterator>

inline void require(bool valid, const std::string& message) {
    if (!valid) throw std::runtime_error(message);
}

inline std::string line(std::istream& in, size_t limit = 16384) {
    std::string s;
    char c;
    while (in.get(c) && c != '\n') {
        require(s.size() < limit, "Слишком длинный ввод.");
        require(c != '\0', "Нулевой символ запрещён.");
        s += c;
    }
    if (!s.empty() && s.back() == '\r') s.pop_back();
    return s;
}

inline double number(const std::string& text, double low, double high) {
    require(!text.empty() && text.size() <= 64, "Введите число (не более 64 символов).");
    std::string s = text;
    std::replace(s.begin(), s.end(), ',', '.');
    std::istringstream input(s);
    input.imbue(std::locale::classic());
    double value;
    require(bool(input >> value), "Некорректное число.");
    input >> std::ws;
    require(input.eof() && std::isfinite(value), "Нужно конечное число без лишних символов.");
    require(value >= low && value <= high, "Число вне допустимого диапазона.");
    return value;
}

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

int main() {
    try {
        lab22(std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
