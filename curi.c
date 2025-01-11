#include "curi.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static struct protocol* create_protocol(const char* protocol_name)
{
  struct protocol* proto = malloc(sizeof(struct protocol));
  proto->name = strdup(protocol_name);
  proto->type = CUSTOM;

  if (strcasecmp(protocol_name, "http"))
  {
    proto->type = HTTP;
  }
  else if (strcasecmp(protocol_name, "https"))
  {
    proto->type = HTTPS;
  }
  else if (strcasecmp(protocol_name, "ftp"))
  {
    proto->type = FTP;
  }
  else if (strcasecmp(protocol_name, "ftps"))
  {
    proto->type = FTPS;
  }
  else if (strcasecmp(protocol_name, "smtp"))
  {
    proto->type = SMTP;
  }

  return proto;
}

struct uri* parse_uri(const char* unparsed_url)
{
  struct uri* parsed_uri = malloc(sizeof(struct uri));
  parsed_uri->full_path = strdup(unparsed_url);

  return parsed_uri;
}

void print_uri(struct uri* uri)
{
  printf("Full URI: %s\n", uri->full_path);
}

void free_uri(struct uri* uri)
{
  if (!uri)
    return

  free(uri->full_path);
  free(uri->port);

  // free protocol
  if (uri->protocol)
  {
    free(uri->protocol->name);
    free(uri->protocol);
  }

  if (uri->domain)
  {
    // free domain
    free(uri->domain->subdomain);
    free(uri->domain->domain);
    free(uri->domain->extension);
    free(uri->domain);
  }

  // free path parts
  struct path_part* current = uri->head;
  while (current)
  {
    struct path_part* next = current->next;
    if (current->is_variable)
    {
      free(current->variable.name);
      free(current->variable.value);
      free(&current->variable);
    }
    free(current->path_part);
    free(current);
    current = next;
  }

  free(uri);
}