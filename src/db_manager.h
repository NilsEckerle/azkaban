#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#define SQLITE_HAS_CODEC
#include "entry.h"
#include <sqlcipher/sqlite3.h>

void db_close(sqlite3 *db);
sqlite3 *db_open(const char *filename, const char *password);
void _db_create_table(sqlite3 *db, const char *sql_statement);
void _db_setup(sqlite3 *db);
EntryNode *db_get_all_entries(sqlite3 *db);
Entry *db_get_entry(sqlite3 *db, char *name);
EntryDetailNode *db_get_all_entryDetail(sqlite3 *db, int from_entry_id);
const char *db_get_password(sqlite3 *db, int id);
int db_change_entry(sqlite3 *db, int entry_id, Entry new_entry);
int db_change_entryDetail(sqlite3 *db, int id, EntryDetail new_entry_detail);
int db_write_entry(sqlite3 *db, Entry entry);
int db_write_entryDetail(sqlite3 *db, EntryDetail entry_detail);
int db_delete_entry(sqlite3 *db, int entry_id);
int db_delete_entryDetail(sqlite3 *db, int entry_detail_id);

void _db_decrypt(sqlite3 *db, const char *password);

#endif
