#include "entry.h"
#include <time.h>

FolderNode *folder_list_init(void) { return NULL; }
void folder_list_prepent(FolderNode **head, char *folder_name) {
  FolderNode *new_node = malloc(sizeof(FolderNode));
  if (new_node == NULL) {
    perror("Failed to allocate memory for new node");
    exit(EXIT_FAILURE);
  }
  new_node->folder_name = folder_name;
  new_node->next = *head;
  *head = new_node;
}
void folder_list_itterate_function(FolderNode *head,
                                   void *callback(void *parameter,
                                                  char *folder_name),
                                   void *parameter_first_given_to_callback) {
  FolderNode *current = head;
  while (current != NULL) {
    callback(parameter_first_given_to_callback, current->folder_name);
    current = current->next;
  }
}
int folder_list_get_length(FolderNode *head) {
  int length = 0;
  FolderNode *current = head;

  while (current != NULL) {
    length++;
    current = current->next;
  }

  return length;
}

EntryNode *entry_list_init(void) { return NULL; }
void entry_list_prepend_item(EntryNode **head, Entry entry) {
  EntryNode *new_node = malloc(sizeof(EntryNode));
  if (new_node == NULL) {
    perror("Failed to allocate memory for new node");
    exit(EXIT_FAILURE);
  }
  new_node->data = entry;
  new_node->next = *head;
  *head = new_node;
}
void entry_list_itterate_function(EntryNode *head,
                                  void *callback(void *parameter, Entry entry),
                                  void *parameter_first_given_to_callback) {
  EntryNode *current = head;
  while (current != NULL) {
    callback(parameter_first_given_to_callback, current->data);
    current = current->next;
  }
}
int entry_list_get_length(EntryNode *head) {
  int length = 0;
  EntryNode *current = head;

  while (current != NULL) {
    length++;
    current = current->next;
  }

  return length;
}

EntryDetailNode *entryDetail_list_init(void) { return NULL; }
void entryDetail_list_prepend_item(EntryDetailNode **head,
                                   EntryDetail entry_detail) {
  EntryDetailNode *new_node = malloc(sizeof(EntryDetailNode));
  if (new_node == NULL) {
    perror("Failed to allocate memory for new node");
    exit(EXIT_FAILURE);
  }
  new_node->data = entry_detail;
  new_node->next = *head;
  *head = new_node;
}

void entryDetail_list_itterate_function(
    EntryDetailNode *head,
    void *callback(void *parameter, EntryDetail entry_detail),
    void *parameter_first_given_to_callback) {
  EntryDetailNode *current = head;
  while (current != NULL) {
    callback(parameter_first_given_to_callback, current->data);
    current = current->next;
  }
}
int entryDetail_list_get_length(EntryDetailNode *head) {
  int length = 0;
  EntryDetailNode *current = head;

  while (current != NULL) {
    length++;
    current = current->next;
  }

  return length;
}
