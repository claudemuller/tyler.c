#include <math.h>
#include <SDL2/SDL_image.h>

#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "../libs/nuklear/nuklear.h"
#include "../libs/nuklear/nuklear_sdl_renderer.h"

#include "engine.h"
#include "eventbus.h"
#include "logger.h"

const bool LOG = true;

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000/FPS;

const int TILE_SIZE = 32;
const int TILE_COLS = 60;
const int TILE_ROWS = 33;

int millisecs_previous_frame = 0;

bool mouse_down;
SDL_Event mouse_event;
eventbus_t eventbus;

struct nk_context *ctx;
struct nk_colorf bg;

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
	e.motion.x > 0 && e.motion.x < engine->tilemap.width
	&& e.motion.y > engine->window_height-engine->tilemap.height && e.motion.y < engine->window_height
    ) {
	log_info("clicked on tilemap");

	int x = e.motion.x;
	int y = e.motion.y-(engine->window_height-engine->tilemap.height);

	current_tile = (int)(x/TILE_SIZE)+(y/TILE_SIZE*engine->tilemap.cols);

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


    engine->window_width = TILE_SIZE*TILE_COLS;
    engine->window_height = TILE_SIZE*TILE_ROWS;
    engine->window = SDL_CreateWindow(
	    NULL,
	    SDL_WINDOWPOS_CENTERED,
	    SDL_WINDOWPOS_CENTERED,
	    engine->window_width,
	    engine->window_height,
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

    /* GUI */
    ctx = nk_sdl_init(engine->window,engine->renderer);
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
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        nk_style_set_font(ctx, &font->handle);
    }
    // set_style(ctx, THEME_DARK);

    engine->is_running = true;

    log_info("engine successfully initialised");
}

void
setup(Engine *engine)
{
    engine->tilemap = (Tilemap){0, 0, 0, 0};

    for (size_t y = 0; y < TILE_ROWS; y++) {
	for (size_t x = 0; x < TILE_COLS; x++) {
	    tile_grid[y][x] = -1;
	}
    }
}

void
run(Engine *engine)
{
    setup(engine);

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
	nk_sdl_handle_event(&event);
    }
    nk_input_end(ctx);
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

SDL_Rect get_tile(Engine *engine, size_t n)
{
    int map_cols = (int)(engine->tilemap.width/TILE_SIZE);
    return (SDL_Rect){
	.x = (int)(n%map_cols)*TILE_SIZE,
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

    SDL_QueryTexture(texture, NULL, NULL, &engine->tilemap.width, &engine->tilemap.height);
    SDL_Rect tilemap_rect = {
	.x = 0,
	.y = engine->window_height-engine->tilemap.height,
	.w = engine->tilemap.width,
	.h = engine->tilemap.height
    };
    engine->tilemap.cols = (int)(engine->tilemap.width/TILE_SIZE);
    engine->tilemap.rows = (int)(engine->tilemap.height/TILE_SIZE);

    SDL_RenderCopy(engine->renderer, texture, NULL, &tilemap_rect);

    SDL_Rect dst_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    SDL_Rect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for (size_t y = 0; y < TILE_ROWS-engine->tilemap.rows; y++) {
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

	    SDL_RenderCopy(engine->renderer, texture, &src_rect, &dst_rect);
	}
    }

    for (size_t y = 0; y < engine->tilemap.rows; y++) {
	for (size_t x = 0; x < engine->tilemap.cols; x++) {
	    dst_rect.x = x*TILE_SIZE;
	    dst_rect.y = (y*TILE_SIZE)+(engine->window_height-engine->tilemap.height);
	    SDL_SetRenderDrawColor(engine->renderer, 200, 200, 200, 255);
	    SDL_RenderDrawRect(engine->renderer, &dst_rect);
	}
    }

    nk_input_begin(ctx);
        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        nk_sdl_render(NK_ANTI_ALIASING_ON);

    SDL_DestroyTexture(texture);

    SDL_RenderPresent(engine->renderer);
}

void
cleanup(Engine *engine)
{
    log_info("let's clean up :P");
    nk_sdl_shutdown();
    SDL_DestroyRenderer(engine->renderer);
    SDL_DestroyWindow(engine->window);
    SDL_Quit();
}
