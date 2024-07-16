#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 256

typedef struct {
    double t[SIZE];
    double wave[SIZE];
    double ref_wave[SIZE];
    double amp, freq, phase;
} CPlusPlusPlus;

void init(CPlusPlusPlus *wg) {
    wg->amp = 1.0;
    wg->freq = 1.0;
    wg->phase = 0.0;
    unsigned seed = (unsigned) time(NULL);
    srand(seed);
    for (int i = 0; i < SIZE; i++) {
        wg->t[i] = 2 * M_PI * i / SIZE;
        wg->ref_wave[i] = sin(wg->t[i]);
        wg->wave[i] = wg->ref_wave[i];  // Initialize wave to match ref_wave
    }
}

void update_wave(CPlusPlusPlus *wg) {
    for (int i = 0; i < SIZE; i++) {
        wg->wave[i] = wg->amp * sin(wg->freq * wg->t[i] + wg->phase);
    }
}

void random_wave(CPlusPlusPlus *wg) {
    wg->amp = 0.1 + (double) rand() / RAND_MAX * 1.9;
    wg->freq = 0.5 + (double) rand() / RAND_MAX * 9.5;
    wg->phase = (double) rand() / RAND_MAX * 2 * M_PI;
    update_wave(wg);
    printf("Generated random wave with:\nAmp = %f, Freq = %f, Phase = %f\n", wg->amp, wg->freq, wg->phase);
}

double constrain(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void inverse_wave(CPlusPlusPlus *wg) {
    double e = exp(1.0);  // Euler's number
    for (int i = 0; i < SIZE; i++) {
        if (fabs(wg->wave[i]) < 1.0/e) {
            // For small absolute values, invert and constrain
            wg->wave[i] = copysign(e, wg->wave[i]);
        } else if (fabs(wg->wave[i]) > e) {
            // For large absolute values, invert and constrain
            wg->wave[i] = copysign(1.0/e, wg->wave[i]);
        } else {
            // For values between -e and -1/e, and between 1/e and e, safely invert
            wg->wave[i] = 1.0 / wg->wave[i];
        }
    }
}

void reset_wave(CPlusPlusPlus *wg) {
    wg->amp = 1.0;
    wg->freq = 1.0;
    wg->phase = 0.0;
    update_wave(wg);
}

void print_waves(const CPlusPlusPlus *wg) {
    printf("Current wave parameters: Amp = %f, Freq = %f, Phase = %f\n", wg->amp, wg->freq, wg->phase);
    printf("Wave:    ");
    for (int i = 0; i < SIZE; i += SIZE / 8) {
        if (isnan(wg->wave[i]) || isinf(wg->wave[i])) {
            printf("ERR ");
        } else {
            printf("%.2f ", wg->wave[i]);
        }
    }
    printf("\nRef Wave:");
    for (int i = 0; i < SIZE; i += SIZE / 8) {
        printf("%.2f ", wg->ref_wave[i]);
    }
    printf("\n");
}

void compare_waves(const CPlusPlusPlus *wg) {
    int result = 1;
    for (int i = 0; i < SIZE; i++) {
        if (fabs(wg->wave[i] - wg->ref_wave[i]) > 1e-10) {
            result = 0;
            break;
        }
    }
    printf(result ? "true\n" : "false\n");
}

void interpret(CPlusPlusPlus *wg, const char *code) {
    int length = strlen(code);
    int update_needed = 0;
    for (int i = 0; i < length; i++) {
        char cmd = code[i];
        switch (cmd) {
            case 'A': wg->amp = fmin(wg->amp + 0.1, 2.0); update_needed = 1; break;
            case 'a': wg->amp = fmax(wg->amp - 0.1, 0.1); update_needed = 1; break;
            case 'F': wg->freq = fmin(wg->freq + 0.5, 10.0); update_needed = 1; break;
            case 'f': wg->freq = fmax(wg->freq - 0.5, 0.5); update_needed = 1; break;
            case 'P': wg->phase = fmod(wg->phase + 0.2, 2 * M_PI); update_needed = 1; break;
            case 'p': wg->phase = fmod(wg->phase - 0.2 + 2 * M_PI, 2 * M_PI); update_needed = 1; break;
            case '*':
                for (int j = 0; j < SIZE; j++) wg->wave[j] *= wg->ref_wave[j];
                break;
            case '+':
                for (int j = 0; j < SIZE; j++) wg->wave[j] += wg->ref_wave[j];
                break;
            case '-':
                for (int j = 0; j < SIZE; j++) wg->wave[j] -= wg->ref_wave[j];
                break;
            case '/':
                for (int j = 0; j < SIZE; j++) {
                    if (wg->ref_wave[j] != 0) wg->wave[j] /= wg->ref_wave[j];
                    else wg->wave[j] = 0; // Avoid division by zero
                }
                break;
            case 'R':
                reset_wave(wg);
                update_needed = 0; // reset_wave already updates the wave
                break;
            case 'N':
                random_wave(wg);
                update_needed = 0; // random_wave already updates the wave
                break;
            case 'x':
                compare_waves(wg);
                break;
            case 'I':
                inverse_wave(wg);
                break;
            case '=':
                print_waves(wg);
                break;
        }
    }
    if (update_needed) {
        update_wave(wg);
    }
}

int main() {
    CPlusPlusPlus wg;
    char input[256];

    init(&wg);
    printf("Welcome to c+++ Interactive Interpreter!\n");
    printf("Commands:\n");
    printf("  A/a (increase/decrease amplitude)\n");
    printf("  F/f (increase/decrease frequency)\n");
    printf("  P/p (increase/decrease phase)\n");
    printf("  * (multiply with reference wave)\n");
    printf("  + (add reference wave)\n");
    printf("  - (subtract reference wave)\n");
    printf("  / (divide by reference wave)\n");
    printf("  I (inverse wave)\n");
    printf("  = (print waves)\n");
    printf("  R (reset wave to initial state)\n");
    printf("  N (generate a new random wave)\n");
    printf("  x (compare wave with reference wave)\n");
    printf("Enter commands (or 'quit' to exit):\n");

    while (1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);
        if (strncmp(input, "quit", 4) == 0) {
            break;
        }
        interpret(&wg, input);
    }

    printf("Thank you for using c+++!\n");
    return 0;
}
