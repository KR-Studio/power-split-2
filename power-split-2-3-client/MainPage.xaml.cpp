//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <string>
#include <sstream>
#include <ctime>

#include <power-split-net/Including.h>

using namespace PowerSplitClient;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace PowerSplitNet;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}

std::wstring s2ws(const std::string& outputDataStr)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &outputDataStr[0], (int)outputDataStr.size(), NULL, 0);
	std::wstring dataWstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &outputDataStr[0], (int)outputDataStr.size(), &dataWstr[0], size_needed);
	return dataWstr;
}


String^ s2ps(const std::string& outputDataStr)
{
	std::wstring dataWstr = s2ws(outputDataStr);
	String^ dataPstr = ref new String(dataWstr.c_str());
	return dataPstr;
}


std::string ws2s(const std::wstring& dataWstr)
{
	auto dataWide = dataWstr.c_str();
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, dataWide, -1, nullptr, 0, NULL, NULL);
	auto dataUtf8 = std::make_unique<char[]>(bufferSize);
	if (0 == WideCharToMultiByte(CP_UTF8, 0, dataWide, -1, dataUtf8.get(), bufferSize, NULL, NULL))
		throw std::exception("Can't convert string to UTF8");
	return std::string(dataUtf8.get());
}


std::string ps2s(String^& dataPstr)
{
	std::wstring dataPstrWstr(dataPstr->Data());
	auto outputDataStr = ws2s(dataPstrWstr);
	return outputDataStr;
}


int ps2i(String^& dataPstr)
{
	std::wstring dataPstrWstr(dataPstr->Data());
	auto dataInt = std::stoi(dataPstrWstr);
	return dataInt;
}


double ps2d(String^& dataPstr)
{
	std::wstring dataPstrWstr(dataPstr->Data());
	auto dataDouble = std::stod(dataPstrWstr);
	return dataDouble;
}


std::wstring i2ws(const int& dataInt)
{
	std::wstring dataWstr = std::to_wstring(dataInt);
	return dataWstr;
}


std::wstring f2ws(const double& dataFloat)
{
	std::wstring dataWstr = std::to_wstring(dataFloat);
	return dataWstr;
}


String^ i2ps(const int& dataInt)
{
	String^ dataPstr = ref new String(i2ws(dataInt).c_str());
	return dataPstr;
}


String^ f2ps(const double& dataFloat)
{
	String^ dataPstr = ref new String(f2ws(dataFloat).c_str());
	return dataPstr;
}


// Main client Socket
NetSocket socketListener;
// Check connection with server
NetResult ifConnected = NetResult::Net_NotYetImplemented;
// Check number of messages
int numberOfMessage = 0;


// Computing output block
TextBlock^ ComputingOutput;
// Info output block
TextBlock^ InfoOutput;


// Matrix/Vector Max Size
const int maxSize = 5;
// Matrix/Vector current Size
int currentSize = maxSize;
// Matrix/Vector Max Random Range
const int maxRandomRange = 10000;


// Matrix A - TextBoxes
std::vector<std::vector<TextBox^>> matrix1Box;
// Matrix A1 - TextBoxes
std::vector<std::vector<TextBox^>> matrix2Box;
// Vector b1 - TextBoxes
std::vector<TextBox^> vector1Box;
// Vector c1 - TextBoxes
std::vector<TextBox^> vector2Box;
// Matrix A2 - TextBoxes
std::vector<std::vector<TextBox^>> matrix3Box;
// Matrix B2 - TextBoxes
std::vector<std::vector<TextBox^>> matrix4Box;


// Matrix A
std::vector<std::vector<int>> matrix1;
// Matrix A1
std::vector<std::vector<int>> matrix2;
// Vector b1
std::vector<int> vector1;
// Vector c1
std::vector<int> vector2;
// Matrix A2
std::vector<std::vector<int>> matrix3;
// Matrix B2
std::vector<std::vector<int>> matrix4;


// Vector y1
std::vector<double> vector_ans_1;
// Vector y2
std::vector<double> vector_ans_2;
// Matrix Y3
std::vector<std::vector<double>> matrix_ans_1;
// Vector X
std::vector<double> vector_ans_x;


