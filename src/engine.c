#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#include "../libs/tinyfiledialogs/tinyfiledialogs.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION

#include "engine.h"
#include "../libs/nuklear/nuklear_sdl_renderer.h"
#include "eventbus.h"
#include "logger.h"

#define MAX(a, b) a > b ? a : b

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000/FPS;

const int TILE_SIZE = 32;
#define TILE_COLS 60
#define TILE_ROWS 33

bool mouse_down;
SDL_Event mouse_event;
eventbus_t eventbus;

// TODO: we don't use all the cells?
int tile_grid[TILE_ROWS][TILE_COLS];
size_t draw_tile = 0;

extern engine_t engine;

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

	save_tile_to_render(ev_data.pos.x, ev_data.pos.y);
}

void tilemap_mouse_click(event_t ev_data)
{
	log_info("clicked the tilemap");
}

void init(engine_t *engine, const bool debug)
{
	eventbus = init_eventbus();
	subscribe(&eventbus, EVENT_NK_MOUSE_CLICK, &ui_mouse_click);
	subscribe(&eventbus, EVENT_SDL_MOUSE_CLICK, &tilemap_mouse_click);

	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		log_err("error initing SDL", SDL_GetError());
		return;
	}

	//    if (TTF_Init() != 0) {
	// log_err("error initing TTF");
	// return;
	//    }

	// SDL_DisplayMode displayMode;
	// SDL_GetCurrentDisplayMode(0, &displayMode);
	// window_width = 1280; //displayMode.w;
	// window_height = 720; //displayMode.h;

	engine->debug = debug;
	engine->window_width = TILE_SIZE*TILE_COLS;
	engine->window_height = TILE_SIZE*TILE_ROWS;
	engine->window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		engine->window_width,
		engine->window_height,
		SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI
	);
	if (engine->window == NULL) {
		log_err("error creating window", SDL_GetError());
		return;
	}

	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");

	engine->renderer = SDL_CreateRenderer(engine->window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	if (engine->renderer == NULL) {
		log_err("error creating renderer", SDL_GetError());
		return;
	}

	float font_scale = 1;
	/* scale the renderer output for High-DPI displays */
	{
		int render_w, render_h;
		int window_w, window_h;
		float scale_x, scale_y;
		SDL_GetRendererOutputSize(engine->renderer, &render_w, &render_h);
		SDL_GetWindowSize(engine->window, &window_w, &window_h);
		scale_x = (float)(render_w) / (float)(window_w);
		scale_y = (float)(render_h) / (float)(window_h);
		SDL_RenderSetScale(engine->renderer, scale_x, scale_y);
		font_scale = scale_y;
	}

	engine->ctx = nk_sdl_init(engine->window, engine->renderer);
	/* Load Fonts: if none of these are loaded a default font will be used  */
	/* Load Cursor: if you uncomment cursor loading please hide the cursor */
	{
		struct nk_font_atlas *atlas;
		struct nk_font_config config = nk_font_config(0);
		struct nk_font *font;

		/* set up the font atlas and add desired font; note that font sizes are
	 * multiplied by font_scale to produce better results at higher DPIs */
		nk_sdl_font_stash_begin(&atlas);
		font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
		/*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14 * font_scale, &config);*/
		/*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16 * font_scale, &config);*/
		/*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13 * font_scale, &config);*/
		/*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12 * font_scale, &config);*/
		/*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10 * font_scale, &config);*/
		/*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13 * font_scale, &config);*/
		nk_sdl_font_stash_end();

		/* this hack makes the font appear to be scaled down to the desired
	 * size and is only necessary when font_scale > 1 */
		font->handle.height /= font_scale;
		/*nk_style_load_all_cursors(engine->ctx, atlas->cursors);*/
		nk_style_set_font(engine->ctx, &font->handle);
	}

	engine->is_running = true;

	log_info("engine successfully initialised");
}

void setup(engine_t *engine)
{
	engine->tilemap = (tilemap_t){
		.cols = 0,
		.rows = 0,
		.width = 0,
		.height = 0,
	};

	memset(tile_grid, -1, sizeof(tile_grid));
}

void run(engine_t *engine)
{
	setup(engine);

	while (engine->is_running) {
		process_input(engine);
		update(engine);
		render(engine);
	}
}

NK_API int nk_consume_keyboard(struct nk_context *ctx)
{
	struct nk_window *iter;

	NK_ASSERT(ctx);
	if (!ctx) {
		return 0;
	}

	iter = ctx->begin;
	while (iter){
		if (iter->edit.active & NK_EDIT_ACTIVE) {
			return 1;
		}
		iter = iter->next;
	}

	return 0;
}

NK_API int nk_consume_mouse(struct nk_context *ctx)
{
	static unsigned sdl_previous_button_state = 0;
	static int nk_consume_mouse_at_button_press = 0;
	unsigned sdl_current_button_state = SDL_GetMouseState(NULL, NULL);

	if (sdl_previous_button_state == 0 && sdl_current_button_state != 0) {
		nk_consume_mouse_at_button_press = nk_item_is_any_active(ctx);
	}

	sdl_previous_button_state = sdl_current_button_state;
	if (sdl_current_button_state != 0) {
		return nk_consume_mouse_at_button_press;
	}

	return nk_item_is_any_active(ctx);
}

