#include <stdio.h>
#include <stdbool.h>
#include "engine.h"

int main(void) {
	bool debug = true;

	engine_t engine;

	init(&engine, debug);
	run(&engine);
	cleanup(&engine);

	return 0;
}
