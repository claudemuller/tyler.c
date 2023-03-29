#include <stdio.h>
#include <stdbool.h>
#include "logger.h"

const bool LOG = false;

char
*log_info(char *msg)
{
    // printf("ℹ️ %s\n", msg);
    return msg;
}

char
*log_warn(char *msg)
{
    // printf("⚠️ %s\n", msg);
    return msg;
}

char
*log_err(char *msg, const char *err)
{
    // printf("☠️ %s: %s\n", msg, err);
    return msg;
}

void
_log(const char *msg)
{
    if (LOG)
	printf("%s\n", msg);
}

