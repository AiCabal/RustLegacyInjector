#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <Windows.h>
#include <unordered_map>
#include <stdint.h>
#include "AsmJit/AsmJit.h"
#include "adolfrust_bytes.h"
#include "utils.h"
typedef unsigned long uptr_t;

//--------------------------ProcessHelper_functions-------------------------------------------------------------
typedef  bool (*connect_to_process)();
typedef  void*  (*GetExportProcedure_t)(char* module_name, char* procedure_name);
typedef  void*  (*mono_image_loaded)(char* name);
typedef  void*  (*mono_get_root_domain)();
typedef  void*  (*mono_assembly_load_from_full)(void* image);
typedef  void*  (*image_open_from_data_full)(char* data, size_t length);
typedef  void*  (*assembly_get_image)(void* assembly);
typedef  void*  (*mono_compile_method)(void* class_method);
typedef  void*  (*mono_find_jit_info_table)(void* domain,void* compile_method);
typedef  void*  (*mono_jit_get_code_start)(void* jitinfo,int countArgs);
typedef  void*  (*mono_thread_attach)(void* domain);
typedef  void*  (*mono_thread_detach)(void* domain);
typedef  void*  (*mono_get_method_from_name)(void* klass,const char* method_name,int countArgs);
typedef  void*  (*mono_class_from_name)(void* image, const char* name_space, const char* class_name);
typedef  void*  (*mono_runtime_invoke)(void* method);
typedef  void   (*mono_security_set_mode)(int mode);
typedef  void*  (*image_open_from_data_ful)(char* file_name);
typedef  void*  (*mono_assembly_inject)(char* file_name, char*name_space, char* class_name, char* method_name,int countArgs);
typedef  size_t (*mono_jit_get_code_size)(void* jitinfo);
typedef  void*  (*get_code_start)(void* image, char* name_space, char* class_name, char* method_name,int countArgs,int offset);
typedef  int    (*get_code_size)(char* image_name, char* name_space, char* class_name, char* method_name,int countArgs,int offset);
//---------------------------------------------------------------------------------------------------------------



class p_helper
{
    public:

    p_helper();
    ~p_helper();

    bool Initialize(HMODULE hProcessHelper);

    bool ConnectToProcess();

    void* GetExportProcedure(char* module_name, char* procedure_name);
    void* MonoImageLoaded(char* image_name);
    void* MonoGetRootDomain();
    void* MonoAssemblyLoadFromFull(void* image);
    void* MonoImageOpenDataFromFull(char* data, size_t length);
    void* MonoImageOpenDataFromFull(char* file_name);
    void* MonoAssemblyGetImage(void* assembly);
    void* MonoCompileMethod(void* class_method);
    void* MonoFindJitInfoTable(void* domain,void* compile_method);
    void* MonoJitGetCodeStart(void* image, char* name_space, char* class_name, char* method_name,int countArgs,int offset);
    void* MonoThreadAttach(void* domain);
    void* MonoThreadDetach(void* domain);
    void* MonoGetMethodFromName(void* klass,const char* method_name,int countArgs);
    void* MonoClassFromName(void* image, const char* name_space, const char* class_name);
    void* MonoRuntimeInvoke(void* method);
    int GetCodeSize(char* image_name, char* name_space, char* class_name, char* method_name,int countArgs,int offset);
    bool InjectAssembly(char* file_name,char* name_space,char* class_name,char* method_name,int countArgs);
    bool InjectNative();
    void MonoSecuritySetMode(int mode);
    bool check_connection(){return is_connecting;}
    bool check_init(){return is_initialize;}
    std::unordered_map<std::string,void*>* get_images() { return &this->mono_images; }

    private:
    HMODULE m_hPHelper;
    bool is_connecting = false;
    bool is_initialize = false;
    std::unordered_map<std::string,void*> mono_images;

    connect_to_process connect_to_process_f;
    GetExportProcedure_t GetExportProcedure_f;
    mono_image_loaded mono_image_loaded_f;
    mono_get_root_domain mono_get_root_domain_f;
    mono_assembly_load_from_full mono_assembly_load_from_full_f;
    image_open_from_data_full image_open_from_data_full_f;
    assembly_get_image mono_assembly_get_image_f;
    mono_compile_method mono_compile_method_f;
    mono_find_jit_info_table mono_find_jit_info_table_f;
    get_code_start mono_jit_get_code_start_f;
    mono_thread_attach mono_thread_attach_f;
    mono_thread_detach mono_thread_detach_f;
    mono_get_method_from_name mono_get_method_from_name_f;
    mono_class_from_name mono_class_from_name_f;
    mono_runtime_invoke mono_runtime_invoke_f;
    mono_security_set_mode mono_security_set_mode_f;
    image_open_from_data_ful mono_image_open_from_data_ful_f;
    mono_assembly_inject mono_assembly_inject_f;
    get_code_size mono_jit_get_code_size_f;
};


#endif // PROCESSHELPER_H
