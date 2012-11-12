#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PROGRAM_NAME "ask"
#define PROGRAM_VERSION "0.0.1"
#define DEFAULT_ANSWER_CHARS "yn"

typedef unsigned char bool;
#define false ((bool) 0)
#define true ((bool) 1)

const char *program_name;

char *question = NULL;
char *answer_chars = NULL;
bool newline = false;

static bool
eqstr (const char *s1, const char *s2)
{
  size_t s2_len = strlen (s2);

  if ((strlen (s1) == s2_len) && (strncmp (s1, s2, s2_len) == 0))
    return true;
  return false;
}

static bool
startswith (const char *str, const char *prefix)
{
  size_t p_len = strlen (prefix);
  size_t i;

  if (p_len > strlen (str))
    return false;
  for (i = 0; i < p_len; ++i)
    if (str[i] != prefix[i])
      return false;
  return true;
}

static char *
get_longopt_value (const char *opt)
{
  char *o = (char *) opt;
  char *v = strchr (o, '=');

  if (v && *v)
    return ++v;
  return o;
}

static bool
valid_answer (const char answer)
{
  size_t i;

  for (i = 0; answer_chars[i]; ++i)
    if (answer == answer_chars[i])
      return true;
  return false;
}

static void
print_usage (bool error)
{
  fprintf (!error ? stdout : stderr,
      "usage: %s -q QUESTION -a ANSWER_CHARS [-n]\n",
      program_name);
}

static void
print_help (void)
{
  print_usage (0);
  fputs ("Options:\n"
      "  -q QUESTION, --question=QUESTION\n"
      "       Ask user QUESTION\n"
      "  -a ANSWER_CHARS, --answer-chars=ANSWER_CHARS\n"
      "       Use each character of ANSWER_CHARS as a valid answer to "
      "QUESTION\n"
      "  -n, --newline\n"
      "       Print a newline immediately after QUESTION and ANSWER_CHARS\n"
      "  -h, --help\n"
      "       Display this message and exit\n"
      "  -v, --version\n"
      "       Display version information and exit\n"
      "\n"
      "To obtain the answer given by the user, use the program's\n"
      "return code. For each given character of ANSWER_CHARS corresponds\n"
      "an integer value of 0-n.\n"
      "\n"
      "For example, if ANSWER_CHARS is the string \"ynrq\", valid return\n"
      "values would come to be y=0, n=1, r=2, and q=3.\n"
      "\n"
      "A negative return value indicates an error occurred.\n"
      "\n"
      "Example on the command line:\n"
      "   user@host$ ask -q \"Would you like to continue\" -a ynq\n"
      "   Would you like to continue? [y/n/q] q\n"
      "   user@host$ echo $?\n"
      "   2\n"
      "   user@host$\n",
    stdout);
}

static void
print_version (void)
{
  fputs (DEFAULT_PROGRAM_NAME " " PROGRAM_VERSION "\n"
      "Nathan Forbes 2012\n", stdout);
}

static void
parse_line (char **v)
{
  bool need_answer_chars = false;
  bool need_question = false;
  size_t i;

  for (i = 1; v[i]; ++i)
  {
    if (eqstr (v[i], "-h") || eqstr (v[i], "--help"))
    {
      print_help ();
      exit (EXIT_SUCCESS);
    }
    else if (eqstr (v[i], "-v") || eqstr (v[i], "--version"))
    {
      print_version ();
      exit (EXIT_SUCCESS);
    }
    else if (eqstr (v[i], "-n") || eqstr (v[i], "--newline"))
      newline = true;
    else if (eqstr (v[i], "-a"))
      need_answer_chars = true;
    else if (startswith (v[i], "--answer-chars="))
      answer_chars = get_longopt_value (v[i]);
    else if (eqstr (v[i], "-q"))
      need_question = true;
    else if (startswith (v[i], "--question="))
      question = get_longopt_value (v[i]);
    else
    {
      if (need_answer_chars)
      {
        answer_chars = v[i];
        need_answer_chars = false;
      }
      else if (need_question)
      {
        question = v[i];
        need_question = false;
      }
      else
        fprintf (stderr, "%s: warning: `%s' unrecognized\n",
            program_name, v[i]);
    }
  }
}

int
main (int argc, char **argv)
{
  int ret = EXIT_SUCCESS;

  program_name = argv[0];
  parse_line (argv);

  if (!answer_chars)
    answer_chars = DEFAULT_ANSWER_CHARS;

  if (question && *question)
  {
    for (;;)
    {
      fputs (question, stdout);
      if (question[strlen (question) - 1] != '?')
        fputc ('?', stdout);
      fputs (" [", stdout);
      if (answer_chars && *answer_chars)
      {
        size_t i;
        for (i = 0; answer_chars[i]; ++i)
        {
          fputc (answer_chars[i], stdout);
          if (answer_chars[i + 1])
            fputc ('/', stdout);
        }
      }
      fputc (']', stdout);
      if (newline)
        fputc ('\n', stdout);
      else
        fputc (' ', stdout);
      char user_answer = fgetc (stdin);
      if (user_answer == EOF)
      {
        ret = -2;
        break;
      }
      if (!valid_answer (user_answer))
      {
        printf ("`%c' unrecognized, try again...\n", user_answer);
        continue;
      }
      size_t r;
      for (r = 0; answer_chars[r]; ++r)
      {
        if (user_answer == answer_chars[r])
        {
          ret = (int) r;
          break;
        }
      }
      break;
    }
  }
  else
  {
    print_usage (1);
    ret = -1;
  }
  return ret;
}

