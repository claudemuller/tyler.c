#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

extern bool LOG;

static void log(const char *msg)
{
    if (LOG) {
	printf("%s\n", msg);
	free((void *)msg);
    }
}

static char *build_msg(const char *emoji, const char *msg)
{
    size_t emoji_len = strlen(emoji);
    size_t str_len = strlen(msg);
    char *log_msg = (char*)malloc(emoji_len+str_len+1);

    strcpy(log_msg, emoji);
    strcat(log_msg, msg);

    return log_msg;
}

void log_info(char *msg)
{
    log(build_msg("ℹ️ ", msg));
}

void log_warn(char *msg)
{
    log(build_msg("⚠️ ", msg));
}

void log_err(char *msg, const char *err)
{
    char *str = build_msg("☠️ ", msg);
    log(build_msg(str, err));
}

char *build_msg_va(char *msg, ...)
{
    va_list args;

    // Get the total length of the result message.
    size_t len = strlen(msg);
    va_start(args, msg);
    char *sval = va_arg(args, char *);
    while (strcmp(sval, msg) != 0) {
	len += strlen(sval);
	sval = va_arg(args, char *);
    }
    va_end(args);

    // Compile the final message.
    char *res_msg = (char *)malloc(sizeof(char)*len);
    if (!res_msg) {
	fprintf(stderr, "error allocating memory for result message");
    }

    strcat(res_msg, msg);
    va_start(args, msg);
    sval = va_arg(args, char *);
    while (strcmp(sval, msg) != 0) {
	strcat(res_msg, sval);
	sval = va_arg(args, char *);
    }
    va_end(args);

    return res_msg;
}

