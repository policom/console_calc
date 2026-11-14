#include "Parser.h"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <string>

Parser::Parser(const PluginManager& plugins) : plugins_(plugins) {}

double Parser::evaluate(const std::string& expressionText) {
    text_ = expressionText;
    pos_ = 0;
    const double result = expression();
    skipSpaces();
    if (pos_ != text_.size()) {
        throw std::runtime_error("Лишний символ в позиции " +
                                 std::to_string(pos_ + 1) + ".");
    }
    if (!std::isfinite(result)) {
        throw std::runtime_error("Результат не является конечным числом.");
    }
    return result;
}

// expression = term (('+' | '-') term)*
double Parser::expression() {
    double value = term();
    while (true) {
        if (match('+')) {
            value += term();
        } else if (match('-')) {
            value -= term();
        } else {
            return value;
        }
    }
}

// term = power (('*' | '/') power)*
double Parser::term() {
    double value = power();
    while (true) {
        if (match('*')) {
            value *= power();
        } else if (match('/')) {
            const double divisor = power();
            if (divisor == 0.0) {
                throw std::runtime_error("Деление на ноль.");
            }
            value /= divisor;
        } else {
            return value;
        }
    }
}

// power = unary ('^' power)?; степень правоассоциативна
double Parser::power() {
    double base = unary();
    if (match('^')) {
        base = std::pow(base, power());
        if (!std::isfinite(base)) {
            throw std::runtime_error("Недопустимое значение степени.");
        }
    }
    return base;
}

double Parser::unary() {
    if (match('+')) return unary();
    if (match('-')) return -unary();
    return primary();
}

// primary = number | '(' expression ')' | function '(' expression ')'
double Parser::primary() {
    skipSpaces();
    if (match('(')) {
        const double value = expression();
        expect(')');
        return value;
    }

    if (pos_ < text_.size() &&
        (std::isdigit(static_cast<unsigned char>(text_[pos_])) ||
         text_[pos_] == '.')) {
        return number();
    }

    const std::string name = identifier();
    if (name.empty()) {
        throw std::runtime_error("Ожидалось число, функция или скобка "
                                 "в позиции " + std::to_string(pos_ + 1) + ".");
    }

    expect('(');
    const double argument = expression();
    expect(')');
    return plugins_.call(name, argument);
}

double Parser::number() {
    skipSpaces();
    const char* begin = text_.c_str() + pos_;
    char* end = nullptr;
    const double value = std::strtod(begin, &end);
    if (end == begin) {
        throw std::runtime_error("Некорректное число.");
    }
    pos_ += static_cast<std::size_t>(end - begin);
    if (!std::isfinite(value)) {
        throw std::runtime_error("Число вне допустимого диапазона.");
    }
    return value;
}

std::string Parser::identifier() {
    skipSpaces();
    const std::size_t start = pos_;
    while (pos_ < text_.size() &&
           (std::isalpha(static_cast<unsigned char>(text_[pos_])) ||
            text_[pos_] == '_')) {
        ++pos_;
    }
    return text_.substr(start, pos_ - start);
}

void Parser::skipSpaces() {
    while (pos_ < text_.size() &&
           std::isspace(static_cast<unsigned char>(text_[pos_]))) {
        ++pos_;
    }
}

bool Parser::match(char c) {
    skipSpaces();
    if (pos_ < text_.size() && text_[pos_] == c) {
        ++pos_;
        return true;
    }
    return false;
}

bool Parser::consume(char c) {
    return match(c);
}

void Parser::expect(char c) {
    if (!consume(c)) {
        throw std::runtime_error(std::string("Ожидался символ '") + c +
                                 "' в позиции " + std::to_string(pos_ + 1) + ".");
    }
}
