#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <iomanip>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
class WaveGrub {
private:
    std::vector<double> t;
    std::vector<double> wave;
    std::vector<double> ref_wave;
    double amp, freq, phase;
    static const int SIZE = 256;
    std::default_random_engine generator;

public:
    WaveGrub() : 
        t(SIZE), wave(SIZE), ref_wave(SIZE),
        amp(1), freq(1), phase(0) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        generator = std::default_random_engine(seed);
        for (int i = 0; i < SIZE; ++i) {
            t[i] = 2 * M_PI * i / SIZE;
            ref_wave[i] = std::sin(t[i]);  // Reference wave is a simple sine wave
        }
        update_wave();
    }
    void set_amplitude(double value) { amp = value; update_wave(); }
    void set_frequency(double value) { freq = value; update_wave(); }
    void set_phase(double value) { phase = value; update_wave(); }
    void add_reference() { interpret("+"); }
    void subtract_reference() { interpret("-"); }
    void multiply_reference() { interpret("*"); }
    void divide_reference() { interpret("/"); }
    void update_wave() {
        for (int i = 0; i < SIZE; ++i) {
            wave[i] = amp * std::sin(freq * t[i] + phase);
        }
    }

    void random_wave() {
        std::uniform_real_distribution<double> amp_dist(0.1, 2.0);
        std::uniform_real_distribution<double> freq_dist(0.5, 10.0);
        std::uniform_real_distribution<double> phase_dist(0, 2 * M_PI);

        amp = amp_dist(generator);
        freq = freq_dist(generator);
        phase = phase_dist(generator);

        update_wave();
        std::cout << "Generated random wave with:" << std::endl;
        std::cout << "Amp = " << amp << ", Freq = " << freq << ", Phase = " << phase << std::endl;
    }

    void interpret(const std::string& code) {
        for (char cmd : code) {
            switch (cmd) {
                case 'A': amp = std::min(amp + 0.1, 2.0); break;
                case 'a': amp = std::max(amp - 0.1, 0.1); break;
                case 'F': freq = std::min(freq + 0.5, 10.0); break;
                case 'f': freq = std::max(freq - 0.5, 0.5); break;
                case 'P': phase = std::fmod(phase + 0.2, 2 * M_PI); break;
                case 'p': phase = std::fmod(phase - 0.2 + 2 * M_PI, 2 * M_PI); break;
                case '*':
                    for (int j = 0; j < SIZE; ++j) wave[j] *= ref_wave[j];
                    break;
                case '+':
                    for (int j = 0; j < SIZE; ++j) wave[j] += ref_wave[j];
                    break;
                case '-':
                    for (int j = 0; j < SIZE; ++j) wave[j] -= ref_wave[j];
                    break;
                case '/':
                    for (int j = 0; j < SIZE; ++j) {
                        if (ref_wave[j] != 0) wave[j] /= ref_wave[j];
                        else wave[j] = 0;  // Avoid division by zero
                    }
                    break;
                case 'I':
                    inverse_wave();
                    break;
                case '=':
                    print_waves();
                    break;
                case 'R':
                    reset_wave();
                    break;
                case 'N':
                    random_wave();
                    break;
            }
            update_wave();
        }
    }

    void inverse_wave() {
        for (int i = 0; i < SIZE; ++i) {
            if (wave[i] != 0) wave[i] = 1 / wave[i];
            else wave[i] = 0;  // Avoid division by zero
        }
    }

    void reset_wave() {
        amp = 1;
        freq = 1;
        phase = 0;
    }

    void print_waves() {
        std::cout << "Current wave parameters: Amp = " << amp << ", Freq = " << freq << ", Phase = " << phase << std::endl;
        std::cout << "Wave:    ";
        for (int i = 0; i < SIZE; i += SIZE/8) 
            std::cout << std::fixed << std::setprecision(2) << wave[i] << " ";
        std::cout << "\nRef Wave:";
        for (int i = 0; i < SIZE; i += SIZE/8) 
            std::cout << std::fixed << std::setprecision(2) << ref_wave[i] << " ";
        std::cout << std::endl;
    }
};

enum class TokenType {
    WAVE, AMP, FREQ, PHASE, PLUS, MINUS, MULTIPLY, DIVIDE, INVERSE,
    ASSIGN, NUMBER, IDENTIFIER, SEMICOLON
};

struct Token{
    TokenType type;
    std::string value;
};

