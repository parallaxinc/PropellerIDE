/************************************************************************* 
* Derived from code by Zach Gorman with the following license:
*
* Serial port enumeration routines
*
* The EnumSerialPort function will populate an array of SSerInfo structs,
* each of which contains information about one serial port present in
* the system. Note that this code must be linked with setupapi.lib,
* which is included with the Win32 SDK.
*
* by Zach Gorman <gormanjz@hotmail.com>
*
* Copyright (c) 2002 Archetype Auction Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following condition is
* met: Redistributions of source code must retain the above copyright
* notice, this condition and the following disclaimer.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ARCHETYPE AUCTION SOFTWARE OR ITS
* AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************/

#define INITGUID

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <initguid.h>
#include <objbase.h>
#include <setupapi.h>

DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);

#ifdef MAIN
int CheckPort(const char* port, void* data)
{
    printf("Found %s\n", port);
    return 0;
}

int main(void)
{
    printf("serial_file returned %d\n", serial_find(NULL, CheckPort, NULL));
    return 0;
}
#endif

int serial_find(const char* prefix, int (*check)(const char* port, void* data), void* data)
{
	GUID *guidDev = (GUID *) &GUID_DEVINTERFACE_COMPORT;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;
	SP_DEVICE_INTERFACE_DATA ifcData;
	DWORD dwDetDataSize;
	int rc = -1;
	BOOL bOk;
	DWORD i;

	hDevInfo = SetupDiGetClassDevs( guidDev,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
		);

	if(hDevInfo == INVALID_HANDLE_VALUE) {
		printf("error: SetupDiGetClassDevs failed. (err=%lx)\n", GetLastError());
		goto done;
	}

	// Enumerate the serial ports
	dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
	pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA *)malloc(dwDetDataSize);

	ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	for (i = 0, bOk = TRUE; bOk; ++i) {

		// get the next device
		bOk = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guidDev, i, &ifcData);
		if (bOk) {

			// get the device details
			SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
			bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifcData, pDetData, dwDetDataSize, NULL, &devdata);
			if (bOk) {
				TCHAR fname[256], desc[256];

				// get some additional info
				BOOL bSuccess = SetupDiGetDeviceRegistryProperty(hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL, (PBYTE)fname, sizeof(fname), NULL)
					            && SetupDiGetDeviceRegistryProperty(hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL, (PBYTE)desc, sizeof(desc), NULL);
				if (bSuccess) {
					char *comx, *p;
					//printf("Device path: %s\n", pDetData->DevicePath);
					//printf("Friendly name: %s\n", fname);
					//printf("Description: %s\n", desc);
					if ((comx = strchr(fname, '(')) != NULL)
					    ++comx;
					else
					    comx = fname;
					if ((p = strchr(comx, ')')) != NULL)
					    *p = '\0';
                                    if ((*check)(comx, data) == 0) {
				        rc = 0;
				        goto done;
				    }
				}

			}
			else {
				printf("error: SetupDiGetDeviceInterfaceDetail failed. (err=%lx)\n", GetLastError());
				return 1;
			}
		}
		else {
			DWORD err = GetLastError();
			if (err != ERROR_NO_MORE_ITEMS) {
				printf("error: SetupDiEnumDeviceInterfaces failed. (err=%lx)\n", err);
				goto done;
			}
		}
	}

done:
    if (pDetData != NULL)
		free(pDetData);
	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	return rc;
}
