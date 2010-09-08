#pragma once
#using <System.dll>

using namespace System;
namespace FFXIVBorderer
{
	namespace Launch
	{
		public ref class Launcher
		{
		private:
			static System::Threading::Semaphore^ _semaphore;
			static String^ CommandLine = "";
			static String^ InstallDir = String::Empty;
			static String^ ExeName = String::Empty;
			static HANDLE ProcessHandle = 0;
			static DWORD ProcessID = 0;
			static System::Threading::Thread^ threadAffiliationThread;
			
			static LPWSTR Convert(System::String^ str)
			{
				pin_ptr<const wchar_t> wch = PtrToStringChars(str);
				return (LPWSTR)wch;
			}

			//local function to keep the debugger active
			void Start_DebugThread();
			void Start_MaximizedWindowThread();
			void Start_PriorityThread();
			void Start();

		public:
			Launcher();
			~Launcher();
			int Launch(array<String^>^ args);
		};
	}
}