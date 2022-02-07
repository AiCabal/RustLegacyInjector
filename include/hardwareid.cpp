#include "include/hardwareid.h"

HardwareId::HardwareId()
{
	if (EnumSystemFirmwareTables(RSMB, NULL, 0) > 0)
	{
		MD5 md5 = MD5();

		UINT bufferLength = GetSystemFirmwareTable(RSMB, 0x0000, NULL, 0);
		//wcout << "BIOS 'RSMB' Table Length: " << length << endl;

		if (bufferLength > 0)
		{
			char* lpBuffer = new char[bufferLength];

			bufferLength = GetSystemFirmwareTable(RSMB, 0, lpBuffer, bufferLength);

			PRawSMBIOSData SMBIOS = (PRawSMBIOSData)lpBuffer;
			md5.Update(&lpBuffer[0], sizeof(RawSMBIOSData));

			#pragma region [Collect SMBIOS Entries of Tables]
			unsigned int index = sizeof(RawSMBIOSData);

			for (; index < bufferLength; index++)
			{
				PSMBIOS_TABLE_HEADER table = (PSMBIOS_TABLE_HEADER)&lpBuffer[index];

				unsigned int length = index + table->Length;

				while (++length < bufferLength)
				{
					if (lpBuffer[length - 1] == 0 && lpBuffer[length] == 0) break;
				}

				switch (table->Type)
				{
					case BIOSInfo:
					md5.Update(&lpBuffer[index], length - index);
					break;
					case SystemInfo:
					md5.Update(&lpBuffer[index], length - index);
					break;
					case BaseBoardInfo:
					md5.Update(&lpBuffer[index], length - index);
					break;
					case ProcessorInfo:
					md5.Update(&lpBuffer[index], length - index);
					break;
				}

				index = length;
			}
			#pragma endregion

			md5.Finalize();        
			hwid = md5.HexDigest();
		}
	}
}

char* HardwareId::ToString()
{
	return this->hwid;
}
