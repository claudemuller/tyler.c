#ifndef EVENTBUS_H_
#define EVENTBUS_H_

#include <stdlib.h>
#include "vector.h"

#define MAX_SUBS 50
#define MAX_FILENAME 255

typedef enum {
	EVENT_MOUSE_MOVE,
	EVENT_NK_MOUSE_CLICK,
	EVENT_SDL_MOUSE_CLICK,
	EVENT_LOAD_TILEMAP
} event_type_t;

typedef struct {
	event_type_t type;
	union {
		vec2_t pos;
		char filename[MAX_FILENAME];
	};
} event_t;

typedef struct {
	void (*func)(event_t);
	event_type_t event_type;
} subscriber_t;

typedef struct {
	size_t num_subs;
	subscriber_t subscribers[MAX_SUBS];
} eventbus_t;

eventbus_t init_eventbus(void);
void subscribe(eventbus_t *eventbus, const event_type_t type, void (*func)(event_t));
void emit_event(eventbus_t *eventbus, const event_t ev);

#endif // EVENTBUS_H_
