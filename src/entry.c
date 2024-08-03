#include "entry.h"

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
