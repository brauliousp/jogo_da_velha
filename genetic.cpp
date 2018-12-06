#include <cstdio>
#include <numeric>
#include <algorithm>
#include <thread>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>

#include "genetic.h"
#include "tictactoe.h"
#include "neural_net.h"

using namespace std;

bool try_move(Indiv const * indiv, TicTacToe & game, int & error, bool print) {
    if (indiv != nullptr) {
        float in[9], out[9];

        for (int i = 0; i < 9; i++) {
            in[i] = (float) game.get_board_as_array()[i];
        }
        
                                                                        if (print) {
                                                                            printf("input: ");
                                                                            for (float x : in) {
                                                                                printf("%.1f ", x);
                                                                            }
                                                                            putchar('\n');
                                                                        }

        indiv->nn.evaluate(in, out);

                                                                        if (print) {
                                                                            printf("normalized output: ");
                                                                            float avg = std::accumulate(out, out + 9, 0.0f) / 9;
                                                                            for (float x : out) {
                                                                                printf("%.1f ", x / avg);
                                                                            }
                                                                            putchar('\n');
                                                                        }

        auto max_elem = std::max_element(out, out + 9);
        auto move = max_elem - out;
        auto n_moves = std::count(out, out + 9, *max_elem);

        if (n_moves > 1) {
                                                                        if (print) {
                                                                            printf("Too many moves: %lu.\n", n_moves);
                                                                        }
            error = RESULT_TOO_MANY_MOVES + (9 - n_moves);
            return false;
        }

        if (game.move(move, game.next_player())) {
            return true;
        } else {
                                                                        if (print) {
                                                                            printf("Invalid move: %lu\n", move);
                                                                        }
            error = RESULT_INVALID_MOVE;
            return false;
        }
    } else {
        int move;
        do {
            move = rand() % 9;
        } while (not game.move(move, game.next_player()));
        return move;
    }
}

float play(Indiv const * indiv_a, Indiv const * indiv_b, bool print) {
    TicTacToe game;

                                                                        if (print) {
                                                                            printf("ind_a: \n");
                                                                            if (indiv_a != nullptr) {
                                                                                indiv_a->nn.print();
                                                                            } else {
                                                                                puts("random\n");
                                                                            }

                                                                            printf("ind_b: \n");
                                                                            if (indiv_b != nullptr) {
                                                                                indiv_b->nn.print();
                                                                            } else {
                                                                                puts("random\n");
                                                                            }
                                                                        }
    Indiv const * indiv[] = {indiv_a, indiv_b};
    size_t curr = 0;
    while (game.status() == TTT_GAME_RUNNING) {

                                                                        if (print) {
                                                                            game.print();
                                                                        }

        int error = 0;
        if (! try_move(indiv[curr], game, error, print)) {
            return error + game.moves_made();
        }

        curr = (curr + 1) % 2;
    }

                                                                        if (print) {
                                                                            game.print();
                                                                        }

    switch (game.status()) {
        case TTT_TIE:
            return RESULT_TIE;
        case TTT_PLAYER_X_WON:
            return RESULT_VICTORY;
        case TTT_PLAYER_O_WON:
            return RESULT_LOST;
        default:
            return RESULT_TIE;
    }
}

float calc_fitness(Indiv const & curr, int games_per_calc, Population const & pop) {

    float fitness = 0.0;

    for (int i = 0; i < games_per_calc; i++) {
        fitness +=
            (
                    play(&curr, &pop[i % pop.size()], false)
                    - play(&pop[i % pop.size()], &curr, false)
                    + play(&curr, nullptr, false)
            ) / 3;
    }

    return fitness / (games_per_calc * RESULT_VICTORY);
}

void calc_fitness_thread(Population::iterator begin, Population::iterator end, int games_per_calc, Population const & pop) {

    for (auto ind = begin; ind < end; ind++) {
        ind->fitness = calc_fitness(*ind, games_per_calc, pop);
    }
}

