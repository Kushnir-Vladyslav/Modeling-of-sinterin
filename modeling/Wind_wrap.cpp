#include "Wind_wrap.h"

void Run_thread(const wchar_t* Title, int width, int height, HDC* hdc)
{
	Window* window = new Window({ Title, width, height });
	*hdc = GetDC(window->GetHWND());
	bool running = true;
	while (running)
	{
		if (!window->ProcessMassages())
		{

			running = false;
		}

		Sleep(10);
	}
}

Wind_wrap::Wind_wrap(const wchar_t* Title, int width, int height) : hdc(0), t(Run_thread, Title, width, height, &hdc)
{
	t.detach();

	{
		BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
		BitmapInfo.bmiHeader.biWidth = width;
		// Negative height makes top left as the coordinate system origin for the DrawPixel function, otherwise its bottom left
		BitmapInfo.bmiHeader.biHeight = -height;
		BitmapInfo.bmiHeader.biPlanes = 1;
		BitmapInfo.bmiHeader.biBitCount = 32;
		BitmapInfo.bmiHeader.biCompression = BI_RGB;
	}

	this->ClientWidth = width;
	this->ClientHeight = height;
	
	Sleep(100);
}



void Wind_wrap::Draw(void* BitmapMemory, int BitmapWidth, int BitmapHeight)
{
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	StretchDIBits(hdc,
		0, 0,
		ClientWidth, ClientHeight,
		0, 0,
		BitmapWidth, BitmapHeight,
		BitmapMemory, &BitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}



Wind_wrap::~Wind_wrap()
{

}


void SetPixel(void* BitmapMemory, int x, int y, int width, COLORREF color)
{

	UINT32* Pixel = (UINT32*)BitmapMemory;
	Pixel += y * width + x;
	*Pixel = color;
}