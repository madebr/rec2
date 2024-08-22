#include "drmem.h"

#include "errors.h"
#include "loading.h"

#include "rec2_macros.h"
#include "rec2_types.h"

#include "brender/brender.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_allocator, gAllocator, 0x005937e8, { "Death Race", C2V(DRStdlibAllocate), C2V(DRStdlibFree), C2V(DRStdlibInquire), C2V(Claim4ByteAlignment) });
C2_HOOK_VARIABLE_IMPLEMENT(int, gNon_fatal_allocation_errors, 0x00681fa4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gMem_names, 256, 0x005933e8, {
    "",
    "BR_MEMORY_SCRATCH",
    "BR_MEMORY_PIXELMAP",
    "BR_MEMORY_PIXELS",
    "BR_MEMORY_VERTICES",
    "BR_MEMORY_FACES",
    "BR_MEMORY_GROUPS",
    "BR_MEMORY_MODEL",
    "BR_MEMORY_MATERIAL",
    "BR_MEMORY_MATERIAL_INDEX",
    "BR_MEMORY_ACTOR",
    "BR_MEMORY_PREPARED_VERTICES",
    "BR_MEMORY_PREPARED_FACES",
    "BR_MEMORY_LIGHT",
    "BR_MEMORY_CAMERA",
    "BR_MEMORY_BOUNDS",
    "BR_MEMORY_CLIP_PLANE",
    "BR_MEMORY_STRING",
    "BR_MEMORY_REGISTRY",
    "BR_MEMORY_TRANSFORM",
    "BR_MEMORY_RESOURCE_CLASS",
    "BR_MEMORY_FILE",
    "BR_MEMORY_ANCHOR",
    "BR_MEMORY_POOL",
    "BR_MEMORY_RENDER_MATERIAL",
    "BR_MEMORY_DATAFILE",
    "BR_MEMORY_IMAGE",
    "BR_MEMORY_IMAGE_ARENA",
    "BR_MEMORY_IMAGE_SECTIONS",
    "BR_MEMORY_IMAGE_NAMES",
    "BR_MEMORY_EXCEPTION_HANDLER",
    "BR_MEMORY_RENDER_DATA",
    "BR_MEMORY_TOKEN",
    "BR_MEMORY_TOKEN_MAP",
    "BR_MEMORY_OBJECT",
    "BR_MEMORY_OBJECT_DATA",
    "BR_MEMORY_DRIVER",
    "BR_MEMORY_LEXER",
    "BR_MEMORY_OBJECT_LIST",
    "BR_MEMORY_OBJECT_LIST_ENTRY",
    "BR_MEMORY_ENABLED_ACTORS",
    "BR_MEMORY_FMT_RESULTS",
    "BR_MEMORY_PREPARED_MODEL",
    "BR_MEMORY_ORDER_TABLE",
    "BR_MEMORY_TOKEN_VALUE",
    "BR_MEMORY_TOKEN_TEMPLATE",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "kBoner_mem_type_remap",
    "kBoner_mem_type_sequence",
    "kMem_nodes_array",
    "kMem_sections_array",
    "kBoner_mem_type_frame_2",
    "kMem_columns_z",
    "kMem_columns_x",
    "kMem_non_car_list",
    "kMem_simp_level",
    "kMem_crush_data",
    "kMem_drone_collision_stuff",
    "kBoner_mem_type_form",
    "kBoner_mem_type_actor_sets",
    "kBoner_mem_type_simple_phys",
    "kBoner_mem_type_boned_phys",
    "kBoner_mem_type_bone_info",
    "kBoner_mem_type_boned_coll",
    "kBoner_mem_type_boned_coll_obj",
    "kBoner_mem_type_simple_coll_obj",
    "kBoner_mem_type_actor_ptrs",
    "kBoner_mem_type_moves",
    "kBoner_mem_type_stored_ptrs",
    "kBoner_mem_type_stored",
    "kBoner_mem_type_person",
    "kBoner_mem_type_person_parts",
    "kBoner_mem_type_person_moves",
    "kBoner_mem_type_char_instance",
    "kMem_boner_type_frames",
    "kMem_smash_levels",
    "kMem_smash_side_effects",
    "kMem_damage_clauses",
    "kMem_undamaged_vertices",
    "kMem_race_text_chunk",
    "kMem_race_text_str",
    "kMem_oppo_array",
    "kMem_oppo_text_chunk",
    "kMem_oppo_text_str",
    "kMem_br_font",
    "kMem_br_font_wid",
    "kMem_br_font_enc",
    "kMem_br_font_glyphs",
    "kMem_oppo_car_spec",
    "kMem_misc_string",
    "kMem_mac_host_buffer_1",
    "kMem_mac_host_buffer_2",
    "kMem_mac_net_details",
    "kMem_back_pixels",
    "kMem_quit_vfy_pixels",
    "kMem_quit_vfy_pal",
    "kMem_net_min_messages",
    "kMem_net_mid_messages",
    "kMem_net_max_messages",
    "kMem_net_pid_details",
    "kMem_net_car_spec",
    "kMem_dynamic_message",
    "kMem_player_list_join",
    "kMem_player_list_leave",
    "kMem_oppo_new_nodes",
    "kMem_oppo_new_sections",
    "kMem_drone_model_dup",
    "kMem_oppo_bit_per_node",
    "kMem_smash_poly_array",
    "kMem_smash_vertex_array",
    "kMem_ped_array_stain",
    "kMem_ped_array",
    "kMem_ped_instructions",
    "kMem_ped_new_instruc",
    "kMem_pipe_model_geometry",
    "kMem_powerup_array",
    "kMem_powerup_parms",
    "kMem_noisey_parts",
    "kMem_sound_generator",
    "kMem_pratcam_flic_array",
    "kMem_pratcam_flic_data",
    "kMem_pratcam_sequence_array",
    "kMem_pratcam_pixelmap",
    "kMem_video_pixels",
    "kMem_funk_prox_array",
    "kMem_new_mat_id",
    "kMem_packed_file",
    "kMem_stored_smash",
    "kMem_special_volume",
    "kMem_special_screen",
    "kMem_new_special_vol",
    "kMem_saved_game",
    "kMem_new_save_game",
    "kMem_stor_space_table",
    "kMem_boner",
    "kMem_ped_face_cache",
    "kMem_misc_poly_ped",
    "kMem_physics_joint",
    "kMem_funk_spec",
    "kMem_groove_spec",
    "kMem_non_car_spec",
    "kMem_S3_scan_name",
    "kMem_S3_sound_header",
    "kMem_S3_sample",
    "kMem_S3_mac_channel",
    "kMem_S3_mac_path",
    "kMem_S3_sentinel",
    "kMem_S3_outlet",
    "kMem_S3_channel",
    "kMem_S3_descriptor",
    "kMem_S3_reverse_buffer",
    "kMem_S3_source",
    "kMem_drone_specs",
    "kMem_drone_paths",
    "kMem_S3_DOS_SOS_patch",
    "kMem_S3_DOS_SOS_song_structure",
    "kMem_S3_DOS_SOS_song_data",
    "kMem_S3_Windows_95_load_WAV_file",
    "kMem_S3_Windows_95_create_temp_buffer_space_to_reverse_sample",
    "kMem_S3_Windows_95_path",
    "kMem_drone_funk_groove",
    "kMem_abuse_text",
    "kMem_action_replay_buffer",
    "kMem_master_crush_data",
    "kMem_collision_object",
    "kMem_collision_shape",
    "kMem_smashable_info",
    "kMem_smashable_env_info",
    "kMem_detached_bit_non_car_spec",
    "kMem_cloned_block",
    "kMem_explosion_data",
    "kMem_misc",
    "kMem_exception",
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_resource_class, gStainless_classes, 126, 0x006815c8);
C2_HOOK_VARIABLE_IMPLEMENT(FILE*, gGlobalPackedFile, 0x00681fa0);

void C2_HOOK_FASTCALL SetNonFatalAllocationErrors(void) {

    C2V(gNon_fatal_allocation_errors) = 1;
}
C2_HOOK_FUNCTION(0x0044c7f0, SetNonFatalAllocationErrors)

void C2_HOOK_FASTCALL ResetNonFatalAllocationErrors(void) {

    C2V(gNon_fatal_allocation_errors) = 0;
}
C2_HOOK_FUNCTION(0x0044c800, ResetNonFatalAllocationErrors)

int C2_HOOK_FASTCALL AllocationErrorsAreFatal(void) {

    return !C2V(gNon_fatal_allocation_errors);
}
C2_HOOK_FUNCTION(0x0044c810, AllocationErrorsAreFatal)

void C2_HOOK_FASTCALL MAMSInitMem(void) {
}
C2_HOOK_FUNCTION(0x0044c820, MAMSInitMem)

void (C2_HOOK_FASTCALL * CloseGlobalPackedFile_original)(void);
void C2_HOOK_FASTCALL CloseGlobalPackedFile(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    CloseGlobalPackedFile_original();
#else

    if (C2V(gGlobalPackedFile) != NULL) {
        PFfclose(C2V(gGlobalPackedFile));
        C2V(gGlobalPackedFile) = NULL;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044c830, CloseGlobalPackedFile, CloseGlobalPackedFile_original)

void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle) {
    dr_dprintf("%s: pTitle=\"%s\" pFlags=%d", __FUNCTION__, pTitle, pFlags);
}
C2_HOOK_FUNCTION(0x0044c850, PrintMemoryDump);

void* (C2_HOOK_CDECL * DRStdlibAllocate_original)(br_size_t size, br_uint_8 type);
void* C2_HOOK_CDECL DRStdlibAllocate(br_size_t size, br_uint_8 type) {

    void* p;
    char s[256];

    if (size != 0) {
        p = malloc(size);
        if (p == NULL && !C2V(gNon_fatal_allocation_errors)) {
            c2_sprintf(s, "%s/%d", C2V(gMem_names)[type], size);
            FatalError(kFatalError_OOM_S, s);
        }
        return p;
    }
    return NULL;
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044c8c0, DRStdlibAllocate, DRStdlibAllocate_original)

void (C2_HOOK_CDECL * DRStdlibFree_original)(void* mem);
void C2_HOOK_CDECL DRStdlibFree(void* mem) {
    free(mem);
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044c990, DRStdlibFree, DRStdlibFree_original)

br_size_t C2_HOOK_CDECL DRStdlibInquire(br_uint_8 type) {
    return 0;
}
C2_HOOK_FUNCTION(0x0044c9c0, DRStdlibInquire)

br_uint_32 C2_HOOK_CDECL Claim4ByteAlignment(br_uint_8 type) {
    return 4;
}
C2_HOOK_FUNCTION(0x0044c9d0, Claim4ByteAlignment)

void (C2_HOOK_FASTCALL * InstallDRMemCalls_original)(void);
void C2_HOOK_FASTCALL InstallDRMemCalls(void) {
#if 0 //defined(C2_HOOKS_ENABLED)
    InstallDRMemCalls_original();
#else
    BrAllocatorSet(&C2V(gAllocator));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044c9e0, InstallDRMemCalls, InstallDRMemCalls_original)

void (C2_HOOK_FASTCALL * CreateStainlessClasses_original)(void);
void C2_HOOK_FASTCALL CreateStainlessClasses(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    CreateStainlessClasses_original();
#else
    int i;

    for (i = 129; i < 129 + REC2_ASIZE(C2V(gStainless_classes)); i++) {
        C2V(gStainless_classes)[i - 129].res_class = i;
        if (!BrResClassAdd(&C2V(gStainless_classes)[i - 129])) {
            FatalError(kFatalError_OOM_S);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044ca40, CreateStainlessClasses, CreateStainlessClasses_original)

void C2_HOOK_FASTCALL CheckMemory(void) {
}
C2_HOOK_FUNCTION(0x0044ca80, CheckMemory)
