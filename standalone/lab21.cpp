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

int main() {
    try {
        lab21(std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
