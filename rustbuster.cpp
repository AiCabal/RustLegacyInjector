#include "rustbuster.h"
#include <QMessageBox>

bool rust_buster_bypass(p_helper* helper,char* file_name,char* name_space,char* class_name,char* method_name,bool is_native)
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


    QMessageBox box;
    void* mono_domain = helper->MonoGetRootDomain();
    helper->MonoThreadAttach(mono_domain);
    helper-> MonoSecuritySetMode(0);

    unsigned char ret  = 0xC3;
    unsigned char rtvm1_original_byte,rtvm2_original_byte;

    void* rtvm1 = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"RustBuster2016"),(char*)"RustBuster2016",(char*)"Hooks",(char*)"RTVM",2,0);
    void* rtvm2 = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"RustBuster2016"),(char*)"RustBuster2016",(char*)"Hooks",(char*)"RTVM2",1,0);
    void* add_component = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"UnityEngine"),(char*)"UnityEngine",(char*)"GameObject",(char*)"AddComponent",1,0x38);
    void* add_component_internal = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"UnityEngine"),(char*)"UnityEngine",(char*)"GameObject",(char*)"Internal_AddComponentWithType",1,0);
    void* start_coroutine = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"UnityEngine"),(char*)"UnityEngine",(char*)"MonoBehaviour",(char*)"StartCoroutine",1,0);
    void* start_coroutine_auto = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"UnityEngine"),(char*)"UnityEngine",(char*)"MonoBehaviour",(char*)"StartCoroutine_Auto",1,0);
    void* start_coroutine_hook = helper->MonoJitGetCodeStart(helper->MonoImageLoaded((char*)"RustBuster2016"),(char*)"RustBuster2016",(char*)"Hooks",(char*)"SCRTHandle2",2,0);

    int add_component_size = helper->GetCodeSize((char*)"UnityEngine",
                                                      (char*)"UnityEngine",
                                                      (char*)"GameObject",
                                                (char*)"AddComponent",1,0x38);


    int start_coroutine_size = helper->GetCodeSize((char*)"UnityEngine",
                                                   (char*)"UnityEngine",
                                                   (char*)"MonoBehaviour",
                                             (char*)"StartCoroutine",1,0);

    if(!rtvm1 || !rtvm2 || !add_component || !add_component_internal || !add_component_size ||!start_coroutine ||!start_coroutine_size ||!start_coroutine_hook || !start_coroutine_auto){
        box.setText("Can not find needed functions!");
        box.exec();
        return false;
    }

    ReadProcessMemory(hProcess,rtvm1,&rtvm1_original_byte,sizeof(unsigned char),0);
    ReadProcessMemory(hProcess,rtvm2,&rtvm2_original_byte,sizeof(unsigned char),0);

    WriteProcessMemory(hProcess,rtvm1,&ret,sizeof(unsigned char),0);
    WriteProcessMemory(hProcess,rtvm2,&ret,sizeof(unsigned char),0);

    void* buffer_1 = malloc(add_component_size);
    ZeroMemory(buffer_1,add_component_size);
    ReadProcessMemory(hProcess,add_component,buffer_1,add_component_size,NULL);

    void* buffer_2 = malloc(start_coroutine_size);
    ZeroMemory(buffer_2,start_coroutine_size);
    ReadProcessMemory(hProcess,start_coroutine,buffer_2,start_coroutine_size,NULL);

    for(int i = 0;i<start_coroutine_size;i++)
    {

        if(((unsigned char*)buffer_2)[i] == 0xE8)
        {
            unsigned long relative_address = *(unsigned long*)((unsigned long)buffer_2+i+1);
            unsigned long absolute_address = ORIGINAL_ADDRESS(relative_address,(unsigned long)start_coroutine+i);
            unsigned long t_address = 0;
            if(absolute_address == (unsigned long)start_coroutine_hook)
            {
                 unsigned long hook_func_relative = (unsigned long)start_coroutine_auto - ((unsigned long)start_coroutine+i) - 5;
                 WriteProcessMemory(hProcess,(void*)((unsigned long)start_coroutine+i+1),&hook_func_relative,sizeof(void*),NULL);
                 continue;
            }
        }
    }

    for(int i = 0;i<add_component_size;i++)
    {

        if(((unsigned char*)buffer_1)[i] == 0xE8)
        {
            unsigned long relative_address = *(unsigned long*)((unsigned long)buffer_1+i+1);
            unsigned long absolute_address = ORIGINAL_ADDRESS(relative_address,(unsigned long)add_component+i);
            unsigned long t_address = 0;

            if(absolute_address == (unsigned long)rtvm1)
            {

                 unsigned long hook_func_relative = (unsigned long)add_component_internal - ((unsigned long)add_component+i) - 5;
                 WriteProcessMemory(hProcess,(void*)((unsigned long)add_component+i+1),&hook_func_relative,sizeof(void*),NULL);
                 continue;
            }
        }
    }

    QMessageBox box2;
    box2.exec();
    free(buffer_1);
    free(buffer_2);
    if(!is_native){
        if(!helper->InjectAssembly(file_name,name_space,class_name,method_name,0)){
            CloseHandle(hProcess);
            return false;
        }
    }
    else {
       if(!helper->InjectNative()){
            CloseHandle(hProcess);
            return false;
        }
    }


   WriteProcessMemory(hProcess,rtvm1,&rtvm1_original_byte,sizeof(unsigned char),0);
   WriteProcessMemory(hProcess,rtvm2,&rtvm2_original_byte,sizeof(unsigned char),0);

   WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_1),buffer,sizeof(buffer),NULL);
   WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_2),buffer2,sizeof(buffer2),NULL);
   WriteProcessMemory(hProcess,(void*)((uptr_t)_load_from_full_address+mono_assembly_load_from_full_offset_3),buffer3,sizeof(buffer3),NULL);

   CloseHandle(hProcess);
   return true;



}
