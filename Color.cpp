#include "Color.hpp"

namespace prog {
    Color::Color() : Color::Color(0, 0, 0) {}
    Color::Color(const Color& other) {
        this->_red = other._red;
        this->_green = other._green;
        this->_blue = other._blue;
    }
    Color::Color(rgb_value red, rgb_value green, rgb_value blue) {
        this->_red = red;
        this->_green = green;
        this->_blue = blue;
    }
    rgb_value Color::red() const {
        return this->_red;
    }
    rgb_value Color::green() const {
        return this->_green;
    }
    rgb_value Color::blue() const {
        return this->_blue;
    }
    rgb_value& Color::red()  {
        return this->_red;
    }
    rgb_value& Color::green()  {
      return this->_green;
    }
    rgb_value& Color::blue()  {
      return this->_blue;
    }

    bool operator==(const Color& color1, const Color& color2) {
        return color1.red() == color2.red() &&
            color1.green() == color2.green() &&
            color1.blue() == color2.blue();
    }
}
