#ifndef RUSTPROTECT_H
#define RUSTPROTECT_H
#include "processhelper.h"
#include <QMessageBox>

#define ORIGINAL_ADDRESS(RELATIVE_ADDRESS,CALLING_ADDRESS) (RELATIVE_ADDRESS + CALLING_ADDRESS + 5)

#define mono_assembly_load_from_full_offset_1 0x161
#define mono_assembly_load_from_full_offset_2 0x169
#define mono_assembly_load_from_full_offset_3 0x175
#define mono_image_open_offset 0xCA

#define mono_assembly_load_from_full_count_nop1 5
#define mono_assembly_load_from_full_count_nop2 5
#define mono_assembly_load_from_full_count_nop3 5

bool rust_protect_bypass(p_helper* helper,char* file_name,char* name_space,char* class_name,char* method_name,bool is_native);


#endif // RUSTPROTECT_H
