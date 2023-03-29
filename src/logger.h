#ifndef LOGGER_H
#define LOGGER_H

void log_info(char *msg);
void log_warn(char *msg);
void log_err(char *msg, const char *err);

void _log(const char *msg);
char *_build_msg(const char *emoji, const char *msg);

#endif // LOGGER_H

