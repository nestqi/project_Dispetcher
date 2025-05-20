#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

namespace Utils {
    void printTable(const std::vector<std::vector<int>>& data,
        const std::vector<std::string>& rowHeaders,
        const std::vector<std::string>& colHeaders);

    void clearScreen();
    void pause();

    template<typename T>
    void printVector(const std::vector<T>& vec, const std::string& title = "") {
        if (!title.empty()) {
            std::cout << title << ": ";
        }
        for (const auto& item : vec) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }

    int getIntInput(const std::string& prompt, int min, int max);
}