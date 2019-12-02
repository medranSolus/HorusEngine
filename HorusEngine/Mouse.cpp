#include "Mouse.h"
#include "WinAPI.h"

namespace WinAPI
{
	void Mouse::OnLeftDown(int x, int y) noexcept
	{
		left = true;
		eventBuffer.emplace_back(Event::Type::LeftDown, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnLeftUp(int x, int y) noexcept
	{
		left = false;
		eventBuffer.emplace_back(Event::Type::LeftUp, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnRightDown(int x, int y) noexcept
	{
		right = true;
		eventBuffer.emplace_back(Event::Type::RightDown, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnRightUp(int x, int y) noexcept
	{
		right = false;
		eventBuffer.emplace_back(Event::Type::RightUp, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnWheelDown(int x, int y) noexcept
	{
		wheel = true;
		eventBuffer.emplace_back(Event::Type::WheelDown, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnWheelUp(int x, int y) noexcept
	{
		wheel = false;
		eventBuffer.emplace_back(Event::Type::WheelUp, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnWheelForward(int x, int y) noexcept
	{
		eventBuffer.emplace_back(Event::Type::WheelForward, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnWheelBackward(int x, int y) noexcept
	{
		eventBuffer.emplace_back(Event::Type::WheelBackward, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnWheelRotation(int x, int y, int rotation) noexcept
	{
		// Rotation for high precision mouse wheels https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
		wheelRotation += rotation;
		while (wheelRotation >= WHEEL_DELTA)
		{
			wheelRotation -= WHEEL_DELTA;
			OnWheelForward(x, y);
		}
		while (wheelRotation <= -WHEEL_DELTA)
		{
			wheelRotation += WHEEL_DELTA;
			OnWheelBackward(x, y);
		}
	}

	void Mouse::OnMouseMove(int x, int y) noexcept
	{
		eventBuffer.emplace_back(Event::Type::Move, *this, x, y);
		this->x = x;
		this->y = y;
		TrimBuffer();
	}

	void Mouse::OnEnter() noexcept
	{
		window = true;
		eventBuffer.emplace_back(Event::Type::Enter, *this, x, y);
		TrimBuffer();
	}

	void Mouse::OnLeave() noexcept
	{
		window = false;
		eventBuffer.emplace_back(Event::Type::Leave, *this, x, y);
		TrimBuffer();
	}

	void Mouse::TrimBuffer() noexcept
	{
		while (eventBuffer.size() > bufferSize)
			eventBuffer.pop_back();
	}

	std::optional<Mouse::Event> Mouse::Read() noexcept
	{
		if (eventBuffer.size() > 0)
		{
			Event input = eventBuffer.front();
			eventBuffer.pop_front();
			return input;
		}
		return {};
	}
}
