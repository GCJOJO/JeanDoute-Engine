#include "enginepch.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"

#include <bgfx\bgfx.h>
#include "bx/bx.h"
#include "bx/readerwriter.h"
#include <bx\file.h>
#include <fstream>
#include <limits>
#include <iosfwd>

namespace Engine
{
	struct PosColorVertex
	{
		float m_x;
		float m_y;
		float m_z;
		uint32_t m_abgr;

		static void init()
		{
			ms_layout
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
				.end();
		};

		static bgfx::VertexLayout ms_layout;
	};

	bgfx::VertexLayout PosColorVertex::ms_layout;

	static PosColorVertex s_cubeVertices[] =
	{
		{-1.0f,  1.0f,  1.0f, 0xff000000 },
		{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
		{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
		{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
		{-1.0f,  1.0f, -1.0f, 0xffff0000 },
		{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
		{-1.0f, -1.0f, -1.0f, 0xffffff00 },
		{ 1.0f, -1.0f, -1.0f, 0xffffffff },
	};

	static const uint16_t s_cubeTriList[] =
	{
		0, 1, 2, // 0
		1, 3, 2,
		4, 6, 5, // 2
		5, 6, 7,
		0, 2, 4, // 4
		4, 2, 6,
		1, 5, 3, // 6
		5, 7, 3,
		0, 4, 1, // 8
		4, 5, 1,
		2, 3, 6, // 10
		6, 3, 7,
	};

	static const uint16_t s_cubeTriStrip[] =
	{
		0, 1, 2,
		3,
		7,
		1,
		5,
		0,
		4,
		2,
		6,
		7,
		4,
		5,
	};

	static const uint16_t s_cubeLineList[] =
	{
		0, 1,
		0, 2,
		0, 4,
		1, 3,
		1, 5,
		2, 3,
		2, 6,
		3, 7,
		4, 5,
		4, 6,
		5, 7,
		6, 7,
	};

	static const uint16_t s_cubeLineStrip[] =
	{
		0, 2, 3, 1, 5, 7, 6, 4,
		0, 2, 6, 4, 5, 7, 3, 1,
		0,
	};

	static const uint16_t s_cubePoints[] =
	{
		0, 1, 2, 3, 4, 5, 6, 7
	};

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

	bgfx::VertexBufferHandle s_vbh;
	bgfx::IndexBufferHandle s_ibh[BX_COUNTOF(s_ptState)];
	bgfx::ProgramHandle s_program;

	static bgfx::ShaderHandle loadShader(const bx::StringView& _name)
	{
		// Use std::filesystem instead
		bx::FilePath filePath("shaders/");

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
		std::ifstream shaderFile(filePath.getCPtr(), std::ios::binary); // and since you want bytes rather than
		if(!shaderFile.is_open())
		{
			ENGINE_CORE_ERROR("Unable to open file", filePath.getCPtr());
			return BGFX_INVALID_HANDLE;
		}
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

	void RenderCommand::Init()
	{
		PosColorVertex::init();
		
		// Create static vertex buffer.
		s_vbh = bgfx::createVertexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices))
			, PosColorVertex::ms_layout
		);

		// Create static index buffer for triangle list rendering.
		s_ibh[0] = bgfx::createIndexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
		);

		// Create static index buffer for triangle strip rendering.
		s_ibh[1] = bgfx::createIndexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeTriStrip, sizeof(s_cubeTriStrip))
		);

		// Create static index buffer for line list rendering.
		s_ibh[2] = bgfx::createIndexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeLineList, sizeof(s_cubeLineList))
		);

		// Create static index buffer for line strip rendering.
		s_ibh[3] = bgfx::createIndexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeLineStrip, sizeof(s_cubeLineStrip))
		);

		// Create static index buffer for point list rendering.
		s_ibh[4] = bgfx::createIndexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubePoints, sizeof(s_cubePoints))
		);


		// Create program from shaders.
		bgfx::ProgramHandle s_program = loadProgram("vs_cubes", "fs_cubes");
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
		/*std::vector<bgfx::VertexBufferHandle> vertexBuffers{};
		for (auto& buffer : vertexArray->GetVertexBuffers())
		{
			bgfx::VertexBufferHandle bufferHandle = bgfx::createVertexBuffer(
				bgfx::makeRef(buffer->data(), sizeof(VertexBuffer) * buffer->size()),
				vertexArray->GetLayout());

			vertexBuffers.push_back(bufferHandle);
			bgfx::setVertexBuffer(0, bufferHandle);
		}

		IndexBuffer* indexBuffer = vertexArray->GetIndexBuffer().get();

		bgfx::IndexBufferHandle indexBufferHandle = bgfx::createIndexBuffer(bgfx::makeRef(indexBuffer, sizeof(IndexBuffer)));
		bgfx::setIndexBuffer(indexBufferHandle);*/

		const bx::Vec3 at = { 0.0f, 0.0f,   0.0f };
		const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

		Window& window = Application::Get().GetWindow();
		uint32_t width = window.GetWidth();
		uint32_t height = window.GetHeight();

		// Set view and projection matrix for view 0.
		{
			float view[16];
			bx::mtxLookAt(view, eye, at);

			float proj[16];
			bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
			bgfx::setViewTransform(0, view, proj);

			// Set view 0 default viewport.
			bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
		}


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

		// Submit 11x11 cubes.
		for (uint32_t yy = 0; yy < 11; ++yy)
		{
			for (uint32_t xx = 0; xx < 11; ++xx)
			{
				float mtx[16];
				bx::mtxRotateXY(mtx, xx * 0.21f, yy * 0.37f);
				mtx[12] = -15.0f + float(xx) * 3.0f;
				mtx[13] = -15.0f + float(yy) * 3.0f;
				mtx[14] = 0.0f;

				// Set model matrix for rendering.
				bgfx::setTransform(mtx);

				bgfx::setVertexBuffer(0, s_vbh);
				bgfx::setIndexBuffer(s_ibh[0]);

				bgfx::setState(state);
				bgfx::submit(0, s_program);
			}
		}
	}

}