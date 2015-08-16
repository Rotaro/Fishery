from distutils.core import setup, Extension

include_files = [
 'C:\\Users\\SuperSSD\\Documents\\Visual Studio 2013\\Projects\\Fishery\\fishery_py_module.c',
 'C:\\Users\\SuperSSD\\Documents\\Visual Studio 2013\\Projects\\Fishery\\fishery_functions.c',
 'C:\\Users\\SuperSSD\\Documents\\Visual Studio 2013\\Projects\\Fishery\\help_functions.c']

fishery_module = Extension('fishery',include_files, include_dirs=['C:\\Users\\SuperSSD\\Documents\\Visual Studio 2013\\Projects\\Fishery'])

setup(name='fishery', version='1.0', ext_modules=[fishery_module])
