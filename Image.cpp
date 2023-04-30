#include "Image.hpp"

namespace prog
{
  Image::Image(int w, int h, const Color &fill)
  {
    this->_width = w;
    this->_height = h;
    // array de cores de 1 dimensao (mas que vai ser utilizado como se fosse uma matriz de 2 dimensões) com tamanho w * h e preenchido com a cor fill
    this->_image = new Color[this->_width * this->_height];
    for (int i = 0; i < this->_width * this->_height; i++)
    {
      this->_image[i] = fill;
    }
  }
  Image::~Image()
  {
    delete[] _image;
  }
  int Image::width() const
  {
    return this->_width;
  }
  int Image::height() const
  {
    return this->_height;
  }

  Color& Image::at(int x, int y)
  {
    return this->_image[x + y * this->_width];
  }

  const Color& Image::at(int x, int y) const
  {
    return this->_image[x + y * this->_width];
  }
}
