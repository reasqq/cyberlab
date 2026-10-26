#define NOMINMAX
#include "../src/file_io.hpp"

int main() {
    std::filesystem::path directory = std::filesystem::path("build") / ("files-" + std::to_string(GetCurrentProcessId()));
    require(std::filesystem::create_directory(directory), "Test directory already exists.");
    int checks = 0;
    auto check = [&](bool value) { require(value, "File test failed."); ++checks; };
    auto rejected = [&](auto operation) {
        try { operation(); } catch (const std::runtime_error&) { ++checks; return; }
        throw std::runtime_error("Expected file operation to fail.");
    };
    try {
        auto original = directory / L"исходный.txt";
        writeNewFile(original, "Привет мир\nкод\n");
        check(readTextFile(original) == "Привет мир\nкод\n");
        rejected([&] { writeNewFile(original, "changed"); });
        check(readTextFile(original) == "Привет мир\nкод\n");
        writeNewFile(directory / "empty.txt", "");
        check(readTextFile(directory / "empty.txt").empty());
        writeNewFile(directory / "bom.txt", "\xEF\xBB\xBFtext");
        check(readTextFile(directory / "bom.txt") == "text");
        writeNewFile(directory / "zero-byte.txt", std::string("a\0b", 3));
        rejected([&] { readTextFile(directory / "zero-byte.txt"); });
        writeNewFile(directory / "encoding.txt", std::string("\xff\xfe", 2));
        rejected([&] { readTextFile(directory / "encoding.txt"); });
        writeNewFile(directory / "limit.txt", std::string(262144, 'x'));
        check(readTextFile(directory / "limit.txt").size() == 262144);
        writeNewFile(directory / "large.txt", std::string(262145, 'x'));
        rejected([&] { readTextFile(directory / "large.txt"); });
        rejected([&] { readTextFile(directory / "missing.txt"); });
        rejected([&] { writeNewFile(directory / "missing" / "file.txt", "text"); });
        rejected([&] { writeNewFile(directory / "NUL.txt", "text"); });
        rejected([&] { writeNewFile(directory / "COM1.txt", "text"); });
        rejected([&] { readTextFile(directory); });
        std::cout << "PASS: " << checks << " native file I/O checks\n";
        for (const auto& file : std::filesystem::directory_iterator(directory)) std::filesystem::remove(file.path());
        std::filesystem::remove(directory);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "After " << checks << " checks: " << error.what();
        return 1;
    }
}
