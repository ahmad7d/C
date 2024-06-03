#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "markov_chain.h"

#define BUFFER_LENGTH 1000

static void print_node (MarkovNode *node, bool last_word, MarkovChain *
markov_chain)
{
  if (last_word)
  {
    markov_chain->print_func(node->data);
    printf ("\n");
  }
  else
  {
    markov_chain->print_func(node->data);
    printf(" ");
  }
}
static void free_markov_node (MarkovNode *node, MarkovChain ** ptr_chain)
{
  free(node->counter_list);
  (*ptr_chain)->free_data(node->data);
  free(node);
}
static void free_node (Node *node, MarkovChain ** ptr_chain)
{
  if (node == NULL)
  {
    return;
  }
  free_markov_node (node->data, ptr_chain);
  free(node);
}

static void free_list (LinkedList *lst, MarkovChain ** ptr_chain)
{
  while (lst->first != lst->last && lst->first != NULL)
  {
    Node *node = lst->first;
    lst->first = node->next;
    free_node (node, ptr_chain);
  }
  free_node (lst->first, ptr_chain);
  free(lst);
}

void free_markov_chain (MarkovChain **ptr_chain)
{
  free_list ((*ptr_chain)->database, ptr_chain);
  free(*ptr_chain);
  *ptr_chain = NULL;
}

static void create_node (MarkovNode **node, void *data_ptr,
                         MarkovChain * markov_chain)
{
  *node = malloc (sizeof (MarkovNode));
  if (!*node)
  {
    return;
  }
  (*node)->counter_list = NULL;
  (*node)->len = 0;
  (*node)->freq = 1;
  void * my_copy = markov_chain->copy_func(data_ptr);
  (*node)->data = my_copy;
}

static int get_random_number(int max_number)
{
  return rand() % max_number;
}

static MarkovNode *get_node_at (MarkovChain *markov_chain, int i)
{
  Node *node = markov_chain->database->first;
  for (int j = 0; j < i; ++j)
  {
    node = node->next;
  }
  return node ? node->data : NULL;
}

//static bool is_last_word (MarkovNode *node)
//{
//  if (node == NULL)
//  {
//    return false;
//  }
//  for (int i = 0; node->data[i]; i++)
//  {
//    if (node->data[i] == '.' && !node->data[i + 1])
//    {
//      return true;
//    }
//  }
//  return false;
//}

static int all_next_words_count (MarkovNode *state_struct_ptr)
{
  int c = 0;
  for (int i = 0; i < state_struct_ptr->len; ++i)
  {
    c += state_struct_ptr->counter_list[i].frequency;
  }

  return c;
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  MarkovNode *node;
  do
  {
    node = get_node_at (markov_chain, get_random_number (
        markov_chain->database->size));
  }
  while (node && markov_chain->is_last(node->data));
  return node;
}

MarkovNode * pick_randomly(MarkovNode * my_node, int r)
{
  for (int i = 0; i < my_node->len; ++i)
  {
    r -= my_node->counter_list[i].frequency;
    if(r < 0)
    {
      return my_node->counter_list[i].node_markov;
    }
  }
  return my_node->counter_list[my_node->len].node_markov;


}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  int r = get_random_number (all_next_words_count (state_struct_ptr));
  MarkovNode * my_node = pick_randomly (state_struct_ptr, r);
  return my_node;
}

void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  MarkovNode *node;
  if (!first_node)
  {
    first_node = get_first_random_node (markov_chain);
    if (!first_node)
    {
      return;
    }
  }
  node = first_node;
  bool is_last = markov_chain->is_last(node->data) || max_length==0;
  print_node (node, is_last, markov_chain);
  do
  {
    node = get_next_random_node (node);
    is_last = markov_chain->is_last(node->data) || --max_length==1;
    print_node (node, is_last, markov_chain);
    if (is_last)
    {
      return;
    }
  }
  while (max_length > 1);
}


bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain * markov_chain)
{
  for (int i = 0; i < first_node->len; ++i)
  {
    NextNodeCounter * next_node = &first_node->counter_list[i];
    if(markov_chain->comp_func(second_node->data,
                               next_node->node_markov->data) ==0)
    {
      next_node->frequency++;
      return true;
    }
  }
  first_node->counter_list = realloc
      (first_node->counter_list,
       (first_node->len + 1) * sizeof (NextNodeCounter));
  if (first_node->counter_list == NULL)
  {
    return false;
  }
  NextNodeCounter *new_node = &first_node->counter_list[first_node->len];
  new_node->frequency = 1;
  new_node->node_markov = second_node;
  first_node->len++;
  return true;
}

Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *node = markov_chain->database->first;
  while (node != markov_chain->database->last && node != NULL)
  {
    if (markov_chain->comp_func(data_ptr, node->data->data) == 0)
    {
      return node;
    }
    node = node->next;
  }
  return node && markov_chain->comp_func(data_ptr, node->data->data) == 0
         ? node : NULL;
}

Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *node = get_node_from_database (markov_chain, data_ptr);
  if (!node)
  {
    MarkovNode *markov_node;
    create_node (&markov_node, data_ptr, markov_chain);
    if (!markov_node)
    {
      return NULL;
    }
    add (markov_chain->database, markov_node);
    node = get_node_from_database (markov_chain, data_ptr);
  }
  return node;
}