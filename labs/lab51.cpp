#include "common.hpp"

void insertionSort(int* a, int n) {
    for (int i = 1; i < n; ++i) {
        int value = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > value) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = value;
    }
}

void lab51(std::istream& in, std::ostream& out) {
    int n = integer(line(in), 1, 200);
    auto values = std::make_unique<int[]>(n);
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> random(-99, 99);
    for (int i = 0; i < n; ++i) values[i] = random(generator);
    out << "Исходный массив:\n";
    printArray(out, values.get(), n);
    insertionSort(values.get(), n);
    out << "После сортировки вставками:\n";
    printArray(out, values.get(), n);
}
