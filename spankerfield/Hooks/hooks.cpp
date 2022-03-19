#include "hooks.h"
#include "../MinHook.h"
#include "../Utilities/vtablehook.h"
#include "../Utilities/other.h"
#include "../Features/main.h"
#include "../Rendering/renderer.h"
#include "../Rendering/gui.h"
#include "../global.h"

#ifdef _WIN64
#define GWL_WNDPROC GWLP_WNDPROC
#endif

namespace big
{
	namespace ScreenshotCleaner
	{
		typedef BOOL(WINAPI* tBitBlt)(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
		tBitBlt oBitBlt = nullptr;

		BOOL hkBitBlt(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
		{
			g_globals.g_fairfight = true;
			Sleep(15);
			bool result = oBitBlt(hdcDst, x, y, cx, cy, hdcSrc, x1, y1, rop);
			g_globals.g_fairfight = false;

			LOG(EVENT) << xorstr_("Fairfight screenshot taken");
			return result;
		}
	}

	namespace Present
	{
		using Present_t = HRESULT(*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
		Present_t oPresent = nullptr;

		HRESULT hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags)
		{
			const auto pDxRenderer = DxRenderer::GetInstance();
			const auto pGameRenderer = GameRenderer::GetInstance();

			if (pGameRenderer && pDxRenderer)
			{
				g_globals.g_height = pDxRenderer->m_pScreen->m_Height;
				g_globals.g_width = pDxRenderer->m_pScreen->m_Width;
				g_globals.g_viewproj = pGameRenderer->m_pRenderView->m_ViewProjection;

				if (punkbuster_check())
					LOG(EVENT) << xorstr_("Punkbuster screenshot taken");

				g_renderer->on_present();
			}

			return oPresent(pThis, SyncInterval, Flags);
		}
	}

	namespace PreFrame
	{
		using PreFrameUpdate_t = void(*)(uintptr_t pthis, uint64_t a2);
		PreFrameUpdate_t oPreFrameUpdate = nullptr;

		void hkPreFrame(uintptr_t pThis, uint64_t a2)
		{
			oPreFrameUpdate(pThis, a2);

			g_features->pre_frame();
		}
	}

	namespace WndProc
	{
		WNDPROC oWndProc;

		LRESULT hkWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			if (g_globals.g_running)
			{
				g_renderer->wndproc(hwnd, msg, wparam, lparam);

				switch (msg) {
				case WM_SIZE:
					if (g_renderer->m_d3d_device != NULL && wparam != SIZE_MINIMIZED)
					{
						g_renderer->pre_reset();
						g_renderer->m_dxgi_swapchain->ResizeBuffers(0, (UINT)LOWORD(lparam), (UINT)HIWORD(lparam), DXGI_FORMAT_UNKNOWN, 0);
						g_renderer->post_reset();
					}

					return false;
				}

				if (g_gui.m_opened)
				{
					switch (msg) {
					case WM_MOUSEMOVE: return false;
					default:
						break;
					}

					return true;
				}
			}

			return CallWindowProcW(oWndProc, hwnd, msg, wparam, lparam);
		}
	}

	void hooking::initialize()
	{
		MH_Initialize();
	}

	void hooking::uninitialize()
	{
		MH_Uninitialize();
	}

	void hooking::enable()
	{
		const auto dx_renderer = DxRenderer::GetInstance();
		const auto border_input_node = BorderInputNode::GetInstance();
		static bool terminate{ false };

		while (dx_renderer && border_input_node)
		{
			if (terminate) break;

			WndProc::oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g_globals.g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc::hkWndProc)));
			LOG(INFO) << xorstr_("Hooked WndProc.");

			MH_CreateHook((*reinterpret_cast<void***>(dx_renderer->m_pScreen->m_pSwapChain))[8], Present::hkPresent, reinterpret_cast<PVOID*>(&Present::oPresent));
			MH_EnableHook((*reinterpret_cast<void***>(dx_renderer->m_pScreen->m_pSwapChain))[8]);
			LOG(INFO) << xorstr_("Hooked Present.");

			MH_CreateHook(&BitBlt, &ScreenshotCleaner::hkBitBlt, reinterpret_cast<LPVOID*>(&ScreenshotCleaner::oBitBlt));
			MH_EnableHook(&BitBlt);
			LOG(INFO) << xorstr_("Hooked BitBlt.");

			PreFrame::oPreFrameUpdate = reinterpret_cast<PreFrame::PreFrameUpdate_t>(hook_vtable_func(reinterpret_cast<PDWORD64*>(border_input_node->m_Vtable), reinterpret_cast<PBYTE>(&PreFrame::hkPreFrame), 3));
			LOG(INFO) << xorstr_("Hooked PreFrame.");

			terminate = !terminate;
		}
	}

	void hooking::disable()
	{
		SetWindowLongPtrW(g_globals.g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc::oWndProc));
		LOG(INFO) << xorstr_("Disabled WndProc.");

		MH_DisableHook(&BitBlt);
		LOG(INFO) << xorstr_("Disabled BitBlt.");

		MH_DisableHook((*reinterpret_cast<void***>(DxRenderer::GetInstance()->m_pScreen->m_pSwapChain))[8]);
		LOG(INFO) << xorstr_("Disabled Present.");

		hook_vtable_func(reinterpret_cast<PDWORD64*>(BorderInputNode::GetInstance()->m_Vtable), reinterpret_cast<PBYTE>(PreFrame::oPreFrameUpdate), 3);
		LOG(INFO) << xorstr_("Disabled PreFrame.");
	}
}