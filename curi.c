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

static struct path_part* parse_path_part(const char* path, struct path_part* prev)
{
  struct path_part* part = malloc(sizeof(struct path_part));
  part->path_part = strdup(path);
  if (prev)
  {
    part->prev = prev;
  }
  else
  {
    part->prev = NULL;
  }
  part->next = NULL;
  part->is_variable = false;
  part->variable = NULL;

  // Check if this part is a variable (starts with :)
  if (path[0] == ':')
  {
    struct variable* variable = malloc(sizeof(struct variable));
    variable->name = strdup(path + 1);
    variable->value = NULL;
    variable->type = UNKNOWN;
    part->is_variable = true;
    part->variable = variable;
  }

  return part;
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

  struct path_part* current = NULL;
  while ((token = strsep(&rest, "/")) != NULL)
  {
    if (strlen(token) == 0)
      continue;

    struct path_part* new_part = parse_path_part(token, current);

    if (current)
    {
      current->next = new_part;
    }
    else
    {
      parsed_uri->head = new_part;
    }
    current = new_part;
  }

  free(temp);

  return parsed_uri;
}

void print_uri(struct uri* uri)
{
  printf("Full URI: %s\n", uri->full_path);
  printf("URI Port: %s\n", uri->port ? uri->port : "80");
  printf("URI Protocol:\n");
  printf("  Protocol name: %s\n", uri->protocol->name);
  printf("URI full domain: %s.%s.%s\n", uri->domain->subdomain, uri->domain->domain, uri->domain->extension);
  printf("URI Domain:\n");
  printf("  Domain subdomain: %s\n", uri->domain->subdomain);
  printf("  Domain main: %s\n", uri->domain->domain);
  printf("  Domain extension: %s\n", uri->domain->extension);
  printf("URI Path Parts:\n");
  struct path_part* current = uri->head;
  while (current)
  {
    printf("  - %s\n", current->path_part);
    if (current->is_variable)
    {
      printf("    Variable: %s\n", current->variable->name);
      if (current->variable->value)
      {
        printf("    Value: %s (type %d)\n", current->variable->value, current->variable->type);
      }
    }
    current = current->next;
  }
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