#include "fmt.h"

#include <cstdio>

fmt::fmt(fmt_col col, const char* fmt, ...)
{
	char buffer[4096];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (col != fc_none) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
	}

	printf("%s", buffer);

	if (col != fc_none) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fc_reset);
	}
}