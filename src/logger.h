#ifndef LOGGER_H
#define LOGGER_H

char *log_info(char *msg);
char *log_warn(char *msg);
char *log_err(char *msg, const char *err);

void _log(const char *msg);

#endif // LOGGER_H

