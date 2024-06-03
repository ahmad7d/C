#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define ARGS_NUM 3
#define INV_ARGS "Usage: Args number should be 2 ! \n"
#define BASE_10 10
#define SEED_ARG 1
#define PATHS_AMOUNT_ARG 2

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in
    // case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case
    // there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
  printf("%s", error_msg);
  if (database != NULL)
  {
    free_markov_chain(database);
  }
  return EXIT_FAILURE;
}

static bool check_last(void * data)
{
  Cell * cell = (Cell *) data;
  if (cell->number == BOARD_SIZE)
  {
    return true;
  }
  return false;
}

static void printing_cell(Cell * cell, bool is_lsat , bool is_ladder_to, bool
is_snake_to)
{
  if (is_lsat)
  {
    printf ("[%d]", cell->number);
  }
  else if (is_ladder_to)
  {
    printf ("[%d]-ladder to %d ->", cell->number, cell->ladder_to);
  }
  else if (is_snake_to)
  {
    printf ("[%d]-snake to %d ->", cell->number, cell->snake_to);
  }
    // normal cell (with arrow)
  else
  {
    printf ("[%d] ->", cell->number);
  }
}

static void printing_walks(void * data)
{
  Cell * cell = (Cell *)data;

  // last cell without arrow :
  if (check_last (cell))
  {
    printing_cell (cell, true, false, false);
  }
    // if it's not snake to && not ladder to , then it's normal cell
  else if (cell->ladder_to == EMPTY && cell->snake_to == EMPTY &&
           cell->number !=BOARD_SIZE)
  {
    printing_cell (cell, false, false, false);
  }
  else if (cell->ladder_to != EMPTY)
  {
    printing_cell (cell, false, true, false);
  }
  else if(cell->snake_to != EMPTY)
  {
    printing_cell (cell, false, false, true);
  }
}

static int comp_two_elements(void * first_data, void * second_data)
{
  Cell * first_cell = (Cell *)first_data;
  Cell * second_cell = (Cell *)second_data;
  return first_cell->number - second_cell->number;
}

static void free_snake(void * data)
{
  free(data);
  data = NULL;
}

static void * copy(void * data)
{
  Cell * new_cell = malloc (sizeof(Cell));
  if(!new_cell)
  {
    fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  Cell * src = (Cell *)data;
  new_cell->number = src->number;
  new_cell->ladder_to = src->ladder_to;
  new_cell->snake_to = src->snake_to;
  return new_cell;
}

static int create_board(Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    cells[i] = malloc(sizeof(Cell));
    if (cells[i] == NULL)
    {
      for (int j = 0; j < i; j++) {
        free(cells[j]);
      }
      handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
      return EXIT_FAILURE;
    }
    *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
  {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to)
    {
      cells[from - 1]->ladder_to = to;
    }
    else
    {
      cells[from - 1]->snake_to = to;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
  Cell* cells[BOARD_SIZE];
  if(create_board(cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database(markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database(markov_chain,cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
      to_node = get_node_from_database(markov_chain, cells[index_to])
          ->data;
      add_node_to_counter_list(from_node, to_node,
                               markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell*) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database(markov_chain, cells[index_to])
            ->data;
        add_node_to_counter_list(from_node, to_node,
                                 markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free(cells[i]);
  }
  return EXIT_SUCCESS;
}

static int convert_to_int (char *inp)
{
  return strtol (inp, NULL, BASE_10);
}

static void create_paths(MarkovChain * markov_chain, int max_rounds)
{
  for (int round=1 ; round<=max_rounds ; ++round)
  {
    printf ("Random Walk %d: ", round);
    generate_random_sequence (markov_chain,
                              markov_chain->database->first->data,
                              MAX_GENERATION_LENGTH);
  }
//  while (round < max_rounds)
//  {
//    printf ("Random Walk %d: ", round);
//    generate_random_sequence (markov_chain,
//                              markov_chain->database->first->data,
//                              MAX_GENERATION_LENGTH);
//  round++;
//  }
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */

void filling_snakes_func(MarkovChain * markov_chain)
{
  markov_chain->print_func = printing_walks;
  markov_chain->is_last = check_last;
  markov_chain->comp_func = comp_two_elements;
  markov_chain->copy_func = copy;
  markov_chain->free_data = (Free_data) free_snake;
}

int main(int argc, char *argv[])
{
  if (argc != ARGS_NUM)
  {
    fprintf (stderr, INV_ARGS);
    return EXIT_FAILURE;
  }
  int seed = convert_to_int (argv[SEED_ARG]);
  int rounds = convert_to_int (argv[PATHS_AMOUNT_ARG]);
  srand (seed);
  MarkovChain * markov_chain = malloc (sizeof(MarkovChain));
  if(!markov_chain)
  {
    fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  markov_chain->database = calloc (1,
                                   sizeof(LinkedList));
  if(!markov_chain->database)
  {
    fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
    free_markov_chain (&markov_chain);
    return EXIT_FAILURE;
  }
  filling_snakes_func(markov_chain);
  fill_database (markov_chain);
  create_paths (markov_chain, rounds);
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}