std::vector<double> MatrixMultVector(std::vector<std::vector<double>> matrix, std::vector<double> vector)
{
	std::vector<double> newVector;

	int i = 0;
	int j = 0;
	double newVectorIndex = 0;
	for each (std::vector<double> matrixRow in matrix)
	{
		if (i < currentSize)
		{
			newVectorIndex = 0;

			j = 0;
			for each (double matrixIndex in matrixRow)
			{
				if (j < currentSize)
				{
					InfoOutput->Text += "i" + i2ps(i) + "=" + f2ps(matrixIndex * vector[i]) + " ";
					newVectorIndex += matrixIndex * vector[i];
					j++;
				}
			}
			InfoOutput->Text += "\r\n";
			newVector.push_back(newVectorIndex);
			i++;
		}
	}

	return newVector;
}


std::vector<std::vector<double>> MatrixMultMatrix(std::vector<std::vector<double>> matrix1, std::vector<std::vector<double>> matrix2)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			for (int r = 0; r < currentSize; r++)
			{
				newMatrixIndex += matrix1[i][r] * matrix2[r][j];
			}

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}

std::vector<std::vector<double>> MatrixMultMatrix(std::vector<std::vector<int>> matrix1, std::vector<std::vector<double>> matrix2)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			for (int r = 0; r < currentSize; r++)
			{
				newMatrixIndex += matrix1[i][r] * matrix2[r][j];
			}

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


std::vector<std::vector<double>> MatrixAddMatrix(std::vector<std::vector<double>> matrix1, std::vector<std::vector<double>> matrix2)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			newMatrixIndex += matrix1[i][j] + matrix2[i][j];

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}

std::vector<std::vector<double>> MatrixAddMatrix(std::vector<std::vector<int>> matrix1, std::vector<std::vector<double>> matrix2)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			newMatrixIndex += matrix1[i][j] + matrix2[i][j];

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


std::vector<double> VectorMultVector(std::vector<double> vector1, std::vector<double> vector2)
{
	std::vector<double> newVector;

	for (int i = 0; i < currentSize; i++)
	{
		double newVectorIndex = 0;

		newVectorIndex += vector1[i] * vector2[i];

		newVector.push_back(newVectorIndex);
		newVectorIndex = 0;
	}

	return newVector;
}


std::vector<double> VectorAddVector(std::vector<double> vector1, std::vector<double> vector2)
{
	std::vector<double> newVector;

	for (int i = 0; i < currentSize; i++)
	{
		double newVectorIndex = 0;

		newVectorIndex += vector1[i] + vector2[i];

		newVector.push_back(newVectorIndex);
		newVectorIndex = 0;
	}

	return newVector;
}


std::vector<double> VectorSubtrVector(std::vector<double> vector1, std::vector<double> vector2)
{
	std::vector<double> newVector;

	for (int i = 0; i < currentSize; i++)
	{
		double newVectorIndex = 0;

		newVectorIndex += vector1[i] - vector2[i];

		newVector.push_back(newVectorIndex);
		newVectorIndex = 0;
	}

	return newVector;
}


std::vector<std::vector<double>> MatrixSubtrMatrix(std::vector<std::vector<double>> matrix1, std::vector<std::vector<double>> matrix2)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			newMatrixIndex += matrix1[i][j] - matrix2[i][j];

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}

std::vector<std::vector<double>> MatrixSubtrMatrix(std::vector<std::vector<int>> matrix1, std::vector<std::vector<double>> matrix2)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			newMatrixIndex += matrix1[i][j] - matrix2[i][j];

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


std::vector<std::vector<double>> MatrixMultNumber(std::vector<std::vector<double>> matrix, double number)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			newMatrixIndex += matrix[i][j] * number;

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}

std::vector<std::vector<double>> MatrixMultNumber(std::vector<std::vector<int>> matrix, double number)
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			double newMatrixIndex = 0;

			newMatrixIndex += matrix[i][j] * number;

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


std::vector<double> VectorMultNumber(std::vector<double> vector, double number)
{
	std::vector<double> newVector;

	for (int i = 0; i < currentSize; i++)
	{
		double newVectorIndex = 0;

		newVectorIndex += vector[i] * number;

		newVector.push_back(newVectorIndex);
		newVectorIndex = 0;
	}

	return newVector;
}


std::vector<double> VectorMultNumber(std::vector<int> vector, double number)
{
	std::vector<double> newVector;

	for (int i = 0; i < currentSize; i++)
	{
		double newVectorIndex = 0;

		newVectorIndex += vector[i] * number;

		newVector.push_back(newVectorIndex);
		newVectorIndex = 0;
	}

	return newVector;
}


std::vector<double> VectorInv(std::vector<double> vector)
{
	std::vector<double> newVector;

	for (int i = currentSize - 1; i >= 0; i--)
	{
		double newVectorIndex = 0;

		newVectorIndex += vector[i];

		newVector.push_back(newVectorIndex);
		newVectorIndex = 0;
	}

	return newVector;
}


