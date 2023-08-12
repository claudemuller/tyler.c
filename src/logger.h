#ifndef LOGGER_H
#define LOGGER_H

#define MAX(a, b) a > b ? a : b

void log_info(char *msg);
void log_warn(char *msg);
void log_err(char *msg, const char *err);
char *build_msg_va(char *msg, ...);

#endif // LOGGER_H

