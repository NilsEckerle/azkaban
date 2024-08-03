#include "entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQLITE_HAS_CODEC
#include <sqlcipher/sqlite3.h>

#include "db_manager.h"

static int _create_entries(void *entries_linked_list, int argc, char **argv,
                           char **azColName) {
  EntryNode **head = (EntryNode **)entries_linked_list;
  if (argv[0] && argv[1] && argv[2]) {
    Entry entry;
    entry.id = atoi(argv[0]);
    entry.name = strdup(argv[1]); // Make sure to free these later
    entry.user_name = strdup(argv[2]);
    entry.password = NULL; // No password provided in query
    entry_list_prepend_item(head, entry);
  }
  return 0;
}

EntryNode *db_get_all_entries(sqlite3 *db) {
  const char *stmt = "SELECT id, name, user_name FROM Entry ORDER BY id DESC;";
  char *zErrMsg = 0;
  EntryNode *entry_list = entry_list_init();
  if (sqlite3_exec(db, stmt, _create_entries, &entry_list, &zErrMsg) !=
      SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  return entry_list;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void _db_create_table(sqlite3 *db, const char *sql_statement) {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(db, sql_statement, callback, 0, &zErrMsg);
  if (rc) {
    fprintf(stderr, "failed to create table: %s\n", sqlite3_errmsg(db));
    exit(EXIT_FAILURE);
  }
}

void _db_setup(sqlite3 *db) {
  char *stmt;

  /* create Entry table */
  stmt = "CREATE TABLE IF NOT EXISTS Entry("
         "id INTEGER PRIMARY KEY,"
         "name  VARCHAR(50)   NOT NULL,"
         "user_name VARCHAR(50),"
         "password VARCHAR(50));";
  _db_create_table(db, stmt);

  /* create EntryDetails table */
  stmt = "CREATE TABLE IF NOT EXISTS EntryDetails("
         "id INTEGER PRIMARY KEY,"
         "f_entry_id INT,"
         "type TEXT,"
         "content BLOB,"
         "size INT,"
         "FOREIGN KEY(f_entry_id) REFERENCES Entry(id)"
         ");";
  _db_create_table(db, stmt);
}

void _db_decrypt(sqlite3 *db, const char *password) {
  int rc = sqlite3_key(db, password, strlen(password));
  if (rc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_key failed!\n");
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }
}

sqlite3 *db_open(const char *filename, const char *password) {
  sqlite3 *db;

  /* open/create database */
  int rc = sqlite3_open(filename, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_open failed!\n");
    exit(EXIT_FAILURE);
  }

  _db_decrypt(db, password);

  _db_setup(db); // This must happen after decrypt!!

  return db;
}

void db_close(sqlite3 *db) { sqlite3_close(db); }
