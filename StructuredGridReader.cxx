#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
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
#include <vtkArray.h>
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
void MakeCellData(size_t const & tableSize, vtkLookupTable *lut, vtkUnsignedCharArray *colors)
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

		//lut->GetColor(static_cast<double>(i) / (tableSize - 1), rgb);
		lut->GetColor(abs(static_cast<double>(i)) / tableSize, rgb);
		for (size_t j = 0; j < 3; ++j)
		{
			ucrgb[j] = static_cast<unsigned char>(rgb[j] * 255);
		}
		colors->InsertNextTuple3(ucrgb[0], ucrgb[1], ucrgb[2]);
		// Print out what we have.
		/*std::cout << i << " ";
		std::cout << "(";
		PrintColour<double[3]>(rgb);
		std::cout << ") (";
		PrintColour<unsigned char[3]>(ucrgb);
		std::cout << ")" << std::endl;*/
	}
}

void FindAllData(vtkPolyData* polydata)
{
	std::cout << "Normals: " << polydata->GetPointData()->GetNormals() << std::endl;

	vtkIdType numberOfPointArrays = polydata->GetPointData()->GetNumberOfArrays();
	std::cout << "Number of PointData arrays: " << numberOfPointArrays << std::endl;

	vtkIdType numberOfCellArrays = polydata->GetCellData()->GetNumberOfArrays();
	std::cout << "Number of CellData arrays: " << numberOfCellArrays << std::endl;

	std::cout << "Type table/key: " << std::endl;;
	//more values can be found in <VTK_DIR>/Common/vtkSetGet.h
	std::cout << VTK_UNSIGNED_CHAR << " unsigned char" << std::endl;
	std::cout << VTK_UNSIGNED_INT << " unsigned int" << std::endl;
	std::cout << VTK_FLOAT << " float" << std::endl;
	std::cout << VTK_DOUBLE << " double" << std::endl;

	for (vtkIdType i = 0; i < numberOfPointArrays; i++)
	{
		// The following two lines are equivalent
		//arrayNames.push_back(polydata->GetPointData()->GetArray(i)->GetName());
		//arrayNames.push_back(polydata->GetPointData()->GetArrayName(i));
		int dataTypeID = polydata->GetPointData()->GetArray(i)->GetDataType();
		std::cout << "Array " << i << ": " << polydata->GetPointData()->GetArrayName(i)
			<< " (type: " << dataTypeID << ")" << std::endl;
	}

	for (vtkIdType i = 0; i < numberOfCellArrays; i++)
	{
		// The following two lines are equivalent
		//polydata->GetPointData()->GetArray(i)->GetName();
		//polydata->GetPointData()->GetArrayName(i);
		int dataTypeID = polydata->GetCellData()->GetArray(i)->GetDataType();
		std::cout << "Array " << i << ": " << polydata->GetCellData()->GetArrayName(i)
			<< " (type: " << dataTypeID << ")" << std::endl;
	}
}

int main(int, char *[])
{
	vtkSmartPointer<vtkNamedColors> nc =
		vtkSmartPointer<vtkNamedColors>::New();

	std::string inputFilename = "C:/Users/YULIA/Desktop/Semestr letni/VTK_files/VTK/frac_bottom/frac_bottom_199500.vtk";
	//std::string inputFilename = "C:/Users/YULIA/Desktop/SCFD_199500.vtk";

	// Read the file
	vtkSmartPointer<vtkPolyDataReader> reader =
		vtkSmartPointer<vtkPolyDataReader>::New();

	/*vtkSmartPointer<vtkUnstructuredGridReader> reader =
		vtkSmartPointer<vtkUnstructuredGridReader>::New();*/
	reader->SetFileName(inputFilename.c_str());
	reader->Update();

	vtkSmartPointer<vtkPolyData> polydata =
		vtkSmartPointer<vtkPolyData>::New();
	//polydata->ShallowCopy(reader->GetOutput());
	polydata = reader->GetOutput();

	FindAllData(polydata);

	// Create a lookup table to map cell data to colors
	int arr_num = 11;
	int dataTypeID = polydata->GetCellData()->GetArray(arr_num)->GetDataType();
	std::cout << "Array " << arr_num << ": " << polydata->GetCellData()->GetArrayName(arr_num)
		<< " (type: " << dataTypeID << ")" << std::endl;

	double arrayRange[2];
	
	polydata->GetCellData()->GetArray(arr_num)->GetRange(arrayRange);
	//reader->GetOutput()->GetScalarRange(arrayRange);
	cout << "Range of values for arrays: " << arrayRange[0] << " " << arrayRange[1] << endl;
	
	double scaleRange = abs(arrayRange[0]) + abs(arrayRange[1]);

	vtkSmartPointer<vtkLookupTable> lookupTable =
		vtkSmartPointer<vtkLookupTable>::New();

	// Generate the colors for each point based on the color map
	vtkSmartPointer<vtkUnsignedCharArray> colors =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	int tableSize = polydata->GetNumberOfCells();

	std::cout << "There are " << tableSize << " cells." << std::endl;

	//MakeLUTFromCTF12(tableSize, scaleRange, arr_num, polydata, lookupTable);
	MakeLUTFromCTF(tableSize,   lookupTable);
	vtkSmartPointer<vtkUnsignedCharArray> colorData =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colorData->SetName("colors");
	colorData->SetNumberOfComponents(3);
	std::cout << "Using a lookup table created from a color transfer function."
		<< std::endl;
	//polydata->Print(std::cout);

	//MakeCellData(tableSize, scaleRange, arr_num, polydata, lookupTable, colorData);
	MakeCellData(tableSize,  lookupTable, colorData);
	
	//polydata->GetPointData()->SetScalars(colors);
	polydata->GetCellData()->SetScalars(colorData);
	//polydata->Print(std::cout);

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	mapper->SetInputData(polydata);
	mapper->SetScalarRange(0, tableSize - 1);
	mapper->SetLookupTable(lookupTable);
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
