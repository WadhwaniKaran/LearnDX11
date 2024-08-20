#pragma once
#include "MyWin.h"
#include "MyException.h"

class Window
{
public:
	class WindowException: public MyException
	{
	public:
		WindowException(int line, const char* file, HRESULT errorCode) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		static std::string TranslateErrorCode(HRESULT errorCode) noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT m_errorCode;
	};
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
	Window(unsigned int Width, unsigned int Height, const wchar_t* Title);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(Window&) = delete;

private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT WINAPI HandleMsgThump(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	unsigned int m_width;
	unsigned int m_height;
	HWND m_hWnd;
};

// exception helper macros
#define MYWND_EXECPT(errorCode) Window::WindowException(__LINE__, __FILE__, errorCode)
#define MYWND_LAST_EXCEPT() Window::WindowException(__LINE__, __FILE__, GetLastError())