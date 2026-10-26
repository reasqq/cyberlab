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

inline std::vector<int> array(const std::string& s, size_t limit = 200) {
    std::istringstream input(s);
    std::vector<int> values;
    std::string token;
    while (input >> token) {
        require(values.size() < limit, "Слишком много элементов.");
        values.push_back(integer(token, -1000000, 1000000));
    }
    require(!values.empty(), "Массив не должен быть пустым.");
    return values;
}

inline void printArray(std::ostream& out, const int* a, size_t n) {
    for (size_t i = 0; i < n; ++i) out << (i ? " " : "") << a[i];
    out << '\n';
}

void lab52(std::istream& in, std::ostream& out) {
    auto source = array(line(in), 30);
    int n = static_cast<int>(source.size());
    auto values = std::make_unique<int[]>(n);
    for (int i = 0; i < n; ++i) values[i] = source[i];
    out << "Исходный массив:\n";
    printArray(out, values.get(), n);
    for (int i = 1; i < n; ++i) {
        int value = values[i];
        int j = i - 1;
        while (j >= 0 && values[j] > value) {
            values[j + 1] = values[j];
            --j;
        }
        values[j + 1] = value;
        out << "Шаг " << i << ": ";
        printArray(out, values.get(), n);
    }
    out << "Готово:\n";
    printArray(out, values.get(), n);
}

int main() {
    try {
        lab52(std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
