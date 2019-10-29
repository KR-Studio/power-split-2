//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace PowerSplit2
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CheckBoxClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void predefinedSetRBChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void calculationMethodRBChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void gaussElimination();
		void cramersRule();
		void simpleIterations();
		void seidel();
	};
}
