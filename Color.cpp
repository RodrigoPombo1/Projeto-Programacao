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
        return _red;
    }
    rgb_value Color::green() const {
        return _green;
    }
    rgb_value Color::blue() const {
        return _blue;
    }
    rgb_value& Color::red()  {
        return _red;
    }
    rgb_value& Color::green()  {
      return _green;
    }
    rgb_value& Color::blue()  {
      return _blue;
    }
}