// Define type for f(x)
typedef int(*pointFunction)(int);


double y1i(std::vector<int> A, double bi) {
	double y1i = 0;
	for each (int Ai in A)
	{
		y1i += Ai * bi;
	}
	return y1i;
}

double y2i(std::vector<int> A1, double bci) {
	double y2i = 0;
	for each (int Ai in A1)
	{
		y2i += Ai * bci;
	}
	return y2i;
}


// variant 1 (13, 25)
double biV1(int i) {
	if (i % 2 == 0)
		return (1 / (pow(i, 2) + 2));
	else
		return (1 / (double)i);
}

int bciV1(int b1, int c1) {
	return b1 + c1;
}

std::vector<std::vector<double>> C2V1()
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;
		double newMatrixIndex = 0;

		for (int j = 0; j < currentSize; j++)
		{
			newMatrixIndex += (1 / (double)(i + 1) + 2 * (double)(j + 1));
			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


std::vector<std::vector<double>> BCV1()
{
	return (MatrixSubtrMatrix(matrix4, C2V1()));
}

std::vector<double> xV1()
{
	double K1 = 1;
	double K2 = 0;

	std::vector<double> newVector;

	newVector = VectorAddVector(MatrixMultVector(MatrixMultMatrix(matrix_ans_1, matrix_ans_1), vector_ans_2), MatrixMultVector(matrix_ans_1, VectorAddVector(vector_ans_1, VectorMultNumber(vector_ans_2, K1))));
	
	return newVector;
}


// variant 6 (18)
double biV6(int i) {
	return 6 / pow(i, 2);
}

int bciV6(int b1, int c1) {
	return 6 * b1 - c1;
}

std::vector<std::vector<double>> C2V6()
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;
		double newMatrixIndex = 0;

		for (int j = 0; j < currentSize; j++)
		{
			newMatrixIndex += (1 / pow(((double)(i + 1) + (double)(j + 1)), 3));
			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}

std::vector<std::vector<double>> BCV6()
{
	return (MatrixSubtrMatrix(MatrixMultNumber(matrix4, 10), C2V6()));
}

std::vector<double> xV6()
{
	double K1 = 0.0000025;
	double K2 = 0.0000005;

	std::vector<double> newVector;

	newVector = VectorAddVector(VectorMultVector(VectorMultNumber(MatrixMultVector(MatrixMultMatrix(matrix_ans_1, matrix_ans_1), vector_ans_1), K2), VectorInv(vector_ans_2)), VectorAddVector(VectorMultVector(VectorMultNumber(MatrixMultVector(MatrixMultMatrix(matrix_ans_1, matrix_ans_1), VectorInv(vector_ans_1)), K2), vector_ans_2), VectorMultVector(VectorMultNumber(MatrixMultVector(MatrixMultMatrix(matrix_ans_1, matrix_ans_1), vector_ans_1), K2), VectorInv(vector_ans_2))));

	return newVector;
}


// variant 8
double biV8(int i) {
	return (8 / (double)i);
}

int bciV8(int b1, int c1) {
	return 2 * b1 + 3 * c1;
}

std::vector<std::vector<double>> C2V8()
{
	std::vector<std::vector<double>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<double> newMatrixRow;
		double newMatrixIndex = 0;

		for (int j = 0; j < currentSize; j++)
		{
			newMatrixIndex += (1 / ((double)(i + 1) + (double)(j + 1) + 2));
			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}

std::vector<std::vector<double>> BCV8()
{
	return (MatrixSubtrMatrix(matrix4, C2V8()));
}

std::vector<double> xV8()
{
	double K1 = 0.0000001;
	double K2 = 0.000002;

	std::vector<double> newVector;

	newVector = VectorMultVector(VectorMultNumber(VectorAddVector(VectorMultVector(VectorInv(vector_ans_2), VectorMultVector(vector_ans_2, VectorMultVector(vector_ans_1, MatrixMultVector(matrix_ans_1, VectorMultNumber(VectorInv(vector_ans_1), K1))))), VectorMultVector(vector_ans_1, VectorInv(vector_ans_2))), K1), MatrixMultVector(matrix_ans_1, VectorMultVector(vector_ans_1, VectorMultVector(vector_ans_2, VectorInv(vector_ans_2)))));

	return newVector;
}


void PrintInput() {

	ComputingOutput->Text += "Matrix A:\r\n";
	for each (std::vector<int> matrixRow in matrix1)
	{
		for each (int matrixColumn in matrixRow)
		{
			ComputingOutput->Text += i2ps(matrixColumn) + " ";
		}

		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Matrix A1:\r\n";
	for each (std::vector<int> matrixRow in matrix2)
	{
		for each (int matrixColumn in matrixRow)
		{
			ComputingOutput->Text += i2ps(matrixColumn) + " ";
		}

		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Vector b1:\r\n";
	for each (int vectorRow in vector1)
	{
		ComputingOutput->Text += i2ps(vectorRow);
		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Vector c1:\r\n";
	for each (int vectorRow in vector2)
	{
		ComputingOutput->Text += i2ps(vectorRow);
		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Matrix A2:\r\n";
	for each (std::vector<int> matrixRow in matrix3)
	{
		for each (int matrixColumn in matrixRow)
		{
			ComputingOutput->Text += i2ps(matrixColumn) + " ";
		}

		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Matrix B2:\r\n";
	for each (std::vector<int> matrixRow in matrix4)
	{
		for each (int matrixColumn in matrixRow)
		{
			ComputingOutput->Text += i2ps(matrixColumn) + " ";
		}

		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n\r\n";
}


void FillInputWithZeros(int size = currentSize) {

	matrix1.clear();
	int i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix1Box)
	{
		if (i < size) {
			std::vector<int> newRow;
			int j = 0;

			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					newRow.push_back(0);
					matrixIndex->IsEnabled = true;
					matrixIndex->Text = "0";

					j++;
				}
				else {
					matrixIndex->IsEnabled = false;
				}
			}

			matrix1.push_back(newRow);
			newRow.clear();

			i++;
		}
		else {
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				matrixIndex->IsEnabled = false;
			}
		}
	}

	matrix2.clear();
	i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix2Box)
	{
		if (i < size) {
			std::vector<int> newRow;
			int j = 0;

			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					newRow.push_back(0);
					matrixIndex->IsEnabled = true;
					matrixIndex->Text = "0";

					j++;
				}
				else {
					matrixIndex->IsEnabled = false;
				}
			}

			matrix2.push_back(newRow);
			newRow.clear();

			i++;
		}
		else {
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				matrixIndex->IsEnabled = false;
			}
		}
	}

	vector1.clear();
	i = 0;
	for each (TextBox ^ vectorRow in vector1Box)
	{
		if (i < size) {
			vectorRow->IsEnabled = true;
			vectorRow->Text = "0";
			vector1.push_back(0);

			i++;
		}
		else {
			vectorRow->IsEnabled = false;
		}
	}

	vector2.clear();
	i = 0;
	for each (TextBox ^ vectorRow in vector2Box)
	{
		if (i < size) {
			vectorRow->IsEnabled = true;
			vectorRow->Text = "0";
			vector2.push_back(0);

			i++;
		}
		else {
			vectorRow->IsEnabled = false;
		}
	}

	matrix3.clear();
	i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix3Box)
	{
		if (i < size) {
			std::vector<int> newRow;
			int j = 0;

			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					newRow.push_back(0);
					matrixIndex->IsEnabled = true;
					matrixIndex->Text = "0";

					j++;
				}
				else {
					matrixIndex->IsEnabled = false;
				}
			}

			matrix3.push_back(newRow);
			newRow.clear();

			i++;
		}
		else {
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				matrixIndex->IsEnabled = false;
			}
		}
	}

	matrix4.clear();
	i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix4Box)
	{
		if (i < size) {
			std::vector<int> newRow;
			int j = 0;

			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					newRow.push_back(0);
					matrixIndex->IsEnabled = true;
					matrixIndex->Text = "0";

					j++;
				}
				else {
					matrixIndex->IsEnabled = false;
				}
			}

			matrix4.push_back(newRow);
			newRow.clear();

			i++;
		}
		else {
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				matrixIndex->IsEnabled = false;
			}
		}
	}
}


