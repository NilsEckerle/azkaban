#include "db_manager.h"
#include "entry.h"
#include <stdio.h>
#include <string.h>

void test_setup() {
  printf("Hello from azkaban\n");

  const char *filename = "test.azk";
  const char *password = "123";
  sqlite3 *db = db_open(filename, password);

  Entry e1;
  e1.id = -1;
  e1.name = "e1";
  e1.user_name = "e1_un";
  e1.password = "e1_pw";

  EntryDetail ed1;
  IContent content;
  content.type = "text";
  content.data.text = "Hello World, this is my text.";
  content.size = strlen(content.data.text);
  ed1.id = -1;
  ed1.content = &content;
  ed1.f_entry_id = 1;

  EntryDetail ed2;
  IContent content2;
  content2.type = "text";
  content2.data.text = "Hello World, this is my text.";
  content2.size = strlen(content2.data.text);
  ed2.id = -1;
  ed2.content = &content2;
  ed2.f_entry_id = 1;

  Entry e2;
  e2.id = -1;
  e2.name = "e2";
  e2.user_name = "e2_un";
  e2.password = "e2_pw";

  Entry e3;
  e3.id = -1;
  e3.name = "e3";
  e3.user_name = "e3_un";
  e3.password = "e3_pw";

  Entry e4;
  e4.id = -1;
  e4.name = "e4";
  e4.user_name = "e4_un";
  e4.password = "e4_pw";

  db_write_entry(db, e1);
  db_write_entry(db, e2);
  db_write_entry(db, e3);
  db_write_entry(db, e4);

  db_write_entryDetail(db, ed1);
  db_write_entryDetail(db, ed2);

  Entry e1_changed;
  e1_changed.id = -1;
  e1_changed.name = "e1";
  e1_changed.user_name = "e1_un";
  e1_changed.password = "e1_pw_changed";

  db_change_entry(db, 1, e1_changed);

  db_close(db);
}

int main(int argc, char *argv[]) {
  test_setup();

  char *help_message = "Try 'azkaban_cli help'";
  if (argc <= 1) {
    fprintf(stderr, "%s\n", help_message);
    return 1;
  }

  if (strcmp(argv[1], "list") == 0) {
    sqlite3 *db = db_open("test.azk", "123");
    EntryNode *current = db_get_all_entries(db);
    db_close(db);
    char *seperator = "\n";
    char *eol = "\n\n";
    if (argc >= 3) {
      if (strcmp(argv[2], "--oneline") == 0) {
        seperator = ", ";
        eol = "\n";
      }
    }
    while (current != NULL) {
      printf("ID: %d%sNAME: %s%sUSER_NAME: %s%s", current->data.id, seperator,
             current->data.name, seperator, current->data.user_name, eol);

      current = current->next;
    }
  } else if (strcmp(argv[1], "get") == 0) {
    if (argc <= 2) {
      fprintf(stderr, "%s\n", help_message);
      return 1;
    }

    sqlite3 *db = db_open("test.azk", "123");
    printf("%s\n", db_get_password(db, atoi(argv[2])));

    db_close(db);

  } else {
    // Code for unrecognized command
    fprintf(stderr, "Unrecognized command: %s\n", argv[1]);
  }
  return 0;
}
