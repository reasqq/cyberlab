#include "common.hpp"

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
