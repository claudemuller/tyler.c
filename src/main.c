#include <stdio.h>
#include <stdbool.h>
#include "engine.h"

const bool LOG = true;

engine_t engine;

int main(void) {
	bool debug = true;

	init(&engine, debug);
	run(&engine);
	cleanup(&engine);

	return 0;
}
