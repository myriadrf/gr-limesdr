# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hydra/gnuradio/src/gr-limesdr

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hydra/gnuradio/src/gr-limesdr/builddir

# Include any dependencies generated for this target.
include python/limesdr/bindings/CMakeFiles/limesdr_python.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include python/limesdr/bindings/CMakeFiles/limesdr_python.dir/compiler_depend.make

# Include the progress variables for this target.
include python/limesdr/bindings/CMakeFiles/limesdr_python.dir/progress.make

# Include the compile flags for this target's objects.
include python/limesdr/bindings/CMakeFiles/limesdr_python.dir/flags.make

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/flags.make
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.o: ../python/limesdr/bindings/sink_python.cc
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hydra/gnuradio/src/gr-limesdr/builddir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.o"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.o -MF CMakeFiles/limesdr_python.dir/sink_python.cc.o.d -o CMakeFiles/limesdr_python.dir/sink_python.cc.o -c /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/sink_python.cc

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/limesdr_python.dir/sink_python.cc.i"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/sink_python.cc > CMakeFiles/limesdr_python.dir/sink_python.cc.i

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/limesdr_python.dir/sink_python.cc.s"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/sink_python.cc -o CMakeFiles/limesdr_python.dir/sink_python.cc.s

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/flags.make
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.o: ../python/limesdr/bindings/source_python.cc
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hydra/gnuradio/src/gr-limesdr/builddir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.o"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.o -MF CMakeFiles/limesdr_python.dir/source_python.cc.o.d -o CMakeFiles/limesdr_python.dir/source_python.cc.o -c /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/source_python.cc

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/limesdr_python.dir/source_python.cc.i"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/source_python.cc > CMakeFiles/limesdr_python.dir/source_python.cc.i

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/limesdr_python.dir/source_python.cc.s"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/source_python.cc -o CMakeFiles/limesdr_python.dir/source_python.cc.s

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/flags.make
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.o: ../python/limesdr/bindings/python_bindings.cc
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hydra/gnuradio/src/gr-limesdr/builddir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.o"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.o -MF CMakeFiles/limesdr_python.dir/python_bindings.cc.o.d -o CMakeFiles/limesdr_python.dir/python_bindings.cc.o -c /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/python_bindings.cc

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/limesdr_python.dir/python_bindings.cc.i"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/python_bindings.cc > CMakeFiles/limesdr_python.dir/python_bindings.cc.i

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/limesdr_python.dir/python_bindings.cc.s"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/python_bindings.cc -o CMakeFiles/limesdr_python.dir/python_bindings.cc.s

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/flags.make
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.o: ../python/limesdr/bindings/rfe_python.cc
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.o: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hydra/gnuradio/src/gr-limesdr/builddir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.o"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.o -MF CMakeFiles/limesdr_python.dir/rfe_python.cc.o.d -o CMakeFiles/limesdr_python.dir/rfe_python.cc.o -c /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/rfe_python.cc

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/limesdr_python.dir/rfe_python.cc.i"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/rfe_python.cc > CMakeFiles/limesdr_python.dir/rfe_python.cc.i

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/limesdr_python.dir/rfe_python.cc.s"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings/rfe_python.cc -o CMakeFiles/limesdr_python.dir/rfe_python.cc.s

# Object files for target limesdr_python
limesdr_python_OBJECTS = \
"CMakeFiles/limesdr_python.dir/sink_python.cc.o" \
"CMakeFiles/limesdr_python.dir/source_python.cc.o" \
"CMakeFiles/limesdr_python.dir/python_bindings.cc.o" \
"CMakeFiles/limesdr_python.dir/rfe_python.cc.o"

# External object files for target limesdr_python
limesdr_python_EXTERNAL_OBJECTS =

python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/sink_python.cc.o
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/source_python.cc.o
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/python_bindings.cc.o
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/rfe_python.cc.o
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/build.make
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.74.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libboost_system.so.1.74.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libboost_regex.so.1.74.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.so.1.74.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: lib/libgnuradio-limesdr.so.1.0.0.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /home/hydra/gnuradio/lib/libLimeSuite.so
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /home/hydra/gnuradio/lib/libgnuradio-runtime.so.v3.11.0.0git-207-gcaa7e7f1
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libboost_program_options.so.1.74.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /home/hydra/gnuradio/lib/libgnuradio-pmt.so.v3.11.0.0git-207-gcaa7e7f1
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.74.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libboost_atomic.so.1.74.0
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /home/hydra/gnuradio/lib/libvolk.so.2.5.1
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libspdlog.so.1.9.2
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libfmt.so.8.1.1
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libgmpxx.so
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: /usr/lib/x86_64-linux-gnu/libgmp.so
python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so: python/limesdr/bindings/CMakeFiles/limesdr_python.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hydra/gnuradio/src/gr-limesdr/builddir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX shared module limesdr_python.cpython-310-x86_64-linux-gnu.so"
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/limesdr_python.dir/link.txt --verbose=$(VERBOSE)
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/strip /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && /usr/bin/cmake -E copy /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so /home/hydra/gnuradio/src/gr-limesdr/builddir/test_modules/gnuradio/limesdr/

# Rule to build all files generated by this target.
python/limesdr/bindings/CMakeFiles/limesdr_python.dir/build: python/limesdr/bindings/limesdr_python.cpython-310-x86_64-linux-gnu.so
.PHONY : python/limesdr/bindings/CMakeFiles/limesdr_python.dir/build

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/clean:
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings && $(CMAKE_COMMAND) -P CMakeFiles/limesdr_python.dir/cmake_clean.cmake
.PHONY : python/limesdr/bindings/CMakeFiles/limesdr_python.dir/clean

python/limesdr/bindings/CMakeFiles/limesdr_python.dir/depend:
	cd /home/hydra/gnuradio/src/gr-limesdr/builddir && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hydra/gnuradio/src/gr-limesdr /home/hydra/gnuradio/src/gr-limesdr/python/limesdr/bindings /home/hydra/gnuradio/src/gr-limesdr/builddir /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings /home/hydra/gnuradio/src/gr-limesdr/builddir/python/limesdr/bindings/CMakeFiles/limesdr_python.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : python/limesdr/bindings/CMakeFiles/limesdr_python.dir/depend

