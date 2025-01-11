#ifndef _CURI_H_
#define _CURI_H_

#include <stdbool.h>

typedef enum
{
  HTTP,
  HTTPS,
  FTP,
  FTPS,
  SMTP,
  CUSTOM
} ProtocolType;

typedef enum
{
  STRING,
  NUMBER,
  BOOLEAN,
  UNKNOWN
} VariableType;

struct protocol
{
  const char* name;
  ProtocolType type;
};

struct domain
{
  const char* subdomain;
  const char* domain;
  const char* extension;
};

struct path_part
{
  const char* path_part;
  bool is_variable;
  struct variable
  {
    const char* name;
    const char* value;
    VariableType type;
  } variable;
  struct path_part* prev;
  struct path_part* next;
};

struct uri
{
  const char* full_path;
  const char* port;
  struct protocol* protocol;
  struct domain* domain;
  struct path_part* head;
};

struct uri* parse_uri(const char* unparsed_url);
void free_uri(struct uri* uri);
void print_uri(struct uri* uri);

#endif