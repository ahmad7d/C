#include "cipher.h"
#include <ctype.h>

#define LOWER_CASE_A  'a'
#define NUM_LETTERS  26
#define UPPER_CASE_A 'A'

int modulo (int k, int n)
{// This function making the right work for the modulo in negative numbers .
  while (k < 0)
    {
      k += n;
    }
  return k % n;
}

char encode_char (const char first_char, const char c, int k)
// Getting a single character then encoding him by k steps , first char
// represent the mode ('a' if lower case or 'A' if upper case ).
{
  return first_char + modulo (c + k - first_char, NUM_LETTERS);
}
void encode (char s[], int k)
{
  for (int i = 0; s[i]; i++)
    {
      char mode = islower (s[i]) ? LOWER_CASE_A : isupper (s[i])
          ? UPPER_CASE_A : 0;
      if (mode)
        {
          s[i] = encode_char (mode, s[i], k);
        }
    }

}

// See full documentation in header file
void decode (char s[], int k)
{
  encode (s, -k);
}



