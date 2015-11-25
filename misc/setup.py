from distutils.core import setup, Extension

include_files = [
 'C:\\Users\\SuperSSD\\Documents\\Visual Studio 2015\\Projects\\Fishery\\src\\fishery_py_module.c',
 'C:\\Users\\SuperSSD\\Documents\\Visual Studio 2015\\Projects\\Fishery\\src\\fishery_functions.c',
 'C:\\Users\\SuperSSD\\Documents\\Visual Studio 2015\\Projects\\Fishery\\src\\help_functions.c']

fishery_module = Extension('fishery',include_files, include_dirs=['C:\\Users\\SuperSSD\\Documents\\Visual Studio 2015\\Projects\\Fishery\\include'])

setup(name='fishery', version='1.0', ext_modules=[fishery_module])
