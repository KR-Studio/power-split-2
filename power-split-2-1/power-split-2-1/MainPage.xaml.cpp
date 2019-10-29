//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <string>
#include <numeric>
#include <time.h>
#include <bitset>

using namespace PowerSplit2;

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

#define N 4
#define M 5

// Vector with ids of processors that have activeState in checkBoxes
std::vector<int> checkBoxesActive = { 0 };
std::vector<int> equationMultipliers = {};
std::vector<double> averages;

MainPage::MainPage()
{
	InitializeComponent();
}

std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

String^ s2ps(const std::string& dataStr)
{
	std::wstring dataWstr = s2ws(dataStr);
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
	auto dataStr = ws2s(dataPstrWstr);
	return dataStr;
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


String^ i2ps(const int& dataInt)
{
	String^ dataPstr = ref new String(i2ws(dataInt).c_str());
	return dataPstr;
}

void PowerSplit2::MainPage::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
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
}

void buttonStateChange(Windows::UI::Xaml::Controls::Button^ btnName, bool state)
{
	btnName->IsEnabled = state;
}

void thread_average(int vol)
{
	std::vector<int> v(vol);
	std::generate(v.begin(), v.end(), std::rand);
	double average = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
	averages.push_back(average);
}

/*
  Deletes a two dimensional dynamically allocated matrix
  -- rows: The number of rows in the matrix
  -- **matrix: the matrix to be deleted
*/
void deleteMatrix(int rows, double** matrix)
{
	for (int i = 0; i < rows; ++i)
	{
		delete matrix[i];
	}

	delete[] matrix;
}

void PowerSplit2::MainPage::gaussElimination()
{
	int i, j, n, m;
	
	
	n = N;
	m = M;
	double** matrix = new double* [n];
	for (i = 0; i < n; i++)
		matrix[i] = new double[m];

	// Matrix initialization
	int pushedValues = 0;
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++)
		{
			

			matrix[i][j] = equationMultipliers[j + pushedValues];
		}
		pushedValues += 5;
	}

		

	// Initial matrix output
	textBlockOutput->Text = "Initial matrix: \n";
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++) {
			textBlockOutput->Text += matrix[i][j] + " ";
		}
		textBlockOutput->Text += "\n";
	}
	textBlockOutput->Text += "\n";

	// Gaussian elimination, forward elimination, getting upper-triangular matrix
	double  tmp, * xx = new double[m];
	int k;

	for (i = 0; i < n; i++)
	{
		tmp = matrix[i][i];
		for (j = n; j >= i; j--)
			matrix[i][j] /= tmp;
		for (j = i + 1; j < n; j++)
		{
			tmp = matrix[j][i];
			for (k = n; k >= i; k--)
				matrix[j][k] -= tmp * matrix[i][k];
		}
	}
	
	// Reverse elimination, back-substitution
	xx[n - 1] = matrix[n - 1][n];
	for (i = n - 2; i >= 0; i--)
	{
		xx[i] = matrix[i][n];
		for (j = i + 1; j < n; j++) xx[i] -= matrix[i][j] * xx[j];
	}

	// Output results
	for (i = 0; i < n; i++)
	{
		String^ currentXIndexPStr = i2ps(i + 1);
		textBlockOutput->Text += "X" + currentXIndexPStr + " = " + xx[i] + "; \n";
	}
	textBlockOutput->Text += "\n";

	deleteMatrix(n, matrix);
}

class SubMatrix {
	const std::vector<std::vector<double>>* source;
	std::vector<double> replaceColumn;
	const SubMatrix* prev;
	size_t sz;
	int colIndex = -1;

public:
	SubMatrix(const std::vector<std::vector<double>>& src, const std::vector<double>& rc) : source(&src), replaceColumn(rc), prev(nullptr), colIndex(-1) {
		sz = replaceColumn.size();
	}

	SubMatrix(const SubMatrix& p) : source(nullptr), prev(&p), colIndex(-1) {
		sz = p.size() - 1;
	}

