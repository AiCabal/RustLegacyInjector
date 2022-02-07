#include "processhelper.h"
#include <QMessageBox>
#include <QFileInfo>

bool p_helper::Initialize(HMODULE hProcessHelper)
{
    is_initialize = false;
    this->m_hPHelper = hProcessHelper;
    if(this->m_hPHelper){


        connect_to_process_f = (connect_to_process)GetProcAddress(this->m_hPHelper,"SetProcess");
        if(!connect_to_process_f)
            return false;

        GetExportProcedure_f = (GetExportProcedure_t)GetProcAddress(this->m_hPHelper,"GetExportFunc");
        if(!GetExportProcedure_f)
            return false;

        mono_image_loaded_f = (mono_image_loaded)GetProcAddress(this->m_hPHelper,"MonoImageLoaded");
        if(!mono_image_loaded_f)
            return false;

        mono_get_root_domain_f = (mono_get_root_domain)GetProcAddress(this->m_hPHelper,"GetRootDomain");
        if(!mono_get_root_domain_f)
            return false;

        mono_assembly_load_from_full_f = (mono_assembly_load_from_full)GetProcAddress(this->m_hPHelper,"AssemblyLoadFromFull");
        if(!mono_assembly_load_from_full_f)
            return false;

        image_open_from_data_full_f = (image_open_from_data_full)GetProcAddress(this->m_hPHelper,"ImageOpenFromDataFull");
        if(!image_open_from_data_full_f)
            return false;

        mono_assembly_get_image_f = (assembly_get_image)GetProcAddress(this->m_hPHelper,"AssemblyGetImage");
        if(!mono_assembly_get_image_f)
            return false;

        mono_compile_method_f = (mono_compile_method)GetProcAddress(this->m_hPHelper,"MonoCompileMethod");
        if(!mono_compile_method_f)
            return false;

        mono_find_jit_info_table_f = (mono_find_jit_info_table)GetProcAddress(this->m_hPHelper,"MonoJitFindInfoTable");
        if(!mono_find_jit_info_table_f)
            return false;
        mono_jit_get_code_start_f = (get_code_start)GetProcAddress(this->m_hPHelper,"GetCodeStart");
        if(!mono_jit_get_code_start_f)
            return false;

        mono_thread_attach_f = (mono_thread_attach)GetProcAddress(this->m_hPHelper,"ThreadAttach");
        if(!mono_thread_attach_f)
            return false;

        mono_thread_detach_f = (mono_thread_detach)GetProcAddress(this->m_hPHelper,"ThreadDetach");
        if(!mono_thread_detach_f)
            return false;

        mono_get_method_from_name_f = (mono_get_method_from_name)GetProcAddress(this->m_hPHelper,"ClassGetMethodFromName");
        if(!mono_get_method_from_name_f)
            return false;       
        mono_class_from_name_f = (mono_class_from_name)GetProcAddress(this->m_hPHelper,"ClassFromName");
        if(!mono_class_from_name_f)
            return false;

        mono_runtime_invoke_f = (mono_runtime_invoke)GetProcAddress(this->m_hPHelper,"RuntimeInvoke");
        if(!mono_runtime_invoke_f)
            return false;

        mono_security_set_mode_f = (mono_security_set_mode)GetProcAddress(this->m_hPHelper,"SecuritySetMode");
        if(!mono_security_set_mode_f)
            return false;

        mono_assembly_inject_f = (mono_assembly_inject)GetProcAddress(this->m_hPHelper,"InjectAssembly");
        if(!mono_assembly_inject_f)
            return false;

        mono_jit_get_code_size_f = (get_code_size)GetProcAddress(this->m_hPHelper,"GetCodeSize");
        if(!mono_jit_get_code_size_f)
            return false;


        is_initialize = true;
        return true;
    }
    return false;
}
p_helper::p_helper(){    }
p_helper::~p_helper(){
}
bool p_helper::InjectAssembly(char* file_name,char* name_space,char* class_name,char* method_name,int countArgs)
{
    if(check_init() && check_connection())
    {
        void* image = mono_assembly_inject_f(file_name,name_space,class_name,method_name, countArgs);
        if(image){

            mono_images.insert({(QFileInfo(file_name).fileName()+QString("(")+QString::number(mono_images.size()+1)+QString(")")).toLocal8Bit().data(),image});
            return true;
        }
    }
    return false;
}

