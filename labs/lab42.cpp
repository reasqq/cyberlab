#include "common.hpp"

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
