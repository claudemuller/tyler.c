#include <stdlib.h>
#include <stdio.h>
#include "eventbus.h"
#include "logger.h"

eventbus_t init_eventbus(void)
{
 //    subscriber_t *subscribers = malloc(sizeof(subscriber_t) * 2);
 //    if (subscribers == NULL) {
	// log_err("failed to malloc subscribers", "malloc error");
 //    }

 //    eventbus_t eventbus = (eventbus_t){
	// .subscribers = subscribers,
 //    };
 //    malloc(sizeof(eventbus_t) + sizeof(subscriber_t) * 2);
 //    if (eventbus != NULL) {
	// log_err("failed to malloc eventbus", "malloc error");
 //    }

    return (eventbus_t){
	.num_subs = 0,
    };
}

void subscribe(eventbus_t *eventbus, const event_type_t type, void (*func)(event_t))
{
    subscriber_t sub = {
	.event_type = type,
	.func = func
    };
    eventbus->subscribers[eventbus->num_subs++] = sub;
}

void emit_event(eventbus_t *eventbus, const event_t ev) {
    for (size_t i = 0; i < eventbus->num_subs; i++) {
	if (eventbus->subscribers[i].event_type == ev.type) {
	    eventbus->subscribers[i].func(ev);
	}
    }
}