class Lexer {
public:
    std::vector<Token> tokenize(const std::string& code) {
        std::vector<Token> tokens;
        std::istringstream iss(code);
        std::string word;
        while (iss >> word) {
            if (word == "wave") tokens.push_back({TokenType::WAVE, word});
            else if (word == "amplitude") tokens.push_back({TokenType::AMP, word});
            else if (word == "frequency") tokens.push_back({TokenType::FREQ, word});
            else if (word == "phase") tokens.push_back({TokenType::PHASE, word});
            else if (word == "+") tokens.push_back({TokenType::PLUS, word});
            else if (word == "-") tokens.push_back({TokenType::MINUS, word});
            else if (word == "*") tokens.push_back({TokenType::MULTIPLY, word});
            else if (word == "/") tokens.push_back({TokenType::DIVIDE, word});
            else if (word == "inverse") tokens.push_back({TokenType::INVERSE, word});
            else if (word == "=") tokens.push_back({TokenType::ASSIGN, word});
            else if (word == ";") tokens.push_back({TokenType::SEMICOLON, word});
            else if (std::isdigit(word[0])) tokens.push_back({TokenType::NUMBER, word});
            else tokens.push_back({TokenType::IDENTIFIER, word});
        }
        return tokens;
    }
};

class Parser {
public:
    std::string parse(const std::vector<Token>& tokens) {
        std::string output;
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::WAVE) {
                output += "WaveGrub wave;\n";
            } else if (tokens[i].type == TokenType::AMP && i+2 < tokens.size() && tokens[i+1].type == TokenType::ASSIGN) {
                output += "wave.set_amplitude(" + tokens[i+2].value + ");\n";
                i += 2;
            } else if (tokens[i].type == TokenType::FREQ && i+2 < tokens.size() && tokens[i+1].type == TokenType::ASSIGN) {
                output += "wave.set_frequency(" + tokens[i+2].value + ");\n";
                i += 2;
            } else if (tokens[i].type == TokenType::PHASE && i+2 < tokens.size() && tokens[i+1].type == TokenType::ASSIGN) {
                output += "wave.set_phase(" + tokens[i+2].value + ");\n";
                i += 2;
            } else if (tokens[i].type == TokenType::PLUS) {
                output += "wave.add_reference();\n";
            } else if (tokens[i].type == TokenType::MINUS) {
                output += "wave.subtract_reference();\n";
            } else if (tokens[i].type == TokenType::MULTIPLY) {
                output += "wave.multiply_reference();\n";
            } else if (tokens[i].type == TokenType::DIVIDE) {
                output += "wave.divide_reference();\n";
            } else if (tokens[i].type == TokenType::INVERSE) {
                output += "wave.inverse();\n";
            }
        }
        return output;
    }
};


class Compiler {
private:
    Lexer lexer;
    Parser parser;

public:
    std::string compile(const std::string& code) {
        std::string output;
        auto tokens = lexer.tokenize(code);
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::WAVE) {
                output += "WaveGrub wave;\n";
            } else if (tokens[i].type == TokenType::AMP && i+2 < tokens.size() && tokens[i+1].type == TokenType::ASSIGN) {
                output += "wave.set_amplitude(" + tokens[i+2].value + ");\n";
                i += 2;
            } else if (tokens[i].type == TokenType::FREQ && i+2 < tokens.size() && tokens[i+1].type == TokenType::ASSIGN) {
                output += "wave.set_frequency(" + tokens[i+2].value + ");\n";
                i += 2;
            } else if (tokens[i].type == TokenType::PHASE && i+2 < tokens.size() && tokens[i+1].type == TokenType::ASSIGN) {
                output += "wave.set_phase(" + tokens[i+2].value + ");\n";
                i += 2;
            } else if (tokens[i].type == TokenType::PLUS) {
                output += "wave.add_reference();\n";
            } else if (tokens[i].type == TokenType::MINUS) {
                output += "wave.subtract_reference();\n";
            } else if (tokens[i].type == TokenType::MULTIPLY) {
                output += "wave.multiply_reference();\n";
            } else if (tokens[i].type == TokenType::DIVIDE) {
                output += "wave.divide_reference();\n";
            } else if (tokens[i].type == TokenType::INVERSE) {
                output += "wave.inverse();\n";
            }
        }
        return output;
    }
};


// int main() {
//     Compiler compiler;
//     std::string input;

//     std::cout << "Welcome to C+++ Compiler!" << std::endl;
//     std::cout << "Enter C+++ code (or 'quit' to exit):" << std::endl;

//     while (true) {
//         std::cout << "> ";
//         std::getline(std::cin, input);
        
//         if (input == "quit") {
//             break;
//         }

//         std::string compiled_code = compiler.compile(input);
//         std::cout << "Compiled C++ code:" << std::endl;
//         std::cout << compiled_code << std::endl;
//     }

//     std::cout << "Thank you for using C+++ Compiler!" << std::endl;
//     return 0;
// }




















int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename.cppp>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    Compiler compiler;
    WaveGrub wg;

    std::string compiled_code = compiler.compile(code);
    std::cout << "Compiled C++ code:" << std::endl;
    std::cout << compiled_code << std::endl;

    // Execute the compiled code
    std::istringstream iss(compiled_code);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.find("wave.") == 0) {
            std::string command = line.substr(5, line.length() - 7);  // Remove "wave." and ";"
            wg.interpret(command);
        }
    }

    // Print the final wave state
    wg.interpret("=");

    return 0;
}