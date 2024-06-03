#include "cipher.h"
#include "tests.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

// definition for some values .
#define FIRST_ARG 1
#define SECOND_ARG 2
#define THIRD_ARG 3
#define FOURTH_ARG 4
#define TEST "test"
#define ENCODE "encode"
#define DECODE "decode"
#define ARGS_NUM  5
#define TEST_ARGS_COUNT 2
#define WRONG_ARGS_NUM  "The program receives 1 or 4 arguments only.\n"
#define WRONG_TEST_MSG "Usage: cipher test\n"
#define WRONG_COMMAND "The given command is invalid.\n"
#define WRONG_K_VALUE "The given shift value is invalid.\n"
#define FILE_ERROR "The given file is invalid.\n"
#define BUFFER_LENGTH 1024
#define READ_MODE "r"
#define WRITE_MODE "w"
#define BASE_10 10

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

void run_program (FILE *in, FILE *out, int k, char *cmd)
{// reaching this function after not getting any args error , getting the cmd
  // which represent (encode or decode) then calling him .
  // copying the data from the text using strcpy function.
  char data[BUFFER_LENGTH], text[BUFFER_LENGTH];
  while (fgets (data, sizeof (data), in) != NULL)
  {
    strcpy (text, data);
    if (strcmp (cmd, ENCODE) == 0)
    {
      encode (text, k);
    }
    else
    {
      decode (text, k);
    }
    fprintf (out, "%s", text);
  }
}

int run_test ()
{ // return 0 if all the tests passed , else some of them failed .
  return test_encode_non_cyclic_lower_case_positive_k () &&
         test_encode_cyclic_lower_case_special_char_positive_k () &&
         test_encode_non_cyclic_lower_case_special_char_negative_k () &&
         test_encode_cyclic_lower_case_negative_k () &&
         test_encode_cyclic_upper_case_positive_k () &&
         test_decode_non_cyclic_lower_case_positive_k () &&
         test_decode_cyclic_lower_case_special_char_positive_k () &&
         test_decode_non_cyclic_lower_case_special_char_negative_k () &&
         test_decode_cyclic_lower_case_negative_k () &&
         test_decode_cyclic_upper_case_positive_k ();
}

// lines [80 -110] checking input validity, each time we are modifying err_msg
// if it's still NULL then nothing wrong with args . after that opening the
// files and running the program .
int checking_cipher_input (char *argv[])
{
  char *tmp = NULL;
  int k = strtol (argv[SECOND_ARG], &tmp, BASE_10);
  if (strcmp (tmp, "") != 0)
  {
    fprintf (stderr, WRONG_K_VALUE);
    return EXIT_FAILURE;
  }
  else
  {
    FILE *inp_file = NULL, *output_file = NULL;
    if (open_file (&inp_file, argv[THIRD_ARG],
                   READ_MODE) && open_file (&output_file,
                                            argv[FOURTH_ARG],
                                            WRITE_MODE))
    {
      run_program (inp_file, output_file, k,
                   argv[FIRST_ARG]);
    }
    else
    {
      fprintf (stderr, FILE_ERROR);
      return EXIT_FAILURE;
    }
    close_file (&inp_file);
    close_file (&output_file);
    return EXIT_SUCCESS;
  }
}

int checking_test_input (int args, char *argv[])
{
  if (args == TEST_ARGS_COUNT)
  {
    if (strcmp (argv[FIRST_ARG], TEST) != 0)
    {

      fprintf (stderr, WRONG_TEST_MSG);
      return EXIT_FAILURE;
    }
    else
    {
      return run_test() ? EXIT_FAILURE : EXIT_SUCCESS;
    }
  }
  else
  {
    if (args == ARGS_NUM)
    {
      if (strcmp (argv[FIRST_ARG], ENCODE) != 0 &&
          strcmp (argv[FIRST_ARG], DECODE) != 0)
      {
        fprintf (stderr, WRONG_COMMAND);
        return EXIT_FAILURE;
      }
      else
      {
        return checking_cipher_input (argv) ? EXIT_FAILURE : EXIT_SUCCESS;
      }
    }
    else
    {
      return EXIT_FAILURE;
    }
  }
}

int main (int argc, char *argv[])
{
  if (argc != ARGS_NUM && argc != TEST_ARGS_COUNT)
  {
    fprintf (stderr, WRONG_ARGS_NUM);
    return EXIT_FAILURE;
  }
  return checking_test_input (argc, argv);
}
