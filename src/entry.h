#ifndef ENTRY_H
#define ENTRY_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int id;
  char *name;
  char *user_name;
  char *password;
} Entry;

typedef struct EntryNode {
  Entry data;
  struct EntryNode *next;
} EntryNode;

EntryNode *entry_list_init(void);
void entry_list_prepend_item(EntryNode **head, Entry entry);
void entry_list_itterate_function(EntryNode *head,
                                  void *callback(void *parameter, Entry entry),
                                  void *parameter_first_given_to_callback);
#endif
