#include "stdafx.h"
#include "Launcher.h"

namespace FFXIVBorderer
{
	namespace Launch
	{
		Launcher::Launcher()
		{
			//do nothing
			//ideas for gameplay tracking here
			/* 
				* character name
				* server name -- 0x02CD846E read up to ! (login message)
				* character race
				* character rank / class rank
			*/
		}

		Launcher::~Launcher()
		{
			//do nothing
		}
		
		int Launcher::Launch(array<String^>^ args)
		{
			if(args->Length == 0)
			{
				return 0;
			}

			System::IO::FileInfo^ fi = gcnew System::IO::FileInfo(args[0]);
			if( !fi->Exists ) return 0;

			System::IO::DirectoryInfo^ di = fi->Directory;

			InstallDir = di->FullName;
			ExeName = fi->FullName;

			for(int i = 1; i < args->Length; i++)
			{
				CommandLine += " " + args[i];
			}

			Start();

			return 0;
		}

		void Launcher::Start_DebugThread()
		{
			STARTUPINFO StartupInfo;
			PROCESS_INFORMATION ProcessInfo;

			ZeroMemory( &StartupInfo, sizeof(StartupInfo) );
			ZeroMemory( &ProcessInfo, sizeof(ProcessInfo) );

			StartupInfo.cb = sizeof(StartupInfo);
			StartupInfo.dwFlags = STARTF_USESHOWWINDOW || STARTF_FORCEONFEEDBACK;
			StartupInfo.wShowWindow = SW_SHOWNORMAL;
			System::String^ runString = ExeName + CommandLine;
			if (CreateProcess(NULL, Convert(runString), NULL, NULL, false, (DEBUG_PROCESS || DEBUG_ONLY_THIS_PROCESS), NULL, Convert(InstallDir), &StartupInfo, &ProcessInfo))
			{
				ProcessHandle = ProcessInfo.hProcess;
				ProcessID = ProcessInfo.dwProcessId;
				_semaphore->Release();
			}

			DebugSetProcessKillOnExit(true);
			DebugActiveProcessStop(ProcessInfo.dwProcessId);
		}

		void Launcher::Start_MaximizedWindowThread()
		{
			while (true)
			{
				HWND WindowHandle = FindWindow(L"RAPTURE", L"FINAL FANTASY XIV");
				if (WindowHandle != 0)
				{
					LONG OldExStyle = GetWindowLong(WindowHandle, GWL_EXSTYLE);
					LONG OldStyle = GetWindowLong(WindowHandle, GWL_STYLE);
					OldExStyle = OldExStyle & ~WS_EX_CLIENTEDGE;
					OldExStyle = OldExStyle & ~WS_EX_WINDOWEDGE;
					OldStyle = OldStyle & ~WS_DLGFRAME;
					OldStyle = OldStyle & ~WS_THICKFRAME;
					OldStyle = OldStyle & ~WS_OVERLAPPED;
					OldStyle = OldStyle & ~WS_BORDER;
					SetWindowLong(WindowHandle, GWL_STYLE, OldStyle);
					SetWindowLong(WindowHandle, GWL_EXSTYLE, OldExStyle);
					MoveWindow(WindowHandle, 0, 0, GetSystemMetrics( SM_CXFULLSCREEN ), GetSystemMetrics( SM_CYSCREEN ), true);
					break;
				}
				System::Threading::Thread::Sleep(10000);
			}
		}

		void Launcher::Start_PriorityThread()
		{
			while(true)
			{
				SetPriorityClass(ProcessHandle, NORMAL_PRIORITY_CLASS);
				System::Threading::Thread::Sleep(5000);
			}
		}

		void Launcher::Start()
		{
			//start debug process
			_semaphore = gcnew System::Threading::Semaphore(0,1);
			System::Threading::ThreadStart^ threadStartDebugThread = gcnew System::Threading::ThreadStart(this, &FFXIVBorderer::Launch::Launcher::Start_DebugThread);
			System::Threading::Thread^ threadDebugThread = gcnew System::Threading::Thread(threadStartDebugThread);

			threadDebugThread->Start();
			_semaphore->WaitOne();

			//start the maximized / windowed mode
			System::Threading::ThreadStart^ threadStartMaximizedWindow = gcnew System::Threading::ThreadStart(this, &FFXIVBorderer::Launch::Launcher::Start_MaximizedWindowThread);
			System::Threading::Thread^ threadMaximizedWindow = gcnew System::Threading::Thread(threadStartMaximizedWindow);
			threadMaximizedWindow->Start();

			//start priority thread
			System::Threading::ThreadStart^ threadStartPriority = gcnew System::Threading::ThreadStart(this, &FFXIVBorderer::Launch::Launcher::Start_PriorityThread);
			System::Threading::Thread^ threadPriority = gcnew System::Threading::Thread(threadStartPriority);
			threadPriority->Start();

			//media timer
			timeBeginPeriod(1);
			System::GC::Collect(0, System::GCCollectionMode::Forced);

			//wait for the process handle
			WaitForSingleObject(ProcessHandle, INFINITE);

			//reset time
			timeEndPeriod(1);

			CloseHandle(ProcessHandle);
			exit(0);
		}
	}
}