#include "Window.h"

Window::WindowClass Window::WindowClass::m_wndClass;

Window::WindowClass::WindowClass() noexcept
	: m_hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup; // only for setup
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(m_wndClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return m_wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return m_wndClass.m_hInstance;
}

Window::Window(u32 Width, u32 Height, const wchar_t* Title) noexcept
{
	RECT r{};
	r.left = 100;
	r.right = Width + r.left;
	r.top = 100;
	r.bottom = Height + r.top;

	// NOTE: check
	m_width = Width;
	m_height = Height;

	AdjustWindowRectEx(&r, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE, WS_EX_CLIENTEDGE);

	m_hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE, WindowClass::GetName(), Title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(m_hWnd);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(createStruct->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		
		// Set the actual message proc
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThump));

		return pWnd->HandleMsg(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMsgThump(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return pWnd->HandleMsg(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
