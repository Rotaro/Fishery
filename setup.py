from distutils.core import setup, Extension
import os

include_files = [
 os.path.join(os.getcwd(), "src", "fishery_py_module.c"),
 os.path.join(os.getcwd(), "src", "fishery_functions.c"),
 os.path.join(os.getcwd(), "src", "help_functions.c"),
os.path.join(os.getcwd(), "src", "fishery_settings.c")]

fishery_module = Extension('fishery',include_files, include_dirs=[os.path.join(os.getcwd(), "include")])

setup(name='Fishery Simulation', 
      version='1.0', 
      description="Simple simulation of a fishery, i.e. a fish population being fished.",
      url="https://github.com/Rotaro/Fishery/",
      ext_modules=[fishery_module])