	SubMatrix(const SubMatrix& p, int deletedColumnIndex) : source(nullptr), prev(&p), colIndex(deletedColumnIndex) {
		sz = p.size() - 1;
	}

	int columnIndex() const {
		return colIndex;
	}
	void columnIndex(int index) {
		colIndex = index;
	}

	size_t size() const {
		return sz;
	}

	double index(int row, int col) const {
		if (source != nullptr) {
			if (col == colIndex) {
				return replaceColumn[row];
			}
			else {
				return (*source)[row][col];
			}
		}
		else {
			if (col < colIndex) {
				return prev->index(row + 1, col);
			}
			else {
				return prev->index(row + 1, col + 1);
			}
		}
	}

	double det() const {
		if (sz == 1) {
			return index(0, 0);
		}
		if (sz == 2) {
			return index(0, 0) * index(1, 1) - index(0, 1) * index(1, 0);
		}
		SubMatrix m(*this);
		double det = 0.0;
		int sign = 1;
		for (size_t c = 0; c < sz; ++c) {
			m.columnIndex(c);
			double d = m.det();
			det += index(0, c) * d * sign;
			sign = -sign;
		}
		return det;
	}
};

std::vector<double> solve(SubMatrix& matrix) {
	double det = matrix.det();
	if (det == 0.0) {
		throw new std::runtime_error("The determinant is zero.");
	}

	std::vector<double> answer(matrix.size());
	for (int i = 0; i < matrix.size(); ++i) {
		matrix.columnIndex(i);
		answer[i] = matrix.det() / det;
	}
	return answer;
}

std::vector<double> solveCramer(const std::vector<std::vector<double>>& equations) {
	int size = equations.size();
	if (std::any_of(
		equations.cbegin(), equations.cend(),
		[size](const std::vector<double>& a) { return a.size() != size + 1; }
	)) {
		throw new std::runtime_error("Each equation must have the expected size.");
	}

	std::vector<std::vector<double>> matrix(size);
	std::vector<double> column(size);
	for (int r = 0; r < size; ++r) {
		column[r] = equations[r][size];
		matrix[r].resize(size);
		for (int c = 0; c < size; ++c) {
			matrix[r][c] = equations[r][c];
		}
	}

	SubMatrix sm(matrix, column);
	return solve(sm);
}

void PowerSplit2::MainPage::cramersRule() {
	std::vector<std::vector<double>> multipliersCramerRule = {};

	int step = 0;
	for (int i = 0; i < equationMultipliers.size(); i += 5)
	{
		if (step <= 2) {
			std::vector<double> sub(&equationMultipliers[i], &equationMultipliers[i + 5]);
			multipliersCramerRule.emplace_back(sub);
			step++;
		}
		else {
			std::vector<double> sub(&equationMultipliers[i], &equationMultipliers[i + 4]);
			sub.emplace_back(equationMultipliers[i + 4]);
			multipliersCramerRule.emplace_back(sub);
			step++;
		}

	}

	auto solution = solveCramer(multipliersCramerRule);
	textBlockOutput->Text = "\n";
	for (int i = 0; i < solution.size(); i++)
	{
		String^ currentXIndexPStr = i2ps(i + 1);
		textBlockOutput->Text += "X" + currentXIndexPStr + " = " + solution[i] + "; \n";
	}
}

