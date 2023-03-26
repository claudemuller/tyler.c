#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool is_running;
} Engine;

void init(Engine *engine, const bool debug);
void setup(void);
void run(Engine *engine);
void process_input(Engine *engine);
void update(Engine *engine);
void render(Engine *engine);
void cleanup(void);

#endif // ENGINE_H
