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

void lab42(std::istream& in, std::ostream& out) {
    int n = integer(line(in), 1, 20);
    long long sum = 0;
    int count = 0;
    for (int i = 0; i < n; ++i) {
        auto row = array(line(in), 20);
        require(row.size() == static_cast<size_t>(n), "В каждой строке должно быть ровно n чисел.");
        for (int j = i + 1; j < n; ++j) {
            if (row[j] % 2 != 0) {
                sum += row[j];
                ++count;
            }
        }
    }
    require(line(in).empty() && in.eof(), "В матрице должно быть ровно n строк.");
    if (count == 0) out << "Нечётных элементов выше главной диагонали нет. Среднее не определено.\n";
    else out << "Количество: " << count << "\nСумма: " << sum
             << "\nСреднее: " << std::setprecision(12) << double(sum) / count << '\n';
}

int main() {
    try {
        lab42(std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
