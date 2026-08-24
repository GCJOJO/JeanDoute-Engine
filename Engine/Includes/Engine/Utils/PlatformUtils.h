#pragma once

#include <string>

namespace Engine 
{
	class OS
	{
	public:
		static bool ChangeDirectory(const std::filesystem::path& newPath);
		static std::filesystem::path GetCurrentDir();
		static std::filesystem::path GetExecutablePath();

		static inline std::filesystem::path GetExecutableDir()
		{
			return GetExecutablePath().parent_path();
		}
	};

	/*class Download
	{
	public:
		static void DownloadFile(const std::string& url, const std::filesystem::path& outFilePath);
		static void UnzipFile(const std::filesystem::path& zipFile, const std::filesystem::path& extractDirectory);
	};*/

	class Time
	{
	public:
		static float GetTime();
	};

	class FileDialogs
	{
	public:
		// Returns empty string if canceled
		static std::string OpenFile(const char* filter, const char* current_directory = nullptr);
		// Returns empty string if canceled
		static std::string SaveFile(const char* filter, const char* current_directory = nullptr);

		static std::string OpenDirectory(const char* current_directory = nullptr);
	};

	class Process
	{
	public:
		static int StartProcess(const char* commandLine, const char* currentDirectory = nullptr, bool waitForProcess = true);
	};
}