#ifndef DIRWALK_H
#define DIRWALK_H


#include <sys/types.h>
#include <sys/stat.h>

int should_print(const struct stat *statbuf, int opt_l, int opt_d, int opt_f);
void print_entry(const char *dir, const char *name);
void dirwalk(const char *dir, int opt_l, int opt_d, int opt_f, int opt_s);

#endif 
