# Notes API

## Overview

**This project is structured to use CMake for building and includes the Crow C++ microframework for web applications.**

### Key Directories and Files

- CMakeLists.txt: The main CMake configuration file.
- build/: Directory containing build artifacts and CMake-generated files.
- include/: Directory containing header files for the Crow framework.
- src/: Directory containing the source code for the project.

---

## Building the Project

To build the project, follow these steps:

1. Clone the repository:

```sh
git clone <repository-url>
cd api_notes
```

2. Create a build/ directory and navigate into it:

   ```sh
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:
   UNIX:
   `sh
    cmake ..
    `
   WIN32:
   `sh
    cmake .. -G "MinGW Makefiles"
    `

4. Build the project:
   UNIX:
   `sh
    cmake --build .
    `
   or
   `sh
    make
    `
   WIN32:
   `sh
    mingw32-make
    `

---

## Running the Server

After building the project, you can run the server executable:

```sh
./api_notes
```

### Adding New Source Files

To add new source files to the project:

Place your new source files in the src directory.
Update the CMakeLists.txt file to include your new source files.
Dependencies
This project uses the Crow C++ microframework, which is included in the include directory.

### Contributing

To contribute to this project, follow these steps:

Fork the repository.
Create a new branch for your feature or bugfix.
Make your changes.
Submit a pull request.

### License

This project is licensed under the MIT License. See the LICENSE file for more details.

### Contact

For any questions or issues, please open an issue in the repository or contact the maintainers.

---

This document provides an overview of the project structure, build instructions, and guidelines for contributing. For more detailed information, refer to the specific files and directories mentioned above.
