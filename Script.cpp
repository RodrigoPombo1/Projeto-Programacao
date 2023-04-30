#include <iostream>
#include <fstream>
#include "Script.hpp"
#include "PNG.hpp"
#include "XPM2.hpp"

using namespace std;

namespace prog {
    // Use to read color values from a script file.
    istream& operator>>(istream& input, Color& c) {
        int r, g, b;
        input >> r >> g >> b;
        c.red() = r;
        c.green() = g;
        c.blue() = b;
        return input;
    }

    Script::Script(const string& filename) :
            image(nullptr), input(filename) {

    }
    void Script::clear_image_if_any() {
        if (image != nullptr) {
            delete image;
            image = nullptr;
        }
    }
    Script::~Script() {
        clear_image_if_any();
    }

    void Script::run() {
        string command;
        while (input >> command) {
            cout << "Executing command '" << command << "' ..." << endl;
            if (command == "open") {
                open();
                continue;
            }
            if (command == "blank") {
                blank();
                continue;
            }
            // Other commands require an image to be previously loaded.
            if (command == "save") {
                save();
                continue;
            }
            // Transforms each individual pixel (r, g, b) to (255-r,255-g,255-b).
            if (command == "invert") {
                for (int i = 0; i < image->height(); i++) {
                    for (int j = 0; j < image->width(); j++) {
                        image->at(i, j).red() = 255 - image->at(i, j).red();
                        image->at(i, j).green() = 255 - image->at(i, j).green();
                        image->at(i, j).blue() = 255 - image->at(i, j).blue();
                    }
                }
                continue;
            }
            // Transforms each individual pixel (r, g, b) to (v, v, v) where v = (r + g + b)/3.
            if (command == "to_gray_scale") {
                for (int i = 0; i < image->height(); i++) {
                    for (int j = 0; j < image->width(); j++) {
                        int gray = (image->at(i, j).red() + image->at(i, j).green() + image->at(i, j).blue()) / 3;
                        image->at(i, j).red() = gray;
                        image->at(i, j).green() = gray;
                        image->at(i, j).blue() = gray;
                    }
                }
                continue;
            }
            // usage: "replace r1 g1 b1 r2 g2 b2"
            // Replaces all (r1,  g1, b1) pixels by (r2,  g2, b2).
            if (command == "replace") {
                Color c1, c2;
                input >> c1.red() >> c1.green() >> c1.blue() >> c2.red() >> c2.green() >> c2.blue();
                for (int i = 0; i < image->height(); i++) {
                    for (int j = 0; j < image->width(); j++) {
                        if (image->at(i, j).red() == c1.red() && image->at(i, j).green() == c1.green() && image->at(i, j).blue() == c1.blue()) {
                            image->at(i, j) = c2;
                        }
                    }
                }
                continue;
            }
            // usage: "fill x y w h r g b"
            // Assign (r, g, b) to all pixels contained in rectangle defined by top-left corner (x, y), width w, and height h, i.e., all pixels (x', y') such that x <= x' < x + w and y <= y' < y + h.
            if (command == "fill") {
                int top_corner_x, top_corner_y, width, height;
                Color fill;
                input >> top_corner_x >> top_corner_y >> width >> height >> fill.red() >> fill.green() >> fill.blue();
                for (int i = top_corner_x; i < width; i++) {
                    for (int j = top_corner_y; j < height; j++) {
                        image->at(i, j) = fill;
                    }
                }
                continue;
            }
            // Mirror image horizontally.
            if (command == "h_mirror") {
                for (int i = 0; i < image->height(); i++) {
                    for (int j = 0; j < image->width() / 2; j++) {
                        Color temp = image->at(i, j);
                        image->at(i, j) = image->at(i, image->width() - j - 1);
                        image->at(i, image->width() - j - 1) = temp;
                    }
                }
                continue;
            }
            // Mirror image vertically.
            if (command == "v_mirror") {
                for (int i = 0; i < image->height() / 2; i++) {
                    for (int j = 0; j < image->width(); j++) {
                        Color temp = image->at(i, j);
                        image->at(i, j) = image->at(image->height() - i - 1, j);
                        image->at(image->height() - i - 1, j) = temp;
                    }
                }
                continue;
            }
            // usage: "add filename r g b x y"
            // Copy all pixels from image stored in PNG file filename, except pixels in that image with “neutral” color (r, g, b), to the rectangle of the current image with top-left corner (x, y) of the current image. 
            if (command == "add") {
                string filename;
                Color neutral;
                int top_corner_x, top_corner_y;
                input >> filename >> neutral.red() >> neutral.green() >> neutral.blue() >> top_corner_x >> top_corner_y;
                Image* image2 = loadFromPNG(filename);
                for (int i = 0; i < image2->height(); i++) {
                    for (int j = 0; j < image2->width(); j++) {
                        if (image2->at(i, j).red() != neutral.red() || image2->at(i, j).green() != neutral.green() || image2->at(i, j).blue() != neutral.blue()) {
                            image->at(i + top_corner_x, j + top_corner_y) = image2->at(i, j);
                        }
                    }
                }
                delete image2;
                continue;
            }
            // usage: "crop x y w h"
            // Crop the image, reducing it to all pixels contained in the rectangle defined by top-left corner (x, y), width w, and height h.You may assume that the rectangle is always within the current image bounds.
            if (command == "crop") {
                int top_corner_x, top_corner_y, width, height;
                input >> top_corner_x >> top_corner_y >> width >> height;
                Image* image2 = new Image(width, height);
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        image2->at(i, j) = image->at(i + top_corner_x, j + top_corner_y);
                    }
                }
                image = image2;
                delete image2;
                continue;
            }
            // Rotate image left by 90 degrees.
            if (command == "rotate_left") {
                Image* image2 = new Image(image->width(), image->height());
                for (int i = 0; i < image->height(); i++) {
                    for (int j = 0; j < image->width(); j++) {
                        image2->at(i, j) = image->at(j, image->height() - i - 1);
                    }
                }
                image = image2;
                delete image2;
                continue;
            }
            // Rotate image right by 90 degrees.
            if (command == "rotate_right") {
                Image* image2 = new Image(image->width(), image->height());
                for (int i = 0; i < image->height(); i++) {
                    for (int j = 0; j < image->width(); j++) {
                        image2->at(i, j) = image->at(image->width() - j - 1, i);
                    }
                }
                image = image2;
                delete image2;
                continue;
            }
        }
    }
    void Script::open() {
        // Replace current image (if any) with image read from PNG file.
        clear_image_if_any();
        string filename;
        input >> filename;
        image = loadFromPNG(filename);
    }
    void Script::blank() {
        // Replace current image (if any) with blank image.
        clear_image_if_any();
        int w, h;
        Color fill;
        input >> w >> h >> fill;
        image = new Image(w, h, fill);
    }
    void Script::save() {
        // Save current image to PNG file.
        string filename;
        input >> filename;
        saveToPNG(filename, image);
    }
}
