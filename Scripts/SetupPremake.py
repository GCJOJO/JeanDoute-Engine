import platform
import sys
import os
from pathlib import Path

import Utils

class PremakeConfiguration:
    premakeVersion = "5.0.0-beta8"
    premakeZipBaseUrl = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}"
    premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeDirectory = "./vendor/premake/bin"

    @classmethod
    def Validate(cls, skipConfirmation=False):
        if (not cls.CheckIfPremakeInstalled(skipConfirmation)):
            print("Premake is not installed.")
            return False

        print(f"Correct Premake located at {os.path.abspath(cls.premakeDirectory)}")
        return True

    @classmethod
    def CheckIfPremakeInstalled(cls, skipConfirmation=False):
        if platform.system() == "Windows":
            premakeExe = Path(f"{cls.premakeDirectory}/premake5.exe")
        if platform.system() == "Linux":
            premakeExe = Path(f"{cls.premakeDirectory}/premake5")
        if platform.system() == "Darwin" or platform.system() == "macOS":
            premakeExe = Path(f"{cls.premakeDirectory}/premake5")

        if not premakeExe.exists():
            return cls.InstallPremake(skipConfirmation)

        return True

    @classmethod
    def InstallPremake(cls, skipConfirmation=False):
        permissionGranted = skipConfirmation
        while not permissionGranted:
            reply = str(input("Premake not found. Would you like to download Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        if platform.system() == "Windows":
            premakePlatformFile = "windows.zip"
        if platform.system() == "Linux":
            premakePlatformFile = "linux.tar.gz"
        if platform.system() == "Darwin" or platform.system() == "macOS":
            premakePlatformFile = "macosx-x64.tar.gz"

        premakePath = f"{cls.premakeDirectory}/premake-{cls.premakeVersion}-{premakePlatformFile}"
        print("Downloading {0:s} to {1:s}".format(f"{cls.premakeZipBaseUrl}-{premakePlatformFile}", premakePath))
        Utils.DownloadFile(f"{cls.premakeZipBaseUrl}-{premakePlatformFile}", premakePath)
        print("Extracting", premakePath)
        Utils.UnzipArchive(premakePath, deleteArchive=True)
        print(f"Premake {cls.premakeVersion} has been downloaded to '{cls.premakeDirectory}'")

        premakeLicensePath = f"{cls.premakeDirectory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premakeLicenseUrl, premakeLicensePath))
        Utils.DownloadFile(cls.premakeLicenseUrl, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{cls.premakeDirectory}'")

        return True