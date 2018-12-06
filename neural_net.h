#ifndef TIC_TAC_TOE_NEURAL_NET_H
#define TIC_TAC_TOE_NEURAL_NET_H

#define NUM_LAYERS 3

struct Neuron {

    /** weights */
    float w[9];

    /** bias */
    float b;

    /** Creates a neuron with random bias and output weights. */
    Neuron();

};

struct Layer {
    Neuron neuron[9];
};

struct NN {
    Layer layer[NUM_LAYERS];
    void evaluate(const float input[9], float output[9]) const;
    void print() const;
};

#endif //TIC_TAC_TOE_NEURAL_NET_H
