#pragma once
class LongerDays
{
public:
	void Tick();
	void ReadConfig(std::wstring path);

	bool show_welcome = true;

	float day_time = 1.0f;
	float night_time = 1.0f;

	int day_start = 7;
	int day_end = 19;
private:
	void UpdateGameTime();
	float GetTimeFromHour(int hour);
};

