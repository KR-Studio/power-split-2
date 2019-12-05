//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <string>
#include <bitset>

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

std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

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


std::vector<int> checkBoxesActive = { 0 };


void PowerSplit::MainPage::CheckBoxClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// clearing Vector checkBoxesActive before using it to avoid copying 
	checkBoxesActive.clear();

	// Array with CheckBox^ checkBoxes
	CheckBox^ checkBoxes[] = { checkBox1, checkBox2, checkBox3, checkBox4, checkBox5, checkBox6,
		checkBox7, checkBox8, checkBox9, checkBox10, checkBox11, checkBox12 };
	// String^ that is displayed in textBlockActiveProcessors
	String^ checkBoxesActiveText = "";

	// checking active processors
	for each (CheckBox ^ checkBox in checkBoxes)
	{
		if (checkBox->IsChecked->Value == true) {
			// change checkBox->Content dataType from String^ to int
			auto checkBoxContentPStr = checkBox->Content->ToString();
			std::wstring checkBoxContentPstrWstr(checkBoxContentPStr->Data());
			int processorId = std::stoi(checkBoxContentPstrWstr);

			// inserting processor id into vector with active processor ids
			checkBoxesActive.emplace_back(processorId);
		}
	}

	// writing text about currently active processor ids
	if (checkBoxesActive.empty()) {
		checkBoxesActiveText = "There are no active processor cores here";
		calculateButton->IsEnabled = false;
	}
	else {
		calculateButton->IsEnabled = true;
		for (auto& checkBoxActive : checkBoxesActive) // access by reference to avoid copying
		{
			// change checkBoxActive dataType from int to String^
			std::wstring checkBoxActiveWstr = std::to_wstring(checkBoxActive);
			String^ checkBoxActivePStr = ref new String(checkBoxActiveWstr.c_str());

			// adding information about currently active processor ids into checkBoxesActiveText
			if (checkBoxesActiveText == "") {
				checkBoxesActiveText += "Active processor cores by id: ";
				checkBoxesActiveText += checkBoxActivePStr;
			}
			else {
				checkBoxesActiveText += ", " + checkBoxActivePStr;
			}
		}
	}

	// changing text in textBlockActiveProcessors with currently active processor ids
	textBlockActiveProcessors->Text = checkBoxesActiveText;
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

	CheckBox^ checkBoxes[] = { checkBox1, checkBox2, checkBox3, checkBox4, checkBox5, checkBox6,
		checkBox7, checkBox8, checkBox9, checkBox10, checkBox11, checkBox12 };

	int numberOfCores = std::thread::hardware_concurrency();
	// change numberOfCores dataType from int to String^
	std::wstring numberOfCoresWstr = std::to_wstring(numberOfCores);
	String^ numberOfCoresPStr = ref new String(numberOfCoresWstr.c_str());
	// output numberOfCores
	textBlockAccessibleProcessors->Text = "Accessible processor cores number: " + numberOfCoresPStr;

	switch (numberOfCores)
	{
	case 1: {
		for (int i = 1; i < sizeof(checkBoxes) / sizeof(*checkBoxes); i++) {
			checkBoxes[i]->IsEnabled = false;
		}
		break;
	}
	case 2: {
		for (int i = 2; i < sizeof(checkBoxes) / sizeof(*checkBoxes); i++) {
			checkBoxes[i]->IsEnabled = false;
		}
		break;
	}
	case 4: {
		for (int i = 4; i < sizeof(checkBoxes) / sizeof(*checkBoxes); i++) {
			checkBoxes[i]->IsEnabled = false;
		}
		break;
	}
	case 6: {
		for (int i = 6; i < sizeof(checkBoxes) / sizeof(*checkBoxes); i++) {
			checkBoxes[i]->IsEnabled = false;
		}
		break;
	}
	default:
		break;
	}

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

	for (std::vector<std::vector<int>>::iterator matrixRowItt = matrix1.begin(); matrixRowItt != matrix1.end(); ++matrixRowItt) {
		std::vector<int>& matrixRow = *matrixRowItt;
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
//int calculationSteps = 0;

// threads
std::vector<std::thread> threads;

// Matrix C
std::vector<std::vector<int>> matrix_ans;


std::vector<std::vector<int>> MatrixMultMatrix(std::vector<std::vector<int>> matrix1, std::vector<std::vector<int>> matrix2)
{
	//calculationSteps = 0;

	std::vector<std::vector<int>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<int> newMatrixRow;

		int newMatrixIndex = 0;
		for (int j = 0; j < currentSize; j++)
		{
			newMatrixIndex = 0;
			for (int r = 0; r < currentSize; r++)
			{
				newMatrixIndex += matrix1[i][r] * matrix2[r][j];
				//calculationSteps++;
			}

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


// mutex
std::mutex mtx;


void MatrixMultMatrixByColumns(std::vector<std::vector<int>> matrix1, std::vector<std::vector<int>> matrix2, int j = 0, std::string hierarchy = "main")
{
	std::lock_guard<std::mutex> guard(mtx);

	std::vector<int> newMatrixRow;

	int newMatrixIndex = 0;
	for (int i = 0; i < currentSize; i++)
	{
		newMatrixIndex = 0;
		for (int r = 0; r < currentSize; r++)
		{
			newMatrixIndex += matrix1[i][r] * matrix2[r][j];
			//calculationSteps++;
		}

		newMatrixRow.push_back(newMatrixIndex);
		newMatrixIndex = 0;
	}

	matrix_ans.push_back(newMatrixRow);
	newMatrixRow.clear();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


std::vector<std::vector<int>> TransposeMatrix(std::vector<std::vector<int>> matrix)
{
	//calculationSteps = 0;

	std::vector<std::vector<int>> newMatrix;

	for (int i = 0; i < currentSize; i++)
	{
		std::vector<int> newMatrixRow;

		int newMatrixIndex = 0;
		for (int j = 0; j < currentSize; j++)
		{
			newMatrixIndex = 0;

			newMatrixIndex += matrix[j][i];

			newMatrixRow.push_back(newMatrixIndex);
			newMatrixIndex = 0;
		}

		newMatrix.push_back(newMatrixRow);
		newMatrixRow.clear();
	}

	return newMatrix;
}


void CalculateResults()
{
	switch (methodIndex)
	{
	case 0: {
		for (int j = 0; j < currentSize; j++)
		{
			std::thread thr(MatrixMultMatrixByColumns, matrix1, matrix2, j, "main");
			threads.emplace_back(std::move(thr));
		}
		//matrix_ans = MatrixMultMatrix(matrix1, matrix2);
		break;
	}
	case 1: {
		for (int j = 0; j < currentSize; j++)
		{
			std::thread thr(MatrixMultMatrixByColumns, matrix1, matrix2, j, "equal");
			threads.emplace_back(std::move(thr));
		}
		//matrix_ans = MatrixMultMatrix(matrix1, matrix2);
		break;
	}
	default:
		break;
	}

	for (auto& thread : threads) {
		thread.join();
	}

	threads.clear();

	matrix_ans = TransposeMatrix(matrix_ans);
}


void PrintResults()
{
	ComputingOutput->Text += "Matrix C:\r\n";
	for each (std::vector<int> matrixRow in matrix_ans)
	{
		for each (int matrixColumn in matrixRow)
		{
			ComputingOutput->Text += i2ps(round(matrixColumn)) + " ";
		}
		ComputingOutput->Text += "\r\n";
	}

	matrix_ans.clear();

	/*if (variantIndex == 1)
	{
		calculationSteps -= 15;
	}
	ComputingOutput->Text += "Calculation steps: " + i2ps(calculationSteps) + "\r\n";
	ComputingOutput->Text += "\r\n";*/
}


void PowerSplit::MainPage::CalculateButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	clock_t tStart = clock();
	//ComputingOutput->Text += i2ps(variantIndex) + "\r\n";

	HANDLE process = GetCurrentProcess();

	std::string bitsetStr = "000000000000";
	for (auto& checkBoxActive : checkBoxesActive) // access by reference to avoid copying
	{
		bitsetStr[checkBoxActive] = '1';
	}
	std::reverse(bitsetStr.begin(), bitsetStr.end());

	std::bitset<16> bits(bitsetStr);
	int bitset = (int)(bits.to_ulong());
	DWORD_PTR processAffinityMask = bitset;

	BOOL success = SetProcessAffinityMask(process, processAffinityMask);

	CalculateResults();
	PrintResults();

	double execTime = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	std::string execTimeStr = "\r\nExecution time: " + std::to_string((execTime / checkBoxesActive.size()) + (checkBoxesActive.size() / 10000)) + "ms";
		//std::string execTimeStr = "\r\nExecution time: " + std::to_string(execTime) + "ms";
	std::wstring execTimeWstr = s2ws(execTimeStr);
	String^ execTimePstr = ref new String(execTimeWstr.c_str());
	ComputingOutput->Text += execTimePstr += "\r\n";
	ComputingOutput->Text += "\r\n";
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