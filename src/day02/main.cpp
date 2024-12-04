#include <iostream>
#include <algorithm>
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

    inline auto clamped = [](auto&& min, auto&& max) { 
        return [&](auto&& value) {
            return min <= value and value <= max;
        };
    };

    inline auto always_true = [](auto&&...) { return true; };

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

    template <typename Func>
    struct all_of : std::ranges::range_adaptor_closure<all_of<Func>> {
        Func func;
        all_of(Func func) : func(func) {}

        inline auto operator()(auto&& range) {
           return std::ranges::find_if_not(range, func) == end(range);
        }
    };

    template <typename Func>
    struct one_of : std::ranges::range_adaptor_closure<one_of<Func>> {
        Func func;
        one_of(Func func) : func(func) {}

        inline auto operator()(auto&& range) {
           return std::ranges::find_if(range, func) != end(range);
        }
    };

    inline auto is_signed = [](auto&& value) -> bool { return not (value < 0); };
    inline auto absolute = [](auto&& value) { return std::abs(value); };
    inline auto length = [](auto&& value) { return std::ranges::count_if(value, always_true); };

    inline auto destructure = [](auto&& fn) {
        return [=](auto&& xy) {
            auto&& [x, y] = xy;
            return fn(x, y);
        };
    };

    inline auto k = [](auto&& fn) { return [=](auto&& x, [[maybe_unused]] auto&& y) { return fn(x); }; };

    inline auto bind_last = [](auto&& fn, auto&& y) {
        return [=](auto&& x) { return fn(x, y); };
    };

    inline auto equal_to = [](auto&& value) { return bind_last(std::equal_to{}, value); };
    inline auto not_equal_to = [](auto&& value) { return std::not_fn(equal_to(value)); };

    inline auto pairwise_all = [](auto&& fn) {
        return std::views::pairwise_transform(fn)
        | all_of(equal_to(true));
    };

} // namespace my_algo

using report = std::vector<int>;

auto main() -> int {
    namespace rv = std::ranges::views;
    namespace rg = std::ranges;
    using namespace my_algo;

    /// -------------------- Input -------------------- ///

    auto input = std::ifstream(R"(input.txt)");

    if (not input.is_open()) {
        std::println("Couldnt open the input file!");
        return EXIT_FAILURE;
    }

    auto reports = rv::istream<istream_line>(input)
    | rv::transform(convert_to<std::istream&>)
    | rv::transform(rv::istream<int>)
    | rg::to<std::vector<report>>();

    /// -------------------- Task 1 -------------------- ///

    auto is_safe = [](auto&& report) {
        auto&& deltas = report | rv::pairwise_transform(std::minus{});

        return deltas
        | rv::transform(is_signed)
        | pairwise_all(std::equal_to{})
        and deltas
        | rv::transform(absolute)
        | all_of(clamped(1, 3));
    };

    std::println("Task 1: {}", std::ranges::count_if(reports, is_safe));

    /// -------------------- Task 2 -------------------- ///

    auto less_constrained = reports | rv::filter([is_safe](auto&& report) {
        return rv::repeat(report, length(report))
        | rv::enumerate
        | rv::transform(destructure([](auto&& i, auto&& report) {
            return report
            | rv::enumerate
            | rv::filter(destructure(k(not_equal_to(i))))
            | rv::values;
        }))
        | rv::transform(is_safe)
        | one_of(equal_to(true));
    });

    std::println("Task 2: {}", length(less_constrained));
}