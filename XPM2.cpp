#include "XPM2.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

namespace prog {
    Color hex_to_rgb(const string& r_hex, const string& g_hex, const string& b_hex) {
        map<char, int> hex_int =
        {
            {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
            {'8', 8}, {'9', 9}, {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}
        };

        int red = hex_int[r_hex[0]] * 16 + hex_int[r_hex[1]];
        int green = hex_int[g_hex[0]] * 16 + hex_int[g_hex[1]];
        int blue = hex_int[b_hex[0]] * 16 + hex_int[b_hex[1]];
        Color result((rgb_value)red, (rgb_value)green, (rgb_value)blue);
        return result;
    }

    Image* loadFromXPM2(const std::string& file) {
        ifstream input_file;
        input_file.open(file); // Abrir o arquivo usando o caminho fornecido.
        string line; // Variável para armazenar linhas.
        getline(input_file, line); // Pode ser usado para verificar se o conteúdo pertence a um arquivo xpm2, ignorado.
        getline(input_file, line); // Parâmetros (Tamanho, Contagem de Cores, Caracteres por pixel).
        istringstream iss(line);
        int largura;
        int altura;
        int num_cores; // Quantidade de cores a serem carregadas.
        int char_por_pixel; // constante 1.
        iss >> largura >> altura >> num_cores >> char_por_pixel;

        Image* res_image = new Image(largura, altura); // Criar uma nova imagem com as dimensões fornecidas.

        char color_char; // Variável para armazenar o caractere que será convertido.
        string hex_value_color; // Variável para armazenar o valor hexadecimal de um determinado caractere.
        map<char, Color> char_to_color_map; // Criar um mapa vazio para armazenar o valor de cor de cada caractere.
        string _; // Variável para armazenar o caractere "c" (ignorado).
        
        // Loop para obter a cor correspondente a cada caractere.
        for (int i = 0; i < num_cores; i++) {
            // As linhas que obtemos dentro deste loop for têm parâmetros que precisamos salvar para poder
            // converter de um caractere para uma cor real.
            getline(input_file, line);
            istringstream line_iss(line);
            line_iss >> color_char >> _ >> hex_value_color;
            hex_value_color = hex_value_color.substr(1, 6); 
            // Certificar-se de que cada caractere esteja em minúsculas, já que
            // o dicionário usado na função hex_to_rgb usa caracteres minúsculos.
            for (char& current_char: hex_value_color) {
                current_char = tolower(current_char);
            }
            char_to_color_map[color_char] = hex_to_rgb(hex_value_color.substr(0, 2), hex_value_color.substr(2, 2), hex_value_color.substr(4, 2));
        }

        // Loop para obter a imagem resultado.
        for (int num_line = 0; num_line < res_image->height(); num_line++) {
            getline(input_file, line);
            for (int num_char = 0; num_char < res_image->width(); num_char++) {
                res_image->at(num_char, num_line) = char_to_color_map[line[num_char]]; // Substituir o pixel atual pela cor retornada pelo mapa.
            }       
        }
        input_file.close();
        return res_image;
    }

    bool check_color_in_vector(const vector<pair<Color, char>> vector, const Color color) {
        for (pair<Color, char> pair_color_to_char : vector) {
            if (pair_color_to_char.first == color) {
                return true;
            }
        }
        return false;
    }

    string convert_from_rgb_value_to_hex(rgb_value value) {
        map<int, char> int_hex =
        {
            {0, '0'}, {1, '1'}, {2, '2'}, {3, '3'}, {4, '4'}, {5, '5'}, {6, '6'}, {7, '7'},
            {8, '8'}, {9, '9'}, {10, 'a'}, {11, 'b'}, {12, 'c'}, {13, 'd'}, {14, 'e'}, {15, 'f'}
        };
        string result = "";
        result += int_hex[value / 16];
        result += int_hex[value % 16];
        return result;
    }

    void saveToXPM2(const std::string& file, const Image* image) {
        ofstream output_file;
        output_file.open(file);
        vector<pair<Color, char>> color_to_char_vector;
        size_t num_colors = 0;
        // Saber cada cor e o seu valor hexadecimal.
        for (int y = 0; y < image->height(); y++) {
            for (int x = 0; x < image->width(); x++) {
                if (!check_color_in_vector(color_to_char_vector, image->at(x, y))){
                    color_to_char_vector.emplace_back(image->at(x, y), 'a' + num_colors);
                    num_colors++;
                }
            }
        }
        // Escrever o inicio do ficheiro.
        output_file << "! XPM2" << endl;
        output_file << image->width() << " " << image->height() << " " << num_colors << " " << "1" << endl;
        for (const auto& pair : color_to_char_vector) {
            const Color& current_color = pair.first;
            char char_of_current_color = pair.second;
            output_file << char_of_current_color << " c " << "#" << convert_from_rgb_value_to_hex(current_color.red()) << convert_from_rgb_value_to_hex(current_color.green()) << convert_from_rgb_value_to_hex(current_color.blue()) << endl;
        }

        //Escrever a imagem.
        for (int y = 0; y < image->height(); y++) {
            for (int x = 0; x < image->width(); x++) {
                const Color& pixel_color = image->at(x, y);
                // Encontra a cor do pixel atual que está no vetor de cores para caracteres.
                auto iterator_pair = find_if(color_to_char_vector.begin(), color_to_char_vector.end(), [&](pair<Color, char> element_pair) { return element_pair.first == pixel_color; });
                if (iterator_pair != color_to_char_vector.end()) {
                    output_file << iterator_pair->second;
                }
            }
            output_file << endl;
        }
        output_file.close();
    }
}