void calc_fitness_parallel(Population & pop, int games_per_calc) {

    std::vector<std::thread> threads;

    auto n_threads = std::max(std::thread::hardware_concurrency(), 4u);
    auto items_per_thread = pop.size() / n_threads;
    auto remainder = pop.size() % n_threads;
    size_t start = 0;

    for (size_t i = 0; i < n_threads; i++) {
        auto size = items_per_thread;
        if (remainder > 0) {
            size++;
            remainder--;
        }
        threads.emplace_back(std::thread(
                calc_fitness_thread,
                pop.begin() + start,
                std::min(pop.begin() + start + size, pop.end()),
                games_per_calc, pop));
        start = start + size;
    }

    for (auto & thr : threads) {
        thr.join();
    }

}

void low_level_crossover(void const * input_a, void const * input_b,
        void * output, size_t gene_size, size_t n_genes) {
    for (size_t i = 0; i < n_genes; i++) {
        if (rand() % 2 == 0) {
            memcpy((uint8_t*)output + i * gene_size, (uint8_t*)input_a + i * gene_size, gene_size);
        } else {
            memcpy((uint8_t*)output + i * gene_size, (uint8_t*)input_b + i * gene_size, gene_size);
        }
    }
}

Indiv crossover(Indiv const & ind_a, Indiv const & ind_b) {
    Indiv child;

    low_level_crossover((void *)&ind_a.nn, (void*) &ind_b.nn, (void*) &child.nn,
            sizeof(float), sizeof(NN) / sizeof(float));

    return child;
}

void generate_new_individuals(Population & pop, size_t how_many, int &next_indiv_id) {

    auto const INITIAL_SIZE = pop.size();

    for (size_t i = 0; i < how_many; i++) {
        Indiv child = crossover(pop[0], pop[i % (INITIAL_SIZE - 1) + 1]);
        child.id = next_indiv_id++;
        pop.emplace_back(child);
    }

}


void mutation(Indiv & ind, float gene_mutation_prob, float mutation_range) {
    auto n_genes = sizeof(NN) / sizeof(float);
    for (size_t i = 0; i < n_genes; i++) {
        if ((rand() % 1000) / 1000.0 <= gene_mutation_prob) {
            *(reinterpret_cast<float*>(&ind) + i) += (rand() % 1000 / 1000.0f * mutation_range - (mutation_range/2));
        }
    }
}

void mutate_some(Population & pop, float indiv_mutation_prob, float gene_mutation_prob, float mutation_range) {
    for (size_t i = 1; i < pop.size(); i++) { // keep the best unmutated
        Indiv & ind = pop[i];
        if ((rand() % 1000) / 1000.0 <= indiv_mutation_prob) {
            mutation(ind, gene_mutation_prob, mutation_range);
        }
    }
}

void selection(Population & pop, int to_size) {
    std::sort(pop.begin(), pop.end());
    pop.resize(to_size);
}

void print_info(Population const & pop, int generation) {
    float avg_fit = 0.0;
    for (auto const & ind: pop) {
        avg_fit += ind.fitness;
    }
    avg_fit = avg_fit / pop.size();

    printf("Gen %d. Avg Fit %.3f \n", generation, avg_fit);
    printf("Best 8:");
    for (int i = 0; i < 8; i++) {
        printf(" (%d) %.3lf", pop[i].id, pop[i].fitness);
    }
    putchar('\n');
}

Indiv::Indiv(char const * filename) {
    ifstream f;
    f.open(filename);

    if (f.fail()) {
        id = -1;
        fitness = - std::numeric_limits<float>::infinity();
        return;
    }

    f >> id;
    f >> fitness;

    for (auto & layer: nn.layer) {
        for (auto & neuron: layer.neuron) {
            for (auto & w: neuron.w) {
                f >> w;
            }
            f >> neuron.b;
        }
    }

    f.close();
}

void Indiv::save_to_file(char const * filename) const {

    std::ofstream f;
    f.open(filename);

    f << id << '\n';
    f << fitness << '\n';

    for (auto & layer: nn.layer) {
        for (auto & neuron: layer.neuron) {
            for (auto & w: neuron.w) {
                f << w << ' ';
            }
            f << '\n';
            f << neuron.b << '\n';
        }
    }

    f.close();
}
