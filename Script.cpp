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
                for (int y = 0; y < image->height(); y++) {
                    for (int x = 0; x < image->width(); x++) {
                        image->at(x, y).red() = 255 - image->at(x, y).red();
                        image->at(x, y).green() = 255 - image->at(x, y).green();
                        image->at(x, y).blue() = 255 - image->at(x, y).blue();
                    }
                }
                continue;
            }
            // Transforms each individual pixel (r, g, b) to (v, v, v) where v = (r + g + b)/3.
            if (command == "to_gray_scale") {
                for (int y = 0; y < image->height(); y++) {
                    for (int x = 0; x < image->width(); x++) {
                        int gray = (image->at(x, y).red() + image->at(x, y).green() + image->at(x, y).blue()) / 3;
                        image->at(x, y).red() = gray;
                        image->at(x, y).green() = gray;
                        image->at(x, y).blue() = gray;
                    }
                }
                continue;
            }
            // usage: "replace r1 g1 b1 r2 g2 b2"
            // Replaces all (r1, g1, b1) pixels by (r2, g2, b2).
            if (command == "replace") {
                Color color_1, color_2;
                input >> color_1.red() >> color_1.green() >> color_1.blue() >> color_2.red() >> color_2.green() >> color_2.blue();
                for (int y = 0; y < image->height(); y++) {
                    for (int x = 0; x < image->width(); x++) {
                        if (image->at(x, y).red() == color_1.red() && image->at(x, y).green() == color_1.green() && image->at(x, y).blue() == color_1.blue()) {
                            image->at(x, y) = color_2;
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
                for (int y = top_corner_y; y < height; y++) {
                    for (int x = top_corner_x; x < width; x++) {
                        image->at(x, y) = fill;
                    }
                }
                continue;
            }
            // Mirror image horizontally.
            if (command == "h_mirror") {
                for (int y = 0; y < image->height(); y++) {
                    for (int x = 0; x < image->width() / 2; x++) {
                        Color temp = image->at(x, y);
                        image->at(x, y) = image->at(image->width() - x - 1, y);
                        image->at(image->width() - x - 1, y) = temp;
                    }
                }
                continue;
            }
            // Mirror image vertically.
            if (command == "v_mirror") {
                for (int y = 0; y < image->height() / 2; y++) {
                    for (int x = 0; x < image->width(); x++) {
                        Color color_temp = image->at(x, y);
                        image->at(x, y) = image->at(x, image->height() - y - 1);
                        image->at(x, image->height() - y - 1) = color_temp;
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
                Image* image_to_add = loadFromPNG(filename);
                for (int y = 0; y < image_to_add->height(); y++) {
                    for (int x = 0; x < image_to_add->width(); x++) {
                        if (image_to_add->at(x, y).red() != neutral.red() || image_to_add->at(x, y).green() != neutral.green() || image_to_add->at(x, y).blue() != neutral.blue()) {
                            image->at(x + top_corner_x, y + top_corner_y) = image_to_add->at(x, y);
                        }
                    }
                }
                continue;
            }
            // usage: "crop x y w h"
            // Crop the image, reducing it to all pixels contained in the rectangle defined by top-left corner (x, y), width w, and height h.
            if (command == "crop") {
                int top_corner_x, top_corner_y, width, height;
                input >> top_corner_x >> top_corner_y >> width >> height;
                Image* image_temp = new Image(width, height);
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        image_temp->at(x, y) = image->at(x + top_corner_x, y + top_corner_y);
                    }
                }
                delete image;
                image = image_temp;
                continue;
            }
            // Rotate image left by 90 degrees.
            if (command == "rotate_left") {
                Image* image_temp = new Image(image->height(), image->width());
                for (int y = 0; y < image->height(); y++) {
                    for (int x = 0; x < image->width(); x++) {
                        image_temp->at(y, x) = image->at(image->width() - x - 1, y);
                    }
                }
                delete image;
                image = image_temp;
                continue;
            }
            // Rotate image right by 90 degrees.
            if (command == "rotate_right") {
                Image* image_temp = new Image(image->height(), image->width());
                for (int y = 0; y < image->height(); y++) {
                    for (int x = 0; x < image->width(); x++) {
                        image_temp->at(y, x) = image->at(x, y - image->height() - 1);
                    }
                }
                delete image;
                image = image_temp;
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

