import os
import sys
import subprocess
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen


class VulkanConfiguration:
    requiredVulkanVersion = "1.4."
    installVulkanVersion = "1.4.328.1"
    vulkanDirectory = "./GamingEngine/vendor/VulkanSDK"
    installDir = ""

    @classmethod
    def Validate(cls, skipConfirmation=False):
        if not cls.CheckVulkanSDK(skipConfirmation):
            print("Vulkan SDK not installed correctly.")
            return
            
        if not cls.CheckVulkanSDKDebugLibs():
            print("\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            print("(see docs.hazelengine.com/GettingStarted for more info).")
            print("Debug configuration disabled.")

    @classmethod
    def CheckVulkanSDK(cls, skipConfirmation=False):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        if vulkanSDK is None:
            print("\nYou don't have the Vulkan SDK installed!")
            cls.__InstallVulkanSDK(skipConfirmation)
            return False
        else:
            print(f"\nLocated Vulkan SDK at {vulkanSDK}")

        if cls.requiredVulkanVersion not in vulkanSDK:
            print(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.requiredVulkanVersion})")
            cls.__InstallVulkanSDK(skipConfirmation)
            return False
    
        print(f"Correct Vulkan SDK located at {vulkanSDK}")
        return True

    @classmethod
    def __InstallVulkanSDK(cls, skipConfirmation=False):
        permissionGranted = skipConfirmation
        while not permissionGranted:
            reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.installVulkanVersion))).lower().strip()[:1]
            if reply == 'n':
                return
            permissionGranted = (reply == 'y')

        vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/windows/vulkansdk-windows-X64-{cls.installVulkanVersion}.exe"
        vulkanPath = f"{cls.vulkanDirectory}/vulkansdk-windows-X64-{cls.installVulkanVersion}.exe"
        print("Downloading {0:s} to {1:s}".format(vulkanInstallURL, vulkanPath))
        Utils.DownloadFile(vulkanInstallURL, vulkanPath)
        print("Running Vulkan SDK installer...")
        #subprocess.call(os.path.abspath(vulkanPath), arguments="--accept-licenses --default-answer --confirm-command install com.lunarg.vulkan.sdl2 com.lunarg.vulkan.glm com.lunarg.vulkan.volk com.lunarg.vulkan.vma com.lunarg.vulkan.debug com.lunarg.vulkan.arm64")
        # installDir = f"{os.path.abspath(cls.vulkanDirectory)}/{cls.installVulkanVersion}"
        # if not skipConfirmation:
        #     print(f"Install Vulkan at C:/VulkanSDK/{cls.installVulkanVersion} or into {os.path.abspath(cls.vulkanDirectory)}/{cls.installVulkanVersion} ?")
        #     answer = ""
        #     while(answer != "y" and answer != "n"):
        #         answer = input("C: root ? Y or N")
        #
        #     if(answer == "y"):
        #         installDir = f"C:/VulkanSDK/{cls.installVulkanVersion}"
        #     else:
        #         installDir = f"{os.path.abspath(cls.vulkanDirectory)}/{cls.installVulkanVersion}"


        #os.system("SETX {0} {1} /M".format("VULKAN_SDK", installDir))
        subprocess.call([os.path.abspath(vulkanPath), "--accept-licenses", "--default-answer", "--confirm-command", "install", "com.lunarg.vulkan.sdl2", "com.lunarg.vulkan.glm", "com.lunarg.vulkan.volk", "com.lunarg.vulkan.vma", "com.lunarg.vulkan.debug", "com.lunarg.vulkan.arm64"])
        print("Vulkan is installed !")
        print("Restart to finish")
        if not skipConfirmation:
            input("Press enter to continue...")
        quit()


    @classmethod
    def CheckVulkanSDKDebugLibs(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")
        
        return shadercdLib.exists()

if __name__ == "__main__":
    VulkanConfiguration.Validate()