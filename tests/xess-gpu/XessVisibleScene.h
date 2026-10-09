#pragma once
#include "InteropTestRig.h"
#include <dxgi1_4.h>
#include <string>
// Test-only presentation. Product XeSS adds no new swap-chain owner.
class XessVisibleScene final
{
public:
    ~XessVisibleScene(){chain_.Reset();if(window_)DestroyWindow(window_);}
    void Create(InteropFixture::Rig& rig,unsigned width,unsigned height)
    {
        WNDCLASSW type{};type.lpfnWndProc=Procedure;type.hInstance=GetModuleHandleW(nullptr);type.lpszClassName=L"RaZkolbaS-XeSS-SR-Probe";type.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        RegisterClassW(&type);RECT area{0,0,static_cast<LONG>(width),static_cast<LONG>(height)};AdjustWindowRect(&area,WS_OVERLAPPEDWINDOW,FALSE);
        window_=CreateWindowW(type.lpszClassName,L"RaZkolbaS XeSS SR — synthetic scene",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,area.right-area.left,area.bottom-area.top,nullptr,nullptr,type.hInstance,this);
        InteropFixture::Require(window_!=nullptr,"visible scene window");
        DXGI_SWAP_CHAIN_DESC1 desc{};desc.Width=width;desc.Height=height;desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;
        desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=2;desc.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> chain;
        InteropFixture::Check(rig.factory->CreateSwapChainForHwnd(rig.device11.Get(),window_,&desc,nullptr,nullptr,&chain),"synthetic D3D11 presentation");
        InteropFixture::Check(chain.As(&chain_),"indexed test presentation");rig.factory->MakeWindowAssociation(window_,DXGI_MWA_NO_ALT_ENTER);
        ShowWindow(window_,SW_SHOW);UpdateWindow(window_);
    }
    bool Pump()
    {
        MSG message{};while(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}
        return !closed_;
    }
    void Mode(const wchar_t* name)
    { const std::wstring title=std::wstring(L"RaZkolbaS XeSS ")+name+L" — watch checker edges and camera motion";SetWindowTextW(window_,title.c_str()); }
    void Present(InteropFixture::Rig& rig,ID3D11Texture2D* image)
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> back;
        InteropFixture::Check(chain_->GetBuffer(chain_->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&back)),"visible test backbuffer");
        rig.context11->CopyResource(back.Get(),image);
        InteropFixture::Check(chain_->Present(1,0),"visible test present");
    }
private:
    static LRESULT CALLBACK Procedure(HWND window,UINT message,WPARAM wParam,LPARAM lParam)
    {
        if(message==WM_NCCREATE){const auto* creation=reinterpret_cast<CREATESTRUCTW*>(lParam);SetWindowLongPtrW(window,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(creation->lpCreateParams));}
        auto* self=reinterpret_cast<XessVisibleScene*>(GetWindowLongPtrW(window,GWLP_USERDATA));
        if(self && message==WM_CLOSE){self->closed_=true;ShowWindow(window,SW_HIDE);return 0;}
        return DefWindowProcW(window,message,wParam,lParam);
    }
    HWND window_{};bool closed_{};Microsoft::WRL::ComPtr<IDXGISwapChain3> chain_;
};
