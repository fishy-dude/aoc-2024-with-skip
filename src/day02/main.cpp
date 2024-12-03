#include <iostream>
#include <functional>
#include <sstream>
#include <string>
#include <fstream>
#include <print>
#include <vector>
#include <ranges>

namespace my_algo {
    template <typename T>
    struct convert_to_fn {
        inline constexpr auto operator()(auto&& value) -> T {
            return static_cast<T>(value);
        } 
    };

    template <typename T>
    convert_to_fn<T> convert_to{};

    inline auto three_way_compare = [](auto&& l, auto&& r) { return l <=> r; };

    struct istream_line {
        friend auto operator>>(std::istream& is, istream_line& il) -> std::istream& {
            std::string line;
            std::getline(is, line);
            il.line = std::istringstream(line);
            return is;
        }

        inline operator std::istream&() {
            return line;
        }

        inline operator std::string() const {
            return line.str();
        }

        std::istringstream line{};
    };

    inline constexpr auto bind_first(auto&& function, auto&& value) {
        return std::bind(std::forward<decltype(function)>(function), std::forward<decltype(value)>(value), std::placeholders::_1);
    }

    inline constexpr auto bind_second(auto&& function, auto&& value) {
        return std::bind(std::forward<decltype(function)>(function), std::placeholders::_1, std::forward<decltype(value)>(value));
    }
} // namespace my_algo

using report = std::vector<int>;

auto main() -> int {
    auto input = std::ifstream(R"(input.txt)");

    if (not input.is_open()) {
        std::println("Couldnt open the input file!");
        return EXIT_FAILURE;
    }

    auto reports = std::views::istream<my_algo::istream_line>(input)
    | std::views::transform(my_algo::convert_to<std::istream&>)
    | std::views::transform(std::views::istream<int>)
    | std::ranges::to<std::vector<report>>();

    for (auto&& report : reports) {
        for (auto&& v : report) {
            std::print("{} ", v);
        }

        std::println("");
    }
    
}