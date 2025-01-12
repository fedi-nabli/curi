#include <stdio.h>

#include "curi.h"

int main(int argc, const char** argv)
{
  struct uri* parsed_uri = parse_uri("https://www.example.com/api/users/:id");
  print_uri(parsed_uri);

  struct path_part* current = parsed_uri->head;
  while (current)
  {
    if (current->is_variable)
    {
      set_variable(current, "9UGT8");
    }
    current = current->next;
  }
  printf("\n\n");
  print_uri(parsed_uri);

  return 0;
}