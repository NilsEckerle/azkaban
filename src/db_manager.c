#include "entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SQLITE_HAS_CODEC
#include <sqlcipher/sqlite3.h>

#include "db_manager.h"

FolderNode *db_get_from_folder(sqlite3 *db, char *folder_name);

FolderNode *db_get_all_folder(sqlite3 *db) {

  const char *sql = "SELECT DISTINCT name FROM Folders;";
  char *zErrMsg = 0;
  FolderNode *folders = folder_list_init();

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement\n");
    return NULL;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *folder_n = sqlite3_column_text(stmt, 0);
    folder_list_prepent(&folders, (char *)folder_n);
  }

  sqlite3_finalize(stmt);

  return folders;
}

int db_add_to_folder(sqlite3 *db, int entry_id, char *folder_name) {
  sqlite3_stmt *stmt;
  const char *sql = "INSERT INTO Folders(name, f_entry_id) VALUES (?, ?);";
  int rc;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  sqlite3_bind_int(stmt, 1, entry_id);
  sqlite3_bind_text(stmt, 2, folder_name, -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Failed to execute statment: %s\n", sqlite3_errmsg(db));
    return 1;
  }
  return 0;
}

int db_delete_entryDetail(sqlite3 *db, int entry_detail_id) {
  // Delete Entry
  sqlite3_stmt *stmt;
  const char *sql = "UPDATE EntryDetails SET is_deleted = 1 WHERE id = ?;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement\n");
    return 1;
  }

  sqlite3_bind_int(stmt, 1, entry_detail_id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return 1;
  }

  sqlite3_finalize(stmt);
  return 0;
}

void *callback_delete_entry_details_of_entry(void *db,
                                             EntryDetail entry_detail) {
  db_delete_entryDetail(db, entry_detail.id);
  return NULL;
}

int db_delete_entry(sqlite3 *db, int entry_id) {
  EntryDetailNode *entry_details_list = db_get_all_entryDetail(db, entry_id);
  entryDetail_list_itterate_function(
      entry_details_list, callback_delete_entry_details_of_entry, db);

  // Delete Entry
  sqlite3_stmt *stmt;
  const char *sql = "UPDATE Entry SET is_deleted = 1 WHERE id = ?;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement\n");
    return 1;
  }

  sqlite3_bind_int(stmt, 1, entry_id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return 1;
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

Entry *db_get_entry(sqlite3 *db, char *name) {
  const char *stmt =
      "SELECT id, name, user_name FROM Entry WHERE name = ? ORDER BY id DESC;";
  sqlite3_stmt *res;
  Entry *entry = malloc(sizeof(Entry));

  if (entry == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  int rc = sqlite3_prepare_v2(db, stmt, -1, &res, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement\n");
    return NULL;
  }

  sqlite3_bind_text(res, 1, name, -1, NULL);

  rc = sqlite3_step(res);
  if (rc == SQLITE_ROW) {
    entry->id = sqlite3_column_int(res, 0);
    entry->name = strdup((char *)sqlite3_column_text(res, 1));
    entry->user_name = strdup((char *)sqlite3_column_text(res, 2));
  } else {
    free(entry);
    entry = NULL;
  }

  sqlite3_finalize(res);
  return entry;
}

int db_change_entry(sqlite3 *db, int entry_id, Entry new_entry) {
  EntryDetailNode *entry_detail_list = db_get_all_entryDetail(db, entry_id);

  // write new entry
  db_write_entry(db, new_entry);
  Entry *new_entry_from_get = db_get_entry(db, new_entry.name);
  int new_entry_id = new_entry_from_get->id;
  printf("%d\n", new_entry_id);

  // write entryDetails to new entry
  EntryDetailNode *current = entry_detail_list;
  while (current != NULL) {
    current->data.f_entry_id = new_entry_id;
    current->data.id = -1;
    db_write_entryDetail(db, current->data);
    current = current->next;
  }

  // succsessful written, now delete
  if (db_delete_entry(db, entry_id) == 1) {
    fprintf(stderr, "Failed to delete");
  }
  current = entry_detail_list;
  while (current != NULL) {
    db_delete_entryDetail(db, current->data.id);
    current = current->next;
  }

  return 0;
}

const char *db_get_password(sqlite3 *db, int id) {
  const char *sql =
      "SELECT password FROM Entry WHERE id = ? AND is_deleted = 0;";
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement\n");
    return NULL;
  }

  sqlite3_bind_int(stmt, 1, id);

  char *password = NULL;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    password = strdup((char *)sqlite3_column_text(stmt, 0));
  }

  sqlite3_finalize(stmt);
  return password;
}

EntryDetailNode *db_get_all_entryDetail(sqlite3 *db, int from_entry_id) {
  const char *sql = "SELECT id, type, content, size FROM EntryDetails WHERE "
                    "f_entry_id = ? AND is_deleted = 0 ORDER BY id DESC;";
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
  const char *stmt = "SELECT id, name, user_name FROM Entry WHERE is_deleted = "
                     "0 ORDER BY id DESC;";
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
         "password VARCHAR(50),"
         "is_deleted INTEGER DEFAULT 0);";
  _db_create_table(db, stmt);

  /* create EntryDetails table */
  stmt = "CREATE TABLE IF NOT EXISTS EntryDetails("
         "id INTEGER PRIMARY KEY,"
         "f_entry_id INT,"
         "type TEXT,"
         "content BLOB,"
         "size INT,"
         "is_deleted INTEGER DEFAULT 0,"
         "FOREIGN KEY(f_entry_id) REFERENCES Entry(id)"
         ");";
  _db_create_table(db, stmt);

  /* create Folders table */
  stmt = "CREATE TABLE IF NOT EXISTS Folders("
         "name VARCHAR(5),"
         "f_entry_id INT,"
         "PRIMARY KEY (name, f_entry_id),"
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
