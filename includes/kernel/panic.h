
#pragma once

void panic(const char* message);
void panic_at(uint32_t line, const char* file, const char* message);

#define PANIC(msg) panic_at(__LINE__, __FILE__, msg)
#define ASSERT(cond) if (!(cond)) PANIC("Assertion failed: " #cond);