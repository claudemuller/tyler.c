#include <stdio.h>
#include <stdbool.h>
#include "engine.h"

int main(int argc, char* argv[]) {
	bool debug = true;

	Engine engine;

	init(&engine, debug);
	run(&engine);
	cleanup(&engine);

	return 0;
}
