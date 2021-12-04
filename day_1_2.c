#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

void
read_input (long **out)
{
  char buf[8];
  char *end;
  long n;
  while (fgets (buf, 8, stdin))
    {
      n = strtol (buf, &end, 10);
      vector_push (*out, n);
    }
}

int
main ()
{
  long *input = NULL;
  long a, b, c, d;
  size_t i;
  unsigned inc = 0;

  read_input (&input);

  for (i = 0; i < vector_size (input) - 3; ++i)
    {
      a = input[i];
      b = input[i+1];
      c = input[i+2];
      d = input[i+3];

      //if (b + c + d > a + b + c)
      if (d > a)
        ++inc;
    }

  printf ("%u\n", inc);
  vector_free (input);
}
