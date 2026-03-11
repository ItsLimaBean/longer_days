#pragma once
#include "pch.h"


namespace utility
{
	static bool is_key_pressed(int key)
	{
		static const HWND game = FindWindowA("sgaWindow", "Red Dead Redemption 2");
		static int key_tmr = timeGetTime();

		const SHORT key_pressed = GetForegroundWindow() == game ? GetAsyncKeyState(key) & 0x8000 : 0;

		const int now = timeGetTime();
		bool is_pressed_and_timer_expired = key_pressed && (now - key_tmr) >= 200;
		if (is_pressed_and_timer_expired)
			key_tmr = now;

		return is_pressed_and_timer_expired;
	}

	static void draw_text(float x, float y, std::string str, bool centre = false)
	{
		HUD::SET_TEXT_SCALE(0.342f, 0.342f);
		HUD::SET_TEXT_CENTRE(centre);
		HUD::_SET_TEXT_COLOR(255, 0, 0, 255);

		const char* text = MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", str.c_str());
		HUD::_DRAW_TEXT(text, x, y);
	}
}



namespace debug
{
	static void draw_text_line(int& line, std::stringstream& text)
	{
		utility::draw_text(0.0f, line++ * 0.03f, text.str(), false);
		text.str(std::string());
	}
}