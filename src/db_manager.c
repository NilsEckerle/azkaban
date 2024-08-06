#include "entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQLITE_HAS_CODEC
#include <sqlcipher/sqlite3.h>

#include "db_manager.h"

int db_write_entry(sqlite3 *db, Entry entry) {
  if (entry.id != -1) {
    fprintf(stderr,
            "Your entry.id isn't -1. That could mean, that your entry is "
            "retrieved from the database and already exists.\n"
            "Please set it to -1 before writing if you want to write it.");
    return -1;
  }
  sqlite3_stmt *stmt;
  const char *sql =
      "INSERT INTO Entry(name, user_name, password) VALUES(?, ?, ?);";
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_text(stmt, 1, entry.name, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, entry.user_name, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, entry.password, -1, SQLITE_STATIC);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return -1;
  }

  sqlite3_finalize(stmt);
  return 0;
}

int db_write_entryDetail(sqlite3 *db, EntryDetail entry_detail) {
  if (entry_detail.id != -1) {
    fprintf(stderr,
            "Your EntryDetail.id isn't -1. That could mean, that your "
            "EntryDetail is retrieved from the database and already exists.\n "
            "Please set it to -1 before writing if you want to write it.");
    return -1;
  }

  sqlite3_stmt *stmt;
  const char *sql = "INSERT INTO EntryDetails(f_entry_id, type, content, "
                    "size) VALUES (?, ?, ?, ?);";
  int rc;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  sqlite3_bind_int(stmt, 1, entry_detail.f_entry_id);
  sqlite3_bind_text(stmt, 2, entry_detail.content->type, -1, SQLITE_TRANSIENT);
  if (strcmp(entry_detail.content->type, "text") == 0) {
    sqlite3_bind_text(stmt, 3, entry_detail.content->data.text, -1,
                      SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_text(stmt, 3, entry_detail.content->data.blob,
                      (int)entry_detail.content->size, SQLITE_TRANSIENT);
  }
  sqlite3_bind_int(stmt, 4, (int)entry_detail.content->size);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Failed to execute statment: %s\n", sqlite3_errmsg(db));
    return 1;
  }
  return 0;
}

static int _create_entries(void *entries_linked_list, int argc, char **argv,
                           char **azColName) {
  EntryNode **head = (EntryNode **)entries_linked_list;
  if (argv[0] && argv[1] && argv[2]) {
    Entry entry;
    entry.id = atoi(argv[0]);
    entry.name = strdup(argv[1]);
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

EntryDetailNode *db_get_all_entryDetail(sqlite3 *db, int from_entry_id) {

  // the table is created like:
  // stmt = "CREATE TABLE IF NOT EXISTS EntryDetails("
  //        "id INTEGER PRIMARY KEY,"
  //        "f_entry_id INT,"
  //        "type TEXT,"
  //        "content BLOB,"
  //        "size INT,"
  //        "FOREIGN KEY(f_entry_id) REFERENCES Entry(id)"
  //        ");";

  const char *sql = "SELECT id, type, content, size FROM EntryDetails WHERE "
                    "f_entry_id = ? ORDER BY id DESC;";
  char *zErrMsg = 0;
  EntryDetailNode *entry_details_list = entryDetail_list_init();

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement\n");
    return NULL;
  }

  sqlite3_bind_int(stmt, 1, from_entry_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char *type = (const char *)sqlite3_column_text(stmt, 1);
    const void *content = sqlite3_column_blob(stmt, 2);
    int size = sqlite3_column_int(stmt, 3);

    EntryDetail entry_detail;
    entry_detail.id = id;
    entry_detail.f_entry_id = from_entry_id;

    IContent *i_content = (IContent *)malloc(sizeof(IContent));
    i_content->type = strdup(type);
    i_content->size = size;

    if (strcmp(i_content->type, "text") == 0) {
      i_content->data.text = strdup(content);
    } else {
      i_content->data.blob = malloc(i_content->size);
      memcpy(i_content->data.blob, content, i_content->size);
    }

    entry_detail.content = i_content;
    entryDetail_list_prepend_item(&entry_details_list, entry_detail);
  }

  sqlite3_finalize(stmt);

  return entry_details_list;
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
