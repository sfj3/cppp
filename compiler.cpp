#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <iomanip>
#include <random>
#include <chrono>
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

int main() {
    WaveGrub wg;
    std::string input;

    std::cout << "Welcome to c+++ Interactive Interpreter!" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  A/a (increase/decrease amplitude)" << std::endl;
    std::cout << "  F/f (increase/decrease frequency)" << std::endl;
    std::cout << "  P/p (increase/decrease phase)" << std::endl;
    std::cout << "  * (multiply with reference wave)" << std::endl;
    std::cout << "  + (add reference wave)" << std::endl;
    std::cout << "  - (subtract reference wave)" << std::endl;
    std::cout << "  / (divide by reference wave)" << std::endl;
    std::cout << "  I (inverse wave)" << std::endl;
    std::cout << "  = (print waves)" << std::endl;
    std::cout << "  R (reset wave to initial state)" << std::endl;
    std::cout << "  N (generate a new random wave)" << std::endl;
    std::cout << "Enter commands (or 'quit' to exit):" << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "quit") {
            break;
        }

        wg.interpret(input);
    }

    std::cout << "Thank you for using c+++!" << std::endl;
    return 0;
}
