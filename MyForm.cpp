#include "MyForm.h"
#define MAX_PROCESS 100

using namespace System;
using namespace System::Windows::Forms;



[STAThread]
void main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    CPUSchedulers::MyForm form;
    Application::Run(% form);
}



