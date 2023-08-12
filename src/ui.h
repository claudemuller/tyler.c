#ifndef UI_H_
#define UI_H_

#include "eventbus.h"
#include "engine.h"

void ui_mouse_click(event_t ev_data);
void draw_ui(engine_t *engine);

#endif // UI_H_
