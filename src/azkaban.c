#include "db_manager.h"
#include <stdio.h>

void *callback(void *not_used, Entry entry) {
  printf("id = %d\nname = %s\nuser_name = %s\n\n", entry.id, entry.name,
         entry.user_name);
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
