#include "loader.h"

#include "core/fw/diag.h"
#include "core/fw/file.h"
#include "core/fw/image.h"
#include "core/fw/resource.h"

#define IMAGE_DOS_SIGNATURE                 0x5A4D      /* MZ */
#define IMAGE_NT_SIGNATURE                  0x00004550  /* PE00 */
#define IMAGE_FILE_MACHINE_I386              0x014c     /* Intel 386. */

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  /* Relocation info stripped from file. */
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  /* File is executable  (i.e. no unresolved external references). */
#define IMAGE_FILE_32BIT_MACHINE             0x0100  /* 32 bit word machine. */

#define DIRECTORY_EXPORT 0
#define DIRECTORY_IMPORT 1
#define DIRECTORY_BASERELOC 5

#define IMAGE_REL_BASED_ABSOLUTE              0
#define IMAGE_REL_BASED_HIGH                  1
#define IMAGE_REL_BASED_LOW                   2
#define IMAGE_REL_BASED_HIGHLOW               3

#define READ_BLOCK(var, fh) \
    do { \
        if (BrFileRead(&(var), 1, sizeof(var),(fh)) != sizeof(var)) { \
            return NULL; \
        }; \
    } while (0)
#define ENTRY_TYPE(e) ((e) >> 12)
#define ENTRY_OFFSET(e) ((e) & 0xfff)