int p_helper::GetCodeSize(char* image_name, char* name_space, char* class_name, char* method_name,int countArgs,int offset)
{
    if(check_init() && check_connection())
    {
        return mono_jit_get_code_size_f(image_name,name_space,class_name,method_name, countArgs,offset);
    }
    return 0;
}

bool p_helper::InjectNative()
{
    void* mono_domain = MonoGetRootDomain();
    MonoThreadAttach(mono_domain);
    MonoSecuritySetMode(0);

    unsigned char rCall_nop[] = {0x90,0x90,0x90,0x90,0x90};
    if(mono_domain)
    {      
        void* raw_image = 0;
        raw_image = MonoImageOpenDataFromFull((char*)adolfrust_bytes,sizeof(adolfrust_bytes));
        if(raw_image)
        {

            void* assembly = MonoAssemblyLoadFromFull(raw_image);
            if(assembly)
            {

                void* image = MonoAssemblyGetImage(assembly);
                if(image)
                {

                    void* klass = MonoClassFromName(image,"AdolfRust","Loader");
                    if(klass)
                    {

                        void* method = MonoGetMethodFromName(klass,"Init",0);
                        if(method)
                        {

                            mono_images.insert({(QString("AdolfRust.dll")+QString("(")+QString::number(mono_images.size()+1)+QString(")")).toLocal8Bit().data(),image});
                            MonoRuntimeInvoke(method);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
bool p_helper::ConnectToProcess()
{
    if(!check_init())
        return false;

    this->is_connecting = connect_to_process_f();
    return this->is_connecting;
}
void* p_helper::MonoImageOpenDataFromFull(char* file_name)
{
     if(check_init() && check_connection())
         return mono_image_open_from_data_ful_f(file_name);
     else return 0;

}
void* p_helper::GetExportProcedure(char* module_name, char* procedure_name)
{
    if(check_connection())
        return GetExportProcedure_f(module_name,procedure_name);
    else return 0;
}
void* p_helper::MonoImageLoaded(char* image_name)
{
    if(check_init() && check_connection())
        return mono_image_loaded_f(image_name);
    else return 0;
}
void* p_helper::MonoGetRootDomain()
{
    if(check_init() && check_connection())
        return mono_get_root_domain_f();
    else return 0;
}
void* p_helper::MonoAssemblyLoadFromFull(void* image)
{
    if(check_init() && check_connection()){
        void* assembly =  mono_assembly_load_from_full_f(image);
            return assembly;
    }
    return 0;
}
void* p_helper::MonoImageOpenDataFromFull(char* data, size_t length)
{
    if(check_init() && check_connection())
        return  image_open_from_data_full_f(data,length);
    else return 0;
}
void* p_helper::MonoAssemblyGetImage(void* assembly)
{
    if(check_init() && check_connection()){
       void* image = mono_assembly_get_image_f(assembly);
           return image;
    }
    else return 0;
}
void* p_helper::MonoCompileMethod(void* class_method)
{
    if(check_init() && check_connection())
        return mono_compile_method_f(class_method);
    else return 0;
}
void* p_helper::MonoFindJitInfoTable(void* domain,void* compile_method)
{
    if(check_init() && check_connection())
        return mono_find_jit_info_table_f(domain,compile_method);
    else return 0;
}
void* p_helper::MonoJitGetCodeStart(void* image, char* name_space, char* class_name, char* method_name,int countArgs,int offset)
{
    if(check_init() && check_connection())
        return mono_jit_get_code_start_f(image,name_space,class_name,method_name, countArgs,offset);
    else return 0;
}
void* p_helper::MonoThreadAttach(void* domain)
{
    if(check_init() && check_connection())
        mono_thread_attach_f(domain);
    else return 0;
}
void* p_helper::MonoThreadDetach(void* domain)
{
    if(check_init() && check_connection())
        mono_thread_detach_f(domain);
    else return 0;
}
void* p_helper::MonoGetMethodFromName(void* klass,const char* method_name,int countArgs)
{
    if(check_init() && check_connection())
        return mono_get_method_from_name_f(klass,method_name,countArgs);
    else return 0;
}
void* p_helper::MonoClassFromName(void* image, const char* name_space, const char* class_name)
{
    if(check_init() && check_connection())
        return mono_class_from_name_f(image,name_space,class_name);
    else return 0;
}
void* p_helper::MonoRuntimeInvoke(void* method)
{
    if(check_init() && check_connection())
        mono_runtime_invoke_f(method);
    else return 0;
}
void p_helper::MonoSecuritySetMode(int mode)
{
    if(check_init() && check_connection())
        mono_security_set_mode_f(mode);
}
