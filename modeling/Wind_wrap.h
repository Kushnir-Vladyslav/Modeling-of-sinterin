#pragma once
#include <thread>
#include "Window.h"

class Wind_wrap 
{
public:
	Wind_wrap(const wchar_t* Title, int width, int height);
	Wind_wrap() = delete;
	Wind_wrap(const Wind_wrap&) = delete;
	Wind_wrap operator = (const Wind_wrap&) = delete;
	void Draw(void* BitmapMemory, int BitmapWidth, int BitmapHeight);
	~Wind_wrap();
private:
	HDC hdc;
	BITMAPINFO BitmapInfo;

	int ClientWidth;
	int ClientHeight;

	std::thread t;
};

void SetPixel(void* BitmapMemory, int x, int y, int width, COLORREF color);