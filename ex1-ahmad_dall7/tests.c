#include "tests.h"
#include "string.h"

#define K_2 2
#define K_3 3
#define K_29 29
#define MINUS_K_1 -1
#define MINUS_K_3 -3

// See full documentation in header file
int test_encode_non_cyclic_lower_case_positive_k ()
{
  char in[] = "abcdefghijklmnopqrstuvw";
  char out[] = "defghijklmnopqrstuvwxyz";
  encode (in, K_3);
  return strcmp (in, out) != 0;
}

// See full documentation in header file
int test_encode_cyclic_lower_case_special_char_positive_k ()
{
  char in[] = "a b%c67@#$%^&BPc9 ";
  char out[] = "c d%e67@#$%^&DRe9 ";
  encode (in, K_2);
  return strcmp (in, out) != 0;
}

// See full documentation in header file
int test_encode_non_cyclic_lower_case_special_char_negative_k ()
{
  char in[] = "Z b%c67@#$%^&BPc9 ";
  char out[] = "Y a%b67@#$%^&AOb9 ";
  encode (in, MINUS_K_1);
  return strcmp (in, out) != 0;
}
// your code goes here


// See full documentation in header file
int test_encode_cyclic_lower_case_negative_k ()
{
  char in[] = "abcdefghijklmnopqrstuvwxyz";
  char out[] = "xyzabcdefghijklmnopqrstuvw";
  encode (in, MINUS_K_3);
  return strcmp (in, out) != 0;
}
// your code goes here}

// See full documentation in header file
int test_encode_cyclic_upper_case_positive_k ()
{
  char in[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char out[] = "DEFGHIJKLMNOPQRSTUVWXYZABC";
  encode (in, K_29);
  return strcmp (in, out) != 0;
}

// See full documentation in header file
int test_decode_non_cyclic_lower_case_positive_k ()
{
  char in[] = "abcdefghijklmnopqrstuvw";
  char out[] = "xyzabcdefghijklmnopqrst";
  decode (in, K_3);
  return strcmp (in, out) != 0;
}

// See full documentation in header file
int test_decode_cyclic_lower_case_special_char_positive_k ()
{
  char in[] = "a b%c67@#$%^&BPc9 ";
  char out[] = "y z%a67@#$%^&ZNa9 ";
  decode (in, K_2);
  return strcmp (in, out) != 0;
}

// See full documentation in header file
int test_decode_non_cyclic_lower_case_special_char_negative_k ()
{
  char in[] = "Y a%b67@#$%^&AOb9 ";
  char out[] = "Z b%c67@#$%^&BPc9 ";
  decode (in, MINUS_K_1);
  return strcmp (in, out) != 0;
}

// See full documentation in header file
int test_decode_cyclic_lower_case_negative_k ()
{
  char in[] = "abcdefghijklmnopqrstuvwxyz";
  char out[] = "defghijklmnopqrstuvwxyzabc";
  decode (in, MINUS_K_3);
  return strcmp (in, out) != 0;
}

// See full documentation in header file
int test_decode_cyclic_upper_case_positive_k ()
{
  char in[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char out[] = "DEFGHIJKLMNOPQRSTUVWXYZABC";
  decode(in, K_29);
  return strcmp (in, out) != 0;
}