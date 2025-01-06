#include "db_manager.h"
#include "entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void quit_app() { exit(EXIT_SUCCESS); }

/**
 * returns int:
 *
 * 1: didn't do anything
 *
 * No other retruns possible because app quit
 */
int check_for_exit(char **inputs, int input_count) {
  if (strcmp(inputs[0], "quit") == 0) {
    quit_app();
  }
  if (strcmp(inputs[0], "q") == 0) {
    quit_app();
  }
  if (strcmp(inputs[0], "exit") == 0) {
    quit_app();
  }
  if (strcmp(inputs[0], "close") == 0) {
    quit_app();
  }

  return 1;
}

/**
 * returns int:
 *
 * 0: executed a command
 *
 * 1: didn't do anything
 */
int check_for_help(char **inputs, int input_count) {
  if (strcmp(inputs[0], "help") == 0) {
    // TODO: print help
    printf("printing help...\n");
    return 0;
  }
  // check for --help
  if (strcmp(inputs[0], "--help") == 0) {
    // TODO: print help
    printf("printing help...\n");
    return 0;
  }
  return 1;
}

void *print_entries(void *not_used, Entry entry) {
  printf("ID: %d \t%s\n", entry.id, entry.name);
  return NULL;
}
void *print_folders(void *not_used, char *folder_name) {
  printf("%s\n", folder_name);
  return NULL;
}

/**
 * returns int:
 *
 * 0: executed a command
 *
 * 1: didn't do anything
 */
int check_for_list(char **inputs, int input_count, sqlite3 *decrypted_db) {
  if (input_count == 1) {
    if (strcmp(inputs[0], "list") == 0 || strcmp(inputs[0], "ls") == 0) {
      EntryNode *list_of_entries = db_get_all_entries(decrypted_db);
      entry_list_itterate_function(list_of_entries, print_entries, NULL);
      return 0;
    }
  }
  // check for list + argument
  if (strcmp(inputs[0], "list") == 0 || strcmp(inputs[0], "ls") == 0) {
    // check for list --help
    if (input_count == 2 && strcmp(inputs[1], "--help") == 0) {
      // TODO: print listhelp
      printf("listhelp...\n");
      return 0;
    }
    // check for list folder
    if (strcmp(inputs[1], "folder") == 0) {
      // TODO: List all folders
      printf("listing all folders...\n");
      FolderNode *folders = db_get_all_folder(decrypted_db);
      folder_list_itterate_function(folders, print_folders, NULL);
      return 0;
    }
    // check for list folder {name}
    if (strcmp(inputs[1], "folder") == 0) {

      // check for list folder --help
      if (strcmp(inputs[2], "--help") == 0) {
        // TODO: print list folder help
        printf("list folder help...\n");
        return 0;
      }
      // TODO: List folder
      char *folder_name = inputs[2];
      printf("listing folder %s...\n", folder_name);
      return 0;
    }
  }
  return 1;
}

int execute_inputs(char **inputs, int input_count, sqlite3 *decrypted_db) {
  // check for no input
  if (input_count <= 0) {
    return 0;
  }

  int ret;

  ret = check_for_exit(inputs, input_count);
  if (ret != 1) {
    fprintf(stderr,
            "something wierd happend. this function should have exited the "
            "program or returned 1.\nreturned was: %d\nThis program will exit "
            "here as a failsafe\n",
            ret);
    exit(EXIT_FAILURE);
  }

  ret = check_for_help(inputs, input_count);
  if (ret == 0) {
    return 0;
  }

  ret = check_for_list(inputs, input_count, decrypted_db);
  if (ret == 0) {
    return 0;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  /*
   * argv order expected:
   * (1) programm name
   * (2) Database File Path (relative or absolute)
   * */

  printf("main is running !!!!!\n");

  char *database_file_path;
  if (argc == 1) {
    // TODO: Create new passwordfile
    exit(EXIT_SUCCESS);
  } else if (argc > 2) {
    fprintf(stderr, "You provided to many arguments. Please use 'azkaban-cli' "
                    "or 'azkaban-cli passwordfile.azk'\n");
    return EXIT_FAILURE;
  } else {
    database_file_path = argv[1];
  }

  // TODO: Open and decrypt passwordfile
  char *password = "123";
  sqlite3 *decrypted_db = db_open(database_file_path, password);

  // Enter command loop
  int flag_run = 1;
  while (flag_run == 1) {
    char *str = NULL;
    size_t len = 0;
    printf("(azk) ");

    getline(&str, &len, stdin);

    // Remove the newline character if present
    if (str[strlen(str) - 1] == '\n') {
      str[strlen(str) - 1] = '\0';
    }

    // TODO: Split input by space
    // Initialize a dynamic array to hold tokens
    char **inputs = NULL;
    int input_count = 0;

    // Split the input string by spaces
    char *token = strtok(str, " ");
    while (token != NULL) {
      // Reallocate memory to accommodate the new token
      inputs = realloc(inputs, sizeof(char *) * (input_count + 1));
      if (inputs == NULL) {
        perror("Unable to reallocate memory");
        free(str);
        return 1;
      }

      // Duplicate the token and store it in the array
      inputs[input_count] = strdup(token);
      if (inputs[input_count] == NULL) {
        perror("Unable to duplicate string");
        free(str);
        for (int i = 0; i < input_count; i++) {
          free(inputs[i]);
        }
        free(inputs);
        return 1;
      }

      input_count++;
      token = strtok(NULL, " ");
    }

    // TODO: Execute input command
    execute_inputs(inputs, input_count, decrypted_db);

    // Cleanup
    free(inputs);
    free(str);
  }

  return EXIT_SUCCESS;
}
