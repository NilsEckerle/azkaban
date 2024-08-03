#include "db_manager.h"
#include <stdio.h>

void *callback(void *not_used, Entry entry) {
  printf("id = %d\n", entry.id);
  printf("name = %s\n", entry.name);
  printf("user_name = %s\n", entry.user_name);
  printf("\n");
}

int main(int argc, char *argv[]) {
  printf("Hello from azkaban\n");

  const char *filename = "test.azk";
  const char *password = "123";
  sqlite3 *db = db_open(filename, password);

  EntryNode *entry_list = db_get_all_entries(db);
  entry_list_itterate_function(entry_list, callback, 0);

  db_close(db);

  return 0;
}
