#include "hooks.h"
#include "../MinHook.h"
#include "../Utilities/vtablehook.h"
#include "../Features/main.h"
#include "../Rendering/base.h"
#include "../global.h"

namespace Hooks
{
	namespace ScreenshotCleaner
	{
		typedef BOOL(WINAPI* tBitBlt)(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
		tBitBlt oBitBlt = nullptr;

		BOOL hkBitBlt(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
		{
			g_FairFight = true;
			Sleep(15);
			bool result = oBitBlt(hdcDst, x, y, cx, cy, hdcSrc, x1, y1, rop);
			g_FairFight = false;
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
				g_Height = pDxRenderer->m_pScreen->m_Height;
				g_Width = pDxRenderer->m_pScreen->m_Width;
				g_ViewProj = pGameRenderer->m_pRenderView->m_ViewProjection;

				static auto SSMODULE = (uintptr_t*)OFFSET_SSMODULE;
				if (!IsValidPtr(SSMODULE)) return oPresent(pThis, SyncInterval, Flags);
				g_PunkBuster = (*(int*)(*SSMODULE + 0x14) != -1);

				ImSetup::BeginDraw(pThis);
				ImSetup::Menu();
				Features::Draw(!g_PunkBuster && !g_FairFight);
				ImSetup::EndDraw();
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
			Features::PreFrame(!g_PunkBuster && !g_FairFight);
		}
	}

	void Hook()
	{
		static bool Terminado = false;
		const auto pDxRenderer = DxRenderer::GetInstance();
		const auto pBorderInputNode = BorderInputNode::GetInstance();

		while (pDxRenderer && pBorderInputNode)
		{
			if (Terminado) break;

			MH_CreateHook((*reinterpret_cast<void***>(pDxRenderer->m_pScreen->m_pSwapChain))[8], Present::hkPresent, reinterpret_cast<PVOID*>(&Present::oPresent));
			MH_EnableHook((*reinterpret_cast<void***>(pDxRenderer->m_pScreen->m_pSwapChain))[8]);
			printf(xorstr_("Hooked Present.\n"));

			MH_CreateHook(&BitBlt, &ScreenshotCleaner::hkBitBlt, reinterpret_cast<LPVOID*>(&ScreenshotCleaner::oBitBlt));
			MH_EnableHook(&BitBlt);
			printf(xorstr_("Hooked BitBlt.\n"));

			PreFrame::oPreFrameUpdate = reinterpret_cast<PreFrame::PreFrameUpdate_t>(Utilities::HookVTableFunction(reinterpret_cast<PDWORD64*>(pBorderInputNode->m_Vtable), reinterpret_cast<PBYTE>(&PreFrame::hkPreFrame), 3));
			printf(xorstr_("Hooked PreFrame.\n"));

			Terminado = true;
		}
	}

	void UnHook()
	{
		MH_DisableHook(MH_ALL_HOOKS);
		Utilities::HookVTableFunction(reinterpret_cast<PDWORD64*>(BorderInputNode::GetInstance()->m_Vtable), reinterpret_cast<PBYTE>(PreFrame::oPreFrameUpdate), 3);
	}
}