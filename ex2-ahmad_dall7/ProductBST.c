//
// Created by ahdal_9lda2pd on 13/11/2022.
//
#include "ProductBST.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



#ifdef OH_TESTS

#include "oh_tests/oh_tests_helpers.h"

#define malloc test_malloc

#define calloc test_calloc

#define free test_free

#define stderr get_test_stderr()

#endif // OH_TESTS



int open_file(FILE** f, const char* path, const char* mode)
{// Opening the file and return 0 if it's invalid .
  if (!(*f = fopen(path, mode)))
  {
    fprintf(stderr, "%s%s", ERROR, FILE_OPEN_FAILED);
    return 0;
  }
  return 1;
}

Node* creating_new_node(char* name, int quantity)
{
  Node* new_node = malloc(sizeof(Node));
  int n = strlen(name);
  if (!new_node)
  {
    fprintf(stderr, "%s%s", ERROR, ALLOCATION_FAILED);
    return NULL;
  }
  new_node->product.name = malloc(sizeof(char) * (n + 1));
  if (!new_node->product.name)
  {
    fprintf(stderr, "%s%s", ERROR, ALLOCATION_FAILED);
    free(new_node);
    return NULL;
  }
  strcpy(new_node->product.name, name);
  new_node->product.name[n] = 0; //terminating char
  new_node->product.quantity = quantity;
  new_node->right_child = NULL;
  new_node->left_child = NULL;
  return new_node;
}

Node* add_product(Node* root, char* name, int quantity)
{
  if (!name)
  {
    fprintf(stderr, "%s%s", ERROR, INVALID_POINTER);
    return root;
  }
  if (quantity <= ZERO)
  {
    fprintf(stderr, "%s%s", ERROR, INVALID_QUANTITY);
    return root;
  }
  if (!root) // empty tree
  {
    return creating_new_node(name, quantity);
  }
  int direction = strcmp(name, root->product.name);
  if(direction < ZERO)
  {
    root->left_child = add_product(root->left_child, name, quantity);
  }
  else if(direction > ZERO)
  {
    root->right_child = add_product(root->right_child, name, quantity);
  }
  else
  {
    fprintf(stderr, "%s%s", ERROR, PRODUCT_EXISTS);
  }

  return root;
}

bool checking_quantity(char quantity[])
{
  int len = strlen(quantity);
  for (int i = 0; i < len; i++)
  {
    if (quantity[i] < MIN_NUMBER_POSSIBLE || quantity[i] > MAX_NUMBER_POSSIBLE)
    {
      return false;
    }
  }
  return true;
}

Node* build_bst(const char* filename)
{
  FILE* f = NULL;
  char line[MAX_LINE_LENGTH] = { 0 };
  char name[MAX_LINE_LENGTH] = { 0 };
  char quantity_chr[MAX_LINE_LENGTH] = { 0 };
  Node* node = NULL;

  if (!filename || !open_file(&f, filename, READ_MODE))
  {
    fprintf(stderr, "%s%s", ERROR, FILE_OPEN_FAILED);
    return NULL;
  }

  while (fgets(line, sizeof(line), f) != NULL)
  {
    char* temp;
    int quantity;

    if (!sscanf(line, "%[^:]: %s", name, quantity_chr))
    {
      return NULL;
    }
    if (!checking_quantity(quantity_chr))
    {
      fprintf(stderr, "%s%s", ERROR, INVALID_LINE);
      continue;
    }
    quantity = strtol(quantity_chr, &temp, BASE);
    node = add_product(node, name, quantity);
  }
  fclose (f);
  return node;
}

Node* find_inorder_successor(Node* node)
{
  while (node && node->left_child != NULL)
  {
    node = node->left_child;
  }
  return node;
}

Node* delete_product(Node* root, char* name)
{
  if (!root || !name)
  {
    fprintf(stderr, "%s%s", ERROR, PRODUCT_NOT_FOUND);
    return root;
  }
  int direction = strcmp(name, root->product.name);
  if(direction == ZERO)
  {
    if (root->left_child == NULL && root->right_child == NULL) // no children
    {
      delete_tree(root);
      root = NULL;
    }
    else if (root->left_child != NULL && root->right_child != NULL)
    {// there is 2 children , so we find the lowest one on the right side.
      Node* in_sc = find_inorder_successor(root->right_child);
      Product temp_product = { .name=in_sc->product.name,
                               .quantity =in_sc->product.quantity };
      in_sc->product.name = root->product.name;
      in_sc->product.quantity = root->product.quantity;
      root->product.name = temp_product.name;
      root->product.quantity = temp_product.quantity;
      root->right_child = delete_product(root->right_child,
                                         in_sc->product.name);
    }
    else
    { // have only one child
      Node* temp = root;
      if (root->left_child == NULL)
      {
        root = root->right_child;
      }
      else
      {
        root = root->left_child;
      }
      temp->left_child = NULL;
      temp->right_child = NULL;
      delete_tree(temp);
    }
  }
  else if(direction > ZERO)
  {
    root->right_child = delete_product(root->right_child, name);
  }
  else
  {
    root->left_child = delete_product(root->left_child, name);
  }
  return root;
}

Node * node_exist(Node * root, char * name)
{
  if (!root)
  {
    return NULL;
  }
  int direction = strcmp(name, root->product.name);
  if(direction < ZERO)
  {
    return node_exist(root->left_child, name);
  }
  if(direction > ZERO)
  {
    return node_exist(root->right_child, name);
  }
  return root;
}

Product* search_product(Node* root, char* name)
{
  if(!name)
  {
    fprintf(stderr, "%s%s", ERROR, INVALID_POINTER);
    return NULL;
  }
  Node * node = node_exist (root, name);
  return node ? &(node->product) : NULL;
}

void delete_tree(Node* root)
{
  // iterate over the bst nodes and free each node .
  if (!root)
  {
    return;
  }
  delete_tree(root->left_child);
  delete_tree(root->right_child);
  free(root->product.name);
  free(root);
}

Node* update_quantity(Node* root, char* name, int amount_to_update)
{
  Node * node;
  if (!root || !name)
  {
    fprintf(stderr, "%s%s", ERROR, INVALID_POINTER);
    return root;
  }

  if (!(node = node_exist(root, name)))
  {
    fprintf(stderr, "%s%s", ERROR, PRODUCT_NOT_FOUND);
    return root;
  }

  if (node->product.quantity + amount_to_update < ZERO)
  {
    fprintf(stderr, "%s%s", ERROR, INVALID_QUANTITY);
  }
  else
  {
    node->product.quantity += amount_to_update;
  }

  if (node->product.quantity == ZERO)
  {
    delete_product(root, name);
  }
  return root;
}
