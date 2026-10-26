#include "../labs/lab11.cpp"
#include "../labs/lab12.cpp"
#include "../labs/lab21.cpp"
#include "../labs/lab22.cpp"
#include "../labs/lab31.cpp"
#include "../labs/lab32.cpp"
#include "../labs/lab41.cpp"
#include "../labs/lab42.cpp"
#include "../labs/lab51.cpp"
#include "../labs/lab52.cpp"
#include "../labs/lab61.cpp"
#include "../labs/lab62.cpp"

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    std::string id = argv[1];
    try {
        if (id == "11") lab11(std::cin, std::cout);
        else if (id == "12") lab12(std::cin, std::cout);
        else if (id == "21") lab21(std::cin, std::cout);
        else if (id == "22") lab22(std::cin, std::cout);
        else if (id == "31") lab31(std::cin, std::cout);
        else if (id == "32") lab32(std::cin, std::cout);
        else if (id == "41") lab41(std::cin, std::cout);
        else if (id == "42") lab42(std::cin, std::cout);
        else if (id == "51") lab51(std::cin, std::cout);
        else if (id == "52") lab52(std::cin, std::cout);
        else if (id == "61") lab61(std::cin, std::cout);
        else if (id == "62") lab62(std::cin, std::cout);
        else return 2;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what();
        return 1;
    }
}
