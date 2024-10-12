#ifndef ENTRY_H
#define ENTRY_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int id;
  char *name;
  char *user_name;
  char *password;
  int is_deleted;
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
int entry_list_get_length(EntryNode *head);

typedef struct {
  char *type;
  union {
    char *text;
    void *blob;
  } data;
  size_t size;
} IContent;

typedef struct {
  int id;
  int f_entry_id;
  IContent *content;
  int is_deleted;
} EntryDetail;

typedef struct EntryDetailNode {
  EntryDetail data;
  struct EntryDetailNode *next;
} EntryDetailNode;

EntryDetailNode *entryDetail_list_init(void);
void entryDetail_list_prepend_item(EntryDetailNode **head,
                                   EntryDetail entry_detail);
void entryDetail_list_itterate_function(
    EntryDetailNode *head,
    void *callback(void *parameter, EntryDetail entry_detail),
    void *parameter_first_given_to_callback);

int entryDetail_list_get_length(EntryDetailNode *head);

typedef struct FolderNode {
  char *folder_name;
  struct FolderNode *next;
} FolderNode;

FolderNode *folder_list_init(void);
void folder_list_prepent(FolderNode **head, char *folder_name);
void folder_list_itterate_function(FolderNode *head,
                                   void *callback(void *parameter,
                                                  char *folder_name),
                                   void *parameter_first_given_to_callback);
int folder_list_get_length(FolderNode *head);

#endif
