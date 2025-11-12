#Molecular Dynamcis Simulator GroupE 
Implementation of a MD simulator following the labcourse Scientific Computing (PSE) Molekulardynamik (IN0012, IN4229) at Technische Universtität München during the Wintersemester 2025/2026

## Table of Contents
- [Building the Project](#building the project)
- [Usage](#usage)
- [Doxygen](#open doxygen documentation)

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

3: Build the project, configure makefiles with ccmake GUI, generate doxygen Documentation
```bash
mkdir -p build
cd build
ccmake ..
make -j$(nproc)
make doc_doxygen
```
## Usage

After building the project, run the simulation with from within the `build` directory:
```bash
./MolSim <input-file> <end_time> <delta_t>
```

### Example:

From the `build` directory using the preexisitng input file eingabe_sonne.txt:
```bash
./MolSim ./../input/eingabe-sonne.txt 1000 0.014
```
## Doxygen 
From the `build` directory open the documentation in browser with:
```bash
xdg-open docs/html/index.html
```

