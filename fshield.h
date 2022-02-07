#ifndef FSHIELD_H
#define FSHIELD_H
#include "processhelper.h"
#include "rustprotect.h"
bool FShieldBypass(p_helper* helper,char* file_name,char* name_space,char* class_name,char* method_name,bool is_native);


#endif // FSHIELD_H