void PowerSplit2::MainPage::CheckBoxClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// clearing Vector checkBoxesActive before using it to avoid copying 
	checkBoxesActive.clear();

	// Array with CheckBox^ checkBoxes
	CheckBox^ checkBoxes[] = { checkBox1, checkBox2, checkBox3, checkBox4, checkBox5, checkBox6,
		checkBox7, checkBox8, checkBox9, checkBox10, checkBox11, checkBox12 };
	// String^ that is displayed in textBlockActiveProcessors
	String^ checkBoxesActiveText = "";

	// checking active processors
	for each (CheckBox^ checkBox in checkBoxes)
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
		buttonStateChange(calculateBtn, false);
	}
	else {
		buttonStateChange(calculateBtn, true);
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


void PowerSplit2::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Start execution time measurement
	clock_t tStart = clock();


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

	// Array with TextBox^ multiplierTextBoxes
	TextBox^ multiplierTextBoxes[] = { textBoxRow1X1Multiplier, textBoxRow1X2Multiplier, textBoxRow1X3Multiplier, textBoxRow1X4Multiplier, textBoxRow1BMultiplier, textBoxRow2X1Multiplier, textBoxRow2X2Multiplier, textBoxRow2X3Multiplier, textBoxRow2X4Multiplier, textBoxRow2BMultiplier, textBoxRow3X1Multiplier, textBoxRow3X2Multiplier, textBoxRow3X3Multiplier, textBoxRow3X4Multiplier, textBoxRow3BMultiplier, textBoxRow4X1Multiplier, textBoxRow4X2Multiplier, textBoxRow4X3Multiplier, textBoxRow4X4Multiplier, textBoxRow4BMultiplier};

	// checking equation multipliers
	for each (TextBox^ multiplierTextBox in multiplierTextBoxes)
	{
		// change multiplierTextBox->Text from String^ to int
		auto multiplierTextBoxContentPStr = multiplierTextBox->Text->ToString();
		std::wstring multiplierTextBoxContentPstrWstr(multiplierTextBoxContentPStr->Data());
		int multiplier = std::stoi(multiplierTextBoxContentPstrWstr);

		// inserting multiplier values into equation multipliers vector
		equationMultipliers.emplace_back(multiplier);
	}

	

	//PowerSplit2::MainPage::gaussElimination();
	PowerSplit2::MainPage::cramersRule();
    
 

	//auto elementsNumPstr = textBoxArray->Text->ToString();
	//std::wstring elementsNumPstrWstr(elementsNumPstr->Data());
	//int elementsNum = std::stoi(elementsNumPstrWstr);

	//auto partsNumPstr = textBoxThreads->Text->ToString();
	//std::wstring partsNumPstrWstr(partsNumPstr->Data());
	//int partsNum = std::stoi(partsNumPstrWstr);

	

	//std::vector<std::thread> threads;

	//for (int c = 0; c <= partsNum; c++) {
	//	if (c == partsNum && remainderFlag == 1) {
	//		partVolume = elementsNum - ((std::floor(elementsNum / partsNum)) * partsNum);
	//	}

	//	std::thread thr(thread_average, partVolume);
	//	threads.emplace_back(std::move(thr));
	//}

	//for (auto& thread : threads) {
	//	thread.join();
	//}

	//double globalAverage = std::accumulate(averages.begin(), averages.end(), 0.0) / averages.size();

	//// Convert calculated average to Platform::String and output it
	//std::string averageStr = "Average: " + std::to_string(globalAverage);
	//std::wstring averageWstr = s2ws(averageStr);
	//String^ averagePstr = ref new String(averageWstr.c_str());
	//textBlockOutput->Text = averagePstr;


	//Output execution time
	double execTime = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	std::string execTimeStr = "\r\nExecution time: " + std::to_string(execTime) + "ms";
	std::wstring execTimeWstr = s2ws(execTimeStr);
	String^ execTimePstr = ref new String(execTimeWstr.c_str());
	textBlockOutput->Text += execTimePstr;
}


