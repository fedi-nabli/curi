#include <stdio.h>

#include "curi.h"

int main(int argc, const char** argv)
{
  struct uri* parsed_uri = parse_uri("https://www.example.com/api/users");
  print_uri(parsed_uri);

  return 0;
}