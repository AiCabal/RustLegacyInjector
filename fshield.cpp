#include "fshield.h"
#include <QMessageBox>
bool FShieldBypass(p_helper* helper,char* file_name,char* name_space,char* class_name,char* method_name,bool is_native)
{
    QMessageBox box;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,0,GetProcessID((wchar_t*)L"rust.exe"));
    helper->ConnectToProcess();

    void* mono_assembly_load_from_full = helper->GetExportProcedure((char*)"mono.dll",(char*)"mono_assembly_load_from_full");
    void* _mono_image_open_address = helper->GetExportProcedure((char*)"mono.dll",(char*)"mono_image_open_from_data_with_name");

    unsigned char nop = 0x90;
    unsigned char jmp_18[] = {0xEB,0x18};

    unsigned char buffer[mono_assembly_load_from_full_count_nop1] ,
                  buffer2[mono_assembly_load_from_full_count_nop2],
                  buffer3[mono_assembly_load_from_full_count_nop3];

    if(!hProcess || !mono_assembly_load_from_full || !_mono_image_open_address){
        return false;        
    }


    asmjit::JitRuntime runtime;
    asmjit::X86Assembler assembler(&runtime, asmjit::kArchHost);

    assembler.push(asmjit::host::ebp);
    assembler.mov(asmjit::host::ebp,asmjit::host::esp);
    assembler.push(asmjit::host::ecx);
    assembler.push(asmjit::host::edi);
    assembler.mov(asmjit::host::edi,asmjit::x86::ptr(asmjit::host::ebp,0x08));

    void* code = assembler.make();
    size_t code_size = assembler.getCodeSize();

    char* buffer4 = (char*)malloc(code_size);

    ReadProcessMemory(hProcess,mono_assembly_load_from_full,buffer4,code_size,NULL);
    ReadProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_1),(void*)buffer,sizeof(buffer),NULL);
    ReadProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_2),(void*)buffer2,sizeof(buffer2),NULL);
    ReadProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_3),(void*)buffer3,sizeof(buffer3),NULL);


    WriteProcessMemory(hProcess,mono_assembly_load_from_full,code,code_size,nullptr);
    for(int i = 0;i<mono_assembly_load_from_full_count_nop1;i++)
    {
        WriteProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_1+i),&nop,sizeof(char),NULL);
    }

    for(int i = 0;i<mono_assembly_load_from_full_count_nop2;i++)
    {
        WriteProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_2+i),&nop,sizeof(char),NULL);
    }

    for(int i = 0;i<mono_assembly_load_from_full_count_nop3;i++)
    {
        WriteProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_3+i),&nop,sizeof(char),NULL);
    }
    WriteProcessMemory(hProcess,(void*)((uptr_t)_mono_image_open_address+mono_image_open_offset),&jmp_18,sizeof(jmp_18),NULL);





    bool ret_flag = false;

    if(!is_native){
        ret_flag = helper->InjectAssembly(file_name,name_space,class_name,method_name,0);
    }
    else {
        ret_flag = helper->InjectNative();
    }

    WriteProcessMemory(hProcess,mono_assembly_load_from_full,buffer4,code_size,NULL);
    WriteProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_1),buffer,sizeof(buffer),NULL);
    WriteProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_2),buffer2,sizeof(buffer2),NULL);
    WriteProcessMemory(hProcess,(void*)((uptr_t)mono_assembly_load_from_full+mono_assembly_load_from_full_offset_3),buffer3,sizeof(buffer3),NULL);

    free(buffer4);
    runtime.release(code);
    CloseHandle(hProcess);
    return ret_flag;
}
