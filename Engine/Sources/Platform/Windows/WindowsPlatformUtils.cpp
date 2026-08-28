#include "enginepch.h"
#if ENGINE_PLATFORM_WINDOWS
#include "Engine/Utils/PlatformUtils.h"

#include <commdlg.h>
#include <shlobj.h>
#include <curl/curl.h>
#include <miniunz.c>
#include <iowin32.c>
#include <unzip.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Engine/Core/Application.h"

namespace Engine
{
	static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
		return written;
	}

	const std::filesystem::path Filesystem::GetUserDirectory()
	{
		char* appdata = getenv("APPDATA");
		return std::filesystem::path(appdata) / "JeanDoute-Engine";
	}

	bool Filesystem::ChangeDirectory(const std::filesystem::path& newPath)
	{
		return _chdir(newPath.string().c_str()) == 0 ? true : false;
	}

	std::filesystem::path Filesystem::GetCurrentDir()
	{
		return std::filesystem::current_path();
	}

	std::filesystem::path Filesystem::GetExecutablePath()
	{
		wchar_t path[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, path, MAX_PATH);
		return path;
	}

	
	void Download::DownloadFile(const std::string& url, const std::filesystem::path& outFilePath)
	{
		CURL* curl;
		FILE* fp;
		CURLcode res;
		curl = curl_easy_init();
		if (curl)
		{
			std::filesystem::create_directories(outFilePath.parent_path());
			fp = fopen(outFilePath.string().c_str(), "wb");
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // 1L => CURLFOLLOW_ALL
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			fclose(fp);
		}

	}

	void Download::UnzipFile(const std::filesystem::path& zipFile, const std::filesystem::path& extractDirectory)
	{
		std::filesystem::path currentDir = Filesystem::GetCurrentDir();

		if (Filesystem::ChangeDirectory(extractDirectory))
		{
			unzFile file = NULL;

			zlib_filefunc64_def ffunc;
			fill_win32_filefunc64A(&ffunc);

			file = unzOpen2_64(zipFile.string().c_str(), &ffunc);

			do_extract(file, 0, 0, NULL);

			unzClose(file);

			Filesystem::ChangeDirectory(currentDir);
		}
	}
	
	float Time::GetTime()
	{
		return (float)glfwGetTime();
	}

	std::string FileDialogs::OpenFile(const char* filter, const char* current_directory /*= nullptr*/)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		if (current_directory != nullptr)
		{
			ofn.lpstrInitialDir = current_directory;
		}
		else
		{
			if (GetCurrentDirectoryA(256, currentDir))
				ofn.lpstrInitialDir = currentDir;
		}
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return {};
	}
	
	std::string FileDialogs::SaveFile(const char* filter, const char* current_directory /*= nullptr*/)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (current_directory != nullptr)
		{
			ofn.lpstrInitialDir = current_directory;
		}
		else
		{
			if (GetCurrentDirectoryA(256, currentDir))
				ofn.lpstrInitialDir = currentDir;
		}
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return {};
	}

	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{

		if (uMsg == BFFM_INITIALIZED)
		{
			std::string tmp = (const char*)lpData;
			std::cout << "path: " << tmp << std::endl;
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		}

		return 0;
	}

	std::string FileDialogs::OpenDirectory(const char* current_directory /*= nullptr*/)
	{
		TCHAR path[MAX_PATH];

		BROWSEINFO bi = { 0 };
		bi.lpszTitle = L"Browse for folder...";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		bi.lpfn = BrowseCallbackProc;
		if (current_directory != nullptr)
		{
			bi.lParam = (LPARAM)current_directory;
		}
		else
		{
			CHAR currentDir[256] = { 0 };
			if (GetCurrentDirectoryA(256, currentDir))
				bi.lParam = (LPARAM)currentDir;
		}

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (pidl != 0)
		{
			//get the name of the folder and put it in path
			SHGetPathFromIDList(pidl, path);

			//free memory used
			IMalloc* imalloc = 0;
			if (SUCCEEDED(SHGetMalloc(&imalloc)))
			{
				imalloc->Free(pidl);
				imalloc->Release();
			}

			return (const char*)path;
		}

		return "";
	}

	int Process::StartProcess(const char* commandLine, const char* currentDirectory /*= nullptr*/, bool waitForProcess/* = true*/)
	{
		STARTUPINFOA info = { sizeof(info) };
		PROCESS_INFORMATION processInfo;

		if (CreateProcessA(nullptr, (LPSTR)commandLine, NULL, NULL, TRUE, 0, NULL, currentDirectory, &info, &processInfo))
		{
			DWORD exitCode = 0;
			if (waitForProcess)
			{
				WaitForSingleObject(processInfo.hProcess, INFINITE);
				GetExitCodeProcess(processInfo.hProcess, &exitCode);
			}
			CloseHandle(processInfo.hProcess); // Cleanup since you don't need this
			CloseHandle(processInfo.hThread); // Cleanup since you don't need this
			return (int)exitCode;
		}

		return -1;
	}
}

#endif