#include "pch.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <windows.h>
#include <ctime>
#include <sstream> 
#include <string>
#include <shellapi.h>
#include <thread>
#include <chrono>
#include <winbio.h>
#include <iostream>

#pragma comment(lib, "winbio.lib")

#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::AI_Virtual_Assistant::implementation
{

    void DebugLog(const std::wstring& message) {
        OutputDebugStringW(message.c_str());
        OutputDebugStringW(L"\n");
    }


    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

    void MainWindow::GreetMe()
    {
        time_t now = time(0);
        struct tm timeInfo;
        localtime_s(&timeInfo, &now);

        hstring greeting;
        if (timeInfo.tm_hour < 12)
        {
            greeting = L"Good Morning, Boss!";
        }
        else if (timeInfo.tm_hour >= 12 && timeInfo.tm_hour <= 16)
        {
            greeting = L"Good Afternoon, Boss!";
        }
        else
        {
            greeting = L"Good Evening, Boss!";
        }

        GreetingBox().Subtitle(greeting);
        GreetingBox().IsOpen(true);

        SpeakText(greeting);
    }

    void MainWindow::SpeakText(hstring text)
    {
        std::wstring wtext(text);
        std::string textStr(wtext.begin(), wtext.end());
        std::string command = "espeak \"" + textStr + "\"";

        STARTUPINFOA startupInfo = { 0 };
        PROCESS_INFORMATION processInfo = { 0 };

        startupInfo.cb = sizeof(startupInfo);
        startupInfo.dwFlags = STARTF_USESHOWWINDOW;
        startupInfo.wShowWindow = SW_HIDE;

        if (CreateProcessA(NULL, command.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo))
        {
            WaitForSingleObject(processInfo.hProcess, INFINITE);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }
        else
        {
            OutputDebugStringA("Failed to execute eSpeak command.\n");
        }
    }

    void MainWindow::ProcessCommand(winrt::hstring const& command)
    {
        hstring response;

        if (command == L"hi" || command == L"hello")
        {
            response = L"Hello Boss!";
        }
        else if (command == L"bye")
        {
            response = L"Goodbye, have a nice day!";
        }
        else if (command == L"who are you")
        {
            response = L"I'm a virtual assistant created by Shaikh Saad!";
        }
        else if (command == L"how are you")
        {
            response = L"I'm good, sir. How can I assist you?";
        }
        else if (command == L"time")
        {
            time_t now = time(0);
            char timeBuffer[100];
            ctime_s(timeBuffer, sizeof(timeBuffer), &now);
            response = L"The current date and time is:\n" + winrt::to_hstring(timeBuffer);
        }
        else if (command.size() > 4 && std::wstring_view(command).substr(0, 4) == L"msg ")
        {
            std::wstring_view commandView = std::wstring_view(command);
            std::wstring remainingText = std::wstring(commandView.substr(4));
            size_t firstSpace = remainingText.find(L' ');
            if (firstSpace == std::wstring::npos)
            {
                response = L"Please provide a phone number and a message!";
            }
            else
            {
                std::wstring phoneNumber = remainingText.substr(0, firstSpace);
                std::wstring message = remainingText.substr(firstSpace + 1);
                response = L"Sending message: " + winrt::hstring(message);

                std::wstring encodedMessage;
                for (wchar_t c : message)
                {
                    if (c == L' ') encodedMessage += L"%20";
                    else encodedMessage += c;
                }

                std::wstring whatsappCmd = L"whatsapp://send?phone=" + phoneNumber + L"&text=" + encodedMessage;
                ShellExecuteW(NULL, L"open", whatsappCmd.c_str(), NULL, NULL, SW_SHOWNORMAL);
                std::this_thread::sleep_for(std::chrono::seconds(3));
                keybd_event(VK_RETURN, 0, 0, 0);
                keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
            }
        }
        else if (command.size() > 5 && std::wstring_view(command).substr(0, 5) == L"call ")
        {
            std::wstring_view commandView = std::wstring_view(command);
            std::wstring phoneNumber = std::wstring(commandView.substr(5));
            response = L"Calling contact...";

            std::wstring whatsappCallCmd = L"whatsapp://call?phone=" + phoneNumber;
            ShellExecuteW(NULL, L"open", whatsappCallCmd.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        else
        {
            response = L"Searching Google for: " + command;
            std::wstring searchUrl = L"https://www.google.com/search?q=" + std::wstring(command.c_str());
            for (auto& c : searchUrl) if (c == L' ') c = L'+';
            ShellExecuteW(NULL, L"open", searchUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }

        DispatcherQueue().TryEnqueue([this, response]()
            {
                Answers().Text(response);
                SpeakText(response);
            });
    }

    void MainWindow::SendPrompt_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        hstring userInput = UserInput().Text();
        ProcessCommand(userInput);
    }


    void MainWindow::Sign_In_Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        auto user = Login_Username().Text();
        auto pass = Login_Password().Password();

        if (user == L"saad" && pass == L"shaikh") {
            Main().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
            Login().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
            GreetMe();
        }
        else {
            MessageBoxW(NULL, L"Invalid username or password!", L"Login Failed", MB_OK | MB_ICONERROR);
        }
    }
}


