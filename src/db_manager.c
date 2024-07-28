#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQLITE_HAS_CODEC
#include <sqlcipher/sqlite3.h>

#include "db_manager.h"

sqlite3 *db_open(const char *filename, const char *password) {
  sqlite3 *db;
  int rc = sqlite3_open(filename, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_open failed!\n");
    exit(EXIT_FAILURE);
  }
  rc = sqlite3_key(db, password, strlen(password));
  if (rc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_key failed!\n");
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  return db;
}

void db_close(sqlite3 *db) { sqlite3_close(db); }

static int callback_populate_zErrMsg(void *NotUsed, int argc, char **argv,
                                     char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void test_the_db(sqlite3 *db) {
  char *zErrMsg = 0;
  int rc;
  char *stmt;
  stmt = "CREATE TABLE test("
         "id INT PRIMARY KEY   NOT NULL,"
         "name           TEXT  NOT NULL,"
         "age            INT   NOT NULL);";
  rc = sqlite3_exec(db, stmt, callback_populate_zErrMsg, 0, &zErrMsg);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(EXIT_FAILURE);
  }

  stmt = "INSERT INTO test (id, name, age) VALUES"
         "(1, 'name1', 20);";
  rc = sqlite3_exec(db, stmt, callback_populate_zErrMsg, 0, &zErrMsg);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(EXIT_FAILURE);
  }
}
