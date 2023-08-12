#include <stdio.h>
#include <SDL2/SDL_image.h>
#include "ui.h"
#include "../libs/nuklear/nuklear_sdl_renderer.h"
#include "../libs/tinyfiledialogs/tinyfiledialogs.h"
#include "engine.h"
#include "logger.h"
#include "utils.h"

extern engine_t engine;
extern eventbus_t eventbus;
extern int draw_tile;

extern const int TILE_SIZE;

void ui_mouse_click(event_t ev_data)
{
	char msg[100];
	sprintf(msg, "the mouse clicked at: %d:%d", ev_data.pos.x, ev_data.pos.y);
	log_info(msg);

	if (
		ev_data.pos.x > 0 && ev_data.pos.x < engine.tilemap.width
		&& ev_data.pos.y > engine.window_height-engine.tilemap.height && ev_data.pos.y < engine.window_height
	) {
		log_info("clicked on tilemap sprite");

		int x = ev_data.pos.x;
		int y = ev_data.pos.y-(engine.window_height-engine.tilemap.height);

		draw_tile = (int)(x/TILE_SIZE)+(y/TILE_SIZE*engine.tilemap.cols);

		return;
	}
}

void draw_ui(engine_t *engine)
{
	int win_w = 230;
	if (nk_begin(engine->ctx, "Tilemap Tools", nk_rect(engine->window_width-win_w-20, 20, win_w, 280),
			  NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
			  NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
	{
		int row_h = 30;
		nk_layout_row_dynamic(engine->ctx, row_h, 1);

		if (nk_button_label(engine->ctx, "Load Tilemap")) {
			char const *tilemap_file_filters[1] = {"*.png"};
			char const *tilemap_fname = tinyfd_openFileDialog(
				"Open tilemap",
				"",
				1,
				tilemap_file_filters,
				"Tilemaps",
				0
			);

			if (!tilemap_fname) {
				log_err("error opening tilemap", "");
			}

			char msg[255];
			sprintf(msg, "loading tilemap: %s", tilemap_fname);
			log_info(msg);

			strncpy(engine->tilemap.filename, tilemap_fname, strlen(tilemap_fname));

			event_t ev;
			ev.type = EVENT_LOAD_TILEMAP;
			strncpy(ev.filename, tilemap_fname, MAX(MAX_FILENAME, strlen(tilemap_fname)));
			emit_event(&eventbus, ev);
		}

		if (strlen(engine->tilemap.filename)) {
			SDL_Surface *surface = IMG_Load(engine->tilemap.filename);
			SDL_Texture *texture = SDL_CreateTextureFromSurface(engine->renderer, surface);

			int w, h;
			SDL_QueryTexture(texture, NULL, NULL, &w, &h);
			// SDL_DestroyTexture(texture);

			int cur_cols = w/TILE_SIZE;
			int cur_rows = h/TILE_SIZE;
			int num_tiles = cur_cols*cur_rows;
			int new_cols = 5;
			int new_rows = num_tiles/new_cols;

			SDL_Rect src = {
				.x = 0,
				.y = 0,
				.w = TILE_SIZE,
				.h = TILE_SIZE,
			};
			SDL_Rect dst = src;
			dst.h = TILE_SIZE+10;
			int padding = 2;
			SDL_Surface *new_tilemap = SDL_CreateRGBSurface(
				0,
				new_cols*TILE_SIZE+(padding*(new_cols-1)),
				new_rows*TILE_SIZE+(padding*(new_rows-1)),
				32,
				0,0,0,0x000000FF
			);

			SDL_FillRect(new_tilemap, NULL, SDL_MapRGB(surface->format, 45, 45, 45));

			for (int row = 0; row < cur_cols; row++) {
				src.y = TILE_SIZE*row;
				for (int col = 0; col < cur_cols; col++) {
					src.x = TILE_SIZE*col;
					SDL_BlitSurface(surface, &src, new_tilemap, &dst);

					dst.x += TILE_SIZE+padding;
					if (dst.x >= new_cols*TILE_SIZE) {
						dst.y += 32+padding;
						dst.x = 0;
					}
				}
			}

			SDL_Texture *new_texture = SDL_CreateTextureFromSurface(engine->renderer, new_tilemap);
			if (!new_texture) {
				log_err("error creating new tilemap texture", "");
			}
			SDL_FreeSurface(surface);
			SDL_FreeSurface(new_tilemap);

			struct nk_image im = nk_image_ptr(new_texture);
			nk_layout_row_static(engine->ctx, new_cols*TILE_SIZE+(padding*(new_rows)), new_rows*TILE_SIZE, 1);
			nk_image(engine->ctx, im);
		}
	}

	nk_end(engine->ctx);

	nk_sdl_render(NK_ANTI_ALIASING_ON);
}
