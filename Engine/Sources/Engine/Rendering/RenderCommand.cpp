#include "enginepch.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Timestep.h"

#include <bgfx/bgfx.h>
#include "bx/readerwriter.h"
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Engine
{
	float time = 0.0f;

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
				.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
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


	bgfx::VertexBufferHandle s_vbh;
	bgfx::IndexBufferHandle s_ibh;
	bgfx::ProgramHandle s_program;

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
		s_ibh = bgfx::createIndexBuffer(
			// Static data can be passed with bgfx::makeRef
			bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
		);

		// Create program from shaders.
		s_program = ShaderManager::LoadProgram("cubes.vert", "cubes.frag");
	}

	void RenderCommand::SetViewport(bgfx::ViewId viewId, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		bgfx::setViewRect(viewId, x, y, uint16_t(width), uint16_t(height));
	}

	void RenderCommand::SetWindowSize(uint32_t width, uint32_t height)
	{
		bgfx::reset(width, height, BGFX_RESET_VSYNC | BGFX_RESET_HIDPI, bgfx::TextureFormat::RGBA8);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color, const bgfx::ViewId& viewId)
	{
		uint32_t bgfxColor =
			(uint8_t(color.r * 255.0f) << 24) |
			(uint8_t(color.g * 255.0f) << 16) |
			(uint8_t(color.b * 255.0f) << 8) |
			(uint8_t(color.a * 255.0f));

		bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, bgfxColor, 1.0f, 0);
	}

	void RenderCommand::BeginFrame(const bgfx::ViewId& viewId)
	{
		bgfx::touch(viewId);
	}

	void RenderCommand::EndFrame()
	{
		bgfx::frame();
	}

	void RenderCommand::SetCamera(const bgfx::ViewId& viewId, glm::mat4 cameraViewMatrix, glm::mat4 cameraProjectionMatrix)
	{
		float* view = glm::value_ptr(cameraViewMatrix);
		float* proj = glm::value_ptr(cameraProjectionMatrix);
		bgfx::setViewTransform(viewId, view, proj);
	}

	void RenderCommand::DrawTestCubes(Timestep ts, Ref<ShaderProgram> shaderProgram)
	{
		time += ts.GetSeconds();

		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CCW
			| BGFX_STATE_MSAA
			| BGFX_STATE_PT_TRISTRIP;

		float mtx[16];
		bx::mtxIdentity(mtx);
 		bx::mtxTranslate(mtx, 0, 0, 0);
 		bx::mtxScale(mtx, 10);

		// Set model matrix for rendering.
		bgfx::setTransform(mtx);

		bgfx::setVertexBuffer(0, s_vbh);
		bgfx::setIndexBuffer(s_ibh);

		bgfx::setState(state);
		bgfx::submit(0, shaderProgram->GetProgramHandle());
	}

	void RenderCommand::DrawMesh(bgfx::ViewId view, bgfx::VertexBufferHandle vbh, bgfx::IndexBufferHandle ibh, bgfx::ProgramHandle shaderProgram, const glm::mat4& transforms)
	{
		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CCW
			| BGFX_STATE_MSAA
			| BGFX_STATE_PT_TRISTRIP;

		bgfx::setTransform(glm::value_ptr(transforms));

		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);

		bgfx::submit(view, shaderProgram);
	}

}