#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "markov_chain.h"
#include "linked_list.h"

#define FIRST_ARG 1
#define SECOND_ARG 2
#define THIRD_ARG 3
#define FOURTH_ARG 4
#define MAX_ARGS_NUM 5
#define MIN_ARGS_NUM 4
#define BASE_10 10
#define MAX_LENGTH 20
#define BUFFER_LENGTH 1000
#define WORD_LENGTH 100
#define READ_MODE "r"
#define ARGS_NUM_ERR "Usage: Args number should be 3 or 4 ! \n"
#define PATH_ERR "Error: File path is unreachable ! \n"
#define DOT '.'

static void print_func(void * data)
{
  printf("%s", (char *)data);
}

static int comp_func(void * first , void * second)
{
  return strcmp ((char *)first, (char *)second);
}

static void * copy_func(void * data)
{
  char * new_data = malloc (sizeof (char) * strlen ((char*) data) + 1);
  if (!new_data)
  {
    fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  strcpy (new_data, (char *)data);
  return new_data;
}

static bool is_last(void * data)
{
  int len = (int)strlen ((char *) data);
  char * new_data = (char *)data;
  for (int i=0 ; i<=len ; ++i)
  {
    if (new_data[i] == DOT && !new_data[i + 1])
    {
      return true;
    }
  }
  return false;
}

static void free_data(void * data)
{
  char *temp = (char *) data;
  free (temp);
  data = NULL;
}

int open_file (FILE **f, const char *path, const char *mode)
{// Opening the file and return 0 if it's invalid .
  *f = fopen (path, mode);
  if (*f == NULL)
  {
    return 0;
  }
  return 1;
}

void close_file (FILE **f)
{// closing file function and make it equal to NULL .
  if (!*f)
  {
    return;
  }
  fclose (*f);
  *f = NULL;
}

void fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char data[BUFFER_LENGTH], *data_ptr = data;
  while (fgets (data, BUFFER_LENGTH, fp) != NULL
         && words_to_read != 0)
  {
    char word[WORD_LENGTH];
    Node *before_node = NULL;
    data_ptr = data;
    while (sscanf(data_ptr,"%s ",word) == 1 && words_to_read != 0)
    {
      Node *node = add_to_database (markov_chain, word);
      if (!node)
      {
        markov_chain->error = true;
      }
      else if (before_node)
      {
        add_node_to_counter_list (before_node->data,
                                  node->data, markov_chain);
      }
      before_node = node;
      data_ptr += strlen (word);
      while (*data_ptr == ' ')
      {
        data_ptr++;
      }
      words_to_read--;
    }
  }
}

int convert_to_int (char *inp)
{
  return strtol (inp, NULL, BASE_10);
}

static void print_tweets (MarkovChain *markov_chain, int num_tweets)
{
  for (int i = 0; i < num_tweets; ++i)
  {
    printf ("Tweet %d: ", i + 1);
    generate_random_sequence (markov_chain, get_first_random_node
        (markov_chain), MAX_LENGTH);
  }
}


MarkovChain * create_markov(MarkovChain * words)
{
  words = malloc (sizeof (MarkovChain));
  if (!words)
  {
    return NULL;
  }
  else
  {
    words->database = malloc (sizeof (LinkedList));
    if (!words->database)
    {
      free(words);
      words = NULL;
      return NULL;
    }
    else
    {
      words->database->first = NULL;
      words->database->last = NULL;
      words->database->size = 0;
      words->error = false;
    }
  }
  return words;
}

void filling_tweets_func(MarkovChain * markov_chain)
{
  markov_chain->comp_func = comp_func;
  markov_chain->copy_func = copy_func;
  markov_chain->is_last = is_last;
  markov_chain->free_data = free_data;
  markov_chain->print_func = print_func;
}


int main (int argc, char *argv[])
{
  FILE *txt_file;
  int seed;
  int tweets, words_to_read;
  MarkovChain *words = NULL;
  if (argc > MAX_ARGS_NUM || argc < MIN_ARGS_NUM)
  {
    fprintf (stderr, ARGS_NUM_ERR);
    return EXIT_FAILURE;
  }
  else
  {
    seed = convert_to_int (argv[FIRST_ARG]);
    tweets = convert_to_int (argv[SECOND_ARG]);
    words_to_read = argc == MAX_ARGS_NUM ?
                    convert_to_int (argv[FOURTH_ARG]) : -1;
  }
  if (!open_file (&txt_file, argv[THIRD_ARG], READ_MODE))
  {
    fprintf (stderr, PATH_ERR);
    close_file (&txt_file);
    return EXIT_FAILURE;
  }
  words = create_markov (words);
  if (!words)
  {
    fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  else
  {
    srand (seed);
    filling_tweets_func(words);
    fill_database (txt_file, words_to_read, words);
    close_file (&txt_file);
    if (words->error)
    {
      fprintf (stderr, ALLOCATION_ERROR_MASSAGE);
      return EXIT_FAILURE;
    }
    else
    {
      print_tweets (words, tweets);
    }
    free_markov_chain (&words);
  }
  return EXIT_SUCCESS;
}