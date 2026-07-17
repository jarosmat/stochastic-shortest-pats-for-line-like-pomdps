#include <fstream>
#include <sstream>

#include "plotting.hpp"

int main(const int argc, char *argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.size() != 3) {
        std::cerr << "Invalid number of arguments, expected: 3, got: " << args.size() << std::endl;
        return -1;
    }

    const auto &file = args[0];
    auto t = std::stoull(args[1]);
    auto dim = std::stoull(args[2]);
    if (dim < 2) {
        std::cerr << "Invalid dimension, must be greater than one" << std::endl;
        return -1;
    }

    std::ifstream input_file(file);
    if (!input_file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    std::vector<prob> probs;

    std::string line;
    while (std::getline(input_file, line)) {
        std::istringstream iss(line);
        double num;
        iss >> num;
        probs.emplace_back(num);
    }

    if (dim == 2) {
        plot_table({}, std::move(probs), t, std::cout);
    } else {
        // TODO implement
    }


    return 0;
}
