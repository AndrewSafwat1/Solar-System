# Installation and Usage

1. Open a terminal and go to the project directory
```
cd /path/to/the/project
```

2. Create a build/ directory (if not already created)
```
mkdir -p build
cd build
```

3. Run CMake to generate build files
```
cmake -G "MinGW Makefiles" ..
```

4. Build the project
```
cmake --build .
```

5. Run the executable
```
.\inOneWeekend.exe > image.ppm
```
