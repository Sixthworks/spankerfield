#include "hooks.h"
#include "../MinHook.h"
#include "../Utilities/other.h"
#include "../Utilities/thread_pool.h"
#include "../Rendering/renderer.h"
#include "../Rendering/gui.h"
#include "../Features/main.h"
#include "../settings.h"
#include "../global.h"

#define GWL_WNDPROC GWLP_WNDPROC

namespace big
{
	namespace ScreenshotCleaner
	{
		// FFSS
		typedef BOOL(WINAPI* tBitBlt)(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
		tBitBlt oBitBlt = nullptr;

		BOOL WINAPI hkBitBlt(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
		{
			LOG(INFO) << xorstr_("FairFight initiated a screenshot.");
			g_globals.g_fairfight = true;
			g_globals.screenshots_ff++;

			bool result = FALSE;
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				if (oBitBlt)
					result = oBitBlt(hdcDst, x, y, cx, cy, hdcSrc, x1, y1, rop);
			}

			g_globals.g_fairfight = false;
			return result;
		}

		// PBSS
		using takeScreenshot_t = void(__thiscall*)(void* pThis);
		takeScreenshot_t oTakeScreenshot = nullptr;

		void __fastcall hkTakeScreenshot(void* pThis)
		{
			LOG(INFO) << xorstr_("PunkBuster initiated a screenshot (using a delay of ") << g_settings.screenhots_pb_delay << xorstr_("ms)");
			g_globals.g_punkbuster = true;
			g_globals.screenshots_pb++;

			{
				std::this_thread::sleep_for(std::chrono::milliseconds(g_settings.screenhots_pb_delay)); // Using the desired delay of the user
				if (oTakeScreenshot)
					oTakeScreenshot(pThis);
			}

			g_globals.g_punkbuster = false;
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

			if (IsValidPtr(renderer) && IsValidPtr(game_renderer))
			{
				const auto screen = renderer->m_pScreen;
				if (IsValidPtr(screen))
				{
					g_globals.g_height = renderer->m_pScreen->m_Height;
					g_globals.g_width = renderer->m_pScreen->m_Width;
					g_globals.g_viewproj = game_renderer->m_pRenderView->m_ViewProjection;

					// AA flag check + Hooked PB Take Screenshot Function + Hooked BitBlt
					g_globals.g_should_draw = !punkbuster_capturing() && !g_globals.g_punkbuster && !g_globals.g_fairfight;

					if (g_globals.g_should_draw)
						g_renderer->on_present();
				}
			}

			return oPresent(pThis, SyncInterval, Flags);
		}
	}

	namespace PreFrame
	{
		std::unique_ptr<VMTHook> pPreFrameHook;

		int __fastcall PreFrameUpdate(void* ecx, void* edx, float delta_time)
		{
			static auto oPreFrameUpdate = pPreFrameHook->GetOriginal<PreFrameUpdate_t>(3);
			auto result = oPreFrameUpdate(ecx, edx, delta_time);
			
			g_features->pre_frame(delta_time);

			return result;
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

				switch (msg)
				{
				case WM_SIZE:
					if (g_renderer->m_d3d_device != NULL && wparam != SIZE_MINIMIZED)
					{
						g_renderer->pre_reset();
						g_renderer->m_dxgi_swapchain->ResizeBuffers(0, (UINT)LOWORD(lparam), (UINT)HIWORD(lparam), DXGI_FORMAT_UNKNOWN, 0);
						g_renderer->post_reset();
					}

					return false;
				case WM_SYSCOMMAND:
					if ((wparam & 0xfff0) == SC_KEYMENU)
						return false;

					break;
				}

				if (g_gui.m_opened)
				{
					switch (msg)
					{
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

		// We do a while loop and break it, this way we wait both for renderer and border_input_node to initialize
		while (IsValidPtr(renderer) && IsValidPtr(border_input_node))
		{
			if (terminate) break;

			WndProc::oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g_globals.g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc::hkWndProc)));
			LOG(INFO) << xorstr_("Hooked WndProc.");

			HMODULE hGdi32 = GetModuleHandleA(xorstr_("Gdi32.dll"));
			if (!hGdi32)
			{
				LOG(INFO) << xorstr_("Failed to get handle for Gdi32.dll.");
				return;
			}

			void* pBitBlt = GetProcAddress(hGdi32, xorstr_("BitBlt"));
			if (!pBitBlt)
			{
				LOG(INFO) << xorstr_("Failed to get address of BitBlt.");
				return;
			}

			MH_CreateHook(pBitBlt, &ScreenshotCleaner::hkBitBlt, reinterpret_cast<LPVOID*>(&ScreenshotCleaner::oBitBlt));
			MH_EnableHook(pBitBlt);
			LOG(INFO) << xorstr_("Hooked BitBlt.");

			MH_CreateHook(reinterpret_cast<void*>(OFFSET_TAKESCREENSHOT), &ScreenshotCleaner::hkTakeScreenshot, reinterpret_cast<PVOID*>(&ScreenshotCleaner::oTakeScreenshot));
			MH_EnableHook(reinterpret_cast<void*>(OFFSET_TAKESCREENSHOT));
			LOG(INFO) << xorstr_("Hooked PBSS.");

			MH_CreateHook((*reinterpret_cast<void***>(renderer->m_pScreen->m_pSwapChain))[8], Present::hkPresent, reinterpret_cast<PVOID*>(&Present::oPresent));
			MH_EnableHook((*reinterpret_cast<void***>(renderer->m_pScreen->m_pSwapChain))[8]);
			LOG(INFO) << xorstr_("Hooked Present.");

			PreFrame::pPreFrameHook = std::make_unique<VMTHook>();
			PreFrame::pPreFrameHook->Setup(border_input_node->m_Vtable);
			PreFrame::pPreFrameHook->Hook(3, PreFrame::PreFrameUpdate);
			LOG(INFO) << xorstr_("Hooked PreFrameUpdate.");

			terminate = true;
		}
	}

	void hooking::disable()
	{
		PreFrame::pPreFrameHook->Release();
		LOG(INFO) << xorstr_("Disabled PreFrameUpdate.");

		MH_DisableHook((*reinterpret_cast<void***>(DxRenderer::GetInstance()->m_pScreen->m_pSwapChain))[8]);
		LOG(INFO) << xorstr_("Disabled Present.");

		MH_DisableHook(&BitBlt);
		LOG(INFO) << xorstr_("Disabled BitBlt.");

		MH_DisableHook(reinterpret_cast<void*>(OFFSET_TAKESCREENSHOT));
		LOG(INFO) << xorstr_("Disabled PBSS.");

		if (WndProc::oWndProc)
		{
			SetWindowLongPtrW(g_globals.g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc::oWndProc));
			LOG(INFO) << xorstr_("Disabled WndProc.");
		}
	}
}