#include "hooks.h"
#include <MinHook/MinHook.h>
#include "../Utilities/other.h"
#include "../Utilities/auto_yield.h"
#include "../Utilities/texture_saver.h"
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
		// -------------------------------------------------------------------------
		// FairFight (BitBlt)
		// -------------------------------------------------------------------------
		typedef BOOL(WINAPI* tBitBlt)(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
		tBitBlt oBitBlt = nullptr;

		std::mutex ff_screenshot_mutex;

		class scoped_ff_reset
		{
		public:
			scoped_ff_reset() { g_globals.g_fairfight = true; }
			~scoped_ff_reset() { g_globals.g_fairfight = false; }
		};

		BOOL WINAPI hkBitBlt(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
		{
			LOG(INFO) << xorstr_("FairFight initiated a screenshot.");
			g_globals.g_screenshots_ff++;

			std::lock_guard<std::mutex> lock(ff_screenshot_mutex);

			scoped_ff_reset reset_fairfight;
			auto_yield(std::chrono::milliseconds(50));

			BOOL result = FALSE;
			if (oBitBlt)
				result = oBitBlt(hdcDst, x, y, cx, cy, hdcSrc, x1, y1, rop);

			return result;
		}

		// -------------------------------------------------------------------------
		// PunkBuster (TakeScreenshot / CopySubresourceRegion)
		// -------------------------------------------------------------------------
		using takeScreenshot_t = void(__thiscall*)(void* pThis);
		takeScreenshot_t oTakeScreenshot = nullptr;

		std::mutex pb_screenshot_mutex;

		class scoped_pb_reset
		{
		public:
			scoped_pb_reset() { g_globals.g_punkbuster = true; }
			~scoped_pb_reset() { g_globals.g_punkbuster = false; }
		};

		void __fastcall hkTakeScreenshot(void* pThis)
		{
			g_globals.g_screenshots_pb++;
			LOG(INFO) << xorstr_("PunkBuster initiated a screenshot [hkTakeScreenshot]");

			if (g_settings.screenshots_pb_temp_disable || !g_settings.screenshots_pb_clean)
			{
				LOG(INFO) << xorstr_("PunkBuster initiated a screenshot [hkTakeScreenshot] (using a delay of ")
					<< g_settings.screenhots_pb_delay << xorstr_("+")
					<< static_cast<int>(g_settings.screenhots_post_pb_delay) << xorstr_("ms)");

				std::lock_guard<std::mutex> lock(pb_screenshot_mutex);

				scoped_pb_reset reset_punkbuster;

				auto_yield(std::chrono::milliseconds(g_settings.screenhots_pb_delay));

				if (oTakeScreenshot)
					oTakeScreenshot(pThis);

				auto_yield(std::chrono::milliseconds(g_settings.screenhots_post_pb_delay));
			}
			else
			{
				if (oTakeScreenshot)
					oTakeScreenshot(pThis);
			}
		}

		// -------------------------------------------------------------------------
		// PBSS clean-frame texture
		// -------------------------------------------------------------------------
		ID3D11Texture2D* pCleanScreenShot = nullptr;
		ULONGLONG        last_clean_frame = 0;

		std::mutex clean_screenshot_mutex;
		void* g_bit_blt_address = nullptr;

		void UpdateCleanFrame(IDXGISwapChain* pSwapChain, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
		{
			if (!g_settings.screenshots_pb_clean && !g_settings.screenshots_pb_use_both)
				return;

			ULONGLONG current_tick_count = GetTickCount64();

			if (current_tick_count <= last_clean_frame + g_settings.screenshots_pb_clean_delay)
				return;

			if (g_globals.g_screenshots_clean_frames <= 5)
			{
				// Not enough clean frames yet - keep visuals suppressed
				g_globals.g_punkbuster_alt = true;
				return;
			}

			// Capture new clean frame
			ID3D11Texture2D* pBuffer = nullptr;
			HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBuffer));
			if (FAILED(hr))
			{
				LOG(INFO) << xorstr_("UpdateCleanFrame: GetBuffer failed, will retry next frame.");
				return;
			}

			D3D11_TEXTURE2D_DESC td;
			pBuffer->GetDesc(&td);

			// Create and fill the new texture OUTSIDE the lock
			ID3D11Texture2D* pNewTexture = nullptr;

			// Check CreateTexture2D
			hr = pDevice->CreateTexture2D(&td, nullptr, &pNewTexture);
			if (FAILED(hr) || !pNewTexture)
			{
				LOG(INFO) << xorstr_("UpdateCleanFrame: CreateTexture2D failed.");
				pBuffer->Release();

				// Retry next frame
				return;
			}

			pContext->CopyResource(pNewTexture, pBuffer);
			pBuffer->Release();

			{
				std::lock_guard<std::mutex> lock(clean_screenshot_mutex);

				if (pCleanScreenShot)
				{
					pCleanScreenShot->Release();
					pCleanScreenShot = nullptr;
				}

				pCleanScreenShot = pNewTexture;
			}

			last_clean_frame = current_tick_count;
			g_globals.g_punkbuster_alt = false;
		}

		using CopySubresourceRegion_t = void(*)(ID3D11DeviceContext*, ID3D11Resource*, UINT, UINT, UINT, UINT, ID3D11Resource*, UINT, const D3D11_BOX*);
		CopySubresourceRegion_t oCopySubresourceRegion = nullptr;

		void hkCopySubresourceRegion(ID3D11DeviceContext* pContext, ID3D11Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource* pSrcResource, UINT SrcSubresource, const D3D11_BOX* pSrcBox)
		{
			void* return_address = _ReturnAddress();

			if (reinterpret_cast<DWORD_PTR>(return_address) != OFFSET_PBSSRETURN)
			{
				oCopySubresourceRegion(pContext, pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
				return;
			}

			g_globals.g_screenshots_pb++;
			LOG(INFO) << xorstr_("PunkBuster initiated a screenshot [hkCopySubresourceRegion]");

			if ((g_settings.screenshots_pb_clean || g_settings.screenshots_pb_use_both) && !g_settings.screenshots_pb_temp_disable)
			{
				ID3D11Texture2D* pSnapshot = nullptr;
				{
					std::lock_guard<std::mutex> lock(clean_screenshot_mutex);
					pSnapshot = pCleanScreenShot;
					if (pSnapshot)
						pSnapshot->AddRef(); // keep alive while we use it outside the lock
				}

				if (!pSnapshot)
				{
					LOG(INFO) << xorstr_("hkCopySubresourceRegion: no clean frame yet, passing original.");
					oCopySubresourceRegion(pContext, pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
					return;
				}

				// Replace source with the clean texture.
				oCopySubresourceRegion(pContext, pDstResource, DstSubresource, DstX, DstY, DstZ, pSnapshot, SrcSubresource, pSrcBox);

				if (g_settings.screenshots_pb_save_to_folder)
				{
					std::filesystem::path screenshot_path = get_appdata_folder();
					screenshot_path /= xorstr_("PBSS");
					validate_path(screenshot_path);
					screenshot_path /= std::string(current_time() + xorstr_(".jpg"));

					if (!std::filesystem::exists(screenshot_path))
					{
						TextureSaver texture_saver;
						texture_saver.Initialize(g_renderer->m_d3d_device.Get());
						texture_saver.SaveTextureToFile(pSnapshot, screenshot_path);

						LOG(INFO) << xorstr_("Saved PBSS screenshot to: ")
							<< screenshot_path.parent_path().string()
							<< " as: " << screenshot_path.filename().string();
					}
				}

				pSnapshot->Release();
			}
			else
			{
				oCopySubresourceRegion(pContext, pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
			}
		}
	}

	// -------------------------------------------------------------------------
	// Present
	// -------------------------------------------------------------------------
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

					ScreenshotCleaner::UpdateCleanFrame(pThis, renderer->m_pDevice, renderer->m_pContext);

					g_globals.g_should_draw =
						!punkbuster_capturing()        // Anti-Aliasing flag
						&& !g_globals.g_punkbuster     // hkTakeScreenshot
						&& !g_globals.g_punkbuster_alt // Suppressed for clean-frame capture
						&& !g_globals.g_fairfight;     // BitBlt

					// GPU stall from UpdateCleanFrame no longer competes with this lock
					{
						std::lock_guard<std::mutex> lock(ScreenshotCleaner::clean_screenshot_mutex);

						if (g_globals.g_should_draw)
						{
							g_globals.g_screenshots_clean_frames = 0;
							g_renderer->on_present();
						}
						else
							g_globals.g_screenshots_clean_frames++;
					}
				}
			}

			return oPresent(pThis, SyncInterval, Flags);
		}
	}

	// -------------------------------------------------------------------------
	// PreFrame
	// -------------------------------------------------------------------------
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

	// -------------------------------------------------------------------------
	// WndProc
	// -------------------------------------------------------------------------
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
					return 0;

				case WM_SYSCOMMAND:
					if ((wparam & 0xfff0) == SC_KEYMENU)
						return 0;
					break;
				}

				if (g_gui.m_opened)
				{
					switch (msg)
					{
					case WM_MOUSEMOVE: return 0;
					default: break;
					}
					return true;
				}
			}

			return CallWindowProcW(oWndProc, hwnd, msg, wparam, lparam);
		}
	}

	// -------------------------------------------------------------------------
	// hooking::enable / disable / initialize / uninitialize
	// -------------------------------------------------------------------------

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

		while (IsValidPtr(renderer) && IsValidPtr(border_input_node))
		{
			if (terminate) break;

			WndProc::oWndProc = reinterpret_cast<WNDPROC>(
				SetWindowLongPtrW(g_globals.g_hwnd, GWLP_WNDPROC,
					reinterpret_cast<LONG_PTR>(&WndProc::hkWndProc)));
			LOG(INFO) << xorstr_("Hooked WndProc.");

			HMODULE gdi_32 = GetModuleHandleA(xorstr_("Gdi32.dll"));
			if (!gdi_32)
			{
				LOG(INFO) << xorstr_("Failed to get handle for Gdi32.dll.");
				return;
			}

			void* bit_blt = GetProcAddress(gdi_32, xorstr_("BitBlt"));
			if (!bit_blt)
			{
				LOG(INFO) << xorstr_("Failed to get address of BitBlt.");
				return;
			}
			ScreenshotCleaner::g_bit_blt_address = bit_blt;

			MH_CreateHook(bit_blt, ScreenshotCleaner::hkBitBlt, reinterpret_cast<LPVOID*>(&ScreenshotCleaner::oBitBlt));
			MH_EnableHook(bit_blt);
			LOG(INFO) << xorstr_("Hooked BitBlt.");

			MH_CreateHook(reinterpret_cast<void*>(OFFSET_TAKESCREENSHOT), ScreenshotCleaner::hkTakeScreenshot, reinterpret_cast<PVOID*>(&ScreenshotCleaner::oTakeScreenshot));
			MH_EnableHook(reinterpret_cast<void*>(OFFSET_TAKESCREENSHOT));
			LOG(INFO) << xorstr_("Hooked TakeScreenshot.");

			MH_CreateHook((*reinterpret_cast<void***>(renderer->m_pScreen->m_pSwapChain))[8], Present::hkPresent, reinterpret_cast<PVOID*>(&Present::oPresent));
			MH_EnableHook((*reinterpret_cast<void***>(renderer->m_pScreen->m_pSwapChain))[8]);
			LOG(INFO) << xorstr_("Hooked Present.");

			// RTSS support
			std::filesystem::path riva_path = xorstr_("C:\\Program Files (x86)\\RivaTuner Statistics Server\\Profiles\\bf4.exe.cfg");
			bool rtss_disabled = has_rivatuner_hooking_zero(riva_path);
			if (is_rivatuner_running() && !rtss_disabled)
			{
				LOG(RAW_RED) << xorstr_("[RTSS] RivaTuner detected - rendering compatibility issue detected.\n"
					"       Manual fix required: In 'RivaTuner Statistics Server', hold SHIFT while clicking the '+' button.\n"
					"       Select bf4.exe, add it, and set application detection level to 'None'.");
			}

			PreFrame::pPreFrameHook = std::make_unique<VMTHook>();
			PreFrame::pPreFrameHook->Setup(border_input_node->m_Vtable);
			PreFrame::pPreFrameHook->Hook(3, PreFrame::PreFrameUpdate);
			LOG(INFO) << xorstr_("Hooked PreFrameUpdate.");

			MH_CreateHook((*reinterpret_cast<void***>(renderer->m_pContext))[46], ScreenshotCleaner::hkCopySubresourceRegion, reinterpret_cast<PVOID*>(&ScreenshotCleaner::oCopySubresourceRegion));
			MH_EnableHook((*reinterpret_cast<void***>(renderer->m_pContext))[46]);
			LOG(INFO) << xorstr_("Hooked CopySubresourceRegion.");

			terminate = true;
		}
	}

	void hooking::disable()
	{
		MH_DisableHook((*reinterpret_cast<void***>(DxRenderer::GetInstance()->m_pContext))[46]);
		LOG(INFO) << xorstr_("Disabled CopySubresourceRegion.");

		PreFrame::pPreFrameHook->Release();
		LOG(INFO) << xorstr_("Disabled PreFrameUpdate.");

		MH_DisableHook((*reinterpret_cast<void***>(DxRenderer::GetInstance()->m_pScreen->m_pSwapChain))[8]);
		LOG(INFO) << xorstr_("Disabled Present.");

		MH_DisableHook(reinterpret_cast<void*>(OFFSET_TAKESCREENSHOT));
		LOG(INFO) << xorstr_("Disabled TakeScreenshot.");

		if (ScreenshotCleaner::g_bit_blt_address)
		{
			MH_DisableHook(ScreenshotCleaner::g_bit_blt_address);
			LOG(INFO) << xorstr_("Disabled BitBlt.");
		}

		if (WndProc::oWndProc)
		{
			SetWindowLongPtrW(g_globals.g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc::oWndProc));
			LOG(INFO) << xorstr_("Disabled WndProc.");
		}
	}
}