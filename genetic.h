#ifndef TIC_TAC_TOE_GENETIC_H
#define TIC_TAC_TOE_GENETIC_H

#include <vector>
#include "neural_net.h"

enum Result {
    RESULT_VICTORY = 20,
    RESULT_TIE = 10,
    RESULT_INVALID_OPPONENT_MOVE = 0,
    RESULT_LOST = -1,
    RESULT_INVALID_MOVE = -10,
    RESULT_TOO_MANY_MOVES = -20
};

struct Indiv {
    NN nn;
    int id;
    float fitness;

    Indiv() : id(0), fitness(0.0) {}
    Indiv(int id) : id(id), fitness(0.0) {}
    Indiv(char const * filename);

    bool operator<(Indiv const & that) const {
        if (this->fitness != that.fitness)
            return this->fitness > that.fitness;
        else
            return this->id < that.id;
    }

    void save_to_file(char const * filename) const;
};

typedef std::vector<Indiv> Population;

void calc_fitness_parallel(Population & pop, int games_per_calc);
void generate_new_individuals(Population & pop, size_t how_many, int & next_indiv_id) ;
void mutate_some(Population & pop, float indiv_mutation_prob, float gene_mutation_prob, float mutation_range);
void selection(Population & pop, int to_size);
void print_info(Population const & pop, int generation);
float play(Indiv const * indiv_a, Indiv const * indiv_b, bool print);

#endif //TIC_TAC_TOE_GENETIC_H
