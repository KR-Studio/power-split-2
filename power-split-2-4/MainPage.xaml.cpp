//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <string>

using namespace PowerSplit;

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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409


std::wstring i2ws(const int& dataInt)
{
	std::wstring dataWstr = std::to_wstring(dataInt);
	return dataWstr;
}


String^ i2ps(const int& dataInt)
{
	String^ dataPstr = ref new String(i2ws(dataInt).c_str());
	return dataPstr;
}


int ps2i(String^& dataPstr)
{
	std::wstring dataPstrWstr(dataPstr->Data());
	auto dataInt = std::stoi(dataPstrWstr);
	return dataInt;
}


MainPage::MainPage()
{
	InitializeComponent();
}


// Computing output block
TextBlock^ ComputingOutput;
// Info output block
TextBlock^ InfoOutput;

// Matrix A - TextBoxes
std::vector<std::vector<TextBox^>> matrix1Box;
// Matrix B - TextBoxes
std::vector<std::vector<TextBox^>> matrix2Box;


// Matrix A
std::vector<std::vector<int>> matrix1;
// Matrix B
std::vector<std::vector<int>> matrix2;


// Matrix/Vector Max Size
const int maxSize = 5;
// Matrix/Vector current Size
int currentSize = maxSize;


// Picked variant
int variantIndex = 0;


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
}


void PowerSplit::MainPage::PageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
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

	FillInputWithZeros();
}


// Picked method index
int methodIndex = 0;


void PowerSplit::MainPage::MethodsRadioChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (ssaRadioButton->IsChecked->Value == true) {
		methodIndex = 0;
	}
	else if (lrpRadioButton->IsChecked->Value == true) {
		methodIndex = 1;
	}
}


void PowerSplit::MainPage::SizeButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	String^ sizeStr = matrixSize->Text;
	int size = ps2i(sizeStr);
	if (size <= maxSize) {
		currentSize = size;
		FillInputWithZeros();
		//PrintInput();
	}
	else {
		ComputingOutput->Text += "Max Matrix/Vector size is 5";
		ComputingOutput->Text += "\r\n";
	}
}


// Matrix/Vector Max Random Range
const int maxRandomRange = 1000;


void RandomizeInput(int range = maxRandomRange) {

	srand(time(0));

	int i = 0;
	int starter_i = 0;
	int border_i = 0;
	int j = 0;
	std::vector<int> starter_j;
	std::vector<int> border_j;

	switch (variantIndex)
	{
	case 0: {
		starter_i = 0;
		border_i = currentSize;
		starter_j = { 0, 1, 2, 3, 4 };
		border_j = { 0, 1, 2, 3, 4 };
		break;
	}
	case 1: {
		starter_i = 0;
		border_i = currentSize;
		starter_j = { 0, 0, 0, 0, 0 };
		border_j = { 4, 4, 4, 4, 4 };
		break;
	}
	default: {
		break;
	}
	}

	i = starter_i;
	for (std::vector<std::vector<int>>::iterator matrixRowItt = matrix1.begin(); matrixRowItt != matrix1.end(); ++matrixRowItt) {
		std::vector<int>& matrixRow = *matrixRowItt;
		if (i < border_i)
		{
			j = 0;
			for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
				if (j >= starter_j[i] && j <= border_j[i] && j < currentSize)
				{
					switch (variantIndex)
					{
					case 0: {
						int n = 0;

						if (i == 0 || i == (currentSize - 1)) {
							n = currentSize;
						}
						else
						{
							n = currentSize - 1;
						}
						
						*matrixIndexItt = n;
						break;
					}
					case 1: {
						int n = 0;


						if (i - j == 0) {
							n = maxSize - 4;
						}
						else if (i - j == 1 || i - j == -1) {
							n = maxSize - 3;
						}
						else if (i - j == 2 || i - j == -2) {
							n = maxSize - 2;
						}
						else if (i - j == 3 || i - j == -3) {
							n = maxSize - 1;
						}
						else if (i - j == 4 || i - j == -4) {
							n = maxSize - 0;
						}

						*matrixIndexItt = n;
						break;
					}
					default:
						*matrixIndexItt += 0;
						break;
					}
				}
				else
				{
					*matrixIndexItt = 0;
				}
				j++;
			}
		}
		else
		{
			for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
				*matrixIndexItt = 0;
			}
		}
		i++;
	}

	i = 0;
	j = 0;
	starter_i = 0;
	border_i = 0;
	starter_j.clear();
	border_j.clear();

	switch (variantIndex)
	{
	case 0: {
		starter_i = 0;
		border_i = currentSize;
		starter_j = { 0, 1, 2, 3, 4 };
		border_j = { 4, 4, 4, 4, 4 };
		break;
	}
	case 1: {
		starter_i = 0;
		border_i = 3;
		starter_j = { 0, 1, 2 };
		border_j = { 4, 3, 2 };
		break;
	}
	default: {
		break;
	}
	}

	i = starter_i;
	for (std::vector<std::vector<int>>::iterator matrixRowItt = matrix2.begin(); matrixRowItt != matrix2.end(); ++matrixRowItt) {
		std::vector<int>& matrixRow = *matrixRowItt;
		if (i < border_i) 
		{
			j = 0;
			for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
				if (j >= starter_j[i] && j <= border_j[i] && j < currentSize) 
				{
					*matrixIndexItt = rand() % range;
				}
				else
				{
					*matrixIndexItt = 0;
				}
				j++;
			}
		}
		else
		{
			for (std::vector<int>::iterator matrixIndexItt = matrixRow.begin(); matrixIndexItt != matrixRow.end(); ++matrixIndexItt) {
				*matrixIndexItt = 0;
			}
		}
		i++;
	}

	starter_j.clear();
	border_j.clear();
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
}


