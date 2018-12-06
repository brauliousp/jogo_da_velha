#include "neural_net.h"

#include <cstdlib>
#include <vector>
#include <cmath>
#include <cstdio>

using std::vector;

Neuron::Neuron() {

    for (auto &i : this->w) {
        i = (float) (rand()%1000)/1000.0f - 0.5f;
    }

    this->b = (rand()%1000)/1000.0f - 0.5f;
}

void NN::evaluate(const float input[9], float output[9]) const {

    float x[NUM_LAYERS][9];

    for (size_t i = 0; i < 9; ++i) {
        x[0][i] = input[i];
    }

    for (int lay = 1; lay < NUM_LAYERS - 1; ++lay) {
        for (int neu = 0; neu < 9; ++neu) {

            x[lay][neu] = layer[lay].neuron[neu].b;

            for (int prev_neuron = 0; prev_neuron < 9; ++prev_neuron) {
                x[lay][neu] += layer[lay-1].neuron[prev_neuron].w[neu] * x[lay-1][prev_neuron];
            }

            // step function
            // x[lay][neu] = x[lay][neu] > 0.0f ? 1.0f : 0.0f;

            // sigmoid
            // x[lay][neu] = std::tanh(x[lay][neu]);

            // logistic
            x[lay][neu] = 0.5f + 0.5f * std::tanh(x[lay][neu] * 10);

            // linear
            // x[lay][neu] = x[lay][neu];
        }
    }

    const auto LAST_LAY = NUM_LAYERS - 1;
    for (int neu = 0; neu < 9; ++neu) {

        x[LAST_LAY][neu] = layer[LAST_LAY].neuron[neu].b;

        for (int prev_neuron = 0; prev_neuron < 9; ++prev_neuron) {
            x[LAST_LAY][neu] += layer[LAST_LAY - 1].neuron[prev_neuron].w[neu] * x[LAST_LAY-1][prev_neuron];
        }
    }

    for (size_t i = 0; i < 9; ++i) {
        output[i] = x[NUM_LAYERS-1][i];
    }
}

void NN::print() const {
    for (auto const & lay: layer) {
        printf("    layer\n");
        for (auto const & neuron: lay.neuron) {
            printf("        neuron (bias = %.2f) weights: ", neuron.b);
            for (auto const & w: neuron.w) {
                printf("%.2f ", w);
            }
            putchar('\n');
        }
    }
}
