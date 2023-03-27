#include <math.h>
#include <SDL2/SDL_image.h>
#include "engine.h"
#include "eventbus.h"
#include "logger.h"

const bool LOG = true;

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000/FPS;

const int TILE_SIZE = 32;
const int TILE_COLS = 60;
const int TILE_ROWS = 33;

int window_width = TILE_SIZE*TILE_COLS;
int window_height = TILE_SIZE*TILE_ROWS;
int tilemap_width = 0;
int tilemap_height = 0;
int map_cols = 0;
int map_rows = 0;

int millisecs_previous_frame = 0;

bool mouse_down;
SDL_Event mouse_event;
eventbus_t eventbus;

// TODO: we don't use all the cells?
int tile_grid[TILE_ROWS][TILE_COLS];
size_t current_tile = 0;

void
save_tile_to_render(const int x, const int y) {
    log_info("placing tile");
    tile_grid[(int)(y/TILE_SIZE)][(int)(x/TILE_SIZE)] = current_tile;
}

void
on_mouse_down(Engine *engine, SDL_Event e)
{
    printf("clicked at: %d:%d\n", e.motion.x, e.motion.y);

    if (
	e.motion.x > 0 && e.motion.x < tilemap_width
	&& e.motion.y > window_height-tilemap_height && e.motion.y < window_height
    ) {
	log_info("clicked on tilemap");

	int x = e.motion.x;
	int y = e.motion.y-(window_height-tilemap_height);

	current_tile = (int)(x/TILE_SIZE)+(y/TILE_SIZE*map_cols);

	return;
    }

    save_tile_to_render(e.motion.x, e.motion.y);
}

void
init(Engine *engine, const bool debug)
{
    // eventbus = init_eventbus();
    // subscribe(eventbus, EVENT_MOUSE_MOVE, test);
    // emit_event(eventbus, EVENT_MOUSE_MOVE);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
	log_err("error initing SDL");
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

    engine->window = SDL_CreateWindow(
	    NULL,
	    SDL_WINDOWPOS_CENTERED,
	    SDL_WINDOWPOS_CENTERED,
	    window_width,
	    window_height,
	    SDL_WINDOW_BORDERLESS
	);
    if (engine->window == NULL) {
	log_err("error creating window");
	return;
    }

    engine->renderer = SDL_CreateRenderer(engine->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (engine->renderer == NULL) {
	log_err("error creating renderer");
	return;
    }

    engine->is_running = true;

    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    // ImGui_ImplSDLRenderer_Init(renderer);

    log_info("engine successfully initialised");
}

void
setup()
{
    for (size_t y = 0; y < TILE_ROWS; y++) {
	for (size_t x = 0; x < TILE_COLS; x++) {
	    tile_grid[y][x] = -1;
	}
    }
}

void
run(Engine *engine)
{
    setup();

    while (engine->is_running) {
	process_input(engine);
	update(engine);
	render(engine);
    }
}

void
process_input(Engine *engine)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
	// ImGui_ImplSDL2_ProcessEvent(&event);
	// ImGuiIO &io = ImGui::GetIO();

	// int mouseX, mouseY;
	// const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

	// io.MousePos = ImVec2(mouseX, mouseY);
	// io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	// io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

	switch (event.type) {
	    case SDL_QUIT:
		engine->is_running = false;
	    break;
	    case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
		    engine->is_running = false;
		}
		// eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);
	    case SDL_MOUSEBUTTONDOWN:
		mouse_down = true;
		mouse_event = event;
		break;
	    case SDL_MOUSEBUTTONUP:
		mouse_down = false;
		break;
	    break;
	}
    }
}

void
update(Engine *engine)
{
    int time_to_wait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecs_previous_frame);
    if (time_to_wait > 0 && time_to_wait <= MILLISECS_PER_FRAME) SDL_Delay(time_to_wait);

    double delta_time = (SDL_GetTicks() - millisecs_previous_frame) / 1000.0;
    millisecs_previous_frame = SDL_GetTicks();

    if (mouse_down)
	on_mouse_down(engine, mouse_event);
}

SDL_Rect get_tile(size_t n)
{
    int map_cols = (int)(tilemap_width/TILE_SIZE);
    return (SDL_Rect){
	.x = (n%map_cols)*TILE_SIZE,
	.y = (int)(n/map_cols)*TILE_SIZE,
	.w = TILE_SIZE,
	.h = TILE_SIZE
    };
}

void
render(Engine *engine)
{
    SDL_SetRenderDrawColor(engine->renderer, 21, 21, 21, 255);
    SDL_RenderClear(engine->renderer);

    SDL_Surface *surface = IMG_Load("./assets/tilemaps/tilemap.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(engine->renderer, surface);
    SDL_FreeSurface(surface);

    SDL_QueryTexture(texture, NULL, NULL, &tilemap_width, &tilemap_height);
    SDL_Rect tilemap_rect = {
	.x = 0,
	.y = window_height-tilemap_height,
	.w = tilemap_width,
	.h = tilemap_height
    };
    map_cols = (int)(tilemap_width/TILE_SIZE);
    map_rows = (int)(tilemap_height/TILE_SIZE);

    SDL_RenderCopy(engine->renderer, texture, NULL, &tilemap_rect);

    SDL_Rect dst_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    SDL_Rect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for (size_t y = 0; y < TILE_ROWS-map_rows; y++) {
	for (size_t x = 0; x < TILE_COLS; x++) {
	    int tile = tile_grid[y][x];
	    dst_rect.x = x*TILE_SIZE;
	    dst_rect.y = y*TILE_SIZE;

	    if (tile == -1) {
		SDL_SetRenderDrawColor(engine->renderer, 11, 11, 11, 255);
		SDL_RenderDrawRect(engine->renderer, &dst_rect);
		continue;
	    }

	    src_rect = get_tile(tile);

	    SDL_RenderCopy(engine->renderer, texture, &src_rect, &dst_rect);
	}
    }

    for (size_t y = 0; y < map_rows; y++) {
	for (size_t x = 0; x < map_cols; x++) {
	    dst_rect.x = x*TILE_SIZE;
	    dst_rect.y = (y*TILE_SIZE)+(window_height-tilemap_height);
	    SDL_SetRenderDrawColor(engine->renderer, 200, 200, 200, 255);
	    SDL_RenderDrawRect(engine->renderer, &dst_rect);
	}
    }

    SDL_DestroyTexture(texture);

    SDL_RenderPresent(engine->renderer);
}

void
cleanup(void)
{
    log_info("let's clean up :P");
    SDL_Quit();
}
