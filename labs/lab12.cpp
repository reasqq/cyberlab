#include "common.hpp"

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
