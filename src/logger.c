#include <stdio.h>
#include "logger.h"

void log_info(const char *msg) {
    printf("ℹ️ %s\n", msg);
}

void log_warn(const char *msg) {
    printf("⚠️ %s\n", msg);
}

void log_err(const char *msg) {
    printf("☠️ %s\n", msg);
}
