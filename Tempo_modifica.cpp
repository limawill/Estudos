#include <windows.h>
#include <stdio.h>
#include <atlstr.h>
#include <iostream>
#include <string>
#include <ctime>
#include <sstream>

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

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

SYSTEMTIME Temp_arquivo(){
	HANDLE hFile1;
	DWORD dwFileSize,dwFileType;
	FILETIME ftCreate, ftAccess, ftWrite;
	SYSTEMTIME stUTC2, stLocal2;
	struct tm newyear;
	time_t t;

	//Convertendo o valor vindo do Registro
	CT2CA pszConvertedAnsiString (ReadProxyServer());
	std::string Local (pszConvertedAnsiString);
	//Criando o arquivo
	Local = Local + "\\enumwindows.txt";
	std::wstring stemp = s2ws(Local);
	LPCWSTR result = stemp.c_str();
	
	// Opening the existing file
	hFile1 = CreateFile(result, //file to open
			 GENERIC_READ, //open for reading
			 FILE_SHARE_READ, //share for reading
			 NULL, //default security
			 OPEN_EXISTING, //existing file only
			 FILE_ATTRIBUTE_NORMAL, //normal file
			 NULL); //no attribute template

	if(hFile1 == INVALID_HANDLE_VALUE)
	{
		printf("Could not open %S file, error %d\n", result, GetLastError());
		//return 4;
	}

	// Retrieve the file times for the file.
	if(!GetFileTime(hFile1, &ftCreate, &ftAccess, &ftWrite))
	{
		printf("Something wrong lol!\n");
		//return FALSE;
	}

	
	// Convert the last-write time to local time.
	FileTimeToSystemTime(&ftWrite, &stUTC2);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC2, &stLocal2);
	
	printf("written : %02d:%02d\n", stLocal2.wHour, stLocal2.wMinute);
	
	// close the file's handle and itself
	if(CloseHandle(hFile1) == 0)
		printf("Can't close the %S handle!\n", result);

	return stLocal2; 

}






_int64 Delta(const SYSTEMTIME st1, const SYSTEMTIME st2)
{
    union timeunion {
        FILETIME fileTime;
        ULARGE_INTEGER ul;
    } ;
    
    timeunion ft1;
    timeunion ft2;

    SystemTimeToFileTime(&st1, &ft1.fileTime);
    SystemTimeToFileTime(&st2, &ft2.fileTime);

    return ft2.ul.QuadPart - ft1.ul.QuadPart;
}



int main(void)
{
    SYSTEMTIME lt,st;
	double seconds;
 
    GetLocalTime(&lt);
    
	st = Temp_arquivo();

    printf(" The local time is: %02d:%02d\n", lt.wHour, lt.wMinute);
	printf("The system time is: %02d:%02d\n", st.wHour, st.wMinute);

	
	_int64 i = Delta(st, lt);
    std::cout << "times are " << i / 10000000 << " seconds apart\n";


	system("pause");
	return 0;
}
