import platform
import subprocess
import sys
import os
from pathlib import Path

import Utils

class CmakeConfiguration:
    cmakeInstallVersion = "4.4.3"
    cmakeDownloadUrl = f"https://github.com/Kitware/CMake/releases/download/v{cmakeInstallVersion}/cmake-{cmakeInstallVersion}-windows-x86_64.msi"

    @classmethod
    def validate(cls, skipConfirmation=False):
        if (not cls.check_if_cmake_installed(skipConfirmation)):
            return cls.install_cmake(skipConfirmation)
        return True

    @classmethod
    def check_if_cmake_installed(cls, skipConfirmation=False):
        try:
            subprocess.run(["cmake", "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            return False

    @classmethod
    def install_cmake(cls, skipConfirmation=False):
        permissionGranted = skipConfirmation
        while not permissionGranted:
            reply = str(input("CMake not found. Would you like to download and install CMake? [Y/N]: ")).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        if platform.system() == "Windows":
            print(f"Downloading CMake installer from {cls.cmakeDownloadUrl}...")
            cmakeInstallerPath = Path(f"./vendor/cmake-{cls.cmakeInstallVersion}-windows-x86_64.msi")
            Utils.DownloadFile(cls.cmakeDownloadUrl, cmakeInstallerPath)

            print(f"Running CMake installer from {cmakeInstallerPath}...")
            exit_code = subprocess.run(["msiexec", "/i", str(cmakeInstallerPath), "/quiet", "/norestart"], check=True).returncode

            if(exit_code != 0):
                print(f"CMake installation failed with exit code {exit_code}. Please retry or install CMake manually.")
                return False
            
            print("CMake installation completed.\n Relaunch this script to continue the setup process.")
            return True
             
        elif platform.system() == "Linux":
            exit_code = subprocess.run(["sudo", "apt", "install", "-y", "cmake"], check=True).returncode

            if(exit_code != 0):
                print(f"CMake installation failed with exit code {exit_code}. Please retry or install CMake manually.")
                return False
            
            print("CMake installation completed.\n Relaunch this script to continue the setup process.")
            return True

        elif platform.system() == "Darwin" or platform.system() == "macOS":
            exit_code = subprocess.run(["brew", "install", "cmake"], check=True).returncode

            if(exit_code != 0):
                print(f"CMake installation failed with exit code {exit_code}. Please retry or install CMake manually.")
                return False
            
            print("CMake installation completed.\n Relaunch this script to continue the setup process.")
            return True