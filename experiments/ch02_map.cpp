#include <iostream>
#include <map>

int main() {
    std::map<int, std::string> prices;

    prices[105] = "order_A";
    prices[101] = "order_B";
    prices[110] = "order_C";
    prices[103] = "order_D";

    std::cout << "Iterating the map:\n";
    for (const auto& item : prices)
    {
        std::cout << item.first << " "
                << item.second << '\n';
    }

    std::cout << "Smallest price (begin): " << prices.begin()->first << "\n";
    std::cout << "Largest price (rbegin): " << prices.rbegin()->first << "\n";
}