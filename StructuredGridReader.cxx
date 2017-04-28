#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkAlgorithmOutput.h>
#include <vtkNamedColors.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkCellData.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkFieldData.h>
#include <iostream>
#include <iomanip>
#include <algorithm>


template <typename T>
void PrintColour(T &rgb)
{
	// Don't do this in real code! Range checking etc. is needed.
	for (size_t i = 0; i < 3; ++i)
	{
		if (i < 2)
		{
			std::cout << static_cast<double>(rgb[i]) << " ";
		}
		else
		{
			std::cout << static_cast<double>(rgb[i]);
		}
	}
}


//! Use a color transfer Function to generate the colors in the lookup table.
void MakeLUTFromCTF(size_t const & tableSize, vtkLookupTable *lut)
{
	vtkSmartPointer<vtkColorTransferFunction> ctf =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	ctf->SetColorSpaceToDiverging();

	// Green to tan.
	ctf->AddRGBPoint(0.0, 0.085, 0.532, 0.201);
	ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
	ctf->AddRGBPoint(1.0, 0.677, 0.492, 0.093);

	lut->SetNumberOfTableValues(tableSize);
	lut->Build();

	for (size_t i = 0; i < tableSize; ++i)
	{
		double *rgb;
		rgb = ctf->GetColor(static_cast<double>(i) / tableSize);
		lut->SetTableValue(i, rgb);
	}
}

//! Create the cell data using the colors from the lookup table.
void MakeCellData(size_t const & tableSize, vtkLookupTable *lut,
	vtkUnsignedCharArray *colors)
{
	for (size_t i = 1; i < tableSize; i++)
	{
		double rgb[3];
		unsigned char ucrgb[3];
		// Get the interpolated color.
		// Of course you can use any function whose range is [0...1]
		// to get the required color and assign it to a cell Id.
		// In this case we are just using the cell (Id + 1)/(tableSize - 1)
		// to get the interpolated color.
		lut->GetColor(static_cast<double>(i) / (tableSize - 1), rgb);
		for (size_t j = 0; j < 3; ++j)
		{
			ucrgb[j] = static_cast<unsigned char>(rgb[j] * 255);
		}
		colors->InsertNextTuple3(ucrgb[0], ucrgb[1], ucrgb[2]);
		// Print out what we have.
		std::cout << i << " ";
		std::cout << "(";
		PrintColour<double[3]>(rgb);
		std::cout << ") (";
		PrintColour<unsigned char[3]>(ucrgb);
		std::cout << ")" << std::endl;
	}
}

int main(int, char *[])
{
	vtkSmartPointer<vtkNamedColors> nc =
		vtkSmartPointer<vtkNamedColors>::New();

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
	double scalarRange[2];
	reader->GetOutput()->GetScalarRange(scalarRange);

	cout << scalarRange[0] << " " << scalarRange[1];

	// Create a lookup table to map cell data to colors
	vtkSmartPointer<vtkLookupTable> lookupTable =
		vtkSmartPointer<vtkLookupTable>::New();

	//lookupTable->SetTableRange(0, 1);
	//lookupTable->Build();

	// Generate the colors for each point based on the color map
	vtkSmartPointer<vtkUnsignedCharArray> colors =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	int tableSize = polydata->GetNumberOfPoints();

	std::cout << "There are " << tableSize << " points." << std::endl;

	MakeLUTFromCTF(tableSize, lookupTable);

	vtkSmartPointer<vtkUnsignedCharArray> colorData =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colorData->SetName("colors");
	colorData->SetNumberOfComponents(3);
	std::cout << "Using a lookup table created from a color transfer function."
		<< std::endl;
	MakeCellData(tableSize, lookupTable, colorData);
	polydata->GetPointData()->SetScalars(colors);

	//plane12->GetOutput()->GetCellData()->SetScalars(colorData);

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	mapper->SetInputData(polydata);
	mapper->SetScalarModeToUseCellData();
	mapper->Update();

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// Create a renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Add the actor to the scene
	renderer->AddActor(actor);
	renderer->SetBackground(.1, .2, .3);

	// Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

}
