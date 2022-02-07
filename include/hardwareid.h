#ifndef _EXT_HARDWAREID_H_
#define _EXT_HARDWAREID_H_

#include <windows.h>
#include <string>

#include "md5.h"

enum SystemFirmwareTableProvider : DWORD
{
	ACPI = (BYTE)'A' << 24 | (BYTE)'C' << 16 | (BYTE)'P' << 8 | (BYTE)'I',
	FIRM = (BYTE)'F' << 24 | (BYTE)'I' << 16 | (BYTE)'R' << 8 | (BYTE)'M',
	RSMB = (BYTE)'R' << 24 | (BYTE)'S' << 16 | (BYTE)'M' << 8 | (BYTE)'B'
};

typedef enum _SMBIOSTableType : BYTE
{
	BIOSInfo = 0,
	SystemInfo = 1,
	BaseBoardInfo = 2,
	EnclosureInfo = 3,
	ProcessorInfo = 4,
	MemoryControllerInfo = 5,
	MemoryModuleInfo = 6,
	CacheInfo = 7,
	PortConnectorInfo = 8,
	SystemSlotsInfo = 9,
	OnBoardDevicesInfo = 10,
	OEMStrings = 11,
	SystemConfigurationOptions = 12,
	BIOSLanguageInfo = 13,
	GroupAssociations = 14,
	SystemEventLog = 15,
	PhysicalMemoryArray = 16,
	MemoryDevice = 17,
	MemoryErrorInfo = 18,
	MemoryArrayMappedAddress = 19,
	MemoryDeviceMappedAddress = 20,
	EndofTable = 127
} SMBIOSTableType;

#pragma pack(push)
#pragma pack(1)
typedef struct _RawSMBIOSData
{
	BYTE	Used20CallingMethod;
	BYTE	MajorVersion;
	BYTE	MinorVersion;
	BYTE	DmiRevision;
	UINT	Length;
} RawSMBIOSData, *PRawSMBIOSData;

typedef struct _SMBIOS_TABLE_HEADER
{
	SMBIOSTableType Type;
	BYTE Length;
	WORD Handle;
} SMBIOS_TABLE_HEADER, *PSMBIOS_TABLE_HEADER;

typedef struct _SMBIOS_TABLE_BIOS_INFO
{
	SMBIOS_TABLE_HEADER Header;
	BYTE Vendor;
	BYTE Version;
	WORD StartingSegment;
	BYTE ReleaseDate;
	BYTE BiosRomSize;
	LONG Characteristics;
	BYTE ExtensionBytes[2];
} SMBIOS_TABLE_BIOS_INFO, *PSMBIOS_TABLE_BIOS_INFO;

typedef struct _SMBIOS_TABLE_SYSTEM_INFO
{	
	SMBIOS_TABLE_HEADER Header;
	BYTE Manufacturer;
	BYTE ProductName;
	BYTE Version;
	BYTE SerialNumber;
	BYTE UUID[16];
	BYTE WakeUpType;
} SMBIOS_TABLE_SYSTEM_INFO, *PSMBIOS_TABLE_SYSTEM_INFO;

typedef struct _SMBIOS_TABLE_BASEBOARD_INFO
{
	SMBIOS_TABLE_HEADER Header;
	BYTE Manufacturer;
	BYTE ProductName;
	BYTE Version;
	BYTE SerialNumber;
} SMBIOS_TABLE_BASEBOARD_INFO, *PSMBIOS_TABLE_BASEBOARD_INFO;

typedef struct _SMBIOS_TABLE_ENCLOSURE_INFO
{
	SMBIOS_TABLE_HEADER Header;
	BYTE Manufacturer;
	BYTE Type;
	BYTE Version;
	BYTE SerialNumber;
	BYTE AssetTagNumber;
	BYTE BootUpState;
	BYTE PowerSupplyState;
	BYTE ThermalState;
	BYTE SecurityStatus;
	LONG OEM_Defined;
} SMBIOS_TABLE_ENCLOSURE_INFO, *PSMBIOS_TABLE_ENCLOSURE_INFO;

typedef struct _SMBIOS_TABLE_PROCESSOR_INFO
{
	SMBIOS_TABLE_HEADER Header;
	BYTE SocketDesignation;
	BYTE ProcessorType;
	BYTE ProcessorFamily;
	BYTE ProcessorManufacturer;
	ULONG ProcessorID;
	BYTE ProcessorVersion;
	BYTE ProcessorVoltage;
	WORD ExternalClock;
	WORD MaxSpeed;
	WORD CurrentSpeed;
	BYTE Status;
	BYTE ProcessorUpgrade;
	WORD L1CacheHandler;
	WORD L2CacheHandler;
	WORD L3CacheHandler;
	BYTE SerialNumber;
	BYTE AssetTag;
	BYTE PartNumber;
} SMBIOS_TABLE_PROCESSOR_INFO, *PSMBIOS_TABLE_PROCESSOR_INFO;

typedef struct _SMBIOS_TABLE_CACHE_INFO
{
	SMBIOS_TABLE_HEADER Header;
	BYTE SocketDesignation;
	LONG CacheConfiguration;
	WORD MaximumCacheSize;
	WORD InstalledSize;
	WORD SupportedSRAMType;
	WORD CurrentSRAMType;
	BYTE CacheSpeed;
	BYTE ErrorCorrectionType;
	BYTE SystemCacheType;
	BYTE Associativity;
} SMBIOS_TABLE_CACHE_INFO, *PSMBIOS_TABLE_CACHE_INFO;
#pragma pack(push)

class HardwareId
{	
	char *hwid;

public:	
	HardwareId();
	char* ToString();
};

#endif