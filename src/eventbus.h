#ifndef EVENTBUS_H
#define EVENTBUS_H

typedef enum events {
	EVENT_MOUSE_MOVE
} event_type_t;

typedef struct subscribers {
	void (*func)(void);
	event_type_t type;
} subscriber_t;

typedef struct eventbus {
	void (*subscribers)(void);
} eventbus_t;

eventbus_t init_eventbus(void);
void subscribe(eventbus_t eventbus, const event_type_t type, void (*func)(void));
void emit_event(eventbus_t eventbus, const event_type_t type);

#endif // EVENTBUS_H
