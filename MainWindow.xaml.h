#pragma once

#include "MainWindow.g.h"
#include "MainWindow.xaml.h"


namespace winrt::AI_Virtual_Assistant::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        void CaptureFingerprint();
        MainWindow();
        void GreetMe();
        void SpeakText(hstring text);
        void ProcessCommand(hstring const& command);
        void SendPrompt_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Sign_In_Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::AI_Virtual_Assistant::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