void RandomizeInput(int range = maxRandomRange) {

	srand(time(0));

	for (std::vector<std::vector<int>>::iterator matrixRowItt = matrix1.begin(); matrixRowItt != matrix1.end(); ++matrixRowItt) {
		std::vector<int> &matrixRow = *matrixRowItt;
		for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
			*matrixIndexItt = rand() % range;
		}
	}

	for (std::vector<std::vector<int>>::iterator matrixRowItt = matrix2.begin(); matrixRowItt != matrix2.end(); ++matrixRowItt) {
		std::vector<int>& matrixRow = *matrixRowItt;
		for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
			*matrixIndexItt = rand() % range;
		}
	}

	for (std::vector<int>::iterator matrixRowItt = vector1.begin(); matrixRowItt != vector1.end(); ++matrixRowItt) {
		*matrixRowItt = rand() % range;
	}

	for (std::vector<int>::iterator matrixRowItt = vector2.begin(); matrixRowItt != vector2.end(); ++matrixRowItt) {
		*matrixRowItt = rand() % range;
	}

	for (std::vector<std::vector<int>>::iterator matrixRowItt = matrix3.begin(); matrixRowItt != matrix3.end(); ++matrixRowItt) {
		std::vector<int>& matrixRow = *matrixRowItt;
		for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
			*matrixIndexItt = rand() % range;
		}
	}

	for (std::vector<std::vector<int>>::iterator matrixRowItt = matrix4.begin(); matrixRowItt != matrix4.end(); ++matrixRowItt) {
		std::vector<int>& matrixRow = *matrixRowItt;
		for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
			*matrixIndexItt = rand() % range;
		}
	}
}


