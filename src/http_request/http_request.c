#include "http_request.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Header *head = NULL;
struct HeaderString *heads = NULL;

int is_only_whitespace_or_escape(char *string) {
  for (int i = 0; i < strlen(string); i++) {
    if (!isspace(string[i])) {
      return 0;
    }
  }
  return 1;
}

void request_add_headerstring(char *string) {
  struct HeaderString *temp =
      (struct HeaderString *)malloc(sizeof(struct HeaderString));
  temp->next = NULL;
  temp->string = strdup(string);
  if (heads == NULL) {
    heads = temp;
    return;
  } else {
    struct HeaderString *temp2 = heads;
    while (temp2->next != NULL) {
      temp2 = temp2->next;
    }
    temp2->next = temp;
  }
}

void request_add_header(char *key, char *value) {
  struct Header *temp = (struct Header *)malloc(sizeof(struct Header));
  temp->key = key;
  temp->value = value;
  temp->next = NULL;
  if (head == NULL) {
    head = temp;
    return;
  } else {
    struct Header *temp2 = head;
    while (temp2->next != NULL) {
      temp2 = temp2->next;
    }
    temp2->next = temp;
  }
}

struct Header *request_get_header(char *key) {
  struct Header *temp = head;
  while (temp != NULL) {
    if (strcmp(temp->key, key) == 0) {
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

void print_headers() {
  struct Header *temp = head;
  printf("===========Headers==========\n");
  while (temp != NULL) {
    printf("%s: %s\n", temp->key, temp->value);
    temp = temp->next;
  }
  printf("============================\n");
}

void print_headerstrings() {
  struct HeaderString *temp = heads;
  printf("===========HeaderStrings==========\n");
  while (temp != NULL) {
    printf("%s\n", temp->string);
    temp = temp->next;
  }
  printf("============================\n");
}

void empty_headers() {
  struct Header *temp = head;
  struct Header *next;
  while (temp != NULL) {
    next = temp->next;
    free(temp);
    temp = next;
  }
  head = NULL;
}

void empty_headerstrings() {
  struct HeaderString *temp = heads;
  struct HeaderString *next;
  while (temp != NULL) {
    next = temp->next;
    free(temp);
    temp = next;
  }
  heads = NULL;
}

// DONE: parse headers. i am blasting my head for this
void parse_headers(char *header_fields) {
  empty_headers();
  empty_headerstrings();
  char fields[strlen(header_fields)];
  strcpy(fields, header_fields);
  char *field = strtok(fields, "\n");
  // First parse out the wholes lines
  while (field) {
    if (is_only_whitespace_or_escape(field)) {
      break;
    }
    request_add_headerstring(field);
    if (strstr(field, "Content-Length") != NULL) {
      field = strtok(NULL, "?0");
    } else {
      field = strtok(NULL, "\n");
    }
  }

  print_headerstrings();
  // And then parse out the key value pairs
  struct HeaderString *temp = heads;
  while (temp != NULL) {
    char *field = strtok(temp->string, ":");
    char *key = field;
    // if key is null, then break
    if (key == NULL) {
      break;
    }
    char *value = strtok(NULL, "\0");
    if (value != NULL) {
      request_add_header(key, value);
    }
    temp = temp->next;
  }
}

char *get_last_line(char *str) {
    char *last_newline = strrchr(str, '\n');
    if (last_newline != NULL) {
        return last_newline + 1;
    } else {
        return str;
    }
}

struct Request request_constructor(char *string) {
  // seperate data and body
  for (int i = 0; i < strlen(string) - 1; i++) {
    if (string[i] == '\n' && string[i + 1] == '\n') {
      string[i + 1] = '|';
    }
  }
  char *dup = strdup(string);
  char *request_line = strtok(string, "\n");
  char *header_fields =
      strtok(NULL, "|"); // NULL means continue using the line we were using
  struct Request request;
    char *body = "";

  // stirng parsing :sob:
  char *method = strtok(request_line, " ");
  char *URI = strtok(NULL, " ");
  char *HTTPVersion = strtok(NULL, " ");
  HTTPVersion = strtok(HTTPVersion, "/");
  HTTPVersion = strtok(NULL, "/");

  request.method = method;
  request.URI = URI;
  request.HTTPVersion = (float)atof(HTTPVersion);

  parse_headers(header_fields);

  if (strcmp(method, "GET") != 0) {
    body = get_last_line(dup); 
  }

  request.body = body;

  request.request_headers_head = head;
  // print_headers();
  return request;
}
