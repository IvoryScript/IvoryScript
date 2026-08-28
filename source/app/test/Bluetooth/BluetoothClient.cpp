// BluetoothClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <bthutil.h>
#include <Bt_api.h>


/////////////////////////////////////////////////////////////////////////////
// Sets Bluetooth power

DWORD BluetoothPower(DWORD dwMode) 
{
	DWORD	dwCurrent = 0;

	BthGetMode(&dwCurrent);

	if ( dwMode != dwCurrent )
		BthSetMode( dwMode );
	
	return(dwCurrent);
}

int GetBA ( const TCHAR* pp, BT_ADDR* pba) 
{
	for (int i = 0 ; i < 4 ; ++i, ++pp) 
    {
		if ( !iswxdigit (*pp) )
			return FALSE;

		int c = *pp;

		if (c >= 'a')
			c = c - 'a' + 0xa;
		else if (c >= 'A')
			c = c - 'A' + 0xa;
		else c = c - '0';

		if ((c < 0) || (c > 16))
			return FALSE;

		*pba = *pba * 16 + c;
	}

	for (i = 0 ; i < 8 ; ++i, ++pp) 
    {
		if (! iswxdigit (*pp))
			return FALSE;

		int c = *pp;
		
		if (c >= 'a')
			c = c - 'a' + 0xa;
		else if (c >= 'A')
			c = c - 'A' + 0xa;
		else c = c - '0';

		if ((c < 0) || (c > 16))
			return FALSE;

		*pba = *pba * 16 + c;
	}

	if (*pp != '\0')
		return FALSE;

	return TRUE;
}


DWORD	BluetoothVirtualPort = 0;

HANDLE CreateVirtualCommPort(const TCHAR* DeviceAddress, DWORD PortNumber )
{
	PORTEMUPortParams pp;
	memset (&pp, 0, sizeof(pp));
	
	GetBA(DeviceAddress, &pp.device);
	
	pp.channel = 1;
	pp.uiportflags = RFCOMM_PORT_FLAGS_REMOTE_DCB;

	BluetoothVirtualPort = PortNumber;

	return RegisterDevice(TEXT("COM"), BluetoothVirtualPort, TEXT("btd.dll"), (DWORD)&pp);
}

BOOL DestroyVirtualCommPort( HANDLE handle )
{
	return	DeregisterDevice( handle );
}

