# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/olga/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.7142.39/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/olga/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.7142.39/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/olga/itmo/3course/distrComp/DistributedComputing/pa4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/pa4.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/pa4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pa4.dir/flags.make

CMakeFiles/pa4.dir/main.c.o: CMakeFiles/pa4.dir/flags.make
CMakeFiles/pa4.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/pa4.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/pa4.dir/main.c.o   -c /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/main.c

CMakeFiles/pa4.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pa4.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/main.c > CMakeFiles/pa4.dir/main.c.i

CMakeFiles/pa4.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pa4.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/main.c -o CMakeFiles/pa4.dir/main.c.s

CMakeFiles/pa4.dir/log.c.o: CMakeFiles/pa4.dir/flags.make
CMakeFiles/pa4.dir/log.c.o: ../log.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/pa4.dir/log.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/pa4.dir/log.c.o   -c /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/log.c

CMakeFiles/pa4.dir/log.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pa4.dir/log.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/log.c > CMakeFiles/pa4.dir/log.c.i

CMakeFiles/pa4.dir/log.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pa4.dir/log.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/log.c -o CMakeFiles/pa4.dir/log.c.s

CMakeFiles/pa4.dir/priorityQueue.c.o: CMakeFiles/pa4.dir/flags.make
CMakeFiles/pa4.dir/priorityQueue.c.o: ../priorityQueue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/pa4.dir/priorityQueue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/pa4.dir/priorityQueue.c.o   -c /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/priorityQueue.c

CMakeFiles/pa4.dir/priorityQueue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pa4.dir/priorityQueue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/priorityQueue.c > CMakeFiles/pa4.dir/priorityQueue.c.i

CMakeFiles/pa4.dir/priorityQueue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pa4.dir/priorityQueue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/priorityQueue.c -o CMakeFiles/pa4.dir/priorityQueue.c.s

# Object files for target pa4
pa4_OBJECTS = \
"CMakeFiles/pa4.dir/main.c.o" \
"CMakeFiles/pa4.dir/log.c.o" \
"CMakeFiles/pa4.dir/priorityQueue.c.o"

# External object files for target pa4
pa4_EXTERNAL_OBJECTS =

pa4: CMakeFiles/pa4.dir/main.c.o
pa4: CMakeFiles/pa4.dir/log.c.o
pa4: CMakeFiles/pa4.dir/priorityQueue.c.o
pa4: CMakeFiles/pa4.dir/build.make
pa4: ../libruntime.so
pa4: CMakeFiles/pa4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable pa4"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pa4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pa4.dir/build: pa4

.PHONY : CMakeFiles/pa4.dir/build

CMakeFiles/pa4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pa4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pa4.dir/clean

CMakeFiles/pa4.dir/depend:
	cd /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/olga/itmo/3course/distrComp/DistributedComputing/pa4 /home/olga/itmo/3course/distrComp/DistributedComputing/pa4 /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug /home/olga/itmo/3course/distrComp/DistributedComputing/pa4/cmake-build-debug/CMakeFiles/pa4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/pa4.dir/depend
