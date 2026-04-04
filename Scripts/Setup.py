import argparse
import os
from pathlib import Path
import subprocess
import platform

from SetupPython import PythonConfiguration as PythonRequirements

import ctypes, sys

def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

if is_admin():
    # Code of your program here

    parser = argparse.ArgumentParser()
    skip_parser = parser.add_mutually_exclusive_group(required=False)
    skip_parser.add_argument(
            "-y", "--skip-confirmation", help="Skips all install confirmation", dest="skip", action="store_true"
        )

    skip_vulkan = parser.add_mutually_exclusive_group(required=False)
    skip_vulkan.add_argument(
        "-v", "--skip-vulkan", help="Skips all vulkan installation", dest="skipVk", action="store_true"
    )
    parser.set_defaults(skip=False)
    parser.set_defaults(skipVk=False)
    args = parser.parse_args()

    # Make sure everything we need for the setup is installed
    PythonRequirements.Validate(args.skip)

    from SetupPremake import PremakeConfiguration as PremakeRequirements
    from SetupVulkan import VulkanConfiguration as VulkanRequirements
    os.chdir('./../') # Change from devtools/scripts directory to root

    premakeInstalled = PremakeRequirements.Validate(args.skip)
    if not args.skipVk:
        VulkanRequirements.Validate(args.skip)

    print("\nUpdating submodules...")
    subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

    if premakeInstalled:
        if platform.system() == "Windows":
            print("\nRunning premake...")
            subprocess.call([os.path.abspath("./scripts/Win-GenProjects.bat"), "nopause"])
            engine_dir = Path(os.getcwd()).resolve()
            print("\nSetting Environment Variable JEANDOUTEENGINE_DIR to", engine_dir)
            os.system("SETX {0} {1} /M".format("JEANDOUTEENGINE_DIR", engine_dir.__str__()))

        print("\nSetup completed!")
        if not args.skip:
            input("Press Enter to continue...")
    else:
        print("Gaming Engine requires Premake to generate project files.")

else:
    # Re-run the program with admin rights
    ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)