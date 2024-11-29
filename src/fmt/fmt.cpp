#include "fmt.h"

#include <iostream>
#include <thread>

#define MSG_BUFFER_SIZE 4096

fmt::fmt(fmt_mode mode, fmt_col col, const char* fmt, ...)
{
	char buffer[MSG_BUFFER_SIZE];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (col != fc_none) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
	}

	if (mode != fmt_def) {
		char* word = strtok(buffer, " ");
		while (word != NULL) {
			std::cout << " " << word;
			if (mode == fmt_15ms)
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
			word = strtok(NULL, " ");
		}
		std::cout << std::endl;
	}
	else {
		std::cout << buffer;
	}

	if (col != fc_none) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fc_reset);
	}
}