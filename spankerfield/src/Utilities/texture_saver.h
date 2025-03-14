#pragma once
#include "../common.h"
#include <wrl/client.h>
#include <wincodec.h>

class TextureSaver
{
public:
    TextureSaver();
    ~TextureSaver();

    // Initialize the TextureSaver with a D3D11 device
    bool Initialize(ID3D11Device* p_device);

    // Save a texture to a file at the specified path
    // Supports BMP, PNG, JPEG, TIFF, GIF, and WMP formats (determined by file extension)
    bool SaveTextureToFile(ID3D11Texture2D* p_texture, const std::wstring& file_path);

private:
    // Helper method to determine WIC format based on file extension
    GUID GetWICFormatFromFileExtension(const std::wstring& file_path);

    // Helper method to create a staging texture for CPU access
    ID3D11Texture2D* CreateStagingTexture(ID3D11Texture2D* p_texture);

    // Map texture data to memory
    bool MapTextureData(ID3D11Texture2D* p_staging_texture, D3D11_MAPPED_SUBRESOURCE& mapped_resource);

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_context;
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wic_factory;
};