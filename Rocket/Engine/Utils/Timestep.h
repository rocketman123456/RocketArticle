#pragma once

// Should Give Millisecond
class Timestep
{
public:
	Timestep(float time = 0.0f) : time_(time) {}

	operator float() const { return time_; }

	float GetSeconds() const { return time_ * 0.001f; }
	float GetMilliseconds() const { return time_; }

private:
	float time_;
};
