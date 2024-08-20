#pragma once
#include "MyWin.h"

class Window
{
private:
	// singleton class for registering and deregistering window class
	class WindowClass {
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(WindowClass&) = delete;

		static constexpr const wchar_t* m_wndClassName = L"DX11 Window Class";
		static WindowClass m_wndClass;
		HINSTANCE m_hInstance;
	};

public:
	Window(u32 Width, u32 Height, const wchar_t* Title) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(Window&) = delete;

private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI HandleMsgThump(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	u32 m_width;
	u32 m_height;
	HWND m_hWnd;
};

