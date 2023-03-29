#include <stdlib.h>
#include <stdio.h>
#include "eventbus.h"
#include "logger.h"

eventbus_t
init_eventbus()
{
    subscriber_t subscribers[2];
    printf("%zu\n", sizeof(subscribers));
 //    if (!subscribers) {
	// log_err("failed to malloc subscribers space");
 //    }
    return (eventbus_t){
	// .subscribers = malloc(16)
    };
}

void
subscribe(eventbus_t eventbus, const event_type_t type, void (*func)(void))
{
    subscriber_t sub = {
	.type = type,
	.func = func
    };
    eventbus.subscribers = func;
}

void
emit_event(eventbus_t eventbus, const event_type_t type) {
    eventbus.subscribers();
}
