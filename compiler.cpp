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
#include <stack>
#include <cctype>
#include <stdexcept>
enum class TokenType {
    WAVE, AMP, FREQ, PHASE, PLUS, MINUS, MULTIPLY, DIVIDE, INVERSE,
    ASSIGN, NUMBER, IDENTIFIER,
    IF, ELSE, WHILE, 
    EQUAL, LESS, GREATER, NOT_EQUAL, RANDOM, PRINT
};
struct Token {
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
            else if (word == "random") tokens.push_back({TokenType::RANDOM, word});
            else if (word == "print") tokens.push_back({TokenType::PRINT, word});
            else if (word == "if") tokens.push_back({TokenType::IF, word});
            else if (word == "else") tokens.push_back({TokenType::ELSE, word});
            else if (word == "while") tokens.push_back({TokenType::WHILE, word});
            else if (word == "==") tokens.push_back({TokenType::EQUAL, word});
            else if (word == "<") tokens.push_back({TokenType::LESS, word});
            else if (word == ">") tokens.push_back({TokenType::GREATER, word});
            else if (word == "!=") tokens.push_back({TokenType::NOT_EQUAL, word});
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
            switch (tokens[i].type) {
                case TokenType::WAVE:
                    output += "CPlusPlusPlus wave;\n";
                    i++; // Skip the identifier after 'wave'
                    break;
                case TokenType::AMP:
                case TokenType::FREQ:
                case TokenType::PHASE:
                    if (i + 2 < tokens.size() && tokens[i+1].type == TokenType::ASSIGN) {
                        output += "wave.set_" + tokens[i].value + "(" + tokens[i+2].value + ");\n";
                        i += 2;
                    }
                    break;
                case TokenType::RANDOM:
                    output += "wave.random_wave();\n";
                    break;
                case TokenType::PRINT:
                    output += "wave.print_wave();\n";
                    break;
                case TokenType::IF:
                if (i + 3 < tokens.size()) {
                    output += "if (wave.compare(\"" + tokens[i+2].value + "\", " + tokens[i+3].value + ")) {\n";
                    i += 3;
                }
                break;
                case TokenType::ELSE:
                    output += "} else {\n";
                    break;
                case TokenType::WHILE:
                    if (i + 3 < tokens.size()) {
                        output += "while (wave.compare(\"" + tokens[i+1].value + "\", " + tokens[i+2].value + ")) {\n";
                        i += 2;
                    }
                    break;
                case TokenType::PLUS:
                    output += "wave.add_reference();\n";
                    break;
                case TokenType::MINUS:
                    output += "wave.subtract_reference();\n";
                    break;
                case TokenType::MULTIPLY:
                    output += "wave.multiply_reference();\n";
                    break;
                case TokenType::DIVIDE:
                    output += "wave.divide_reference();\n";
                    break;
                case TokenType::INVERSE:
                    output += "wave.inverse();\n";
                    break;
                default:
                    break;
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
        auto tokens = lexer.tokenize(code);
        return parser.parse(tokens);
    }
};

class CPlusPlusPlus {
private:
    std::vector<double> t;
    std::vector<double> wave;
    std::vector<double> ref_wave;
    double amp, freq, phase;
    static const int SIZE = 256;
    std::default_random_engine generator;

public:
    CPlusPlusPlus() : 
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
    bool compare(const std::string& op, double value) {
        if (op == "==") return amp == value;
        if (op == "<") return amp < value;
        if (op == ">") return amp > value;
        if (op == "!=") return amp != value;
        return false;
    }
    void set_amplitude(double value) { amp = value; update_wave(); }
    void set_frequency(double value) { freq = value; update_wave(); }
    void set_phase(double value) { phase = value; update_wave(); }
    void add_reference() { interpret("+"); }
    void subtract_reference() { interpret("-"); }
    void multiply_reference() { interpret("*"); }
    void divide_reference() { interpret("/"); }
    void inverse() {
        for (int i = 0; i < SIZE; ++i) {
            if (wave[i] != 0) wave[i] = 1 / wave[i];
            else wave[i] = 0;  // Avoid division by zero
        }
        update_wave();
    }
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
    }

    void print_wave() {
        std::cout << "Wave parameters: Amp = " << amp << ", Freq = " << freq << ", Phase = " << phase << std::endl;
        // ... (existing print_waves() code)
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
    CPlusPlusPlus wave;

    std::string compiled_code = compiler.compile(code);
    std::cout << "Compiled C++ code:" << std::endl;
    std::cout << compiled_code << std::endl;

    // Execute the compiled code
    std::istringstream iss(compiled_code);
    std::string line;
    std::stack<bool> condition_stack;
    condition_stack.push(true);

    while (std::getline(iss, line)) {
        if (condition_stack.top()) {
            if (line.find("wave.") == 0) {
                size_t method_end = line.find('(');
                if (method_end != std::string::npos) {
                    std::string method = line.substr(5, method_end - 5);
                    size_t start = line.find("(") + 1;
                    size_t end = line.find(")");
                    std::string arg = line.substr(start, end - start);

                    try {
                        if (method == "set_amplitude") {
                            wave.set_amplitude(std::stod(arg));
                        } else if (method == "set_frequency") {
                            wave.set_frequency(std::stod(arg));
                        } else if (method == "set_phase") {
                            wave.set_phase(std::stod(arg));
                        } else if (method == "random_wave") {
                            wave.random_wave();
                        } else if (method == "print_wave") {
                            wave.print_wave();
                        } else if (method == "add_reference") {
                            wave.add_reference();
                        } else if (method == "subtract_reference") {
                            wave.subtract_reference();
                        } else if (method == "multiply_reference") {
                            wave.multiply_reference();
                        } else if (method == "divide_reference") {
                            wave.divide_reference();
                        } else if (method == "inverse") {
                            wave.inverse();
                        }
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid argument in method call: " << line << std::endl;
                    } catch (const std::out_of_range& e) {
                        std::cerr << "Argument out of range in method call: " << line << std::endl;
                    }
                }
            } 
            else if (line.find("if (wave.compare(") == 0 || line.find("while (wave.compare(") == 0) {
            size_t start = line.find("(") + 14;
            size_t end = line.find("))");
            std::string condition = line.substr(start, end - start);
            std::istringstream condition_stream(condition);
            std::string op, value;
            condition_stream >> op >> value;
            op = op.substr(1, op.length() - 2); // Remove quotes
            try {
                bool result = wave.compare(op, std::stod(value));
                condition_stack.push(condition_stack.top() && result);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument in comparison: " << line << std::endl;
                condition_stack.push(false);
            } catch (const std::out_of_range& e) {
                std::cerr << "Argument out of range in comparison: " << line << std::endl;
                condition_stack.push(false);
            }
        }
        } else if (line == "}") {
            if (!condition_stack.empty()) {
                condition_stack.pop();
            }
            if (condition_stack.empty()) {
                condition_stack.push(true);
            }
        } else if (line.find("} else {") == 0) {
            bool prev = condition_stack.top();
            condition_stack.pop();
            condition_stack.push(!prev);
        }
    }

    std::cout << "Final wave state:" << std::endl;
    wave.print_wave();

    return 0;
}
