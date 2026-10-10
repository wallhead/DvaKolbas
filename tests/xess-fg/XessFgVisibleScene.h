#pragma once
#include "InteropTestRig.h"
#include <string>

// Window only: all presentation is owned by the real Intel SDK proxy.
class XessFgVisibleScene final
{
public:
    ~XessFgVisibleScene() { if (window_) DestroyWindow(window_); }
    void Create(unsigned width,unsigned height)
    {
        WNDCLASSW wc{};wc.lpfnWndProc=Procedure;wc.hInstance=GetModuleHandleW(nullptr);
        wc.lpszClassName=L"RaZkolbaS-XeSS-FG-Probe";wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        RegisterClassW(&wc);
        RECT area{0,0,static_cast<LONG>(width),static_cast<LONG>(height)};
        AdjustWindowRect(&area,WS_OVERLAPPEDWINDOW,FALSE);
        window_=CreateWindowW(wc.lpszClassName,L"RaZkolbaS XeSS FG — initializing Intel runtime",WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,CW_USEDEFAULT,area.right-area.left,area.bottom-area.top,nullptr,nullptr,wc.hInstance,this);
        InteropFixture::Require(window_!=nullptr,"Intel scene window");
        ShowWindow(window_,SW_SHOW);UpdateWindow(window_);SetForegroundWindow(window_);
    }
    HWND Handle() const { return window_; }
    bool Pump()
    {
        MSG message{};
        while (PeekMessageW(&message,nullptr,0,0,PM_REMOVE)) { TranslateMessage(&message);DispatchMessageW(&message); }
        return !closed_;
    }
    void Mode(const wchar_t* label)
    { const auto title=std::wstring(L"RaZkolbaS XeSS FG — ")+label+L" — red/green HUD and white crosshair";SetWindowTextW(window_,title.c_str()); }
private:
    static LRESULT CALLBACK Procedure(HWND window,UINT message,WPARAM wParam,LPARAM lParam)
    {
        if (message==WM_NCCREATE) SetWindowLongPtrW(window,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams));
        auto* self=reinterpret_cast<XessFgVisibleScene*>(GetWindowLongPtrW(window,GWLP_USERDATA));
        if (self && message==WM_CLOSE) { self->closed_=true;ShowWindow(window,SW_HIDE);return 0; }
        return DefWindowProcW(window,message,wParam,lParam);
    }
    HWND window_{};bool closed_{};
};