void FillInput(int size = currentSize) {

	int i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix1Box)
	{
		if (i < size) {
			int j = 0;
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					matrixIndex->Text = i2ps(matrix1[i][j]);
					j++;
				}
			}
			i++;
		}
	}

	i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix2Box)
	{
		if (i < size) {
			int j = 0;
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					matrixIndex->Text = i2ps(matrix2[i][j]);
					j++;
				}
			}
			i++;
		}
	}

	i = 0;
	for each (TextBox ^ vectorRow in vector1Box)
	{
		if (i < size) {
			vectorRow->Text = i2ps(vector1[i]);
			i++;
		}
	}

	i = 0;
	for each (TextBox ^ vectorRow in vector2Box)
	{
		if (i < size) {
			vectorRow->Text = i2ps(vector2[i]);
			i++;
		}
	}

	i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix3Box)
	{
		if (i < size) {
			int j = 0;
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					matrixIndex->Text = i2ps(matrix3[i][j]);;
					j++;
				}
			}
			i++;
		}
	}

	i = 0;
	for each (std::vector<TextBox^> matrixRow in matrix4Box)
	{
		if (i < size) {
			int j = 0;
			for each (TextBox ^ matrixIndex in matrixRow)
			{
				if (j < size) {
					matrixIndex->Text = i2ps(matrix4[i][j]);
					j++;
				}
			}
			i++;
		}
	}
}


void CalculateResults(int variantIndex = 0)
{
	// Calculate y1
	// y1 = A * b
	vector_ans_1.clear();
	int i = 0;
	for each (std::vector<int> matrixRow in matrix1)
	{
		if (i < currentSize) {
			switch (variantIndex)
			{
			case 0: {
				vector_ans_1.push_back(y1i(matrixRow, biV1(i + 1)));
				break;
			}
			case 1: {
				vector_ans_1.push_back(y1i(matrixRow, biV6(i + 1)));
				break;
			}
			case 2: {
				vector_ans_1.push_back(y1i(matrixRow, biV8(i + 1)));
				break;
			}
			default:
				vector_ans_1.push_back(y1i(matrixRow, biV8(i + 1)));
				break;
			}
			i++;
		}
	}

	// Calculatey2
	// y2 = A1 * bc
	vector_ans_2.clear();
	i = 0;
	for each (std::vector<int> matrixRow in matrix2)
	{
		if (i < currentSize) {
			switch (variantIndex)
			{
			case 0: {
				vector_ans_2.push_back(y2i(matrixRow, bciV1(vector1[i], vector2[i])));
				break;
			}
			case 1: {
				vector_ans_2.push_back(y2i(matrixRow, bciV6(vector1[i], vector2[i])));
				break;
			}
			case 2: {
				vector_ans_2.push_back(y2i(matrixRow, bciV8(vector1[i], vector2[i])));
				break;
			}
			default:
				vector_ans_2.push_back(y2i(matrixRow, bciV1(vector1[i], vector2[i])));
				break;
			}
			i++;
		}
	}

	// Calculate Y3
	// Y3 = A3 * BC
	switch (variantIndex)
	{
	case 0: {
		matrix_ans_1 = MatrixMultMatrix(matrix3, BCV1());
		break;
	}
	case 1: {
		matrix_ans_1 = MatrixMultMatrix(matrix3, BCV6());
		break;
	}
	case 2: {
		matrix_ans_1 = MatrixMultMatrix(matrix3, BCV8());
		break;
	}
	default:
		matrix_ans_1 = MatrixMultMatrix(matrix3, BCV1());
		break;
	}

	// Calculate X
	// X = ...
	switch (variantIndex)
	{
	case 0: {
		vector_ans_x = xV1();
		break;
	}
	case 1: {
		vector_ans_x = xV6();
		break;
	}
	case 2: {
		vector_ans_x = xV8();
		break;
	}
	default:
		vector_ans_x = xV1();
		break;
	}
}


