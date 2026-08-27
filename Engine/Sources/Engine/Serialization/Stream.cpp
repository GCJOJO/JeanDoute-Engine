#include "enginepch.h"
#include "Engine/Serialization/Stream.h"

//Based on https://github.com/StudioCherno/Walnut/blob/dev/Walnut/Source/Walnut/Serialization/StreamWriter.cpp
namespace Engine
{

	void StreamWriter::WriteBuffer(Buffer buffer, bool writeSize /* = true */)
	{
		if (writeSize)
			WriteData((char*)&buffer.Size, sizeof(uint32_t));

		WriteData((char*)buffer.Data, buffer.Size);
	}

	void StreamWriter::WriteZero(uint64_t size)
	{
		char zero = 0;
		for (uint64_t i = 0; i < size; i++)
			WriteData(&zero, 1);
	}

	void StreamWriter::WriteString(const std::string& string)
	{
		size_t size = string.size();
		WriteData((char*)&size, sizeof(size_t));
		WriteData((char*)string.data(), sizeof(char) * string.size());
	}

	void StreamWriter::WriteString(std::string_view string)
	{
		size_t size = string.size();
		WriteData((char*)&size, sizeof(size_t));
		WriteData((char*)string.data(), sizeof(char) * string.size());
	}

	bool StreamReader::ReadBuffer(Buffer& buffer, uint32_t size)
	{
		buffer.Size = size;
		if (size == 0)
		{
			if (!ReadData((char*)&buffer.Size, sizeof(uint32_t)))
				return false;
		}

		buffer.Allocate(buffer.Size);
		return ReadData((char*)buffer.Data, buffer.Size);
	}

	bool StreamReader::ReadString(std::string& string)
	{
		size_t size;
		if (!ReadData((char*)&size, sizeof(size_t)))
			return false;

		string.resize(size);
		return ReadData((char*)string.data(), sizeof(char) * size);
	}

}