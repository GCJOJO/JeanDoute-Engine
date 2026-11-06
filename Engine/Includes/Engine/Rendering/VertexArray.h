#pragma once
#include "bgfx\bgfx.h"

namespace bgfx { struct VertexLayout; }

namespace Engine
{
	typedef struct Vertex {
		float x, y, z;
		glm::vec4 color;
	} Vertex;

	typedef std::vector<Vertex> VertexBuffer;
	typedef std::vector<uint16_t> IndexBuffer;

	class VertexArray
	{
	public:
		VertexArray()
		{
			m_Layout.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8)
				.end();
		}

		void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertexBuffer)
		{
			m_Buffers.push_back(vertexBuffer);
		}

		void SetIndexBuffer(const SharedPtr<IndexBuffer>& indexBuffer)
		{
			m_IndexBuffer = indexBuffer;
		}

		bgfx::VertexLayout& GetLayout() { return m_Layout; }

		const std::vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const { return m_Buffers; };
		const SharedPtr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; };

	private:
		std::vector<SharedPtr<VertexBuffer>> m_Buffers;
		SharedPtr<IndexBuffer> m_IndexBuffer;
		bgfx::VertexLayout m_Layout;
	};

}