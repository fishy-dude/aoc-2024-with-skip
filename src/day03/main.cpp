#include <algorithm>
#include <iostream>
#include <fstream>
#include <print>
#include <ranges>

#include "ctre.hpp"

namespace my_algo {
    struct accumulate : std::ranges::range_adaptor_closure<accumulate> {
        constexpr auto operator()(std::ranges::viewable_range auto&& range) {
            decltype(*begin(range)) total{};

            for (auto&& v : range)
                total += v;
            
            return total;
        }
    };

    inline auto destructure = [](auto&& fn) {
        return [=](auto&& xy) {
            auto&& [x, y] = xy;
            return fn(x, y);
        };
    };

    inline auto flip = [](auto&& fn) { return [&](auto&& x, auto&& y) { return fn(y, x); }; };
    inline auto k = [](auto&& fn) { return [&](auto&& x, [[maybe_unused]] auto&& y) { return fn(x); }; };
    inline auto k1 = [](auto&& fn) { return flip(k(fn)); };

    inline auto bind_last = [](auto&& fn, auto&& y) {
        return [&](auto&& x) { return fn(x, y); };
    };
}

auto main() -> int {
    namespace rg = std::ranges;
    namespace rv = std::views;
    using namespace my_algo;

    /// -------------------- Input -------------------- ///

    auto input_file = std::ifstream("input.txt");

    if (not input_file.is_open()) {
        std::println("Couldn't open the file!");
        return EXIT_FAILURE;
    }

    auto input_string = std::string(std::istreambuf_iterator(input_file), {});

    /// -------------------- Task 1 -------------------- ///

    auto extract_numbers = [](auto&& re) {
        return std::pair(
            re.begin(),
            std::pair(re.get<1>().to_number(), re.get<2>().to_number())
        );
    };
    
    auto sum = ctre::search_all<R"(mul\((\d{1,3}),(\d{1,3})\))">(input_string)
    | rv::transform(extract_numbers)
    | rv::transform(destructure(k1(destructure(std::multiplies{}))))
    | accumulate();

    std::println("Task 1: {}", sum);

    /// -------------------- Task 2 -------------------- ///

    auto extract_do_donts = [](auto&& re) {
        return std::pair(
            re.begin(),
            re.size() == 4
        );
    };

    auto should_be_done = [](auto&& re_iter, auto&& do_donts) {
        auto [result, _] = rg::find_last_if(do_donts, [&re_iter](auto&& do_iter1) {
            return do_iter1 < re_iter;
        }, [](auto&& do_pair) { return do_pair.first; });

        return result == end(do_donts) or (*result).second;
    };

    auto do_donts = ctre::search_all<R"(do\(\)|don't\(\))">(input_string) | rv::transform(extract_do_donts);

    auto filtered_sum = ctre::search_all<R"(mul\((\d{1,3}),(\d{1,3})\))">(input_string)
    | rv::transform(extract_numbers)
    | rv::filter(destructure(k(bind_last(should_be_done, do_donts))))
    | rv::transform(destructure(k1(destructure(std::multiplies{}))))
    | accumulate();

    std::println("Task 2: {}", filtered_sum);
}