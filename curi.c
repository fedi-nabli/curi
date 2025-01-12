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

static struct domain* parse_domain(const char* uri)
{
  struct domain* domain = malloc(sizeof(struct domain));
  char* temp = strdup(uri);
  char* save_ptr;

  char* part = strtok_r(temp, ".", &save_ptr);
  domain->subdomain = strdup(part);

  part = strtok_r(NULL, ".", &save_ptr);
  if (part)
    domain->domain = strdup(part);

  part = strtok_r(NULL, ".", &save_ptr);
  if (part)
    domain->extension = strdup(part);

  free(temp);
  
  return domain;
}

struct uri* parse_uri(const char* unparsed_url)
{
  struct uri* parsed_uri = malloc(sizeof(struct uri));
  parsed_uri->full_path = strdup(unparsed_url);

  char* temp = strdup(unparsed_url);
  char* rest = temp;
  char* token;

  // Parse protocol
  const char* delimiter = strstr(unparsed_url, "://");
  if (!delimiter)
  {
    return NULL;
  }

  token = strsep(&rest, "://");
  parsed_uri->protocol = create_protocol(token);
  rest += 2;

  // Parse domain
  parsed_uri->port = NULL;
  token = strsep(&rest, "/");

  char* domain_part = strdup(token);
  char* port_part = NULL;

  char* domain_token = strsep(&domain_part, ":");
  if (domain_token && domain_part)
  {
    parsed_uri->domain = parse_domain(domain_token);
    parsed_uri->port = strdup(domain_part);
  }
  else
  {
    parsed_uri->domain = parse_domain(token);
  }
  free(domain_part);

  free(temp);

  return parsed_uri;
}

void print_uri(struct uri* uri)
{
  printf("Full URI: %s\n", uri->full_path);
  printf("URI Port: %s\n", uri->port ? uri->port : "80");
  printf("URI Protocol:\n");
  printf("  Protocol name: %s\n", uri->protocol->name);
  printf("URI Domain:\n");
  printf("  Domain subdomain: %s\n", uri->domain->subdomain);
  printf("  Domain main: %s\n", uri->domain->domain);
  printf("  Domain extension: %s\n", uri->domain->extension);
}

void free_uri(struct uri* uri)
{
  if (!uri)
    return;

  free((void*)uri->full_path);
  if (uri->port)
    free((void*)uri->port);

  // free protocol
  if (uri->protocol)
  {
    free((void*)uri->protocol->name);
    free((void*)uri->protocol);
  }

  if (uri->domain)
  {
    // free domain
    free((void*)uri->domain->subdomain);
    free((void*)uri->domain->domain);
    free((void*)uri->domain->extension);
    free((void*)uri->domain);
  }

  // free path parts
  struct path_part* current = uri->head;
  while (current)
  {
    struct path_part* next = current->next;
    if (current->is_variable)
    {
      free((void*)current->variable->name);
      free((void*)current->variable->value);
      free((void*)current->variable);
    }
    free((void*)current->path_part);
    free(current);
    current = next;
  }

  free(uri);
}