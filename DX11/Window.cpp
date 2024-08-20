#include "Window.h"
#include <sstream>

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

Window::Window(unsigned int Width, unsigned int Height, const wchar_t* Title)
{
	RECT r{};
	r.left = 100;
	r.right = Width + r.left;
	r.top = 100;
	r.bottom = Height + r.top;

	// NOTE: check
	m_width = Width;
	m_height = Height;

	
	if (FAILED(AdjustWindowRectEx(&r,
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
			FALSE, WS_EX_CLIENTEDGE))
	)
	{
		throw MYWND_LAST_EXCEPT();
	}

	m_hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE, WindowClass::GetName(), Title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	if (m_hWnd == nullptr)
	{
		throw MYWND_LAST_EXCEPT();
	}

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

// Window Exception Stuff

Window::WindowException::WindowException(int line, const char* file, HRESULT errorCode) noexcept
	:MyException(line, file), m_errorCode(errorCode)
{}

const char* Window::WindowException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code]: " << GetErrorCode() << std::endl
		<< "[Description]: " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char* Window::WindowException::GetType() const noexcept
{
	return "My Window Exception";
}

HRESULT Window::WindowException::GetErrorCode() const noexcept
{
	return m_errorCode;
}

std::string Window::WindowException::TranslateErrorCode(HRESULT errorCode) noexcept
{
	wchar_t* pMsgBuf = nullptr;
	DWORD msgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf),
		0,
		nullptr
	);

	if (msgLen == 0) {
		return "Unknown Error Code";
	}

	std::wstring errorWString = pMsgBuf;
	LocalFree(pMsgBuf);

	std::string errorString;
	errorString.assign(errorWString.begin(), errorWString.end());

	return errorString;

}

std::string Window::WindowException::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_errorCode);
}
