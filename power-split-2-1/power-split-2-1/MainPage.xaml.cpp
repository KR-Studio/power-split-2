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

// Vector with ids of processors that have activeState in checkBoxes
std::vector<int> checkBoxesActive = { 0 };
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


	//auto elementsNumPstr = textBoxArray->Text->ToString();
	//std::wstring elementsNumPstrWstr(elementsNumPstr->Data());
	//int elementsNum = std::stoi(elementsNumPstrWstr);

	//auto partsNumPstr = textBoxThreads->Text->ToString();
	//std::wstring partsNumPstrWstr(partsNumPstr->Data());
	//int partsNum = std::stoi(partsNumPstrWstr);

	//int partVolume = 0;
	//int remainderFlag = 0;
	//if (elementsNum % partsNum != 0.0) {
	//	partsNum += 1;
	//	partVolume = std::floor(elementsNum / partsNum);
	//	remainderFlag = 1;
	//}
	//else {
	//	partVolume = elementsNum / partsNum;
	//}

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
