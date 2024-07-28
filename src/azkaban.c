#include "db_manager.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Hello from azkaban\n");

  const char *filename = "test.azk";
  const char *password = "my_secret_test_password";
  sqlite3 *db = db_open(filename, password);

  test_the_db(db);

  db_close(db);

  return 0;
}
