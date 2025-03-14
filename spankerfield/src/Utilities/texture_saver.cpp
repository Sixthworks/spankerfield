#include "texture_saver.h"
#include <wincodec.h>
#include <shlwapi.h>

TextureSaver::TextureSaver() :
    m_device(nullptr),
    m_device_context(nullptr),
    m_wic_factory(nullptr)
{
}

TextureSaver::~TextureSaver()
{
}

bool TextureSaver::Initialize(ID3D11Device* p_device)
{
    if (p_device == nullptr)
        return false;

    m_device = p_device;
    p_device->GetImmediateContext(&m_device_context);

    // Initialize WIC Factory
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_wic_factory)
    );

    if (FAILED(hr))
        return false;

    return true;
}

bool TextureSaver::SaveTextureToFile(ID3D11Texture2D* p_texture, const std::wstring& file_path)
{
    if (p_texture == nullptr || m_device == nullptr || m_device_context == nullptr || m_wic_factory == nullptr)
        return false;

    // Create a staging texture for CPU access
    ID3D11Texture2D* p_staging_texture = CreateStagingTexture(p_texture);
    if (p_staging_texture == nullptr)
        return false;

    // Get texture description
    D3D11_TEXTURE2D_DESC texture_desc;
    p_texture->GetDesc(&texture_desc);

    // Map the staging texture
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    if (!MapTextureData(p_staging_texture, mapped_resource))
    {
        p_staging_texture->Release();
        return false;
    }

    // Create a WIC bitmap
    Microsoft::WRL::ComPtr<IWICBitmap> wic_bitmap;
    HRESULT hr = m_wic_factory->CreateBitmapFromMemory(
        texture_desc.Width,
        texture_desc.Height,
        GUID_WICPixelFormat32bppBGRA,
        mapped_resource.RowPitch,
        mapped_resource.RowPitch * texture_desc.Height,
        static_cast<BYTE*>(mapped_resource.pData),
        &wic_bitmap
    );

    // Unmap the staging resource
    m_device_context->Unmap(p_staging_texture, 0);
    p_staging_texture->Release();

    if (FAILED(hr))
        return false;

    // Create a WIC stream
    Microsoft::WRL::ComPtr<IWICStream> wic_stream;
    hr = m_wic_factory->CreateStream(&wic_stream);
    if (FAILED(hr))
        return false;

    // Initialize the stream with the file path
    hr = wic_stream->InitializeFromFilename(file_path.c_str(), GENERIC_WRITE);
    if (FAILED(hr))
        return false;

    // Get the WIC encoder format based on file extension
    GUID format = GetWICFormatFromFileExtension(file_path);

    // Create an encoder
    Microsoft::WRL::ComPtr<IWICBitmapEncoder> wic_encoder;
    hr = m_wic_factory->CreateEncoder(format, nullptr, &wic_encoder);
    if (FAILED(hr))
        return false;

    // Initialize the encoder with the stream
    hr = wic_encoder->Initialize(wic_stream.Get(), WICBitmapEncoderNoCache);
    if (FAILED(hr))
        return false;

    // Create a new frame
    Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> wic_frame;
    Microsoft::WRL::ComPtr<IPropertyBag2> property_bag;
    hr = wic_encoder->CreateNewFrame(&wic_frame, &property_bag);
    if (FAILED(hr))
        return false;

    // Initialize the frame
    hr = wic_frame->Initialize(property_bag.Get());
    if (FAILED(hr))
        return false;

    // Set frame size
    hr = wic_frame->SetSize(texture_desc.Width, texture_desc.Height);
    if (FAILED(hr))
        return false;

    // Set pixel format
    WICPixelFormatGUID pixel_format = GUID_WICPixelFormat32bppBGRA;
    hr = wic_frame->SetPixelFormat(&pixel_format);
    if (FAILED(hr))
        return false;

    // Write the bitmap data to the frame
    hr = wic_frame->WriteSource(wic_bitmap.Get(), nullptr);
    if (FAILED(hr))
        return false;

    // Commit the frame
    hr = wic_frame->Commit();
    if (FAILED(hr))
        return false;

    // Commit the encoder
    hr = wic_encoder->Commit();
    if (FAILED(hr))
        return false;

    return true;
}

GUID TextureSaver::GetWICFormatFromFileExtension(const std::wstring& file_path)
{
    std::wstring extension = PathFindExtensionW(file_path.c_str());
    std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

    if (extension == L".png")
        return GUID_ContainerFormatPng;
    else if (extension == L".jpg" || extension == L".jpeg")
        return GUID_ContainerFormatJpeg;
    else if (extension == L".bmp")
        return GUID_ContainerFormatBmp;
    else if (extension == L".tiff" || extension == L".tif")
        return GUID_ContainerFormatTiff;
    else if (extension == L".gif")
        return GUID_ContainerFormatGif;
    else if (extension == L".wmp")
        return GUID_ContainerFormatWmp;

    // Default to PNG if extension is not recognized
    return GUID_ContainerFormatPng;
}

ID3D11Texture2D* TextureSaver::CreateStagingTexture(ID3D11Texture2D* p_texture)
{
    if (p_texture == nullptr || m_device == nullptr)
        return nullptr;

    // Get texture description
    D3D11_TEXTURE2D_DESC texture_desc;
    p_texture->GetDesc(&texture_desc);

    // Create a staging texture description
    D3D11_TEXTURE2D_DESC staging_desc = texture_desc;
    staging_desc.Usage = D3D11_USAGE_STAGING;
    staging_desc.BindFlags = 0;
    staging_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    staging_desc.MiscFlags = 0;

    // Create staging texture
    ID3D11Texture2D* p_staging_texture = nullptr;
    HRESULT hr = m_device->CreateTexture2D(&staging_desc, nullptr, &p_staging_texture);
    if (FAILED(hr))
        return nullptr;

    // Copy resource from original texture to staging texture
    m_device_context->CopyResource(p_staging_texture, p_texture);

    return p_staging_texture;
}

bool TextureSaver::MapTextureData(ID3D11Texture2D* p_staging_texture, D3D11_MAPPED_SUBRESOURCE& mapped_resource)
{
    if (p_staging_texture == nullptr || m_device_context == nullptr)
        return false;

    // Map the staging texture to get access to the data
    HRESULT hr = m_device_context->Map(
        p_staging_texture,
        0,
        D3D11_MAP_READ,
        0,
        &mapped_resource
    );

    if (FAILED(hr))
        return false;

    return true;
}