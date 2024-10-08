#include "db_manager.h"
#include "entry.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL2/SDL.h"

void *callback(void *not_used, Entry entry) {
  printf("id = %d\n", entry.id);
  printf("name = %s\n", entry.name);
  printf("user_name = %s\n", entry.user_name);
  printf("\n");
  return NULL;
}

int test() {
  printf("You shouldn't run azkaban directly.\nI use it to test some code but "
         "when you can run this, you shouldnt be able to.\nPlease Report this "
         "to your IT Admin.\n");

  // some test credentials
  const char *filename = "test.azk";
  const char *password = "123";

  Entry e1;
  e1.id = -1;
  e1.name = "e1";
  e1.user_name = "e1_un";
  e1.password = "e1_pw";

  EntryDetail ed1;
  IContent content;
  content.type = "text";
  content.data.text = "Hello World, this is my text.";
  content.size = strlen(content.data.text);
  ed1.id = -1;
  ed1.content = &content;
  ed1.f_entry_id = 1;

  EntryDetail ed2;
  IContent content2;
  content2.type = "text";
  content2.data.text = "Hello World, this is my text.";
  content2.size = strlen(content2.data.text);
  ed2.id = -1;
  ed2.content = &content2;
  ed2.f_entry_id = 1;

  Entry e2;
  e2.id = -1;
  e2.name = "e2";
  e2.user_name = "e2_un";
  e2.password = "e2_pw";

  Entry e3;
  e3.id = -1;
  e3.name = "e3";
  e3.user_name = "e3_un";
  e3.password = "e3_pw";

  Entry e4;
  e4.id = -1;
  e4.name = "e4";
  e4.user_name = "e4_un";
  e4.password = "e4_pw";
  sqlite3 *db = db_open("test.azk", "123");

  db_write_entry(db, e1);
  db_write_entry(db, e2);
  db_write_entry(db, e3);
  db_write_entry(db, e4);

  db_write_entryDetail(db, ed1);
  db_write_entryDetail(db, ed2);

  Entry e1_changed;
  e1_changed.id = -1;
  e1_changed.name = "e1";
  e1_changed.user_name = "e1_un";
  e1_changed.password = "e1_pw_changed";

  db_change_entry(db, 1, e1_changed);

  // Entry e;
  // e.id = -1;
  // e.name = "test_out_of_program";
  // e.user_name = "toop";
  // e.password = "toop_pw";
  //
  // int rc = db_write_entry(db, e);
  // printf("insert status: %d\n", rc);
  //
  // EntryNode *entry_list = db_get_all_entries(db);
  // // entry_list_itterate_function(entry_list, callback, 0);
  //
  // srand(time(NULL));
  // int entry_list_length = entry_list_get_length(entry_list);
  // int r = 1;
  // if (entry_list_length > 1) {
  //   r = (rand() % (entry_list_length - 1)) + 1;
  // }
  // printf("rand: %d\n", r);
  // if (r >= 0) {
  //   EntryDetail entry_detail;
  //   IContent *i_content;
  //   entry_detail.id = -1;
  //   entry_detail.f_entry_id = r;
  //   entry_detail.content = (IContent *)malloc(sizeof(IContent));
  //   entry_detail.content->type = "text";
  //   char *t = "Test Detail 123";
  //   entry_detail.content->data.text = t;
  //   entry_detail.content->size = strlen(t);
  //   db_write_entryDetail(db, entry_detail);
  // }
  //
  // db_delete_entry(db, 2);

  db_close(db);

  // printf("entries: %d\n", entry_list_get_length(entry_list));
  return 0;
}

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* win = SDL_CreateWindow("Hello World!", 100, 100, 620, 387, SDL_WINDOW_SHOWN);
	if (win == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		if (win != NULL) {
			SDL_DestroyWindow(win);
		}
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Surface* bmp = SDL_LoadBMP("./img/grumpy-cat.bmp");
	if (bmp == NULL) {
		fprintf(stderr, "SDL_LoadBMP Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, bmp);
	if (tex == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_FreeSurface(bmp);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	SDL_FreeSurface(bmp);

	for (int i = 0; i < 20; i++) {
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);
		SDL_Delay(100);
	}

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;

  // int rc = test();
  // return rc;
}
