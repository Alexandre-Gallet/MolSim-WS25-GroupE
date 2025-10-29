# Molecular Dynamcis Simulator GroupE 
Implementation of a MD simulator following the labcourse Scientific Computing (PSE) Molekulardynamik (IN0012, IN4229) at Technische Universtität München during the Wintersemester 2025/2026

## Table of Contents
- [Building the Project](#building the project)
- [Usage](#usage)

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

3: Build the project:
```bash
mkdir -p build
cd build
ccmake ..
make -j$(nproc)
```
## Usage

After building the project, run the simulation with:
```bash
./MolSim <input-file>
```

### Example:

From the `build` directory using the preexisitng input file eingabe_sonne.txt:
```bash
./MolSim ./../input/eingabe_sonne.txt
```


