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

int main() {
    try {
        lab11(std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
