#pragma once

#include <w/windows/gdi.hpp>
#include <w/windows/hresult.hpp>

#include <stdexcept>
#include <filesystem>
#include <string>

#include <windows.h>
#include <wincodec.h>
#include <wrl/client.h>

namespace w::windows::wic
{
    using Microsoft::WRL::ComPtr;

    inline auto create_factory()
    {
        auto result = ComPtr<IWICImagingFactory>{};
        auto const error = CoCreateInstance
        (
            CLSID_WICImagingFactory, 
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&result)
        );
        throw_if_failed(error, "CoCreateInstance(CLSID_WICImagingFactory) failed");
        return result;
    }

    inline auto create_decoder(ComPtr<IWICImagingFactory> const & factory, std::filesystem::path const & filepath)
    {
        auto result = ComPtr<IWICBitmapDecoder>{};
        auto const error = factory->CreateDecoderFromFilename
        (
            filepath.c_str(),              
            nullptr,                       
            GENERIC_READ,                  
            WICDecodeMetadataCacheOnLoad,
            &result
        );
        throw_if_failed(error, "CreateDecoderFromFilename failed");
        return result;
    }

    inline auto get_frame(ComPtr<IWICBitmapDecoder> const & decoder, UINT frameIndex = 0)
    {
        auto result = ComPtr<IWICBitmapFrameDecode>{};
        auto const error = decoder->GetFrame(frameIndex, &result);
        throw_if_failed(error, "GetFrame failed");
        return result;
    }

    inline auto create_converter
    (
        ComPtr<IWICImagingFactory> const & factory,
        ComPtr<IWICBitmapFrameDecode> const & frame
    )
    {
        auto result = ComPtr<IWICFormatConverter>{};
        {
            auto const error = factory->CreateFormatConverter(&result);
            throw_if_failed(error, "CreateFormatConverter failed");
        }

        {
            auto const error = result->Initialize
            (
                frame.Get(),
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                nullptr,
                0.f,
                WICBitmapPaletteTypeCustom
            );
            throw_if_failed(error, "FormatConverter::Initialize failed");
        }
        return result;
    }

    inline auto get_image_size(ComPtr<IWICFormatConverter> const & converter)
    {
        auto width = UINT{}, height = UINT{};
        auto const error = converter->GetSize(&width, &height);
        throw_if_failed(error, "GetSize failed");
        return SIZE{LONG(width), LONG(height)};
    }

    inline auto copy_pixels(ComPtr<IWICFormatConverter> const & converter, gdi::image & image)
    {
        auto i = image.dib.info();

        auto const stride = i.dsBm.bmWidthBytes;
        auto const size = i.dsBmih.biSizeImage;
        auto const bits = i.dsBm.bmBits;
        
        auto const error = converter->CopyPixels(nullptr, stride, size, reinterpret_cast<BYTE*>(bits));
        throw_if_failed(error, "CopyPixels failed");
    }

    struct factory
    {
        decltype(create_factory()) value = create_factory();

        auto load(std::filesystem::path const & filepath, gdi::scoped_dc const & sdc)
        {
            auto const converter = create_converter(value, get_frame(create_decoder(value, filepath)));
            
            auto result = gdi::image(sdc, get_image_size(converter));

            copy_pixels(converter, result);

            return result;
        }
    };
}
