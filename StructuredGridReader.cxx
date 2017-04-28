#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGridReader.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkAlgorithmOutput.h>
#include <vtkUnstructuredGrid.h>

#include <vtkCellData.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkFieldData.h>
#include <vtkSphereSource.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>

#include <iostream>
#include <iomanip>
#include <algorithm>

int main()
{
  
  std::string inputFilename = "C:/Users/YULIA/Desktop/CFD_199500.vtk";
  
  // Read the file
  vtkSmartPointer<vtkUnstructuredGridReader> reader =
    vtkSmartPointer<vtkUnstructuredGridReader>::New();
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  vtkSmartPointer<vtkPolyData> polydata =
	  vtkSmartPointer<vtkPolyData>::New();
 
  polydata->ShallowCopy(reader->GetOutput());
 
  // Find the range of the point scalars
  
  std::cout << "output has " << polydata->GetNumberOfPoints() << " points." << std::endl;
 
  double range[2];
  reader->GetOutput()->GetScalarRange(range);
 
  cout << "Min value in scalar range: " << range[0] << endl;
  cout << "Max value in scalar range: " << range[1] << endl;
 
  std::vector<vtkSmartPointer<vtkAlgorithmOutput>> data;
  data.push_back(reader->GetOutputPort());
	  
  //Create a mapper and actor
  vtkSmartPointer<vtkDataSetMapper> mapper =
	  vtkSmartPointer<vtkDataSetMapper>::New();
  mapper->SetInputConnection(data[0]);

  vtkSmartPointer<vtkActor> actor =
	  vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  //Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
	  vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
	  vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  //Add the actor to the scene
  renderer->AddActor(actor);
 // renderer->SetBackground(.3, .6, .3); // Background color green

									   //Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;

}
