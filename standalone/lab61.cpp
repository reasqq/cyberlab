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

inline size_t bytePosition(const std::string& s, size_t position) {
    size_t characters = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if ((static_cast<unsigned char>(s[i]) & 0xC0) != 0x80) {
            if (characters == position) return i;
            ++characters;
        }
    }
    require(characters == position, "Позиция выходит за конец строки.");
    return s.size();
}

void insertChars(char* target, size_t capacity, const char* source, size_t position) {
    size_t n = std::strlen(target);
    size_t m = std::strlen(source);
    require(position <= n && n + m < capacity, "Недостаточно места или неверная позиция.");
    for (size_t i = n + 1; i > position; --i) target[i + m - 1] = target[i - 1];
    for (size_t i = 0; i < m; ++i) target[position + i] = source[i];
}

void lab61(std::istream& in, std::ostream& out) {
    std::string first = line(in, 2048);
    std::string second = line(in, 2048);
    int mode = integer(line(in), 0, 2);
    int position = integer(line(in), 0, 2048);
    size_t offset = mode == 0 ? 0 : mode == 1 ? first.size() : bytePosition(first, position);
    char target[4097] = {};
    char source[2049] = {};
    std::memcpy(target, first.data(), first.size());
    std::memcpy(source, second.data(), second.size());
    insertChars(target, sizeof(target), source, offset);
    out << target << '\n';
}

int main() {
    try {
        lab61(std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
