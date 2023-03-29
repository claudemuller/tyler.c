#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"

const bool LOG = true;

void
log_info(char *msg)
{
    _log(_build_msg("ℹ️ ", msg));
}

void
log_warn(char *msg)
{
    _log(_build_msg("⚠️ ", msg));
}

void
log_err(char *msg, const char *err)
{
    char *str = _build_msg("☠️ ", msg);
    _log(_build_msg(str, err));
}

void
_log(const char *msg)
{
    if (LOG)
	printf("%s\n", msg);
}

char
*_build_msg(const char *emoji, const char *msg)
{
    size_t emoji_len = strlen(emoji);
    size_t str_len = strlen(msg);
    char *log_msg = (char*)malloc(emoji_len+str_len+1);

    strcpy(log_msg, emoji);
    strcat(log_msg, msg);

    return log_msg;
}

