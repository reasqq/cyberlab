#include "common.hpp"

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
