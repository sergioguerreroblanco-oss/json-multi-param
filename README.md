# JSON multi param

![CI](https://github.com/sergioguerreroblanco-oss/json-multi-param/actions/workflows/ci.yml/badge.svg)


---

## ✨ Core Features

- **acbacbacfb**
  - acbacb.
  - acbacb.
  - acbacb.

---

## 🌟 Project Highlights

- **acbacb**
  - acbacb.

---

## 🏗 Architecture

The following diagram illustrates the internal architecture of the project:

```mermaid
flowchart LR
```

---

## 🗂 Class Diagram

```mermaid
classDiagram
```

---

## 🛠 Build Instructions

### Windows (Visual Studio 2022)

1. Open Visual Studio 2022.

2. Choose “Open Folder” → select the project root (json-multi-param/).

3. Visual Studio automatically detects CMakePresets.json.

4. In the toolbar, select a configuration:

    - debug → includes unit tests (BUILD_TESTING=ON).

    - release → optimized build.

5. Build the project (Ctrl + Shift + B).

6. The executable will be generated at:

```bash
build/<preset>/json-multi-param[.exe]
```

7. Run it directly from Visual Studio or from the command line.

---

### Windows (PowerShell/CLI)
You can also build manually from a terminal with CMake presets:

```powershell
cmake --preset release    # Configure for Release
cmake --build --preset release -j  # Build using Ninja or MSBuild

cmake --preset debug      # Configure for Debug (with tests)
cmake --build --preset debug -j
ctest --preset debug      # Run unit tests
```
---

### Linux / WSL (Debian/Ubuntu based)
Install dependencies:
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build git
```
Clone and build:
```bash
git clone https://github.com/sergioguerreroblanco-oss/json-multi-param.git
cd json-multi-param

cmake --preset release
cmake --build --preset release -j$(nproc)

# Debug build (includes unit tests)
cmake --preset debug
cmake --build --preset debug -j$(nproc)
ctest --preset debug
```
The resulting binary will be located in:
```
build/release/json-multi-param
```

### Docker (fully reproducible environment)
The project includes a Dockerfile that performs a clean build and runs tests automatically:
```bash
docker build -t json-multi-param:local .
docker run --rm json-multi-param:local
```

---

## 🧪 Unit Tests

Unit tests are implemented using GoogleTest and fully integrated into the build system via CTest.
Running a Debug build automatically enables all tests (BUILD_TESTING=ON).

The test suite ensures the correctness and thread-safety of all major components in the project.

### ✅ Covered Scenarios

Below is the list of all behaviors validated by the current test suite:

#### 🧵 ACBACBACB

| Test Name                               | Validates                                                                                                  |
| --------------------------------------- | ---------------------------------------------------------------------------------------------------------- |
| **ACBACB**        | acbacb                                                             |

#### 🧩 Behavioral Guarantees Provided by Tests

The test suite ensures:

✔ ACBACB is safe
- acbacb
- Correct shutdown semantics (shutdown() + shutdownNow())
- Robust to exceptions inside jobs
- No job is silently lost unless shutdownNow() is explicit

### Running Tests (Windows)

On Windows, the project has been validated with Visual Studio 2022.
Tests can be executed directly from Test Explorer:
```Menu → Test → Run All Tests```

Alternatively, you can also build and run tests using the provided CMake presets:
```powershell
cmake --preset debug
cmake --build --preset debug
ctest --preset debug --output-on-failure
```
> Note - Need to have installed: 
> 1. [CMake](https://github.com/Kitware/CMake/releases/download/v4.1.1/cmake-4.1.1-windows-x86_64.msi)
> 2. Ninja: 
>```bash 
> winget install Ninja-build.Ninja
>```

### Running Tests (Linux / Docker)

After building the project, run the following command inside the build directory:

```bash
cd build/debug
ctest --output-on-failure
```

This will automatically discover and execute all registered GoogleTest cases.

### Example output:
(example output inside container) 
```

```

---

## 🐳 Docker

This project includes a Dockerfile to provide a reproducible build and test environment.

Requires Docker installed and running on your system.

Build image:

```docker build -t json-multi-param:dev .```

Run tests inside container:

```docker run --rm json-multi-param:dev```

Run main binary:

```docker run --rm json-multi-param:dev ./build/release/json-multi-param```

By default, the container builds the project in /app/build/. The binary can be invoked as shown.


---

## 🔄 Continuous Integration

This project provides two GitHub Actions workflows under .github/workflows/:

### ci.yml
This workflow performs the following steps:
- Builds the project on ubuntu-latest using CMake + Ninja and g++.
- Runs all unit tests using CTest (with detailed failure output).
- Builds the Docker image and verifies that it executes correctly inside a container.

✅ Ensures that:
- The codebase compiles cleanly on a fresh Linux environment.
- All unit tests pass consistently.
- The Docker image remains functional and up-to-date.

### docs.yml

This workflow automates documentation generation:
- Installs Doxygen, Graphviz, and LaTeX.
- Generates both HTML and PDF documentation.
- Uploads the generated artifacts for download directly from the workflow summary.
- Deploys HTML docs to GitHub Pages when pushing to the main branch.

✅ Ensures that:
- Documentation is always in sync with the current source code.
- Both HTML and PDF outputs are built automatically.
- Public docs can be viewed directly from the project’s GitHub Pages site.

---

## 📂 Project Structure

```

```

---

## 📖 Documentation

This project uses [Doxygen](https://www.doxygen.nl/) to generate API documentation
from source code comments.

### Generate documentation

#### Windows (PowerShell)
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\scripts\generate_docs.ps1
```
Generates HTML (docs/html/index.html) and LaTeX sources (docs/latex/).

PDF (refman.pdf) is not generated on Windows unless you install a full LaTeX toolchain (MiKTeX/TeX Live + make).

#### Linux / WSL (Debian/Ubuntu based)
```bash
sudo apt update
sudo apt install -y doxygen graphviz texlive-latex-base texlive-fonts-recommended texlive-latex-extra make
chmod +x scripts/generate_docs.sh
./scripts/generate_docs.sh
```
Generates HTML (docs/html/index.html) and PDF (docs/latex/refman.pdf).

Open in your browser:
```bash
docs/html/index.html
```

For more details, see docs/README.md

---

## 🎨 Code Style (clang-format)

This project uses **clang-format** to enforce a consistent C++ code style.  
The formatting rules are defined in [`.clang-format`](./.clang-format). 

### 🔧 Chosen Style

The configuration is based on Google C++ Style, with a few modifications to improve readability for embedded and multithreaded systems:

| Setting                                 | Value               | Description                                                                      |
| --------------------------------------- | ------------------- | -------------------------------------------------------------------------------- |
| **BasedOnStyle**                        | `Google`            | Provides a solid, modern baseline for C++ formatting.                            |
| **Language**                            | `Cpp`               | Ensures C++-specific syntax rules (not generic C).                               |
| **IndentWidth / TabWidth**              | `4`                 | Uses 4 spaces per indent (vs Google’s default 2) for improved block readability. |
| **UseTab**                              | `Never`             | Enforces spaces only (consistent across editors).                                |
| **ColumnLimit**                         | `100`               | Balances readability and long template expressions.                              |
| **BreakBeforeBraces**                   | `Allman`            | Places braces on their own line, improving visual structure in nested code.      |
| **AllowShortIfStatementsOnASingleLine** | `false`             | Forces explicit line breaks for clarity.                                         |
| **AllowShortLoopsOnASingleLine**        | `false`             | Prevents compact loop bodies from being overlooked.                              |
| **AllowShortFunctionsOnASingleLine**    | `Inline`            | Allows concise inline functions on a single line only.                           |
| **PointerAlignment**                    | `Left`              | Keeps `T* ptr` instead of `T *ptr` — cleaner and more conventional in C++.       |
| **DerivePointerAlignment**              | `false`             | Disables automatic guessing; enforces explicit `PointerAlignment`.               |
| **SpaceBeforeParens**                   | `ControlStatements` | Adds a space in `if (` / `while (` for readability, not for calls.               |
| **SortIncludes**                        | `true`              | Automatically organizes include directives alphabetically.                       |
| **IncludeBlocks**                       | `Regroup`           | Groups and reorders includes logically within the file.                          |
| **AlignConsecutiveAssignments**         | `true`              | Improves alignment of variable assignments.                                      |
| **AlignConsecutiveDeclarations**        | `true`              | Aligns consecutive variable declarations.                                        |
| **AlignOperands**                       | `true`              | Keeps arithmetic/logical expressions neatly aligned.                             |
| **CommentPragmas**                      | `'^!'`              | Allows pragma-specific comment formatting.                                       |
| **FixNamespaceComments**                | `true`              | Automatically adds trailing `// namespace X` comments when closing.              |
| **SpacesBeforeTrailingComments**        | `2`                 | Keeps a consistent gap before end-of-line comments.                              |
| **KeepEmptyLinesAtTheStartOfBlocks**    | `false`             | Avoids unnecessary blank lines at block starts.                                  |

---

### Windows (Visual Studio Code / PowerShell)

1. Install LLVM (includes clang-format):

    - Download the LLVM installer for Windows (`https://github.com/llvm/llvm-project/releases`)
    
    - During setup, check “Add LLVM to the system PATH”.

2. Verify installation:

```PowerShell
clang-format --version
```
3. Format all project files:
```PowerShell
clang-format -i include\*.h include\*.ipp src\*.cpp tests\*.cpp
```
### Linux / WSL (Debian/Ubuntu based)

1. Install clang-format:
```bash
sudo apt update
sudo apt install -y clang-format
```
2. Verify installation:
```bash
clang-format --version
```
3. Format all project files:
```bash
clang-format -i include/*.h include/*.ipp src/*.cpp tests/*.cpp
```

---

## 📌 Notes

- **C++ Standard**: The project uses **C++14**, configured via ```set(CMAKE_CXX_STANDARD 14)``` ensuring maximum compatibility with embedded systems, legacy compilers, and cross-platform builds.
    
- **Logging**:

    A fully centralized, thread-safe Logger utility is used throughout the project.
    It provides:
    - Atomic writes to avoid interleaved logs
    - Timestamps with millisecond precision
    - Configurable severity levels (DEBUG, INFO, WARN, ERROR)
    - Uniform log formatting across all components (ThreadPool, JobQueue, Jobs, main application)

---

## ❤️ Acknowledgements
