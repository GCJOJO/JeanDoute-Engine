#include "enginepch.h"
#include "Engine/Rendering/Framebuffer.h"
#include "Engine/Rendering/Renderer.h"
#include <bimg\bimg.h>

namespace Engine
{
	const int blitSize = 1;

	bgfx::TextureFormat::Enum ToBgfxFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::RED_INTEGER:
			return bgfx::TextureFormat::R32I;
		case FramebufferTextureFormat::RGBA8:
		default:
			return bgfx::TextureFormat::RGBA8;
		}
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: m_Specs(spec)
	{
		for (auto& attachementSpecs : m_Specs.Attachments.Attachments)
		{
			if (attachementSpecs.TextureFormat != FramebufferTextureFormat::DEPTH24STENCIL8)
				m_ColorAttachmentSpecs.push_back(attachementSpecs);
			else
				m_DepthAttachmentSpec = attachementSpecs;
		}

		Invalidate();
	}

	Framebuffer::~Framebuffer()
	{
		DestroyHandles();
	}

	void Framebuffer::Invalidate()
	{
		DestroyHandles();

		std::vector<bgfx::TextureHandle> textures;
		const uint64_t renderTargetFlags =
			BGFX_TEXTURE_RT |
			BGFX_SAMPLER_MIN_POINT |
			BGFX_SAMPLER_MAG_POINT |
			BGFX_SAMPLER_U_CLAMP |
			BGFX_SAMPLER_V_CLAMP;

		for (auto& spec : m_ColorAttachmentSpecs)
		{
			bgfx::TextureFormat::Enum format = ToBgfxFormat(spec.TextureFormat);
			bgfx::TextureHandle textureHandle = bgfx::createTexture2D(
				(uint16_t)m_Specs.Width, (uint16_t)m_Specs.Height, 
				false, 1, format, renderTargetFlags);

			m_ColorAttachmentHandles.push_back(textureHandle);
			textures.push_back(textureHandle);
		}

		if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
		{
			m_DepthAttachmentHandle = bgfx::createTexture2D(
				(uint16_t)m_Specs.Width, (uint16_t)m_Specs.Height,
				false, 1, bgfx::TextureFormat::D24S8, renderTargetFlags);
			textures.push_back(m_DepthAttachmentHandle);
		}

		m_FramebufferHandle = bgfx::createFrameBuffer((uint8_t)textures.size(), textures.data(), true);

		m_ViewId = Renderer::AllocateViewId();
	}

	void Framebuffer::DestroyHandles()
	{
		if (bgfx::isValid(m_FramebufferHandle))
			bgfx::destroy(m_FramebufferHandle);
		m_ColorAttachmentHandles.clear();
	}

	void Framebuffer::Bind()
	{
		bgfx::setViewFrameBuffer(m_ViewId, m_FramebufferHandle);
		bgfx::setViewRect(m_ViewId, 0, 0, (uint16_t)m_Specs.Width, (uint16_t)m_Specs.Height);
	}

	void Framebuffer::Unbind()
	{
		bgfx::setViewFrameBuffer(m_ViewId, BGFX_INVALID_HANDLE);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0) return;
		m_Specs.Width = width;
		m_Specs.Height = height;
		Invalidate();
	}

	int Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		EnsureBlitTexture(attachmentIndex);

		if (!m_ReadPending)
		{
			bgfx::blit(m_BlitViewId, m_BlitTexture, 0, 0, 
				m_ColorAttachmentHandles[attachmentIndex], (uint16_t)x, (uint16_t)y,
				blitSize, blitSize);

			m_ReadFrame = bgfx::readTexture(m_BlitTexture, m_BlitData.data());
			m_ReadPending = true;
		}

		if (m_ReadPending && bgfx::frame() >= m_ReadFrame)
		{
			m_ReadPending = false;
			size_t pixelIndex = (blitSize / 2) * blitSize + (blitSize / 2);
			int32_t value = *reinterpret_cast<int32_t*>(&m_BlitData[pixelIndex * sizeof(int32_t)]);
			m_LastReadValue = value;
		}

		return m_LastReadValue;
	}

	void Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		uint32_t w = m_Specs.Width, h = m_Specs.Height;
		std::vector<int32_t> clearData(w * h, value);

		const bgfx::Memory* mem = bgfx::copy(clearData.data(), (uint32_t)(clearData.size() * sizeof(int32_t)));
		bgfx::updateTexture2D(m_ColorAttachmentHandles[attachmentIndex], 0, 0, 0, 0, (uint16_t)w, (uint16_t)h, mem);
	}

	bgfx::TextureHandle& Framebuffer::GetColorAttachmentRendererID(uint32_t index /*= 0*/)
	{
		return m_ColorAttachmentHandles[index];
	}

	void Framebuffer::EnsureBlitTexture(uint32_t attachmentIndex)
	{
		if (bgfx::isValid(m_BlitTexture))
			return;

		bgfx::TextureFormat::Enum format = ToBgfxFormat(m_ColorAttachmentSpecs[attachmentIndex].TextureFormat);
		m_BlitTexture = bgfx::createTexture2D(blitSize, blitSize, false, 1, format,
			BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK);

		m_BlitData.resize(blitSize * blitSize * bimg::getBitsPerPixel(bimg::TextureFormat::Enum(format)) / 8);
		m_BlitViewId = Renderer::AllocateViewId();
	}

}