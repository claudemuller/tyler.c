#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <../libs/nuklear/nuklear.h>

typedef struct {
	int width;
	int height;
	size_t cols;
	size_t rows;
	char filename[256];
} tilemap_t;

typedef struct {
	tilemap_t tilemap;
	SDL_Window *window;
	SDL_Renderer *renderer;
	struct nk_context *ctx;
	struct nk_colorf bg;

	bool debug;
	int window_width;
	int window_height;

	int millisecs_previous_frame;
	bool is_running;
} engine_t;

void init(engine_t *engine, const bool debug);
void setup(engine_t *engine);
void run(engine_t *engine);
void process_input(engine_t *engine);
void update(engine_t *engine);
void render(engine_t *engine);
void cleanup(engine_t *engine);

void draw_tile_grid(engine_t *engine);
void draw_ui(engine_t *engine);
void save_tile_to_render(const int x, const int y);

#endif // ENGINE_H
