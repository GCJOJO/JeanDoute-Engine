import sys
import os
from pathlib import Path

import Utils

class WindowsConfiguration:
    vswhere_version = "3.1.7"
    vswhere_download_url = f"https://github.com/microsoft/vswhere/releases/download/{vswhere_version}/vswhere.exe"
    vswhere_path = "./vendor/vswhere.exe"

    @classmethod
    def validate(cls, skipConfirmation=False):
        if not cls.check_if_vswhere_installed(skipConfirmation):
            print("vswhere is not installed.")
            return False

        print(f"Correct vswhere located at {os.path.abspath(cls.vswhere_path)}")
        return True

    @classmethod
    def check_if_vswhere_installed(cls, skipConfirmation=False):
        vswhere_exe = Path(cls.vswhere_path)
        if not vswhere_exe.exists():
            return cls.install_vswhere(skipConfirmation)

        return True

    @classmethod
    def install_vswhere(cls, skipConfirmation=False):
        permissionGranted = skipConfirmation
        while not permissionGranted:
            reply = str(input("vswhere not found. Would you like to download vswhere {0:s}? [Y/N]: ".format(cls.vswhere_version))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        print("Downloading {0:s} to {1:s}".format(cls.vswhere_download_url, cls.vswhere_path))
        Utils.DownloadFile(cls.vswhere_download_url, cls.vswhere_path)
        print(f"vswhere {cls.vswhere_version} has been downloaded to '{cls.vswhere_path}'")

        return True