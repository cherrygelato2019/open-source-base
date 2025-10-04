#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "init_cheat/init.hpp"
using namespace std;

void setupConsole() {
	HWND console = GetConsoleWindow();
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT windowSize = { 0, 0, 60, 20 };
	SetConsoleWindowInfo(hOut, TRUE, &windowSize);
	COORD bufferSize = { 61, 21 };
	SetConsoleScreenBufferSize(hOut, bufferSize);
	SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	ShowScrollBar(console, SB_BOTH, FALSE);
	SetConsoleTitleA("");
	system("cls");
}

int main() {
	setupConsole();
	init::cheat::load();
}
