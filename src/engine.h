#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <../libs/nuklear/nuklear.h>

typedef struct {
	int width;
	int height;
	int cols;
	int rows;
	char filename[256];
} Tilemap;

typedef struct {
	Tilemap tilemap;
	SDL_Window *window;
	SDL_Renderer *renderer;
	struct nk_context *ctx;
	struct nk_colorf bg;

	int window_width;
	int window_height;

	int millisecs_previous_frame;
	bool is_running;
} Engine;

void init(Engine *engine, const bool debug);
void setup(Engine *engine);
void run(Engine *engine);
void process_input(Engine *engine);
void update(Engine *engine);
void render(Engine *engine);
void cleanup(Engine *engine);

void draw_tile_grid(Engine *engine);
void draw_ui(Engine *engine);
void save_tile_to_render(const int x, const int y);
void on_mouse_down(Engine *engine, SDL_Event e);

#endif // ENGINE_H
