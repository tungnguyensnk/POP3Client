#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define TIMEOUT 5

void loading();
char *readDataFile();
void writeDataFile(char *string);