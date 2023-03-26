#include "engine.h"
#include "logger.h"

const bool LOG = true;

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000/FPS;

int window_width = 0;
int window_height = 0;
int millisecs_previous_frame = 0;

void init(Engine *engine, const bool debug) {
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
    window_width = 1280; //displayMode.w;
    window_height = 720; //displayMode.h;

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

void setup() {

}

void run(Engine *engine) {
    setup();

    while (engine->is_running) {
		process_input(engine);
		update(engine);
		render(engine);
    }
}

void process_input(Engine *engine) {
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
			break;
		}
    }
}

void update(Engine *engine) {
    int time_to_wait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecs_previous_frame);
    if (time_to_wait > 0 && time_to_wait <= MILLISECS_PER_FRAME) SDL_Delay(time_to_wait);

    double delta_time = (SDL_GetTicks() - millisecs_previous_frame) / 1000.0;
    millisecs_previous_frame = SDL_GetTicks();
}

void render(Engine *engine) {
    SDL_SetRenderDrawColor(engine->renderer, 21, 21, 21, 255);
    SDL_RenderClear(engine->renderer);

	SDL_SetRenderDrawColor(engine->renderer, 255, 5, 5, 255);

    SDL_RenderPresent(engine->renderer);
}

void cleanup(void) {
	log_info("let's clean up :P");
	SDL_Quit();
}
