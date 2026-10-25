#pragma once
#include "PluginManager.h"
#include <string>

class Parser {
public:
    explicit Parser(const PluginManager& plugins);
    double evaluate(const std::string& expression);

private:
    const PluginManager& plugins_;
    std::string text_;
    std::size_t pos_ = 0;

    double expression();
    double term();
    double power();
    double unary();
    double primary();

    void skipSpaces();
    bool match(char c);
    bool consume(char c);
    std::string identifier();
    double number();
    void expect(char c);
};
