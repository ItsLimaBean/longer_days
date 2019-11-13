#pragma once
class LongerDays
{
public:
	void Tick();

	bool show_welcome = true;
	float multiplier = 1.0f;
private:
	void UpdateGameTime();

	static constexpr float hour_multiplier[] = {
	1.99f,
	1.70f,
	1.50f,
	1.43f,
	1.34f,
	1.15f,
	1.00f,
	0.87f,
	0.77f,
	0.76f,
	0.752f,
	0.71f,
	0.67f,
	0.71f,
	0.752f,
	0.8f,
	0.82f,
	0.85f,
	0.953f,
	1.15f,
	1.34f,
	1.43f,
	1.54f,
	1.76f
	};
};