// Calculation steps
int calculationSteps = 0;


std::vector<std::vector<int>> MatrixMultMatrix(std::vector<std::vector<int>> matrix1, std::vector<std::vector<int>> matrix2)
{
	calculationSteps = 0;

	std::vector<std::vector<int>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<int> newMatrixRow;

		std::vector<int> newMatrixIndexRow;
		int newMatrixIndex = 0;
		for (int j = 0; j < currentSize; j++)
		{
			newMatrixIndex = 0;
			newMatrixIndexRow.clear();
			for (int r = 0; r < currentSize; r++)
			{
				newMatrixIndex += matrix1[i][r] * matrix2[r][j];
				calculationSteps++;
			}

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		for (int r = 0; r < currentSize; r++)
		{
			newMatrixIndexRow.push_back(newMatrixIndex);
			calculationSteps++;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


std::vector<std::vector<int>> MatrixMultMatrixRec(std::vector<std::vector<int>> matrix1, std::vector<std::vector<int>> matrix2)
{
	calculationSteps = 0;

	std::vector<std::vector<int>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<int> newMatrixRow;

		for (int j = 0; j < currentSize; j++)
		{
			int newMatrixIndex = 0;
			std::vector<int> newMatrixIndexRow;

			for (int r = 0; r < currentSize; r++)
			{
				newMatrixIndex += matrix1[i][r] * matrix2[r][j];
				calculationSteps++;
			}

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


// Matrix AB
std::vector<std::vector<int>> matrix_ans;


void CalculateResults()
{
	switch (methodIndex)
	{
	case 0: {
		matrix_ans = MatrixMultMatrix(matrix1, matrix2);
		break;
	}
	case 1: {
		matrix_ans = MatrixMultMatrixRec(matrix1, matrix2);
		break;
	}
	default:
		break;
	}
}


void PrintResults()
{
	ComputingOutput->Text += "Matrix AB:\r\n";
	for each (std::vector<int> matrixRow in matrix_ans)
	{
		for each (int matrixColumn in matrixRow)
		{
			ComputingOutput->Text += i2ps(round(matrixColumn)) + " ";
		}
		ComputingOutput->Text += "\r\n";
	}
	ComputingOutput->Text += "\r\n";

	if (variantIndex == 1)
	{
		calculationSteps -= 15;
	}
	ComputingOutput->Text += "Calculation steps: " + i2ps(calculationSteps);
	ComputingOutput->Text += "\r\n";
}


void PowerSplit::MainPage::CalculateButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//ComputingOutput->Text += i2ps(variantIndex) + "\r\n";
	CalculateResults();
	PrintResults();
}


void PowerSplit::MainPage::RandomButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	variantIndex = variantsList->SelectedIndex;

	String^ rangeStr = randomRange->Text;
	int range = ps2i(rangeStr);
	if (range <= maxRandomRange) {
		RandomizeInput(range);
		FillInput();
		//PrintInput();
	}
	else {
		ComputingOutput->Text += "Max random range size is 1000";
		ComputingOutput->Text += "\r\n";
	}
}
