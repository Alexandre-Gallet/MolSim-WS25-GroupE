/*
 * VTKWriter.cpp
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */


#include "VTKWriter.h"

#include <vtkCellArray.h>
#include <vtkCellType.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <iomanip>
#include <sstream>

namespace outputWriter {

void VTKWriter::plotParticles(Container &particles, const std::string &filename, int iteration) {
  // Initialize points
  vtkNew<vtkPoints> points;
  const vtkIdType numPoints = static_cast<vtkIdType>(particles.size());
  points->SetNumberOfPoints(numPoints);

  // Create and configure data arrays
  vtkNew<vtkFloatArray> massArray;
  massArray->SetName("mass");
  massArray->SetNumberOfComponents(1);
  massArray->SetNumberOfTuples(numPoints);

  vtkNew<vtkFloatArray> velocityArray;
  velocityArray->SetName("velocity");
  velocityArray->SetNumberOfComponents(3);
  velocityArray->SetNumberOfTuples(numPoints);

  vtkNew<vtkFloatArray> forceArray;
  forceArray->SetName("force");
  forceArray->SetNumberOfComponents(3);
  forceArray->SetNumberOfTuples(numPoints);

  vtkNew<vtkIntArray> typeArray;
  typeArray->SetName("type");
  typeArray->SetNumberOfComponents(1);
  typeArray->SetNumberOfTuples(numPoints);

  vtkNew<vtkCellArray> vertices;
  vertices->AllocateEstimate(numPoints, 1);

  vtkIdType idx = 0;
  for (auto &p : particles) {
    points->SetPoint(idx, p.getX().data());
    massArray->SetValue(idx, static_cast<float>(p.getM()));
    velocityArray->SetTuple(idx, p.getV().data());
    forceArray->SetTuple(idx, p.getF().data());
    typeArray->SetValue(idx, p.getType());

    vtkIdType cell[1] = {idx};
    vertices->InsertNextCell(1, cell);
    ++idx;
  }

  // Set up the grid
  auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
  grid->SetPoints(points);
  grid->SetCells(VTK_VERTEX, vertices);

  // Add arrays to the grid
  grid->GetPointData()->AddArray(massArray);
  grid->GetPointData()->AddArray(velocityArray);
  grid->GetPointData()->AddArray(forceArray);
  grid->GetPointData()->AddArray(typeArray);

  // Create filename with iteration number
  std::stringstream strstr;
  strstr << filename << "_" << std::setfill('0') << std::setw(4) << iteration << ".vtu";

  // Create writer and set data
  vtkNew<vtkXMLUnstructuredGridWriter> writer;
  writer->SetFileName(strstr.str().c_str());
  writer->SetInputData(grid);
  writer->SetDataModeToAscii();
  writer->EncodeAppendedDataOff();

  // Write the file
  writer->Write();
}
}  // namespace outputWriter
