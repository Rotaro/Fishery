# Fishery Simulation

##Introduction
A simple simulation of a fishery, or more precisely, of a fish population which is periodically fished. The 
simulation can be considered to consist of four levels:

- Soil
- Vegetation
- Fish population
- Fishing yield

The simulation is inspired by the book “Mathematical Bioeconomics” by Colin W. Clark. The intention is to reproduce 
some of the analytical curves depicting fishing yields presented in the book. To produce these curves, the simulation
reports the total fish population, the amount of fish caught and the amount of underlying vegetation. The conditions of 
the simulation can be altered, including fish and vegetation growth rate, fishing rate and simulation length. For more 
details, see the documentation in the doc directory.

##Python version
The simulation is written in C, but has been extended to work as a _Python3_ module. For this end, included are:

- fishery_py_module.c
  - c-code for functions which are callable from Python
- setup.py
  - Configuration for compilation with distutils. Has been tested with Microsoft Visual C/C++ compiler 2012 and gcc 4.8.4 (Ubuntu 14.04).
  
Alternatively install with pip: pip install git+https://github.com/Rotaro/Fishery.

For more information see the documentation in the doc directory.
