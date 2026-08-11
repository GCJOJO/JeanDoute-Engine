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

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
		{
			m_Buffers.push_back(vertexBuffer);
		}

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
		{
			m_IndexBuffer = indexBuffer;
		}

		bgfx::VertexLayout& GetLayout() { return m_Layout; }

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_Buffers; };
		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; };

	private:
		std::vector<Ref<VertexBuffer>> m_Buffers;
		Ref<IndexBuffer> m_IndexBuffer;
		bgfx::VertexLayout m_Layout;
	};

}