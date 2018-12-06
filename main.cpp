#include <algorithm>
#include <fstream>

#include "genetic.h"
#include "tictactoe.h"

const int   FIXED_POPULATION_SIZE           = 20;
const int   CHILDREN_PER_GENERATION         = 20;
const int   GENERATIONS                     = 800;
const float MUTATION_PROBABILITY_INDIVIDUAL = 1.00;
const float MUTATION_PROBABILITY_GENE       = 0.01;
const float MUTATION_RANGE                  = 10.0f;
const int   INFO_EACH_GENERATION            = 32;
const int   QTY_RANDOM_GAMES_PER_FITNESS    = 1000;

int main() {

    // prepares to plot the data

    const char * PLOT_DATA_FILE = "gnuplot_data.dat";

    std::ofstream gnuplot_data(PLOT_DATA_FILE);

    FILE * gnuplot = popen("gnuplot -persistent", "w"); // FIXME check for errors
    fprintf(gnuplot, "set title \"Generation X Fitness\" \n");
    fprintf(gnuplot, "set style circle radius 0.5 \n");
    fprintf(gnuplot, "set style fill transparent solid 0.10 noborder \n");
    fprintf(gnuplot, "plot '%s' with circles \n", PLOT_DATA_FILE);

    // creates initial population

    int curr_individual = 0;
    Population population;
    for (int i = 0; i < FIXED_POPULATION_SIZE; i++) {
        population.emplace_back(Indiv(curr_individual++));
    }
    calc_fitness_parallel(population, QTY_RANDOM_GAMES_PER_FITNESS);
    std::sort(population.begin(), population.end());
    print_info(population, 0);

    // genetic loop

    Indiv best = population[0];
    for (int gen = 1; gen <= GENERATIONS; gen++) {
        generate_new_individuals(population, CHILDREN_PER_GENERATION, curr_individual);
        mutate_some(population, MUTATION_PROBABILITY_INDIVIDUAL, MUTATION_PROBABILITY_GENE, MUTATION_RANGE);
        calc_fitness_parallel(population, QTY_RANDOM_GAMES_PER_FITNESS);
        selection(population, FIXED_POPULATION_SIZE);

        //if (population[0].fitness > best.fitness) {
            best = population[0];
        //}

        // updates plot

        for (auto const & indiv: population) {
            gnuplot_data << gen << " " << indiv.fitness << std::endl;
        }

        if (gen <= INFO_EACH_GENERATION or gen % INFO_EACH_GENERATION == 0 or gen == GENERATIONS) {
            print_info(population, gen);
            play(&population[0], nullptr, true);

            fprintf(gnuplot, "replot \n");
            fflush(gnuplot);
        }

    }

    // save the nn, fitness and id of the best

    Indiv previous_best("best.txt");
    if (best.fitness > previous_best.fitness) {
        best.save_to_file("best.txt");
    }

    // close gnuplot (the plot will still be open, though)

    gnuplot_data.close();
    fprintf(gnuplot, "quit \n");
    pclose(gnuplot);
}
