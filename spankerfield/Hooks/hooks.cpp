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
			return result;
		}
	}

	namespace Present
	{
		using Present_t = HRESULT(*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
		Present_t oPresent = nullptr;

		HRESULT hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags)
		{
			const auto renderer = DxRenderer::GetInstance();
			const auto game_renderer = GameRenderer::GetInstance();

			if (renderer && game_renderer)
			{
				g_globals.g_height = renderer->m_pScreen->m_Height;
				g_globals.g_width = renderer->m_pScreen->m_Width;
				g_globals.g_viewproj = game_renderer->m_pRenderView->m_ViewProjection;

				punkbuster_check();

				g_renderer->on_present();
			}

			return oPresent(pThis, SyncInterval, Flags);
		}
	}

	namespace PreFrame
	{
		using PreFrameUpdate_t = void(*)(float dt);
		PreFrameUpdate_t oPreFrameUpdate = nullptr;

		void hkPreFrame(float delta_time)
		{
			oPreFrameUpdate(delta_time);
			
			g_features->pre_frame(delta_time);
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
		const auto renderer = DxRenderer::GetInstance();
		const auto border_input_node = BorderInputNode::GetInstance();
		bool terminate{};

		while (renderer && border_input_node)
		{
			if (terminate) break;

			WndProc::oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g_globals.g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc::hkWndProc)));
			LOG(INFO) << xorstr_("Hooked WndProc.");

			MH_CreateHook((*reinterpret_cast<void***>(renderer->m_pScreen->m_pSwapChain))[8], Present::hkPresent, reinterpret_cast<PVOID*>(&Present::oPresent));
			MH_EnableHook((*reinterpret_cast<void***>(renderer->m_pScreen->m_pSwapChain))[8]);
			LOG(INFO) << xorstr_("Hooked Present.");

			MH_CreateHook(&BitBlt, &ScreenshotCleaner::hkBitBlt, reinterpret_cast<LPVOID*>(&ScreenshotCleaner::oBitBlt));
			MH_EnableHook(&BitBlt);
			LOG(INFO) << xorstr_("Hooked BitBlt.");

			PreFrame::oPreFrameUpdate = reinterpret_cast<PreFrame::PreFrameUpdate_t>(hook_vtable_func(reinterpret_cast<PDWORD64*>(border_input_node->m_Vtable), reinterpret_cast<PBYTE>(&PreFrame::hkPreFrame), 3));
			LOG(INFO) << xorstr_("Hooked PreFrame.");

			terminate = true;
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