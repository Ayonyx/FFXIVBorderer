#include "stdafx.h"
#include "Launcher.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow )
{
	/*AllocConsole();

	int hConHandle;
	long lStdHandle;
	FILE *fp;

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	*/
	//start

	System::String^ cmdLine = gcnew System::String(lpCmdLine);
	cmdLine = cmdLine->Replace("\"", "");
	int i = cmdLine->IndexOf(".exe");
	System::String^ cmd = cmdLine->Substring(0, i+4);

	System::String^ cmdArgs = cmdLine->Remove(0, i+4);
	System::Collections::Generic::List<String^>^ list = gcnew System::Collections::Generic::List<String^>();
	list->Add(cmd);

	if(cmdArgs->Length != 0)
	{
		cmdArgs = cmdArgs->Replace(" ", "");
		list->Add(cmdArgs);
	}

	array<System::String^>^ args = list->ToArray();

	FFXIVBorderer::Launch::Launcher^ l = gcnew FFXIVBorderer::Launch::Launcher();
	int ret = l->Launch(args);

	return ret;
}