void PowerSplit2::MainPage::predefinedSetRBChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	textBlockOutput->Text = "\n";
	if (GEPSButton->IsChecked->Value == true) {
		textBlockOutput->Text += "Gaussian elimination predefined set is selected";
		textBoxRow1X1Multiplier->Text = s2ps("10");
		textBoxRow1X2Multiplier->Text = s2ps("-5");
		textBoxRow1X3Multiplier->Text = s2ps("1");
		textBoxRow1X4Multiplier->Text = s2ps("5");
		textBoxRow1BMultiplier->Text = s2ps("21");
		textBoxRow2X1Multiplier->Text = s2ps("2");
		textBoxRow2X2Multiplier->Text = s2ps("30");
		textBoxRow2X3Multiplier->Text = s2ps("-21");
		textBoxRow2X4Multiplier->Text = s2ps("5");
		textBoxRow2BMultiplier->Text = s2ps("6");
		textBoxRow3X1Multiplier->Text = s2ps("2");
		textBoxRow3X2Multiplier->Text = s2ps("-8");
		textBoxRow3X3Multiplier->Text = s2ps("23");
		textBoxRow3X4Multiplier->Text = s2ps("12");
		textBoxRow3BMultiplier->Text = s2ps("5");
		textBoxRow4X1Multiplier->Text = s2ps("2");
		textBoxRow4X2Multiplier->Text = s2ps("-8");
		textBoxRow4X3Multiplier->Text = s2ps("-2");
		textBoxRow4X4Multiplier->Text = s2ps("-14");
		textBoxRow4BMultiplier->Text = s2ps("6");
	}
	else if (CRPSButton->IsChecked->Value == true) {
		textBlockOutput->Text += "Cramer's rule predefined set is selected";
		textBoxRow1X1Multiplier->Text = s2ps("18");
		textBoxRow1X2Multiplier->Text = s2ps("5");
		textBoxRow1X3Multiplier->Text = s2ps("-10");
		textBoxRow1X4Multiplier->Text = s2ps("7");
		textBoxRow1BMultiplier->Text = s2ps("14");
		textBoxRow2X1Multiplier->Text = s2ps("1");
		textBoxRow2X2Multiplier->Text = s2ps("5");
		textBoxRow2X3Multiplier->Text = s2ps("3");
		textBoxRow2X4Multiplier->Text = s2ps("-12");
		textBoxRow2BMultiplier->Text = s2ps("15");
		textBoxRow3X1Multiplier->Text = s2ps("3");
		textBoxRow3X2Multiplier->Text = s2ps("-2");
		textBoxRow3X3Multiplier->Text = s2ps("2");
		textBoxRow3X4Multiplier->Text = s2ps("-9");
		textBoxRow3BMultiplier->Text = s2ps("19");
		textBoxRow4X1Multiplier->Text = s2ps("1");
		textBoxRow4X2Multiplier->Text = s2ps("-2");
		textBoxRow4X3Multiplier->Text = s2ps("-15");
		textBoxRow4X4Multiplier->Text = s2ps("9");
		textBoxRow4BMultiplier->Text = s2ps("-13");
	}
	else {
		textBlockOutput->Text += "Custom multiplier input mode is selected";
		textBoxRow1X1Multiplier->Text = s2ps("");
		textBoxRow1X2Multiplier->Text = s2ps("");
		textBoxRow1X3Multiplier->Text = s2ps("");
		textBoxRow1X4Multiplier->Text = s2ps("");
		textBoxRow1BMultiplier->Text = s2ps("");
		textBoxRow2X1Multiplier->Text = s2ps("");
		textBoxRow2X2Multiplier->Text = s2ps("");
		textBoxRow2X3Multiplier->Text = s2ps("");
		textBoxRow2X4Multiplier->Text = s2ps("");
		textBoxRow2BMultiplier->Text = s2ps("");
		textBoxRow3X1Multiplier->Text = s2ps("");
		textBoxRow3X2Multiplier->Text = s2ps("");
		textBoxRow3X3Multiplier->Text = s2ps("");
		textBoxRow3X4Multiplier->Text = s2ps("");
		textBoxRow3BMultiplier->Text = s2ps("");
		textBoxRow4X1Multiplier->Text = s2ps("");
		textBoxRow4X2Multiplier->Text = s2ps("");
		textBoxRow4X3Multiplier->Text = s2ps("");
		textBoxRow4X4Multiplier->Text = s2ps("");
		textBoxRow4BMultiplier->Text = s2ps("");
	}
}
