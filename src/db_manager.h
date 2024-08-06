#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <stdio.h>
#define SQLITE_HAS_CODEC
#include "entry.h"
#include <sqlcipher/sqlite3.h>

sqlite3 *db_open(const char *filename, const char *password);
void db_close(sqlite3 *db);
void _db_setup(sqlite3 *db);
void _db_create_table(sqlite3 *db, const char *sql_statement);
EntryNode *db_get_all_entries(sqlite3 *db);
EntryDetailNode *db_get_all_entryDetail(sqlite3 *db, int from_entry_id);
int db_write_entry(sqlite3 *db, Entry entry);
int db_write_entryDetail(sqlite3 *db, EntryDetail entry_detail);
int db_change_entry(sqlite3 *db, int id, Entry new_entry);
int db_change_entryDetail(sqlite3 *db, int id, EntryDetail new_entry_detail);

void _db_decrypt(sqlite3 *db, const char *password);

#endif
