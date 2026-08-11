#include "enginepch.h"
#include "Engine/Rendering/RenderCommand.h"
#include <bgfx\bgfx.h>
#include "bx/bx.h"
#include "bx/readerwriter.h"
#include <bx\file.h>
#include <fstream>
#include <limits>
#include <iosfwd>

namespace Engine
{
	static const char* s_ptNames[]
	{
		"Triangle List",
		"Triangle Strip",
		"Lines",
		"Line Strip",
		"Points",
	};

	static const uint64_t s_ptState[]
	{
		UINT64_C(0),
		BGFX_STATE_PT_TRISTRIP,
		BGFX_STATE_PT_LINES,
		BGFX_STATE_PT_LINESTRIP,
		BGFX_STATE_PT_POINTS,
	};
	static_assert(BX_COUNTOF(s_ptState) == BX_COUNTOF(s_ptNames));

	static bgfx::ShaderHandle loadShader(const bx::StringView& _name)
	{
		bx::FilePath filePath("assets/shaders/");

		bgfx::RendererType::Enum rdType = bgfx::getRendererType();

		switch (bgfx::getRendererType())
		{
		case bgfx::RendererType::Noop:
		case bgfx::RendererType::Direct3D11: filePath.join("dxbc");  break;
		case bgfx::RendererType::Direct3D12: filePath.join("dxil");  break;
		case bgfx::RendererType::Agc:
		case bgfx::RendererType::Gnm:        filePath.join("pssl");  break;
		case bgfx::RendererType::Metal:      filePath.join("metal"); break;
		case bgfx::RendererType::Nvn:        filePath.join("nvn");   break;
		case bgfx::RendererType::OpenGL:     filePath.join("glsl");  break;
		case bgfx::RendererType::OpenGLES:   filePath.join("essl");  break;
		case bgfx::RendererType::Vulkan:     filePath.join("spirv"); break;
		case bgfx::RendererType::WebGPU:     filePath.join("wgsl");  break;

		case bgfx::RendererType::Count:
			BX_ASSERT(false, "You should not be here!");
			break;
		}

		char fileName[512];
		bx::strCopy(fileName, BX_COUNTOF(fileName), _name);
		bx::strCat(fileName, BX_COUNTOF(fileName), ".bin");

		filePath.join(fileName);

		char buffer[2048];

		// Open file
		std::ifstream shaderFile(_name.getPtr(), std::ios::binary); // and since you want bytes rather than
		// characters, strongly consider opening the
		// File in binary mode with std::ios_base::binary
		// Get length of file
		shaderFile.seekg(0, std::ios::end);
		size_t shader_filesize = shaderFile.tellg();
		shaderFile.seekg(0, std::ios::beg);

		const bgfx::Memory* memory = bgfx::alloc(shader_filesize + 1);

		bx::StaticMemoryBlockWriter memWriter(memory->data, memory->size);

		// Don't overflow the buffer!
		if (shader_filesize > sizeof(buffer)) {
			shader_filesize = sizeof(buffer);
		}

		// Read file
		shaderFile.read(buffer, shader_filesize);
		shaderFile.close();

		bx::Error error;
		memWriter.write(buffer, shader_filesize, &error);
		ENGINE_ASSERT(error.isOk());

		bgfx::ShaderHandle handle = bgfx::createShader(memory);
		bgfx::setName(handle, _name.getPtr(), _name.getLength());

		return handle;
	}

	bgfx::ProgramHandle loadProgram(const bx::StringView& _vsName, const bx::StringView& _fsName) 
	{
		bgfx::ShaderHandle vsh = loadShader(_vsName);
		bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
		if (!_fsName.isEmpty())
		{
			fsh = loadShader(_fsName);
		}

		return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		bgfx::setViewRect(0, x, y, uint16_t(width), uint16_t(height));
	}

	void RenderCommand::SetWindowSize(uint32_t width, uint32_t height)
	{
		bgfx::reset(width, height, BGFX_RESET_VSYNC, bgfx::TextureFormat::RGBA8);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		uint32_t bgfxColor =
			(uint8_t(color.r * 255.0f) << 24) |
			(uint8_t(color.g * 255.0f) << 16) |
			(uint8_t(color.b * 255.0f) << 8) |
			(uint8_t(color.a * 255.0f));

		bgfx::setViewClear(0, BGFX_CLEAR_COLOR, bgfxColor, 1.0, 0);
	}

	void RenderCommand::BeginFrame()
	{
		bgfx::touch(0);
	}

	void RenderCommand::EndFrame()
	{
		bgfx::frame();
	}

	void RenderCommand::DrawIndexed(Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		std::vector<bgfx::VertexBufferHandle> vertexBuffers { };
		for (auto& buffer : vertexArray->GetVertexBuffers())
		{
			vertexBuffers.push_back(
				bgfx::createVertexBuffer(
					bgfx::makeRef(buffer->data(), sizeof(VertexBuffer) * buffer->size()), 
				vertexArray->GetLayout()));
		}

		IndexBuffer* indexBuffer = vertexArray->GetIndexBuffer().get();

		bgfx::IndexBufferHandle indexBufferHandle = bgfx::createIndexBuffer(bgfx::makeRef(indexBuffer, sizeof(IndexBuffer)));
		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW
			| BGFX_STATE_MSAA
			| s_ptState[0];

		// Create program from shaders.
		bgfx::ProgramHandle m_program = loadProgram("vs_cubes", "fs_cubes");

		bgfx::setState(state);
		bgfx::submit(0, m_program);
	}

}