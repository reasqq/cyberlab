#include "common.hpp"

void lab21(std::istream&, std::ostream& out) {
    out << "while:    ";
    int i = 1;
    while (i <= 21) {
        out << (i == 1 ? "" : " ") << i;
        i += 2;
    }
    out << "\ndo while: ";
    i = 1;
    do {
        out << (i == 1 ? "" : " ") << i;
        i += 2;
    } while (i <= 21);
    out << "\nfor:      ";
    for (i = 1; i <= 21; i += 2) out << (i == 1 ? "" : " ") << i;
    out << '\n';
}
