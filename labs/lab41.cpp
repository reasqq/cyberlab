#include "common.hpp"

void lab41(std::istream& in, std::ostream& out) {
    auto values = array(line(in));
    for (int& value : values) {
        if (value > 0) value = -value;
    }
    printArray(out, values.data(), values.size());
}
