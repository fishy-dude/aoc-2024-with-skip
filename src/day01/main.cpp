#include <print>
#include <fstream>
#include <valarray>
#include <ranges>
#include <numeric>
#include <concepts>
#include <execution>
#include <functional>


namespace my_algo {
    struct accumulate : std::ranges::range_adaptor_closure<accumulate> {
        constexpr auto operator()(std::ranges::viewable_range auto&& range) {
            decltype(*begin(range)) total{};

            for (auto&& v : range)
                total += v;
            
            return total;
        }
    };

    struct difference {
        constexpr auto operator()(auto&& a, auto&& b) {
            if (b > a) 
                return b - a;
            else
                return a - b;
        }
    };

    inline constexpr auto sort_ranges(auto&& execution_policy, auto&... ranges) -> void
    requires (std::sortable<decltype(begin(ranges))> and ...) {
        (std::sort(execution_policy, begin(ranges), end(ranges)), ...);
    }

    inline constexpr auto times(auto&& n) {
        return std::views::iota(0) | std::views::take(n);
    }
}

auto main() -> int {
    auto input = std::ifstream(R"(input.txt)");

    if (not input.is_open()) {
        std::println("Couldnt open the file!");
        return EXIT_FAILURE;
    }

    auto line_count = std::count(std::istreambuf_iterator<char>(input), {}, '\n');
    input.seekg(0); // Reset stream to the beginning
    input.clear(); // Clear EOF flag

    auto left_list = std::valarray<int>(line_count);
    auto right_list = left_list;

    for (int i : my_algo::times(line_count)) {
        input >> left_list[i];
        input >> right_list[i];
    }

    my_algo::sort_ranges(std::execution::par_unseq, left_list, right_list);

    auto distnace_score = std::views::zip_transform(my_algo::difference(), left_list, right_list) 
    | my_algo::accumulate();

    std::println("Distance: {}", distnace_score);

    /// -------------------- Task 2 -------------------- ///

    auto similarity_score = std::views::zip_transform(
        [](auto&& v, auto&& range) { return std::ranges::count(range, v) * v; },
        left_list,
        std::views::repeat(right_list)
    ) | my_algo::accumulate();

    std::println("Similarity: {}", similarity_score);
}