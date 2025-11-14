#Molecular Dynamics Simulator GroupE 
Implementation of a MD simulator following the labcourse Scientific Computing (PSE) Molekulardynamik (IN0012, IN4229) at Technische Universtität München during the Wintersemester 2025/2026

## Table of Contents
- [Building the Project](#building the project)
- [Running the Simulation](#running the simulation)
- [Running tests](#running tests)
- [Doxygen](#open doxygen documentation)
- [Running clang-tidy and clang-format](#running clang-tidy and clang-format)

## Building the Project

1: Clone the repository:
```bash
git clone https://github.com/Alexandre-Gallet/MolSim-WS25-GroupE.git
cd MolSim-WS25-GroupE
```

2: Install dependencies:

**Ubuntu/Debian:**
```bash
sudo apt install libvtk9-dev cmake build-essential
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake vtk
```

**Fedora:**
```bash
sudo dnf install vtk-devel cmake gcc-c++ make
```

3: Build the project with cmake and generate doxygen Documentation. By default CMAKE_BUILD_TYPE is set to "Release" which passes the -O3 and -DNDEBUG flags to the compiler:
```bash
cmake --build build -- -j"$(nproc)"
cmake --build build --target doc_doxygen
```

## Running tests 
Assuming you followed the build instructions above, the tests can be called with:
```bash
ctest --test-dir build --output-on-failure -j"$(nproc)"
```

## Running the Simulation

After building the project run the simulation from within the `build` directory with:
```bash
./MolSim <input_file> <sim_type> [t_start] [t_end] [delta_t] [output_format]
```

#### Example:

From the `build` directory using the preexisting input file eingabe-cuboids or eingabe_sonne.txt:
```bash
./MolSim ../input/eingabe-cuboids.txt Molecule -d 0.0002 -e 5
./MolSim ../input/eingabe-sonne.txt Planet -d 0.14
```

## Doxygen 
Assuming you followed the build instructions from above you can open the doxygen documentaiton in browser from `build` directory with: 
```bash
xdg-open docs/html/index.html
```

## Running clang-tidy and clang-format 
Both tools can be called adhering to rules provided in the configuration files `.clang-tidy` and `.clang-format` with: 
```bash
rm -rf build/
mkdir -p build 
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --target run_clang_tidy 
cmake --build build --target clang_format_check

```