br_image* (C2_HOOK_STDCALL * ImageLoad_original)(char* name);
br_image* C2_HOOK_STDCALL ImageLoad(char* name) {

#if 0//defined(C2_HOOKS_ENABLED)
    return ImageLoad_original(name);
#else
    void* fh;
	int mode = BR_FS_MODE_BINARY;
	struct msdos_header dos_header;
	struct coff_header coff_header;
	struct nt_optional_header nt_header;
	struct section_header section_header;
	br_uint_32 pe;
	br_image* img;
	br_uint_8* arena_base;
	int arena_size,arena_align;
	int i;
	br_uint_32 offset;
	unsigned int n;

	fh = BrFileOpenRead(name, 0, NULL, &mode);
	if (fh == NULL) {
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(dos_header) != 0x40);
	READ_BLOCK(dos_header, fh);
	if (dos_header.magic != IMAGE_DOS_SIGNATURE) {
        return NULL;
    }
	if (dos_header.new_header_offset < sizeof(dos_header)) {
        return NULL;
    }

	BrFileAdvance(dos_header.new_header_offset - sizeof(dos_header), fh);

    C2_HOOK_BUG_ON(sizeof(pe) != 0x4);
	READ_BLOCK(pe, fh);
	if (pe != IMAGE_NT_SIGNATURE) {
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(coff_header) != 0x14);
	READ_BLOCK(coff_header, fh);
	if (coff_header.machine != IMAGE_FILE_MACHINE_I386) {
        return NULL;
    }
	if (coff_header.flags & IMAGE_FILE_RELOCS_STRIPPED) {
        return NULL;
    }
    if (!(coff_header.flags & IMAGE_FILE_EXECUTABLE_IMAGE)) {
        return NULL;
    }
	if (!(coff_header.flags & IMAGE_FILE_32BIT_MACHINE)) {
        return NULL;
    }
	if (coff_header.opt_header_size != sizeof(nt_header)) {
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(nt_header) != 0xe0);
	READ_BLOCK(nt_header, fh);

    C2_HOOK_BUG_ON(sizeof(br_image) != 0x40);
	img = BrResAllocate(NULL, sizeof(br_image), BR_MEMORY_IMAGE);

	img->type = BR_IMG_FRAMEWORK;

    C2_HOOK_BUG_ON(sizeof(br_image_section) != 0x18);
	img->sections = BrResAllocate(img, sizeof(br_image_section) * coff_header.n_sections, BR_MEMORY_IMAGE_SECTIONS);

	if (nt_header.section_alignment & (nt_header.section_alignment - 1)) {
        return NULL;
    }

	for (i = 0; i < coff_header.n_sections; i++) {
        C2_HOOK_BUG_ON(sizeof(section_header) != 0x28);
		READ_BLOCK(section_header, fh);

		img->sections[i].name		 = BrResStrDup(img, section_header.section_name);
		img->sections[i].mem_offset	 = section_header.rva;
		img->sections[i].mem_size	 = section_header.virtual_size;
		img->sections[i].data_size 	 = section_header.data_size;
		img->sections[i].data_offset = section_header.data_offset;
	}

	arena_size = nt_header.image_size;
	arena_align = nt_header.section_alignment;

	arena_base = BrResAllocate(img, arena_size + (arena_align - 1), BR_MEMORY_IMAGE_ARENA);
	arena_base = (br_uint_8*) (((int)arena_base+arena_align - 1) & (~(arena_align - 1)));

    C2_HOOK_BUG_ON(sizeof(pe) + sizeof(coff_header) + sizeof(nt_header) != 0xf8);
    C2_HOOK_BUG_ON(sizeof(section_header) != 0x28);
	offset = dos_header.new_header_offset + sizeof(pe) + sizeof(coff_header) + sizeof(nt_header) + coff_header.n_sections * sizeof(section_header);

	for (i = 0; i < coff_header.n_sections; i++) {

		if (img->sections[i].data_offset == 0) {
            continue;
        }
        if (img->sections[i].data_size == 0) {
            continue;
        }
		if (offset > img->sections[i].data_offset) {
            return NULL;
        }
		BrFileAdvance(img->sections[i].data_offset - offset, fh);

		img->sections[i].base = arena_base + img->sections[i].mem_offset;

		if ((br_uint_32)BrFileRead(img->sections[i].base, 1, img->sections[i].data_size, fh) != img->sections[i].data_size) {
            return NULL;
        }

		offset = img->sections[i].data_offset + img->sections[i].data_size;
	}

	BrFileClose(fh);

	if (nt_header.directories[DIRECTORY_EXPORT].size != 0) {
		struct export_directory* ed;

		ed = (export_directory*)(arena_base + nt_header.directories[DIRECTORY_EXPORT].rva);

		img->identifier = (char*)(arena_base + ed->name);
		img->ordinal_base = ed->ordinal_base;
		img->n_functions = ed->n_entries;
		img->functions = (void**)(arena_base + ed->export_table);
		img->n_names = ed->n_names;
		img->names = (char**)(arena_base + ed->name_table);
		img->name_ordinals = (br_uint_16*)(arena_base + ed->ordinal_table);

		for (n = 0; n < img->n_functions; n++) {
            img->functions[n] = (void*)((br_uint_32)img->functions[n] + arena_base);
        }

		for (n = 0; n < img->n_names; n++) {
            img->names[n] = (char*)((br_uint_32)img->names[n] + arena_base);
        }
	}

	if (nt_header.directories[DIRECTORY_IMPORT].size != 0) {
		struct import_directory* id;
		void** at;
		br_uint_32* lt;
		br_image* import_img;

		n = 0;
		for (id = (import_directory*)(arena_base + nt_header.directories[DIRECTORY_IMPORT].rva); id->lookup_table != 0; id++) {
            n += 1;
        }

		img->imports = BrResAllocate(img, n * sizeof(br_image*), BR_MEMORY_IMAGE_NAMES);
		img->n_imports = n;

		for (n = 0, id = (import_directory*)(arena_base + nt_header.directories[DIRECTORY_IMPORT].rva); id->lookup_table != 0; id++, n++) {

			import_img = BrImageReference((char*)(id->name + arena_base));
			if (import_img == NULL) {
				BrFailure("Could not reference image %s", (char*)(id->name + arena_base));
				return NULL;
			}

			img->imports[n] = import_img;

			at = (void**)(arena_base + id->address_table);

			for (lt = (br_uint_32*)(arena_base + id->lookup_table); *lt != 0; lt++, at++) {
				if (*lt & 0x80000000) {
                    *at = BrImageLookupOrdinal(import_img, *lt & 0x7fffffff);
                }
                if (*at == NULL) {
                    BrFailure("Could not resolve imported ordinal %s:%d", (char *)(id->name + arena_base), *lt & 0x7fffffff);
                    return NULL;
                } else {
					*at = BrImageLookupName(import_img, (char*)(*lt + arena_base + 2), *((br_uint_16*)(*lt + arena_base)));
    				if (*at == NULL) {
                        BrFailure("Could not resolve imported symbol %s:%s (%d)",
		        			(char*)(id->name + arena_base),
	    					(char*)(*lt+arena_base + 2),
                            *((br_uint_16*)(*lt + arena_base)));
					    return NULL;
       				}
                }
			}
		}
	}

	if ((br_uint_32)arena_base != nt_header.image_base && nt_header.directories[DIRECTORY_BASERELOC].size != 0) {
		basereloc_header* header;
		br_uint_16* entry;
		br_uint_8* fixup;
		br_int_32 delta;
		br_int_16 delta_h, delta_l;

		offset = 0;
		delta = (br_int_32)arena_base - nt_header.image_base;
		delta_h = (br_uint_16)(delta >> 16);
		delta_l = (br_uint_16)(delta & 0xFFFF);

		while (offset < nt_header.directories[DIRECTORY_BASERELOC].size) {
			header = (basereloc_header*)(nt_header.directories[DIRECTORY_BASERELOC].rva + arena_base + offset);
			offset += header->size;

            C2_HOOK_BUG_ON(sizeof(basereloc_header) != 8);

			for (n = 0, entry = (br_uint_16*)(header + 1); n < (header->size - sizeof(basereloc_header)) / sizeof(br_uint_16); n++, entry++) {

				fixup = ENTRY_OFFSET(*entry) + header->rva + arena_base;

				if (fixup >= arena_base + arena_size) {
                    return NULL;
                }

				switch (ENTRY_TYPE(*entry)) {
				case IMAGE_REL_BASED_ABSOLUTE:
					break;

				case IMAGE_REL_BASED_HIGH:
					*(br_int_16*)fixup += delta_h;
					break;

				case IMAGE_REL_BASED_LOW:
					*(br_int_16*)fixup += delta_l;
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					*(br_int_32*)fixup += delta;
					break;

				default:
					return NULL;
				}
			}
		}
	}

	return img;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530e70, ImageLoad, ImageLoad_original)
