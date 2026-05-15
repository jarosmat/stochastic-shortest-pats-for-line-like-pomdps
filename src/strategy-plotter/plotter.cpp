#include <fstream>
#include <sstream>
#include "plotting.hpp"


int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.size() != 2) {
        std::cerr << "Invalid number of arguments, expected: 2, got: " << args.size() << std::endl;
    }

    const auto &file = args[0];
    auto T = std::stoull(args[1]);

    std::ifstream inputFile(file);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    std::cout << "Probs" << std::endl;

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        double num1, num2;
        iss >> num1 >> num2;
        auto num_p1 = prob(num1);
        auto num_p2 = prob(num2);

        auto res = plot_probs({num_p1, num_p2}, T);

        std::cout << num1 << ", " << num2 << "\t|\t";

        prob max = prob(0);
        std::size_t maxI = 1;
        std::size_t i = 1;
        for (auto &&plot: res) {
            if (plot > max) {
                max = plot;
                maxI = i;
            }
            std::cout << plot << "\t|\t";
            ++i;
        }

        std::cout << "max probability at " << maxI << " A instructions, with value: " << max << std::endl;

        // std::cout << std::endl;
    }

    inputFile.close();

    return 0;
}
