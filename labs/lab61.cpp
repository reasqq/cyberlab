#include "common.hpp"

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
