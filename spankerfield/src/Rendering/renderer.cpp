#include "renderer.h"
#include "gui.h"
#include "font.h"
#include "../common.h"
#include "../global.h"
#include "../SDK/sdk.h"
#include "../Utilities/path.h"
#include "../Utilities/other.h"
#include "../Features/main.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace big
{
	renderer::renderer() :
		m_dxgi_swapchain(get_swapchain())
	{
		void *d3d_device{};
		if (SUCCEEDED(m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &d3d_device)))
		{
			m_d3d_device.Attach(static_cast<ID3D11Device*>(d3d_device));
		}
		else
		{
			throw std::runtime_error(xorstr_("Failed to get D3D device."));
		}

		m_d3d_device->GetImmediateContext(m_d3d_device_context.GetAddressOf());

		ImGuiContext* context = ImGui::CreateContext();
		context->IO.DeltaTime = 1.0f / 60.0f;

		auto file_path = get_appdata_folder();
		file_path /= xorstr_("imgui.ini");
		static std::string path = file_path.make_preferred().string();
		context->IO.IniFilename = path.c_str();

		ImFontConfig font_cfg{};
		font_cfg.OversampleH = 2;
		std::strcpy(font_cfg.Name, xorstr_("Open Sans"));
		
		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		atlas->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
		atlas->FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoHinting;
		const ImWchar font_range[] = { 0x0020, 0x00FF, 0x0400, 0x044F, 0 }; // Basic Latin, Latin Supplement and Cyrillic

		m_font = atlas->AddFontFromMemoryTTF(
			const_cast<std::uint8_t*>(font_main),
			sizeof(font_main),
			MAIN_FONT_SIZE, // See font.h to edit
			&font_cfg);

		context->IO.FontDefault = m_font;

		atlas->Build();

		ImGui_ImplDX11_Init(m_d3d_device.Get(), m_d3d_device_context.Get());
		ImGui_ImplWin32_Init(g_globals.g_hwnd);

		g_gui.dx_init();
		g_renderer = this;
	}

	renderer::~renderer()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();

		g_renderer = nullptr;
	}

	void renderer::mouse_act()
	{
		const auto input = BorderInputNode::GetInstance();

		if (IsValidPtr(input) && IsValidPtr(input->m_pMouse) && IsValidPtr(input->m_pMouse->m_pDevice))
		{
			if (!input->m_pMouse->m_pDevice->m_CursorMode)
			{
				// Additional fixes for mouse
				input->m_pMouse->m_pDevice->m_UIOwnsInput = g_gui.m_opened;
				input->m_pMouse->m_pDevice->m_UseRawMouseInput = g_gui.m_opened;
				ImGui::GetIO().MouseDrawCursor = g_gui.m_opened;
			}
		}	
	}

	void renderer::on_present()
	{
		// Store original render target and its state
		ID3D11RenderTargetView* ppRenderTargetViews[1] = { nullptr };
		m_d3d_device_context->OMGetRenderTargets(1, ppRenderTargetViews, nullptr);

		// Get the resource from render target view
		ID3D11Resource* pResource = nullptr;
		ID3D11RenderTargetView* pNonSRGBRTV = nullptr;

		if (ppRenderTargetViews[0])
		{
			ppRenderTargetViews[0]->GetResource(&pResource);

			// Create a new render target view with SRGB disabled
			if (pResource)
			{
				// Get texture description
				ID3D11Texture2D* pTexture = nullptr;
				HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
				if (SUCCEEDED(hr) && pTexture)
				{
					D3D11_TEXTURE2D_DESC textureDesc;
					pTexture->GetDesc(&textureDesc);

					// Create render target view desc to disable SRGB
					D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
					ZeroMemory(&rtvDesc, sizeof(rtvDesc));
					rtvDesc.Format = make_srgb_unaware_format(textureDesc.Format);
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

					// Create non-sRGB render target view
					hr = m_d3d_device->CreateRenderTargetView(pResource, &rtvDesc, &pNonSRGBRTV);
					if (SUCCEEDED(hr))
					{
						// Set the non-sRGB render target
						m_d3d_device_context->OMSetRenderTargets(1, &pNonSRGBRTV, nullptr);
					}

					pTexture->Release();
				}

				pResource->Release();
			}
		}

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = g_gui.m_opened;
		io.ConfigFlags = g_gui.m_opened ? (io.ConfigFlags & ~ImGuiConfigFlags_NoMouse) : (io.ConfigFlags | ImGuiConfigFlags_NoMouse);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_features->draw();

		mouse_act();
		if (g_gui.m_opened)
			g_gui.dx_on_tick();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Restore original render target if we changed it
		if (pNonSRGBRTV)
		{
			m_d3d_device_context->OMSetRenderTargets(1, ppRenderTargetViews, nullptr);
			pNonSRGBRTV->Release();
		}

		// Release the original render target view
		if (ppRenderTargetViews[0])
			ppRenderTargetViews[0]->Release();
	}

	DXGI_FORMAT renderer::make_srgb_unaware_format(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM;
		default:
			return format;
		}
	}

	void renderer::pre_reset()
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void renderer::post_reset()
	{
		// Update swapchain reference
		m_dxgi_swapchain = get_swapchain();

		// Get the device again if needed
		if (!m_d3d_device && m_dxgi_swapchain)
		{
			void* d3d_device{};
			if (SUCCEEDED(m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &d3d_device)))
			{
				m_d3d_device.Attach(static_cast<ID3D11Device*>(d3d_device));
				m_d3d_device->GetImmediateContext(m_d3d_device_context.GetAddressOf());
			}
		}

		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_KEYUP && wparam == g_globals.open_key)
		{
			// Persist and restore the cursor position between menu instances.
			static POINT cursor_coords{};
			if (g_gui.m_opened)
			{
				GetCursorPos(&cursor_coords);
			}
			else if (cursor_coords.x + cursor_coords.y != 0)
			{
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			}

			g_gui.m_opened ^= true;
		}
			
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	}
}
