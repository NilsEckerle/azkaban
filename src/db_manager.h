#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <stdio.h>
#define SQLITE_HAS_CODEC
#include "entry.h"
#include <sqlcipher/sqlite3.h>

typedef struct {
  char *type;
  union {
    char *text;
    void *blob;
  } data;
  size_t size;
  void (*print)(struct IContent *self);
} IContent;

typedef struct {
  int id;
  int f_entry_id;
  char *type;
  IContent *content;
} EntryDetail;

sqlite3 *db_open(const char *filename, const char *password);
void db_close(sqlite3 *db);
void _db_setup(sqlite3 *db);
void _db_create_table(sqlite3 *db, const char *sql_statement);
EntryNode *db_get_all_entries(sqlite3 *db);
EntryDetail db_get_all_entryDetail(Entry entry_detail);
int db_write_entry(Entry entry);
int db_write_entryDetail(EntryDetail entry_detail);
int db_change_entry(int id, Entry new_entry);
int db_change_entryDetail(int id, EntryDetail new_entry_detail);

void _db_decrypt(sqlite3 *db, const char *password);

#endif
