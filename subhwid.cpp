#include "subhwid.h"
#include <QMessageBox>						 						 
#include <fstream>

bool FileExists(LPCTSTR fname)
{
    return::GetFileAttributes(fname) != DWORD(-1);
}
unsigned long file_readallbytes(const wchar_t *filename, char *&buffer)
{
    unsigned long length = 0;

    if (filename)
    {
        HANDLE handle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if (handle != INVALID_HANDLE_VALUE)
        {
            unsigned long fileSize = GetFileSize(handle, NULL);

            if (fileSize != INVALID_FILE_SIZE)
            {
                buffer = new char[fileSize];
                ReadFile(handle, buffer, fileSize, &length, NULL);
            }

            CloseHandle(handle);
        }
    }
    return length;
}
void ActivateHWIDChanger(p_helper* helper)
{
        QMessageBox err;

        if(helper->check_init() == false)
        {
            err.setText("Can not to initialize needed library");
            err.exec();
            return;
        }
        if(helper->ConnectToProcess() == false)
        {
            err.setText("Can not to get access to process");
            err.exec();
            return;
        }

        const DWORD encodedVersion = ::GetVersion();
        const unsigned majorVersion = unsigned(LOBYTE(LOWORD(encodedVersion)));
        const unsigned minorVersion = unsigned(HIBYTE(LOWORD(encodedVersion)));

        bool is_windows10 = false;

        if ((majorVersion == 6 && minorVersion == 2) || majorVersion == 10) {
            is_windows10 = true;
        }
        else if (majorVersion == 6 && minorVersion == 1) {
            is_windows10 = false;
        }
        else{
            err.setText("Could not determine your version of windows");
            err.exec();
            return;
        }

        uint32_t enum_firmware_offset = is_windows10 ? WINTEN_ENUMSYSTEM_OFFSET : WINSEVEN_ENUMSYSTEM_OFFSET;
        uint32_t get_system_firmware_offset = is_windows10 ? WINTEN_GETSYSTEM_OFFSET : WINSEVEN_GETSYSTEM_OFFSET;

        void* random_func_address = helper->GetExportProcedure((char*)"msvcrt.dll", (char*)"rand"),
            *srand_function_addres = helper->GetExportProcedure((char*)"msvcrt.dll",(char*) "srand"),
            *time_function_address = helper->GetExportProcedure((char*)"msvcrt.dll", (char*)"time"),
            *rtl_copy_memory_address = helper->GetExportProcedure((char*)"ntdll.dll", (char*)"memcpy"),
            *get_system_firmware_table_address = helper->GetExportProcedure(is_windows10 ? (char*)"KERNELBASE.dll" : (char*)"kernel32.dll", (char*)"GetSystemFirmwareTable"),
            *enum_system_firmware_tables_address = helper->GetExportProcedure(is_windows10 ? (char*)"KERNELBASE.dll" : (char*)"kernel32.dll",(char*) "EnumSystemFirmwareTables");

        if (!random_func_address ||
            !srand_function_addres ||
            !time_function_address ||
            !rtl_copy_memory_address ||
            !get_system_firmware_table_address ||
            !enum_system_firmware_tables_address
            )
        {
            err.setText("Could not find required functions");
            err.exec();
            return;
        }
            asmjit::JitRuntime runtime;
            asmjit::X86Assembler assembler(&runtime, asmjit::kArchHost);
            /*asmjit::Label exit_for = assembler.newLabel();
            asmjit::Label cycle_label = assembler.newLabel();

            assembler.push(asmjit::host::ebx);
            assembler.push(asmjit::host::esi);
            assembler.push(asmjit::host::edi);//save registers

            assembler.push(0x00);
            assembler.mov(asmjit::host::eax, asmjit::imm_ptr(time_function_address));
            assembler.call(asmjit::host::eax);//call time(0);

            assembler.push(asmjit::host::eax);
            assembler.mov(asmjit::host::eax, asmjit::imm_ptr(srand_function_addres));
            assembler.call(asmjit::host::eax);
            assembler.add(asmjit::host::esp, 0x08);//call srand(time(0));

            assembler.mov(asmjit::host::esi,asmjit::imm_ptr(random_func_address));
            assembler.xor_(asmjit::host::ebx, asmjit::host::ebx);
            assembler.call(asmjit::host::esi);//call rand();

            assembler.mov(asmjit::host::edi, asmjit::x86::ptr(asmjit::host::esp, 0x14));//Get address of sources bytes
            assembler.add(asmjit::host::eax, 0x01);
            assembler.test(asmjit::host::eax, asmjit::host::eax);//if rand return zero
            assembler.jle(exit_for);//go to exit
            assembler.push(asmjit::host::ebp);//save ebp register
            assembler.mov(asmjit::host::ebp, asmjit::x86::ptr(asmjit::host::esp, 0x1C));//move to ebp Length - 1 of sources bytes
            assembler.dec(asmjit::host::ebp);
            assembler.nop();
            assembler.bind(cycle_label);
            assembler.call(asmjit::host::esi);//call rand();
            assembler.xor_(asmjit::host::edx, asmjit::host::edx);//move to edx 0
            assembler.div(asmjit::host::ebp);//get the remainder of the division
            assembler.mov(asmjit::host::eax, asmjit::imm_ptr(random_func_address));
            assembler.mov(asmjit::host::esi, asmjit::host::edx);//swap random bytes
            assembler.call(asmjit::host::eax);
            assembler.xor_(asmjit::host::edx, asmjit::host::edx);
            assembler.mov(asmjit::host::cl, asmjit::x86::ptr(asmjit::host::esi, asmjit::host::edi));
            assembler.div(asmjit::host::ebp);
            assembler.inc(asmjit::host::ebx);
            assembler.mov(asmjit::host::al, asmjit::x86::ptr(asmjit::host::edx, asmjit::host::edi));
            assembler.mov(asmjit::x86::ptr(asmjit::host::esi, asmjit::host::edi), asmjit::host::al);
            assembler.mov(asmjit::host::esi, asmjit::imm_ptr(random_func_address));
            assembler.mov(asmjit::x86::ptr(asmjit::host::edx, asmjit::host::edi), asmjit::host::cl);
            assembler.call(asmjit::host::esi);
            assembler.cmp(asmjit::host::ebx, asmjit::host::eax);
            assembler.jl(cycle_label);
            assembler.pop(asmjit::host::ebp);

            assembler.bind(exit_for);
            assembler.push(asmjit::x86::ptr(asmjit::host::esp, 0x18));
            assembler.mov(asmjit::host::eax,asmjit::imm_ptr(rtl_copy_memory_address));
            assembler.push(asmjit::host::edi);
            assembler.push(asmjit::x86::ptr(asmjit::host::esp, 0x18));
            assembler.call(asmjit::host::eax);
            assembler.add(asmjit::host::esp, 0x0C);//call RtlCopyMemory(Destination,Source,Length);

            assembler.pop(asmjit::host::edi);
            assembler.pop(asmjit::host::esi);
            assembler.pop(asmjit::host::ebx);
            assembler.ret();*/

            srand(time(NULL));
            void* outBuffer = nullptr;
            char* inBuffer = nullptr;
            size_t dat_size = 0;
            if (EnumSystemFirmwareTables(RSMB, NULL, 0) > 0)
            {
                UINT bufferLength = GetSystemFirmwareTable(RSMB, 0x0000, NULL, 0);
                //wcout << "BIOS 'RSMB' Table Length: " << length << endl;

                if (bufferLength > 0)
                {

                    bufferLength = GetSystemFirmwareTable(RSMB, 0, outBuffer, bufferLength);
                    if(!FileExists(L"save_hwid.bin"))
                    {
                        outBuffer = malloc(bufferLength);
                        for(unsigned int i = 0;i< rand() % bufferLength;i++)
                        {
                            ((char*)outBuffer)[rand() % bufferLength-1] = ((char*)outBuffer)[(rand() % bufferLength-1)];
                        }
                        std::ofstream of;
                        of.open("save_hwid.bin",std::ios::binary);
                        of.write((char*)outBuffer,bufferLength);
                        of.close();
                    }
                        dat_size = file_readallbytes(L"save_hwid.bin",inBuffer);
                        if(dat_size <= 0 || inBuffer == nullptr){
                            err.setText("Can not to get access to save hwid file");
                            err.exec();
                            return;
                        }
                }
                else
                {
                    err.setText("FAILED!");
                    err.exec();
                }
            }
            else {
                err.setText("FAILED!");
                err.exec();
            }

            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,0,GetProcessID((wchar_t*)L"rust.exe"));

            if(hProcess == NULL)
            {
                err.setText("Can not to get access to process");
                err.exec();
               return;
            }

            void* alloc_buf_data = VirtualAllocEx(hProcess,NULL,dat_size,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
            WriteProcessMemory(hProcess,alloc_buf_data,inBuffer,dat_size,NULL);


            assembler.push(asmjit::host::ebp);//save ebp register
            assembler.mov(asmjit::host::ebp,asmjit::host::esp);
            assembler.push(asmjit::host::ptr(asmjit::host::ebp,0x10));
            assembler.push(asmjit::imm_ptr(alloc_buf_data));
            assembler.push(asmjit::host::ptr(asmjit::host::ebp,0x08));
            assembler.mov(asmjit::host::eax,asmjit::imm_ptr(rtl_copy_memory_address));
            assembler.call(asmjit::host::eax);
            assembler.add(asmjit::host::esp, 0x0C);
            assembler.pop(asmjit::host::ebp);
            assembler.ret();

            void* remote_func = assembler.make();
            size_t code_size = assembler.getCodeSize();

            uptr_t remote_func_address = (uptr_t)VirtualAllocEx(hProcess,0,code_size,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);



            if (remote_func_address != NULL) {


                WriteProcessMemory(hProcess,(void*)remote_func_address,remote_func,code_size,NULL);

                runtime.release(remote_func);


                unsigned char call_remote_func[] = { 0xE8,0,0,0,0 };

                uptr_t replacing_address = (uptr_t)get_system_firmware_table_address + get_system_firmware_offset;
                uptr_t relative_address = (uptr_t)remote_func_address - (replacing_address + sizeof(call_remote_func));
                *(uptr_t*)&call_remote_func[1] = relative_address;

                VirtualProtectEx(hProcess,(void*)replacing_address, sizeof(call_remote_func), PAGE_EXECUTE_READWRITE,NULL);
                WriteProcessMemory(hProcess,(void*)replacing_address, call_remote_func,sizeof(call_remote_func),NULL);

                replacing_address = (uptr_t)enum_system_firmware_tables_address + enum_firmware_offset;
                relative_address = (uptr_t)remote_func_address - (replacing_address + sizeof(call_remote_func));
                *(uptr_t*)&call_remote_func[1] = relative_address;

                VirtualProtectEx(hProcess,(void*)replacing_address, sizeof(call_remote_func), PAGE_EXECUTE_READWRITE,NULL);
                WriteProcessMemory(hProcess,(void*)replacing_address, call_remote_func,sizeof(call_remote_func),NULL);

            }
            runtime.release(remote_func);

    err.setText("Success!");
    err.exec();
    return;
}
