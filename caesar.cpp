#include <iostream>
#include <string>

// Functie die een Caesar-codering uitvoert
std::string translate(std::string line, std::string argument) {
    std::string result = "";
    int shift = std::stoi(argument); // Converteer argument naar een integer voor de verschuiving

    for (char &c : line) {
        if (std::isalpha(c)) {
            char base = std::islower(c) ? 'a' : 'A';
            c = (c - base + shift) % 26 + base;
        }
        result += c;
    }
    return result;
}

int main(int argc, char *argv[]) {
    std::string line;

    if (argc != 2) {
        std::cerr << "Deze functie heeft exact 1 argument nodig" << std::endl;
        return -1;
    }

    while (std::getline(std::cin, line)) {
        std::cout << translate(line, argv[1]) << std::endl;
    }

    return 0;
}