HANDLE OpenBluetoothDataPort( void )
{
	WCHAR szComPort[30];
	wsprintf (szComPort, L"COM%d:", BluetoothVirtualPort);
	HANDLE handle = CreateFile(szComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if ( handle == INVALID_HANDLE_VALUE )
	{
		LPVOID lpMsgBuf;
		FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						0, // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL );

		MessageBox( NULL, (LPCTSTR)lpMsgBuf, L"Open Error", MB_OK | MB_ICONINFORMATION );
		LocalFree( lpMsgBuf );
		return	handle;
	}

	COMMTIMEOUTS timeouts;

	timeouts.ReadIntervalTimeout = 500;			// 0.5 seconds between chars
	timeouts.ReadTotalTimeoutMultiplier = 0;	// not interested in the number we are trying to read
	timeouts.ReadTotalTimeoutConstant = 5000;	// max of 5 seconds regardless
	timeouts.WriteTotalTimeoutMultiplier = 0;	// not interested in the number we are trying to write
	timeouts.WriteTotalTimeoutConstant = 5000;	// max of 5 seconds regardless

	SetCommTimeouts(handle, &timeouts);

	return	handle;
}

void CloseBluetoothDataPort( HANDLE hCommPort )
{
	CloseHandle (hCommPort);
}

BOOL WriteBluetoothData( HANDLE hCommPort, char* buffer )
{
	DWORD NumberOfBytesWritten = 0;
	BOOL  result = TRUE;
	
	if ( WriteFile(hCommPort, buffer, (strlen(buffer) + 1), &NumberOfBytesWritten, NULL) == 0 )
	{
		result = FALSE;
		LPVOID lpMsgBuf;
		FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						0, // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL );

		MessageBox( NULL, (LPCTSTR)lpMsgBuf, L"Write Error", MB_OK | MB_ICONINFORMATION );
		LocalFree( lpMsgBuf );
	}
	
	return	result;
}


BOOL ReadBluetoothData( HANDLE hCommPort, char* buffer, DWORD size, DWORD* BytesRead )
{
	BOOL  result = TRUE;
	
	if ( ReadFile(hCommPort, buffer, size, BytesRead, NULL) == 0 )
	{
		result = FALSE;
		LPVOID lpMsgBuf;
		FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						0, // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL );

		MessageBox( NULL, (LPCTSTR)lpMsgBuf, L"Read Error", MB_OK | MB_ICONINFORMATION );
		LocalFree( lpMsgBuf );
	}

	if ( *BytesRead == 0 )
		result = FALSE;

	return	result;
}


/*
	Bluetooth modes.

	enum BTH_RADIO_MODE
	{
		BTH_POWER_OFF,
		BTH_CONNECTABLE,
		BTH_DISCOVERABLE
	};



	The SDP Record (the binding information) is quite complicated. It is probably easier to bond 
	once and then save the registry output to a reg file to preserve it. Alternatively let the UI
	ask the PIN question every time and set it that way. Of course that will not survive a Cold Boot.

	An example reg file is below

	[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Bluetooth\Device\000a4f003fc8]
	"trusted"=dword:00000001
	"class"=dword:00000000
	"name"="BL-521"

	[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Bluetooth\Device\000a4f003fc8\Services]

	[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Bluetooth\Device\000a4f003fc8\Services\00001101-0000-1000-8000-00805f9b34fb]
	"sdprecord"=hex:\
      36,00,78,36,00,37,09,00,00,0a,00,01,00,00,09,00,05,35,03,19,10,02,09,00,01,\
      35,03,19,11,01,09,00,04,35,0c,35,03,19,01,00,35,05,19,00,03,08,01,09,01,00,\
      25,09,53,65,72,69,61,6c,20,30,31,36,00,3b,09,00,00,0a,00,01,00,01,09,00,05,\
      35,03,19,10,02,09,00,01,35,03,19,11,01,09,00,04,35,0c,35,03,19,01,00,35,05,\
      19,00,03,08,02,09,01,00,25,0d,52,65,6d,6f,74,65,20,43,6f,6e,66,69,67
	"name"="Serial Port"
	"channel"=dword:00000001
  */


int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	TCHAR*		BLUETOOTH_REMOTE_ADDRESS	= TEXT("000A4F003FC8");
	const int	BLUETOOTH_VIRTUAL_PORT		= 8;   
	TCHAR		Message[2048];
	char		buffer[2048];

	memset(Message, 0, 2048 * 2);
	memset(buffer, 0, 2048);

	for (int i = 0; i < 10; i++)
		sprintf( buffer, "%sThis is some test data %d\r\n", buffer, i);

	DWORD	StartTick = GetTickCount();

	DWORD	OldPowerState = BluetoothPower( BTH_CONNECTABLE );
	DWORD	PowerTick = GetTickCount();

	HANDLE	handle	  = CreateVirtualCommPort( BLUETOOTH_REMOTE_ADDRESS, BLUETOOTH_VIRTUAL_PORT );
	HANDLE	hDataPort = OpenBluetoothDataPort();
	DWORD	BondTick  = GetTickCount();
	DWORD	SendTick  = 0;
	DWORD	ExitTick  = 0;

	if ( hDataPort != INVALID_HANDLE_VALUE )
	{
		WriteBluetoothData( hDataPort, buffer );
		SendTick = GetTickCount();

		DWORD BytesRead = 0;
		memset(buffer, 0, 2048);

		while ( true )
		{
			if ( ReadBluetoothData( hDataPort, buffer, 2048, &BytesRead ) == TRUE)
			{
				wsprintf( Message, TEXT("%hs"), buffer);
				MessageBox(0, Message, TEXT("Data read"), MB_OK );
				memset(buffer, 0, 2048);
				BytesRead = 0;
			}
			else
			{
				break;
			}
		}

		ExitTick = GetTickCount();
		CloseBluetoothDataPort( hDataPort );
	}

	DestroyVirtualCommPort( handle );	
	DWORD	CloseTick = GetTickCount();
	
	BluetoothPower( OldPowerState );
	DWORD	EndTick	= GetTickCount();

	if ( hDataPort != INVALID_HANDLE_VALUE )
	{
		TCHAR* Format = TEXT("PowerOn\t%d\r\nBond\t\t%d\r\nSend\t\t%d\r\nClose\t\t%d\r\nPowerOff\t%d");
		wsprintf( Message, Format, PowerTick-StartTick, BondTick-PowerTick, SendTick-BondTick, CloseTick-ExitTick, EndTick-CloseTick);
		MessageBox(0, Message, TEXT("Timing data (msecs)"), MB_OK );
	}

	return 0;
}

