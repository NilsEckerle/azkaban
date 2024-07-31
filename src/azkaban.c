#include "db_manager.h"
#include <stdio.h>
#include <stdlib.h>

int callback(void *, int, char **, char **) { return 0; }

void test_the_db(sqlite3 *db) {
  char *zErrMsg = 0;
  int rc;
  char *stmt;

  stmt = "CREATE TABLE IF NOT EXISTS test("
         "id INTEGER PRIMARY KEY,"
         "name           TEXT  NOT NULL,"
         "age            INT   NOT NULL);";
  _db_create_table(db, stmt);

  stmt = "INSERT INTO test (id, name, age) VALUES"
         "(NULL, 'name1', 20);";
  rc = sqlite3_exec(db, stmt, callback, 0, &zErrMsg);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  printf("Hello from azkaban\n");

  const char *filename = "test.azk";
  const char *password = "123";
  sqlite3 *db = db_open(filename, password);

  test_the_db(db);

  db_close(db);

  return 0;
}
