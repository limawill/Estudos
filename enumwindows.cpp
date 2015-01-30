#include <Windows.h>
#include "Captura.h"
#include <iostream>
#include <fstream> 
#include <atlstr.h>
#include <string>
#include <WinUser.h>
#include <map>
#include <eh.h>
#include <exception>
#include <ctime>

//Criando o arquivo
std::ofstream myfile;

/********************************* Tratamento de erros - Evitar POPUS *******************************************/
class SE_Exception
{
private:
    unsigned int nSE;
public:
    SE_Exception() {}
    SE_Exception( unsigned int n ) : nSE( n ) {}
    ~SE_Exception() {}
    unsigned int getSeNumber() { return nSE; }
};

void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
    printf( "In trans_func.\n" );
    throw SE_Exception();
}


void my_terminate() {
    std::cerr << "my terminate handler";
    std::exit(0);
}

void my_unexpected() {
    std::cerr << "my unexpected handler";
    std::exit(EXIT_FAILURE);
}

/****************************************************************************/

CString ReadProxyServer()
{
	CString cSvar = _T("");
	HKEY hKey;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Automatos\\Ada\\Common\\3.0"),0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
	{
		if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Automatos\\Ada\\Common\\3.0"),0,KEY_WOW64_32KEY|KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS)
		{
			std::cout << "ERROR! Path not found! "<<"\n";
			std::exit;
		}
	}
	TCHAR szData[256];
	DWORD dwKeyDataType;
	DWORD dwDataBufSize = 256;
	if (::RegQueryValueEx(hKey, _T("BaseDir"), NULL, &dwKeyDataType,(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
	{
		switch ( dwKeyDataType )
		{
		case REG_SZ:
			cSvar = CString(szData);
			break;
		}
	}
	::RegCloseKey( hKey );
	return cSvar;
}

struct EnumWindowParam {
	std::map<DWORD, int> *pidWindow;
	HWND foreWin;
};



static BOOL CALLBACK enumWindow(HWND hWnd, LPARAM lParam) throw() {
	EnumWindowParam *enumParam;
	enumParam = reinterpret_cast<EnumWindowParam*>(lParam);
	unsigned int backgroud = 0;
	DWORD pid;

	// see remarks. This functions just check for WS_VISIBLE style. A window can be obscured
	// by another window and still be visible for this function
	if (!IsWindowVisible(hWnd))
		return TRUE;

	if (IsIconic(hWnd))
		return TRUE;

	RECT rect;
	// some tray icons say they are visible
	if (GetWindowRect(hWnd, &rect) != 0 && (rect.top | rect.left | rect.bottom | rect.right) == 0)
		return TRUE;

	// obtém o id da thread que criou este processo
	GetWindowThreadProcessId(hWnd, &pid);
	if(enumParam->foreWin == hWnd)
	{
		backgroud = 2;
	}
	else
	{
		backgroud = 1;
	}


	//std::cout <<pid << " "<<backgroud <<std::endl;
	myfile <<pid << " "<<backgroud <<std::endl;

	// stop if a maximized windows is found. No more windows visible
	if (IsZoomed(hWnd) != 0)
		return FALSE;
	return  TRUE;
}


void EnumWin()
{
	std::map<DWORD, int> pidWindow;
	EnumWindowParam enumParam;
	enumParam.foreWin = GetForegroundWindow();
	enumParam.pidWindow = &pidWindow;
	EnumWindows(enumWindow, reinterpret_cast<LPARAM>(&enumParam));

}

void temp()
{
   time_t now = time(0);
   char* dt = ctime(&now);
   myfile <<"2 - dt" << " "<< dt <<std::endl;
}


int _tmain(int argc, _TCHAR* argv[]) {
	//Convertendo o valor vindo do Registro
	CT2CA pszConvertedAnsiString (ReadProxyServer());
	std::string Local (pszConvertedAnsiString);

	//Criando o arquivo
	Local = Local + "\\enumwindows.txt";
	//while(true)
//	{
		try{
				_set_se_translator( trans_func );
				std::set_terminate(my_terminate);
				std::set_unexpected(my_unexpected);
				//myfile.open (Local,std::ofstream::app);
				myfile.open (Local);
				EnumWin();
				//temp();
				myfile.close();
	               
		}catch(SE_Exception e ){
			std::cerr << "caught int\n";
		}catch( const std::logic_error& ){
			std::cerr << "Caught a __try exception with SE_Exception.\n";
		}
		Sleep(Temp);
//	}
	return 0;
}
