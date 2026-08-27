#pragma once
#include <map>

//Based on https://github.com/StudioCherno/Walnut/blob/dev/Walnut/Source/Walnut/Serialization/StreamWriter.h
namespace Engine
{
	class StreamWriter
	{
	public:
		virtual ~StreamWriter() = default;

		virtual bool IsStreamGood() const = 0;
		virtual uint64_t GetStreamPosition() = 0;

		virtual void SetStreamPosition(uint64_t position) = 0;
		virtual bool WriteData(const char* data, size_t size) = 0;

		operator bool() const { return IsStreamGood(); }

		void WriteBuffer(Buffer buffer, bool writeSize = true);
		void WriteZero(uint64_t size);
		void WriteString(const std::string& string);
		void WriteString(std::string_view string);

		template<typename T>
		void WriteRaw(const T& type)
		{
			bool success = WriteData((char*)&type, sizeof(T));
		}

		template<typename T>
		void WriteObject(const T& obj)
		{
			T::Serialize(this, obj);
		}

		template<typename Key, typename Value>
		void WriteMap(const std::map<Key, Value>& map, bool writeSize = true)
		{
			if (writeSize)
				WriteRaw<uint32_t>((uint32_t)map.size());

			for (const auto& [key, value] : map)
			{
				if constexpr (std::is_trivial<Key>())
					WriteRaw<Key>(key);
				else
					WriteObject<Key>(key);

				if constexpr (std::is_trivial<Value>())
					WriteRaw<Value>(value);
				else
					WriteObject<Value>(value);
			}
		}

		template<typename Key, typename Value>
		void WriteMap(const std::unordered_map<Key, Value>& map, bool writeSize = true)
		{
			if (writeSize)
				WriteRaw<uint32_t>((uint32_t)map.size());

			for (const auto& [key, value] : map)
			{
				if constexpr (std::is_trivial<Key>())
					WriteRaw<Key>(key);
				else
					WriteObject<Key>(key);

				if constexpr (std::is_trivial<Value>())
					WriteRaw<Value>(value);
				else
					WriteObject<Value>(value);
			}
		}

		template<typename Value>
		void WriteMap(const std::unordered_map<std::string, Value>& map, bool writeSize = true)
		{
			if (writeSize)
				WriteRaw<uint32_t>((uint32_t)map.size());

			for (const auto& [key, value] : map)
			{
				WriteString(key);

				if constexpr (std::is_trivial<Value>())
					WriteRaw<Value>(value);
				else
					WriteObject<Value>(value);
			}
		}

		template<typename T>
		void WriteArray(const std::vector<T>& array, bool writeSize = true)
		{
			if (writeSize)
				WriteRaw<uint32_t>((uint32_t)array.size());

			for (const auto& element : array)
			{
				if constexpr (std::is_trivial<T>())
					WriteRaw<T>(element);
				else
					WriteObject<T>(element);
			}
		}

		template<>
		void WriteArray(const std::vector<std::string>& array, bool writeSize)
		{
			if (writeSize)
				WriteRaw<uint32_t>((uint32_t)array.size());

			for (const auto& element : array)
				WriteString(element);
		}
	};

	class StreamReader
	{
	public:
		virtual ~StreamReader() = default;

		virtual bool IsStreamGood() const = 0;
		virtual uint64_t GetStreamPosition() = 0;
		virtual void SetStreamPosition(uint64_t position) = 0;
		virtual bool ReadData(char* destination, size_t size) = 0;

		operator bool() const { return IsStreamGood(); }

		bool ReadBuffer(Buffer& buffer, uint32_t size = 0);
		bool ReadString(std::string& string);

		template<typename T>
		bool ReadRaw(T& type)
		{
			bool success = ReadData((char*)&type, sizeof(T));
			ENGINE_CORE_ASSERT(success);
			return success;
		}

		template<typename T>
		void ReadObject(T& obj)
		{
			T::Deserialize(this, obj);
		}

		template<typename Key, typename Value>
		void ReadMap(std::map<Key, Value>& map, uint32_t size = 0)
		{
			if (size == 0)
				ReadRaw<uint32_t>(size);

			for (uint32_t i = 0; i < size; i++)
			{
				Key key;
				if constexpr (std::is_trivial<Key>())
					ReadRaw<Key>(key);
				else
					ReadObject<Key>(key);

				if constexpr (std::is_trivial<Value>())
					ReadRaw<Value>(map[key]);
				else
					ReadObject<Value>(map[key]);
			}
		}

		template<typename Key, typename Value>
		void ReadMap(std::unordered_map<Key, Value>& map, uint32_t size = 0)
		{
			if (size == 0)
				ReadRaw<uint32_t>(size);

			for (uint32_t i = 0; i < size; i++)
			{
				Key key;
				if constexpr (std::is_trivial<Key>())
					ReadRaw<Key>(key);
				else
					ReadObject<Key>(key);

				if constexpr (std::is_trivial<Value>())
					ReadRaw<Value>(map[key]);
				else
					ReadObject<Value>(map[key]);
			}
		}

		template<typename Value>
		void ReadMap(std::unordered_map<std::string, Value>& map, uint32_t size = 0)
		{
			if (size == 0)
				ReadRaw<uint32_t>(size);

			for (uint32_t i = 0; i < size; i++)
			{
				std::string key;
				ReadString(key);

				if constexpr (std::is_trivial<Value>())
					ReadRaw<Value>(map[key]);
				else
					ReadObject<Value>(map[key]);
			}
		}

		template<typename T>
		void ReadArray(std::vector<T>& array, uint32_t size = 0)
		{
			if (size == 0)
				ReadRaw<uint32_t>(size);

			array.resize(size);

			for (uint32_t i = 0; i < size; i++)
			{
				if constexpr (std::is_trivial<T>())
					ReadRaw<T>(array[i]);
				else
					ReadObject<T>(array[i]);
			}
		}

		template<>
		void ReadArray(std::vector<std::string>& array, uint32_t size)
		{
			if (size == 0)
				ReadRaw<uint32_t>(size);

			array.resize(size);

			for (uint32_t i = 0; i < size; i++)
				ReadString(array[i]);
		}
	};
}