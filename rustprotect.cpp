#include "rustprotect.h"
#include <QTextCodec>
bool rust_protect_bypass(p_helper* helper,char* file_name,char* name_space,char* class_name,char* method_name,bool is_native)
{
    unsigned char nop = 0x90;
    unsigned char jmp_18[] = {0xEB,0x18};

    bool is_connected = helper->ConnectToProcess();
    if(!is_connected){
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,0,GetProcessID((wchar_t*)L"rust.exe"));

    if(!hProcess){
        return false;
    }

    unsigned char buffer[mono_assembly_load_from_full_count_nop1] ,
                  buffer2[mono_assembly_load_from_full_count_nop2],
                  buffer3[mono_assembly_load_from_full_count_nop3];

    void* _load_from_full_address = helper->GetExportProcedure((char*)"mono.dll",(char*)"mono_assembly_load_from_full");
    void* _mono_image_open_address = helper->GetExportProcedure((char*)"mono.dll",(char*)"mono_image_open_from_data_with_name");

    if(!_load_from_full_address || !_mono_image_open_address){
        CloseHandle(hProcess);
        return false;
    }



    ReadProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_1),(void*)buffer,sizeof(buffer),NULL);
    ReadProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_2),(void*)buffer2,sizeof(buffer2),NULL);
    ReadProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_3),(void*)buffer3,sizeof(buffer3),NULL);


    for(int i = 0;i<mono_assembly_load_from_full_count_nop1;i++)
    {
        WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_1+i),&nop,sizeof(char),NULL);
    }

    for(int i = 0;i<mono_assembly_load_from_full_count_nop2;i++)
    {
        WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_2+i),&nop,sizeof(char),NULL);
    }

    for(int i = 0;i<mono_assembly_load_from_full_count_nop3;i++)
    {
        WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_3+i),&nop,sizeof(char),NULL);
    }

    WriteProcessMemory(hProcess,(void*)((uptr_t)_mono_image_open_address+mono_image_open_offset),&jmp_18,sizeof(jmp_18),NULL);

    unsigned char rCall_nop[] = {0x90,0x90,0x90,0x90,0x90};

    QMessageBox box;
    void* mono_domain = helper->MonoGetRootDomain();
    helper->MonoThreadAttach(mono_domain);
    helper-> MonoSecuritySetMode(0);

    if(!is_native){        
        if(!helper->InjectAssembly(file_name,name_space,class_name,method_name,0)){
            CloseHandle(hProcess);
            return false;
        }
    }
    else {       
        void *SKM_address = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"RustProtect.Core"),(char*)"RustProtect",(char*)"Protection",(char*)"SendKickMessage",3,0),
             *DSP_address = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"RustProtect.Core"),(char*)"RustProtect",(char*)"Protection",(char*)"DoScanningPlayer",0,0),
             *SKM_method  = helper->MonoGetMethodFromName(helper->MonoClassFromName(helper->MonoImageLoaded((char*)"RustProtect.Core"),
                                                                                            (char*)"RustProtect",
                                                                                            (char*)"Protection"),
                                                                                            (char*)"SendKickMessage",3);

        int DSP_method_size = helper->GetCodeSize((char*)"RustProtect.Core",
                                                          (char*)"RustProtect",
                                                          (char*)"Protection",
                                                    (char*)"DoScanningPlayer",0,0);

        if(SKM_address != nullptr && DSP_address != nullptr && DSP_method_size != 0)
        {
            void* buffer = malloc(DSP_method_size);
            ZeroMemory(buffer,DSP_method_size);
            ReadProcessMemory(hProcess,DSP_address,buffer,DSP_method_size,NULL);

            for(int i = 0;i<DSP_method_size;i++)
            {

                if(((unsigned char*)buffer)[i] == 0xE8)
                {
                    unsigned long relative_address = *(unsigned long*)((unsigned long)buffer+i+1);
                    unsigned long absolute_address = ORIGINAL_ADDRESS(relative_address,(unsigned long)DSP_address+i);
                    unsigned long t_address = 0;

                    if(absolute_address == (unsigned long)SKM_address)
                    {
                         WriteProcessMemory(hProcess,(void*)((unsigned long)DSP_address+i),&rCall_nop,5,NULL);
                         continue;
                    }

                    DWORD oldProtect;
                    VirtualProtectEx(hProcess,(void*)absolute_address,5,PAGE_EXECUTE_READWRITE,&oldProtect);
                    ReadProcessMemory(hProcess,(void*)(absolute_address+1),&t_address,4,NULL);

                    if(t_address == (unsigned long)SKM_method)
                    {
                        WriteProcessMemory(hProcess,(void*)((unsigned long)DSP_address+i),&rCall_nop,5,NULL);
                    }
                }
            }
            free(buffer);
        }
        else
        {
            box.setText("Can not to find needed functions");
            box.exec();
        }
       if(!helper->InjectNative()){
            CloseHandle(hProcess);
            return false;
        }
    }


   WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_1),buffer,sizeof(buffer),NULL);
   WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_2),buffer2,sizeof(buffer2),NULL);
   WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_3),buffer3,sizeof(buffer3),NULL);
   CloseHandle(hProcess);
   return true;
}