void PrintResults()
{
	ComputingOutput->Text += "Vector y1:\r\n";
	for each (double vectorRow in vector_ans_1)
	{
		ComputingOutput->Text += i2ps(round(vectorRow));
		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Vector y2:\r\n";
	for each (double vectorRow in vector_ans_2)
	{
		ComputingOutput->Text += i2ps(round(vectorRow));
		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Matrix Y3:\r\n";
	for each (std::vector<double> matrixRow in matrix_ans_1)
	{
		for each (int matrixColumn in matrixRow)
		{
			ComputingOutput->Text += i2ps(round(matrixColumn)) + " ";
		}

		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "Vector X:\r\n";
	for each (double vectorRow in vector_ans_x)
	{
		ComputingOutput->Text += i2ps(round(vectorRow));
		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	ComputingOutput->Text += "\r\n";
}


void PowerSplitClient::MainPage::PageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Clearing server output area
	InfoOutput = textBlockInfoOutput;
	InfoOutput->Text = "";
	// Clearing computing output area
	ComputingOutput = textBlockComputingOutput;
	ComputingOutput->Text = "";

	matrix1Box = {
		{ matrix1Row1Column1, matrix1Row1Column2, matrix1Row1Column3, matrix1Row1Column4, matrix1Row1Column5 },
		{ matrix1Row2Column1, matrix1Row2Column2, matrix1Row2Column3, matrix1Row2Column4, matrix1Row2Column5 },
		{ matrix1Row3Column1, matrix1Row3Column2, matrix1Row3Column3, matrix1Row3Column4, matrix1Row3Column5 },
		{ matrix1Row4Column1, matrix1Row4Column2, matrix1Row4Column3, matrix1Row4Column4, matrix1Row4Column5 },
		{ matrix1Row5Column1, matrix1Row5Column2, matrix1Row5Column3, matrix1Row5Column4, matrix1Row5Column5 }
	};

	matrix2Box = {
		{ matrix2Row1Column1, matrix2Row1Column2, matrix2Row1Column3, matrix2Row1Column4, matrix2Row1Column5 },
		{ matrix2Row2Column1, matrix2Row2Column2, matrix2Row2Column3, matrix2Row2Column4, matrix2Row2Column5 },
		{ matrix2Row3Column1, matrix2Row3Column2, matrix2Row3Column3, matrix2Row3Column4, matrix2Row3Column5 },
		{ matrix2Row4Column1, matrix2Row4Column2, matrix2Row4Column3, matrix2Row4Column4, matrix2Row4Column5 },
		{ matrix2Row5Column1, matrix2Row5Column2, matrix2Row5Column3, matrix2Row5Column4, matrix2Row5Column5 }
	};

	vector1Box = {
		{ vector1Row1, vector1Row2, vector1Row3, vector1Row4, vector1Row5 }
	};

	vector2Box = {
		{ vector2Row1, vector2Row2, vector2Row3, vector2Row4, vector2Row5 }
	};

	matrix3Box = {
		{ matrix3Row1Column1, matrix3Row1Column2, matrix3Row1Column3, matrix3Row1Column4, matrix3Row1Column5 },
		{ matrix3Row2Column1, matrix3Row2Column2, matrix3Row2Column3, matrix3Row2Column4, matrix3Row2Column5 },
		{ matrix3Row3Column1, matrix3Row3Column2, matrix3Row3Column3, matrix3Row3Column4, matrix3Row3Column5 },
		{ matrix3Row4Column1, matrix3Row4Column2, matrix3Row4Column3, matrix3Row4Column4, matrix3Row4Column5 },
		{ matrix3Row5Column1, matrix3Row5Column2, matrix3Row5Column3, matrix3Row5Column4, matrix3Row5Column5 }
	};

	matrix4Box = {
		{ matrix4Row1Column1, matrix4Row1Column2, matrix4Row1Column3, matrix4Row1Column4, matrix4Row1Column5 },
		{ matrix4Row2Column1, matrix4Row2Column2, matrix4Row2Column3, matrix4Row2Column4, matrix4Row2Column5 },
		{ matrix4Row3Column1, matrix4Row3Column2, matrix4Row3Column3, matrix4Row3Column4, matrix4Row3Column5 },
		{ matrix4Row4Column1, matrix4Row4Column2, matrix4Row4Column3, matrix4Row4Column4, matrix4Row4Column5 },
		{ matrix4Row5Column1, matrix4Row5Column2, matrix4Row5Column3, matrix4Row5Column4, matrix4Row5Column5 }
	};

	FillInputWithZeros();
	//PrintInput();
}


void PowerSplitClient::MainPage::SizeButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	String^ sizeStr = matrixSize->Text;
	int size = ps2i(sizeStr);
	if (size <= maxSize) {
		currentSize = size;
		FillInputWithZeros();
		//PrintInput();
	}
	else {
		InfoOutput->Text += "Max Matrix/Vector size is 5";
		InfoOutput->Text += "\r\n";
	}
}


void PowerSplitClient::MainPage::RandomButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	String^ rangeStr = randomRange->Text;
	int range = ps2i(rangeStr);
	if (range <= maxRandomRange) {
		RandomizeInput(range);
		FillInput();
		//PrintInput();
	}
	else {
		InfoOutput->Text += "Max random range size is 10000";
		InfoOutput->Text += "\r\n";
	}
}


void PowerSplitClient::MainPage::ConnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Check active connection with server
	if (ifConnected == NetResult::Net_Success)
	{
		std::string dataStr = "Please first disconnect from server\r\n";
		InfoOutput->Text += s2ps(dataStr);
	}
	else
	{
		if (Network::Initialize()) {
			std::string outputDataStr = "WinSock API successfully initialized\r\n";
			InfoOutput->Text += s2ps(outputDataStr);

			if (socketListener.Create() == NetResult::Net_Success)
			{
				std::string outputDataStr = "Socket successfully created\r\n";
				InfoOutput->Text += s2ps(outputDataStr);

				String^ ipPstr = hostTextBox->Text;
				std::string ipStr = ps2s(ipPstr);
				const char* ipChar = ipStr.c_str();

				String^ portPstr = portTextBox->Text;
				int portInt = ps2i(portPstr);

				if (socketListener.Connect(IPEndpoint(ipChar, portInt)) == NetResult::Net_Success)
				{
					ifConnected = NetResult::Net_Success;

					std::string outputDataStr = "Succesfully connected to server!\r\n";
					InfoOutput->Text += s2ps(outputDataStr);

					// Prepare sending data to check connection
					uint32_t messageNumberToSend, messageDataSizeToSend;
					std::string messageDataToSend;
					messageDataToSend = "Hello, Server!";
					messageNumberToSend = ++numberOfMessage;
					messageDataSizeToSend = messageDataToSend.size();
					NetPacket packetToSend;
					packetToSend << messageNumberToSend << messageDataSizeToSend << messageDataToSend;

					// Send the prepared packet with data
					NetResult resultSent = NetResult::Net_Success;
					while (resultSent == NetResult::Net_Success)
					{
						std::string outputDataStr = "Attempting to send set of data...\r\n";
						InfoOutput->Text += s2ps(outputDataStr);

						resultSent = socketListener.Send(packetToSend);
						if (resultSent != NetResult::Net_Success)
							break;

						outputDataStr = "Data has been sent to server\r\n";
						InfoOutput->Text += s2ps(outputDataStr);
						break;
					}

					// Wait for response data from server
					uint32_t messageNumberReceived, messageDataSizeReceived;
					std::string messageDataReceived;
					NetPacket packetReceived;
					NetResult resultReceived = NetResult::Net_Success;
					while (resultReceived == NetResult::Net_Success)
					{
						resultReceived = socketListener.Receive(packetReceived);
						if (resultReceived != NetResult::Net_Success)
							break;

						std::string outputDataStr = "Data received:\r\n";
						InfoOutput->Text += s2ps(outputDataStr);
						try
						{
							packetReceived >> messageNumberReceived >> messageDataSizeReceived >> messageDataReceived;
						}
						catch (NetPacketException& exception)
						{
							std::string outputDataStr = exception.ToString() + "\r\n";
							InfoOutput->Text += s2ps(outputDataStr);
						}
						outputDataStr = std::to_string(messageNumberReceived) + " " + std::to_string(messageDataSizeReceived) + " " + messageDataReceived + "\r\n";
						InfoOutput->Text += s2ps(outputDataStr);
						break;
					}
					outputDataStr = "Data has just been received from the server\r\n";
					InfoOutput->Text += s2ps(outputDataStr);
				}
				else
				{
					std::string outputDataStr = "Failed to connect to server!\r\n";
					InfoOutput->Text += s2ps(outputDataStr);

					socketListener.Close();

					ifConnected = NetResult::Net_NotYetImplemented;

					outputDataStr = "Socket successfully closed\r\n";
					InfoOutput->Text += s2ps(outputDataStr);
				}
			}
			else
			{
				std::string outputDataStr = "Socket failed to create\r\n";
				InfoOutput->Text += s2ps(outputDataStr);

				socketListener.Close();

				ifConnected = NetResult::Net_NotYetImplemented;

				outputDataStr = "Socket successfully closed\r\n";
				InfoOutput->Text += s2ps(outputDataStr);
			}
		}
		else {
			std::string outputDataStr = "WSAStartup failed with error\r\n";
			InfoOutput->Text += s2ps(outputDataStr);

			Network::Shutdown();

			outputDataStr = "WinSock API successfully closed\r\n";
			InfoOutput->Text += s2ps(outputDataStr);
		}
	}
}


