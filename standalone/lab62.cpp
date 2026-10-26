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

std::string filterText(std::istream& in, std::ostream& out, const std::string& search) {
    require(!search.empty() && search.size() <= 2048, "Введите непустую подстроку до 2048 байт.");
    std::string selected;
    size_t total = 0;
    int spaces = 0;
    int matches = 0;
    out << "Строки, содержащие пробел:\n";
    while (in.peek() != std::char_traits<char>::eof()) {
        std::string s = line(in, 262144);
        total += s.size() + 1;
        require(total <= 262144, "Текст должен занимать не более 256 КиБ.");
        if (s.find(' ') != std::string::npos) {
            out << s << '\n';
            ++spaces;
        }
        if (s.find(search) != std::string::npos) {
            selected += s + '\n';
            ++matches;
        }
    }
    require(!in.bad(), "Ошибка чтения текста.");
    if (!spaces) out << "Таких строк нет.\n";
    out << "\nСтрок с подстрокой: " << matches << "\nСодержимое нового файла:\n" << selected;
    return selected;
}

void lab62(std::istream& in, std::ostream& out) {
    std::string search = line(in, 2048);
    filterText(in, out, search);
}

int main() {
    try {
        std::string sourcePath = line(std::cin);
        std::string targetPath = line(std::cin);
        std::string search = line(std::cin, 2048);
        std::ifstream source(sourcePath, std::ios::binary);
        require(bool(source), "Не удалось открыть исходный файл.");
        std::ifstream existing(targetPath, std::ios::binary);
        require(!existing, "Выходной файл уже существует. Выберите новое имя.");
        std::string selected = filterText(source, std::cout, search);
        std::ofstream target(targetPath, std::ios::binary);
        require(bool(target), "Не удалось создать выходной файл.");
        target << selected;
        target.close();
        require(bool(target), "Ошибка записи файла.");
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