void process_input(engine_t *engine)
{
	SDL_Event event;
	nk_input_begin(engine->ctx);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			engine->is_running = false;
			return;
		}

		nk_sdl_handle_event(&event);
		if (nk_consume_keyboard(engine->ctx) == 0){
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t) {
				log_info("pressed T");
			}
		}

		// Nuklear mouse click i.e. on the UI
		if (nk_consume_mouse(engine->ctx) > 0) {
			if (engine->ctx->input.mouse.buttons[0].clicked) {
				event_t ev;
				ev.type = EVENT_NK_MOUSE_CLICK;
				ev.pos = (vec2_t){
					.x = event.motion.x,
					.y = event.motion.y,
				};
				emit_event(&eventbus, ev);
			}
		}

		// SDL Mouse click i.e. everywhere else
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			// mouse_down = true;
			// mouse_event = event;
				event_t ev;
				ev.type = EVENT_SDL_MOUSE_CLICK;
				ev.pos = (vec2_t){
					.x = event.motion.x,
					.y = event.motion.y,
				};
				emit_event(&eventbus, ev);
		}

		if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK)) {
			event_t ev;
			ev.type = EVENT_MOUSE_MOVE;
			ev.pos = (vec2_t){
				.x = event.motion.x,
				.y = event.motion.y,
			};
			emit_event(&eventbus, ev);
		}
	}

	nk_input_end(engine->ctx);
}

void update(engine_t *engine)
{
	int time_to_wait = MILLISECS_PER_FRAME - (SDL_GetTicks() - engine->millisecs_previous_frame);
	if (time_to_wait > 0 && time_to_wait <= MILLISECS_PER_FRAME) SDL_Delay(time_to_wait);

	// double delta_time = (SDL_GetTicks() - engine->millisecs_previous_frame) / 1000.0;
	engine->millisecs_previous_frame = SDL_GetTicks();

	//    if (mouse_down)
	// on_mouse_down(engine, mouse_event);
}

SDL_Rect get_tile(engine_t *engine, size_t n)
{
	int map_cols = (int)(engine->tilemap.width/TILE_SIZE);
	return (SDL_Rect){
		.x = (int)(n%map_cols)*TILE_SIZE,
		.y = (int)(n/map_cols)*TILE_SIZE,
		.w = TILE_SIZE,
		.h = TILE_SIZE
	};
}

void render(engine_t *engine)
{
	SDL_SetRenderDrawColor(engine->renderer, 21, 21, 21, 255);
	SDL_RenderClear(engine->renderer);

	draw_tile_grid(engine);
	draw_ui(engine);

	SDL_RenderPresent(engine->renderer);
}

void cleanup(engine_t *engine)
{
	log_info("let's clean up :P");
	nk_sdl_shutdown();
	SDL_DestroyRenderer(engine->renderer);
	SDL_DestroyWindow(engine->window);
	SDL_Quit();
}

void draw_tile_grid(engine_t *engine)
{
	SDL_Rect dst_rect = {0, 0, TILE_SIZE, TILE_SIZE};
	SDL_Texture *texture;

	if (strlen(engine->tilemap.filename) > 0) {
		SDL_Surface *surface = IMG_Load(engine->tilemap.filename);
		texture = SDL_CreateTextureFromSurface(engine->renderer, surface);
		SDL_FreeSurface(surface);

		SDL_QueryTexture(texture, NULL, NULL, &engine->tilemap.width, &engine->tilemap.height);
		SDL_Rect tilemap_rect = {
			.x = 0,
			.y = engine->window_height-engine->tilemap.height,
			.w = engine->tilemap.width,
			.h = engine->tilemap.height
		};
		engine->tilemap.cols = (int)(engine->tilemap.width/TILE_SIZE);
		engine->tilemap.rows = (int)(engine->tilemap.height/TILE_SIZE);

		// SDL_RenderCopy(engine->renderer, texture, NULL, &tilemap_rect);
	}

	SDL_Rect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
	for (size_t y = 0; y < TILE_ROWS; y++) {
		for (size_t x = 0; x < TILE_COLS; x++) {
			int tile = tile_grid[y][x];
			dst_rect.x = x*TILE_SIZE;
			dst_rect.y = y*TILE_SIZE;

			if (tile == -1) {
				SDL_SetRenderDrawColor(engine->renderer, 11, 11, 11, 255);
				SDL_RenderDrawRect(engine->renderer, &dst_rect);
				continue;
			}

			src_rect = get_tile(engine, tile);

			if (texture) {
				SDL_RenderCopy(engine->renderer, texture, &src_rect, &dst_rect);
			}
		}
	}

	SDL_DestroyTexture(texture);
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

			event_t ev;
			ev.type = EVENT_LOAD_TILEMAP;
			strncpy(ev.filename, tilemap_fname, MAX(MAX_FILENAME, strlen(tilemap_fname)));
			strncpy(engine->tilemap.filename, tilemap_fname, strlen(tilemap_fname));
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

void save_tile_to_render(const int x, const int y)
{
	log_info("placing tile");
	tile_grid[(int)(y/TILE_SIZE)][(int)(x/TILE_SIZE)] = draw_tile;
}
