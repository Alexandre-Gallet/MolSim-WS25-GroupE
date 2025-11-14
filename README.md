# Molecular Dynamics Simulator – Group E

Implementation of a molecular dynamics (MD) simulator developed during the lab course  
**Scientific Computing (PSE) – Molekulardynamik (IN0012 / IN4229)**  
at **Technische Universität München**, Winter Semester **2025/2026**.

## Table of Contents
- [Building the Project](#building-the-project)
- [Running the Simulation](#running-the-simulation)
- [Running Tests](#running-tests)
- [Doxygen Documentation](#doxygen-documentation)
- [Clang-Tidy and Clang-Format](#clang-tidy-and-clang-format)

## Building the Project

### 1. Clone the repository
```bash
git clone https://github.com/Alexandre-Gallet/MolSim-WS25-GroupE.git
cd MolSim-WS25-GroupE
```

### 2. Install dependencies

**Ubuntu/Debian**
```bash
sudo apt install libvtk9-dev cmake build-essential
```

**Arch Linux**
```bash
sudo pacman -S base-devel cmake vtk
```

**Fedora**
```bash
sudo dnf install vtk-devel cmake gcc-c++ make
```

### 3. Configure and build the project

You can optionally specify:

- `-DCMAKE_BUILD_TYPE`  
  - Default: `Release` (`-O3`, `-DNDEBUG`)  
  - Other valid values: `Debug`, `RelWithDebInfo`, `MinSizeRel`

- `-DLOG_LEVEL`  
  - Default: `INFO`  
  - Other options: `TRACE`, `DEBUG`, `WARN`, `ERROR`, `CRITICAL`, `OFF`  
  - Defines which logging statements are compiled into the binary.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DLOG_LEVEL=INFO
cmake --build build -- -j"$(nproc)"
cmake --build build --target doc_doxygen
```

## Running the Simulation

After building, run the simulation from within the `build` directory:

```bash
./MolSim <input_file> <sim_type> [t_start] [t_end] [delta_t] [output_format]
```

### Example

Using the provided example input files:

```bash
./MolSim ../input/eingabe-cuboids.txt Molecule -d 0.0002 -e 5
./MolSim ../input/eingabe-sonne.txt Planet   -d 0.14
```

Simulation output files (VTK, XYZ, or both depending on `output_format`)
will appear in the `output/` directory.

If logging is enabled (default), a `simulation.log` file is also generated
in the working directory.

## Running Tests

```bash
ctest --test-dir build --output-on-failure -j"$(nproc)"
```

## Doxygen Documentation

After generating documentation via:

```bash
cmake --build build --target doc_doxygen
```

open it from the `build` directory:

```bash
xdg-open docs/html/index.html
```

## Clang-Tidy and Clang-Format

To run static analysis and formatting checks:

```bash
rm -rf build/
mkdir -p build
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --target run_clang_tidy
cmake --build build --target clang_format_check
```

These commands respect the configuration files `.clang-tidy` and `.clang-format`.
