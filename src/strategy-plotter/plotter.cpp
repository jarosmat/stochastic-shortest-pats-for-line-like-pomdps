#include <fstream>
#include <sstream>
#include "plotting.hpp"


int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.size() != 2) {
        std::cerr << "Invalid number of arguments, expected: 2, got: " << args.size() << std::endl;
    }

    const auto &file = args[0];
    auto t = std::stoull(args[1]);

    std::ifstream input_file(file);
    if (!input_file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    std::cout << "Probs" << std::endl;

    std::string line;
    while (std::getline(input_file, line)) {
        std::istringstream iss(line);
        double num1, num2;
        iss >> num1 >> num2;
        auto num_p1 = prob(num1);
        auto num_p2 = prob(num2);

        auto res = plot_probs({num_p1, num_p2}, t);

        std::cout << num1 << ", " << num2 << "\t|\t";

        auto max = prob(0);
        std::size_t max_i = 1;
        std::size_t i = 1;
        for (auto &&plot: res) {
            if (plot > max) {
                max = plot;
                max_i = i;
            }
            std::cout << plot << "\t|\t";
            ++i;
        }

        std::cout << "max probability at " << max_i << " A instructions, with value: " << max << std::endl;

        // std::cout << std::endl;
    }

    input_file.close();

    return 0;
}
