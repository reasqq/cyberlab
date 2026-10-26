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

inline int integer(const std::string& s, int low, int high) {
    require(!s.empty() && s.size() <= 16, "Введите целое число.");
    size_t start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    require(start < s.size(), "Введите целое число.");
    for (size_t i = start; i < s.size(); ++i)
        require(s[i] >= '0' && s[i] <= '9', "Разрешены только целые числа без пробелов.");
    return static_cast<int>(number(s, low, high));
}

void lab12(std::istream& in, std::ostream& out) {
    int month = integer(line(in), 1, 12);
    switch (month) {
        case 1: out << "Январь"; break;
        case 2: out << "Февраль"; break;
        case 3: out << "Март"; break;
        case 4: out << "Апрель"; break;
        case 5: out << "Май"; break;
        case 6: out << "Июнь"; break;
        case 7: out << "Июль"; break;
        case 8: out << "Август"; break;
        case 9: out << "Сентябрь"; break;
        case 10: out << "Октябрь"; break;
        case 11: out << "Ноябрь"; break;
        case 12: out << "Декабрь"; break;
    }
    out << '\n';
}

int main() {
    try {
        lab12(std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