void PowerSplitClient::MainPage::DisconnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Check active connection with server
	if (ifConnected == NetResult::Net_NotYetImplemented)
	{
		std::string outputDataStr = "Please first connect to server\r\n";
		InfoOutput->Text += s2ps(outputDataStr);
	}
	else
	{
		socketListener.Close();

		ifConnected = NetResult::Net_NotYetImplemented;

		std::string outputDataStr = "Socket successfully closed\r\n";
		InfoOutput->Text += s2ps(outputDataStr);

		Network::Shutdown();

		outputDataStr = "WinSock API successfully closed\r\n";
		InfoOutput->Text += s2ps(outputDataStr);
	}
}


void PowerSplitClient::MainPage::SendButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	CalculateResults(variantsList->SelectedIndex);
	PrintResults();

	// Check active connection with server
	if (ifConnected == NetResult::Net_NotYetImplemented)
	{
		std::string dataStr = "Please first connect to server\r\n";
		InfoOutput->Text += s2ps(dataStr);
	}
	else
	{
		// Prepare sending data
		uint32_t messageNumberToSend, messageDataSizeToSend;
		std::string messageDataToSend;

		CalculateResults(variantsList->SelectedIndex);

		messageDataToSend = "Calculate";

		messageNumberToSend = ++numberOfMessage;
		messageDataSizeToSend = messageDataToSend.size();
		NetPacket packetToSend;
		packetToSend << messageNumberToSend << messageDataSizeToSend << messageDataToSend;

		// Send the prepared packet with data
		NetResult resultSent = NetResult::Net_Success;
		while (resultSent == NetResult::Net_Success)
		{
			std::string outputDataStr = "Attempting to send set of data...\r\n";
			InfoOutput->Text += s2ps(outputDataStr);

			resultSent = socketListener.Send(packetToSend);
			if (resultSent != NetResult::Net_Success)
				break;

			outputDataStr = "Data has been sent to server\r\n";
			InfoOutput->Text += s2ps(outputDataStr);
			break;
		}

		// Wait for response data from server
		uint32_t messageNumberReceived, messageDataSizeReceived;
		std::string messageDataReceived;
		NetPacket packetReceived;
		NetResult resultReceived = NetResult::Net_Success;
		while (resultReceived == NetResult::Net_Success)
		{
			resultReceived = socketListener.Receive(packetReceived);
			if (resultReceived != NetResult::Net_Success)
				break;

			std::string outputDataStr = "Data received:\r\n";
			ComputingOutput->Text += s2ps(outputDataStr);
			try
			{
				packetReceived >> messageNumberReceived >> messageDataSizeReceived >> messageDataReceived;
			}
			catch (NetPacketException & exception)
			{
				std::string outputDataStr = exception.ToString() + "\r\n";
				ComputingOutput->Text += s2ps(outputDataStr);
			}

			PrintResults();

			outputDataStr = std::to_string(messageNumberReceived) + " " + std::to_string(messageDataSizeReceived) + " " + messageDataReceived + "\r\n";
			ComputingOutput->Text += s2ps(outputDataStr);
			break;
		}
		std::string outputDataStr = "Data has just been received from the server\r\n";
		InfoOutput->Text += s2ps(outputDataStr);
	}
}
