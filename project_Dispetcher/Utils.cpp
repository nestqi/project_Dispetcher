#include "Utils.h"
#include <limits>

namespace Utils {
    void printTable(const std::vector<std::vector<int>>& data,
        const std::vector<std::string>& rowHeaders,
        const std::vector<std::string>& colHeaders) {
        // Вывод заголовков столбцов
        std::cout << std::setw(15) << " ";
        for (const auto& header : colHeaders) {
            std::cout << std::setw(15) << header;
        }
        std::cout << "\n";

        // Вывод данных
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << std::setw(15) << rowHeaders[i];
            for (size_t j = 0; j < data[i].size(); ++j) {
                std::cout << std::setw(15) << data[i][j];
            }
            std::cout << "\n";
        }
    }

    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void pause() {
        std::cout << "Нажмите Enter для продолжения...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    int getIntInput(const std::string& prompt, int min, int max) {
        int value;
        while (true) {
            std::cout << prompt;
            std::cin >> value;
            if (std::cin.fail() || value < min || value > max) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Ошибка! Введите число от " << min << " до " << max << std::endl;
            }
            else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
        }
    }
}