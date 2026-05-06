#include "quantifier.hpp"
#include "lineLikePomdp.hpp"
#include "periodicPolicy.hpp"
#include <fstream>
#include <sstream>

periodicPolicy getPolicy(const std::vector<std::size_t> &counts) {
    std::vector<std::size_t> res;

    for (std::size_t action = 0; action < counts.size(); ++action) {
        const std::size_t a_count = counts[action];
        for (std::size_t i = 0; i < a_count; ++i) {
            res.push_back(action);
        }
    }

    return periodicPolicy(std::move(res));
}


std::vector<prob> plot_probs(std::vector<prob> &&probs, const std::size_t T) {
    const auto pomdp = lineLikePomdp(std::move(probs));

    std::vector<prob> res(T - 1);

    for (std::size_t i = 1; i < T; ++i) {
        std::vector<std::size_t> action_counts;
        action_counts.push_back(i);
        action_counts.push_back(T - i);
        auto policy = getPolicy(action_counts);

        auto res_plot = quantifyThreshold_m(belief({prob(1), prob(0), prob(0)}), pomdp, T, policy);

        res[i - 1] = res_plot[0];
    }

    return res;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.size() != 2) {
        std::cerr << "Invalid number of arguments, expected: 2, got: " << args.size() << std::endl;
    }

    const auto& file = args[0];
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
        for (auto && plot : res) {
            if (plot > max) {
                max = plot;
                maxI = i;
            }
            std::cout << plot << "\t|\t";
            ++i;
        }

        std::cout << "max probability at "  << maxI << " A instructions, with value: " << max <<std::endl;

        // std::cout << std::endl;
    }

    inputFile.close();

    return 0;
}
