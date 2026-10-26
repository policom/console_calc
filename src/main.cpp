#include "Parser.h"
#include "PluginManager.h"

#include <iomanip>
#include <iostream>
#include <string>

int main() {
    try {
        PluginManager plugins;
        plugins.loadFromDirectory("./plugins");

        std::cout << "\nКонсольный калькулятор\n"
                  << "Операции: + - * / ^, скобки и функции из DLL.\n"
                  << "Пример: 16 + 4 * (3 - 1)\n"
                  << "Для выхода введите exit.\n\n";

        Parser parser(plugins);
        std::string line;

        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) {
                break;
            }

            if (line == "exit" || line == "quit") {
                break;
            }
            if (line.empty()) {
                continue;
            }

            try {
                const double result = parser.evaluate(line);
                std::cout << std::setprecision(15) << result << '\n';
            } catch (const std::exception& e) {
                std::cerr << "Ошибка: " << e.what() << '\n';
            } catch (...) {
                std::cerr << "Неизвестная ошибка при вычислении.\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Неизвестная критическая ошибка.\n";
        return 1;
    }

    return 0;
}
