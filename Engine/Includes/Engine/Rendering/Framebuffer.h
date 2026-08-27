#pragma once
#include "Engine/Core/Core.h"

#include "bgfx/bgfx.h"

namespace Engine
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {
		}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO : Filtering/Wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> Attachments)
			: Attachments(Attachments) {
		};

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& spec);
		~Framebuffer();

		void Invalidate();
		void DestroyHandles();

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);
		int ReadPixel(uint32_t attachmentIndex, int x, int y);

		void ClearAttachment(uint32_t attachmentIndex, int value);

		bgfx::TextureHandle& GetColorAttachmentRendererID(uint32_t index = 0);
		const FramebufferSpecification& GetSpecification() { return m_Specs; }
		const bgfx::FrameBufferHandle& GetHandle() const { return m_Handle; }
		const bgfx::ViewId& GetViewId() const { return m_ViewId; }

	private:
		void EnsureBlitTexture(uint32_t attachmentIndex);

		FramebufferSpecification m_Specs;
		bgfx::FrameBufferHandle m_Handle;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec;

		std::vector<bgfx::TextureHandle> m_ColorAttachmentHandles;
		bgfx::TextureHandle m_DepthAttachmentHandle = BGFX_INVALID_HANDLE;
		bgfx::FrameBufferHandle m_FramebufferHandle = BGFX_INVALID_HANDLE;
		bgfx::ViewId m_ViewId = 0;

		bgfx::TextureHandle m_BlitTexture = BGFX_INVALID_HANDLE;
		bgfx::ViewId m_BlitViewId = 0;
		std::vector<uint8_t> m_BlitData;
		uint32_t m_ReadFrame = 0;
		bool m_ReadPending = false;
		int m_LastReadValue = -1;
	};
}