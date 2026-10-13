"""Full15/222B resource paths, genuine3CALL+tailJMP parents.
Native C relocates only authenticated literal expressions into a fixed mirror;
it does not prove native string-pool layout. Authentic original/current PE and
compiled startup declaration are checked separately. All void EAX excluded;
normal ESP/nonvolatile/TOP observed, DF0 entry only. Controlled readonly globals
test first-store/public-prefix order, without real game permission/adjacency or
terminal-normal-ABI/private-fault-stack equivalence claims. No helper substitute.
"""
import argparse,json,re,struct,subprocess,sys,random
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_GRP_CALL,CS_GRP_JUMP,CS_OP_MEM,CS_OP_IMM
ROOT=next(p for p in Path(__file__).resolve().parents if(p/'src/carma2').is_dir());D=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import raw,pe,HASH,sha,words
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
PLAN={'roots': [{'mode': 0, 'name': 'ConfigureZombiePedSoundPath', 'original': '0x457590', 'bytes': 11, 'instructions': 2, 'raw_sha256': '28e9fc2a59d55e9413f48fd862216a4841c354dd0aa472c16b21327d87f84cc9', 'global_name': 'gPed_sound_path', 'global_original': '0x684550', 'literal_original': '0x595f60', 'callees': []}, {'mode': 1, 'name': 'ConfigureZombiePedPowerupTxtPath', 'original': '0x4e0c20', 'bytes': 11, 'instructions': 2, 'raw_sha256': '3088a9a4328ef43863f0372143be233d81eaa14be3957fc51b3db640695914ce', 'global_name': 'gPowerup_txt_path', 'global_original': '0x6a0ad4', 'literal_original': '0x65ed10', 'callees': []}, {'mode': 2, 'name': 'ConfigureZombiePedTextTxtPath', 'original': '0x48ea20', 'bytes': 11, 'instructions': 2, 'raw_sha256': '9a84ba30a2f7fd2bbf3e2c5e878328a83b6aa876bc7d92b929bcfe89584eeaa3', 'global_name': 'gPedTextTxtPath', 'global_original': '0x68c718', 'literal_original': '0x658514', 'callees': []}, {'mode': 3, 'name': 'ConfigureZombiePedTexturePath', 'original': '0x48f810', 'bytes': 11, 'instructions': 2, 'raw_sha256': '3649fe8f2a4ace95e870f8dd2a68a6fe0070b3dd5ad4114011415118c5a8ed4a', 'global_name': 'gPedTexturePath', 'global_original': '0x65852c', 'literal_original': '0x6585d4', 'callees': []}, {'mode': 4, 'name': 'ConfigureBloodPedSoundPath', 'original': '0x457580', 'bytes': 11, 'instructions': 2, 'raw_sha256': '55cbc1fd3a423be3665f562ae8c18f6c638b92eba826f08f2e75e37dc7320033', 'global_name': 'gPed_sound_path', 'global_original': '0x684550', 'literal_original': None, 'callees': []}, {'mode': 5, 'name': 'ConfigureBloodPedPowerupTxtPath', 'original': '0x4e0c10', 'bytes': 11, 'instructions': 2, 'raw_sha256': '3f7875533b72fbae0c9e5d470837ac172c233f5212759855d4e6cce29135ed66', 'global_name': 'gPowerup_txt_path', 'global_original': '0x6a0ad4', 'literal_original': '0x65ec10', 'callees': []}, {'mode': 6, 'name': 'ConfigureBloodPedTextTxtPath', 'original': '0x48ea10', 'bytes': 11, 'instructions': 2, 'raw_sha256': '6db7557f64075354f4b47d7a28c5976442be424392553aeef66a301c5f3f9041', 'global_name': 'gPedTextTxtPath', 'global_original': '0x68c718', 'literal_original': '0x657da0', 'callees': []}, {'mode': 7, 'name': 'ConfigureBloodPedTexturePath', 'original': '0x48f800', 'bytes': 11, 'instructions': 2, 'raw_sha256': '418a3ba252cea5cf72cc7ba9ac7c4d68fd8541b8821e4586d52fbebce74e8c6e', 'global_name': 'gPedTexturePath', 'global_original': '0x65852c', 'literal_original': '0x657a18', 'callees': []}, {'mode': 8, 'name': 'ConfigureAlienPedSoundPath', 'original': '0x4575a0', 'bytes': 11, 'instructions': 2, 'raw_sha256': '0df4f6fc5bad35eb60d94651f73c516e56b805c7bc82bb16933b506173640e4c', 'global_name': 'gPed_sound_path', 'global_original': '0x684550', 'literal_original': '0x595f6c', 'callees': []}, {'mode': 9, 'name': 'ConfigureAlienPedPowerupTxtPath', 'original': '0x4e0c30', 'bytes': 11, 'instructions': 2, 'raw_sha256': 'e30055040e3f641719473c0facb499b8bd6aa58593a6c81d1cf52b33d7c76fd2', 'global_name': 'gPowerup_txt_path', 'global_original': '0x6a0ad4', 'literal_original': '0x65ed20', 'callees': []}, {'mode': 10, 'name': 'ConfigureAlienPedTextTxtPath', 'original': '0x48ea30', 'bytes': 11, 'instructions': 2, 'raw_sha256': '1a227ffa46e80c712b1a9ea7c680b0981a1d888035e4bdcc7c2d302d0219d3d9', 'global_name': 'gPedTextTxtPath', 'global_original': '0x68c718', 'literal_original': '0x658520', 'callees': []}, {'mode': 11, 'name': 'ConfigureAlienPedTexturePath', 'original': '0x48f820', 'bytes': 11, 'instructions': 2, 'raw_sha256': 'df240f9d6f0fbd4becfeaeed631124dfc029b915b7faaa853c65f045cb26bbcb', 'global_name': 'gPedTexturePath', 'global_original': '0x65852c', 'literal_original': '0x6585e0', 'callees': []}, {'mode': 12, 'name': 'SetBloodPedFolderNames', 'original': '0x4d6f30', 'bytes': 30, 'instructions': 5, 'raw_sha256': 'c3dc187d27f5d95d815bf5370ad1e64569fb5efe3c29ad4e115816d2f348bedc', 'global_name': 'gPedsFolder', 'global_original': '0x6a0414', 'literal_original': '0x65e62c', 'callees': [{'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f3a', 'target': '0x457580', 'kind': 'call'}, {'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f3f', 'target': '0x4e0c10', 'kind': 'call'}, {'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f44', 'target': '0x48ea10', 'kind': 'call'}, {'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f49', 'target': '0x48f800', 'kind': 'tail-jump'}]}, {'mode': 13, 'name': 'SetZombiePedFolderNames', 'original': '0x4d6f50', 'bytes': 30, 'instructions': 5, 'raw_sha256': '7b68f3cfb338074938fcf62791b75039b49395960ca90a5d28ed6ae1a4ec7945', 'global_name': 'gPedsFolder', 'global_original': '0x6a0414', 'literal_original': '0x65e82c', 'callees': [{'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f5a', 'target': '0x457590', 'kind': 'call'}, {'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f5f', 'target': '0x4e0c20', 'kind': 'call'}, {'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f64', 'target': '0x48ea20', 'kind': 'call'}, {'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f69', 'target': '0x48f810', 'kind': 'tail-jump'}]}, {'mode': 14, 'name': 'SetAlienPedFolderNames', 'original': '0x4d6f70', 'bytes': 30, 'instructions': 5, 'raw_sha256': '09c8fc4c0b5014fe06593eb002ac7dde6b05e5b443c866ea67d9ce45c0f8c687', 'global_name': 'gPedsFolder', 'global_original': '0x6a0414', 'literal_original': '0x65e834', 'callees': [{'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f7a', 'target': '0x4575a0', 'kind': 'call'}, {'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f7f', 'target': '0x4e0c30', 'kind': 'call'}, {'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f84', 'target': '0x48ea30', 'kind': 'call'}, {'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f89', 'target': '0x48f820', 'kind': 'tail-jump'}]}], 'edges': [{'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f3a', 'target': '0x457580', 'kind': 'call'}, {'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f3f', 'target': '0x4e0c10', 'kind': 'call'}, {'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f44', 'target': '0x48ea10', 'kind': 'call'}, {'caller': 'SetBloodPedFolderNames', 'site': '0x4d6f49', 'target': '0x48f800', 'kind': 'tail-jump'}, {'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f5a', 'target': '0x457590', 'kind': 'call'}, {'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f5f', 'target': '0x4e0c20', 'kind': 'call'}, {'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f64', 'target': '0x48ea20', 'kind': 'call'}, {'caller': 'SetZombiePedFolderNames', 'site': '0x4d6f69', 'target': '0x48f810', 'kind': 'tail-jump'}, {'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f7a', 'target': '0x4575a0', 'kind': 'call'}, {'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f7f', 'target': '0x4e0c30', 'kind': 'call'}, {'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f84', 'target': '0x48ea30', 'kind': 'call'}, {'caller': 'SetAlienPedFolderNames', 'site': '0x4d6f89', 'target': '0x48f820', 'kind': 'tail-jump'}], 'globals': [{'name': 'gPedTexturePath', 'original': '0x65852c', 'current': '0x4e8838', 'bytes': 4, 'type': 'char*', 'owner': '63-loading3.c', 'map_owner': '63-loading3.c.obj', 'original_initial_word': '0x657a18', 'current_initial_word': '0x4dfe84', 'original_evidence': 'raw-backed pointer bytes', 'current_evidence': 'raw-backed pointer bytes', 'original_section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'current_section': {'start': 5103616, 'end': 6729956, 'raw_end': 5237760, 'flags': 3221225536, 'name': '.data'}, 'initializer_literal': 'PIXELMAP', 'header_has_extern': False}, {'name': 'gPed_sound_path', 'original': '0x684550', 'current': '0x59124c', 'bytes': 4, 'type': 'const char*', 'owner': '69-sound.c', 'map_owner': '<common>', 'original_initial_word': '0x0', 'current_initial_word': '0x0', 'original_evidence': 'zero-filled virtual tail', 'current_evidence': 'zero-filled virtual tail', 'original_section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'current_section': {'start': 5103616, 'end': 6729956, 'raw_end': 5237760, 'flags': 3221225536, 'name': '.data'}, 'initializer_literal': None, 'header_has_extern': False}, {'name': 'gPedTextTxtPath', 'original': '0x68c718', 'current': '0x65e620', 'bytes': 4, 'type': 'const char*', 'owner': '10-loading2.c', 'map_owner': '<common>', 'original_initial_word': '0x0', 'current_initial_word': '0x0', 'original_evidence': 'zero-filled virtual tail', 'current_evidence': 'zero-filled virtual tail', 'original_section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'current_section': {'start': 5103616, 'end': 6729956, 'raw_end': 5237760, 'flags': 3221225536, 'name': '.data'}, 'initializer_literal': None, 'header_has_extern': False}, {'name': 'gPedsFolder', 'original': '0x6a0414', 'current': '0x5d0980', 'bytes': 4, 'type': 'const char*', 'owner': '61-pedestrn.c', 'map_owner': '<common>', 'original_initial_word': '0x0', 'current_initial_word': '0x0', 'original_evidence': 'zero-filled virtual tail', 'current_evidence': 'zero-filled virtual tail', 'original_section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'current_section': {'start': 5103616, 'end': 6729956, 'raw_end': 5237760, 'flags': 3221225536, 'name': '.data'}, 'initializer_literal': None, 'header_has_extern': False}, {'name': 'gPowerup_txt_path', 'original': '0x6a0ad4', 'current': '0x5d65f4', 'bytes': 4, 'type': 'const char*', 'owner': '27-powerup.c', 'map_owner': '<common>', 'original_initial_word': '0x0', 'current_initial_word': '0x0', 'original_evidence': 'zero-filled virtual tail', 'current_evidence': 'zero-filled virtual tail', 'original_section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'current_section': {'start': 5103616, 'end': 6729956, 'raw_end': 5237760, 'flags': 3221225536, 'name': '.data'}, 'initializer_literal': None, 'header_has_extern': False}], 'literals': [{'address': '0x595f60', 'bytes': 9, 'raw': '5a4f4d534f554e4400', 'text': 'ZOMSOUND', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedSoundPath']}, {'address': '0x65ed10', 'bytes': 15, 'raw': '5a4f4d504f57455255502e54585400', 'text': 'ZOMPOWERUP.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedPowerupTxtPath']}, {'address': '0x658514', 'bytes': 12, 'raw': '5a4f4d544558542e54585400', 'text': 'ZOMTEXT.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedTextTxtPath']}, {'address': '0x6585d4', 'bytes': 12, 'raw': '5a4f4d504958454c4d415000', 'text': 'ZOMPIXELMAP', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedTexturePath']}, {'address': '0x65ec10', 'bytes': 12, 'raw': '504f57455255502e54585400', 'text': 'POWERUP.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureBloodPedPowerupTxtPath']}, {'address': '0x657da0', 'bytes': 9, 'raw': '544558542e54585400', 'text': 'TEXT.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureBloodPedTextTxtPath']}, {'address': '0x657a18', 'bytes': 9, 'raw': '504958454c4d415000', 'text': 'PIXELMAP', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureBloodPedTexturePath']}, {'address': '0x595f6c', 'bytes': 8, 'raw': '414c534f554e4400', 'text': 'ALSOUND', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedSoundPath']}, {'address': '0x65ed20', 'bytes': 14, 'raw': '414c504f57455255502e54585400', 'text': 'ALPOWERUP.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedPowerupTxtPath']}, {'address': '0x658520', 'bytes': 11, 'raw': '414c544558542e54585400', 'text': 'ALTEXT.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedTextTxtPath']}, {'address': '0x6585e0', 'bytes': 11, 'raw': '414c504958454c4d415000', 'text': 'ALPIXELMAP', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedTexturePath']}, {'address': '0x65e62c', 'bytes': 5, 'raw': '5045445300', 'text': 'PEDS', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['SetBloodPedFolderNames']}, {'address': '0x65e82c', 'bytes': 8, 'raw': '5a4f4d4249455300', 'text': 'ZOMBIES', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['SetZombiePedFolderNames']}, {'address': '0x65e834', 'bytes': 7, 'raw': '414c49454e5300', 'text': 'ALIENS', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['SetAlienPedFolderNames']}], 'existing_default_alias_evidence': [{'name': 'SetDefaultSoundFolderName', 'original': '0x457570', 'current': '0x44e2b0', 'owner': '69-sound.c', 'original_literal': None, 'current_literal': None, 'original_opcode': 'c7055045680000000000', 'current_opcode': 'c7054c12590000000000', 'existing_function_credit': 0}, {'name': 'SetDefaultPowerupFilename', 'original': '0x4e0c00', 'current': '0x4158b0', 'owner': '27-powerup.c', 'original_literal': '0x65ec10', 'current_literal': '0x4e1600', 'original_opcode': 'c705d40a6a0010ec6500', 'current_opcode': 'c705f4655d0000164e00', 'existing_function_credit': 0}, {'name': 'SetDefaultTextFileName', 'original': '0x48ea00', 'current': '0x409fb0', 'owner': '10-loading2.c', 'original_literal': '0x657da0', 'current_literal': '0x4dff20', 'original_opcode': 'c70518c76800a07d6500', 'current_opcode': 'c70520e6650020ff4d00', 'existing_function_credit': 0}, {'name': 'SetDefaultPixelmapFolderName', 'original': '0x48f7f0', 'current': '0x44a0e0', 'owner': '63-loading3.c', 'original_literal': '0x657a18', 'current_literal': '0x4dfe84', 'original_opcode': 'c7052c856500187a6500', 'current_opcode': 'c70538884e0084fe4d00', 'existing_function_credit': 0}, {'name': 'SetDefaultPedFolderNames', 'original': '0x4d6f10', 'current': '0x448b10', 'owner': '61-pedestrn.c', 'original_literal': '0x65e62c', 'current_literal': '0x4e87bc', 'original_opcode': 'c70514046a002ce66500', 'current_opcode': 'c70580095d00bc874e00', 'existing_function_credit': 0}]}
ENTRIES=[('ConfigureZombiePedSoundPath', 4552080, 11), ('ConfigureZombiePedPowerupTxtPath', 5114912, 11), ('ConfigureZombiePedTextTxtPath', 4778528, 11), ('ConfigureZombiePedTexturePath', 4782096, 11), ('ConfigureBloodPedSoundPath', 4552064, 11), ('ConfigureBloodPedPowerupTxtPath', 5114896, 11), ('ConfigureBloodPedTextTxtPath', 4778512, 11), ('ConfigureBloodPedTexturePath', 4782080, 11), ('ConfigureAlienPedSoundPath', 4552096, 11), ('ConfigureAlienPedPowerupTxtPath', 5114928, 11), ('ConfigureAlienPedTextTxtPath', 4778544, 11), ('ConfigureAlienPedTexturePath', 4782112, 11), ('SetBloodPedFolderNames', 5074736, 30), ('SetZombiePedFolderNames', 5074768, 30), ('SetAlienPedFolderNames', 5074800, 30)]
SPECS=[('gPedTexturePath', 6653228, 0, 'char*', '63-loading3.c'), ('gPed_sound_path', 6833488, 4096, 'const char*', '69-sound.c'), ('gPedTextTxtPath', 6866712, 8192, 'const char*', '10-loading2.c'), ('gPedsFolder', 6947860, 12288, 'const char*', '61-pedestrn.c'), ('gPowerup_txt_path', 6949588, 16384, 'const char*', '27-powerup.c')]
GLOBALS={6653228: (0, 4), 6833488: (4096, 4), 6866712: (8192, 4), 6947860: (12288, 4), 6949588: (16384, 4)}
LITERALS=[{'address': '0x595f60', 'bytes': 9, 'raw': '5a4f4d534f554e4400', 'text': 'ZOMSOUND', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedSoundPath'], 'offset': 24576}, {'address': '0x65ed10', 'bytes': 15, 'raw': '5a4f4d504f57455255502e54585400', 'text': 'ZOMPOWERUP.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedPowerupTxtPath'], 'offset': 24608}, {'address': '0x658514', 'bytes': 12, 'raw': '5a4f4d544558542e54585400', 'text': 'ZOMTEXT.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedTextTxtPath'], 'offset': 24640}, {'address': '0x6585d4', 'bytes': 12, 'raw': '5a4f4d504958454c4d415000', 'text': 'ZOMPIXELMAP', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureZombiePedTexturePath'], 'offset': 24672}, {'address': '0x65ec10', 'bytes': 12, 'raw': '504f57455255502e54585400', 'text': 'POWERUP.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureBloodPedPowerupTxtPath'], 'offset': 24704}, {'address': '0x657da0', 'bytes': 9, 'raw': '544558542e54585400', 'text': 'TEXT.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureBloodPedTextTxtPath'], 'offset': 24736}, {'address': '0x657a18', 'bytes': 9, 'raw': '504958454c4d415000', 'text': 'PIXELMAP', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureBloodPedTexturePath'], 'offset': 24768}, {'address': '0x595f6c', 'bytes': 8, 'raw': '414c534f554e4400', 'text': 'ALSOUND', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedSoundPath'], 'offset': 24800}, {'address': '0x65ed20', 'bytes': 14, 'raw': '414c504f57455255502e54585400', 'text': 'ALPOWERUP.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedPowerupTxtPath'], 'offset': 24832}, {'address': '0x658520', 'bytes': 11, 'raw': '414c544558542e54585400', 'text': 'ALTEXT.TXT', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedTextTxtPath'], 'offset': 24864}, {'address': '0x6585e0', 'bytes': 11, 'raw': '414c504958454c4d415000', 'text': 'ALPIXELMAP', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['ConfigureAlienPedTexturePath'], 'offset': 24896}, {'address': '0x65e62c', 'bytes': 5, 'raw': '5045445300', 'text': 'PEDS', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['SetBloodPedFolderNames'], 'offset': 24928}, {'address': '0x65e82c', 'bytes': 8, 'raw': '5a4f4d4249455300', 'text': 'ZOMBIES', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['SetZombiePedFolderNames'], 'offset': 24960}, {'address': '0x65e834', 'bytes': 7, 'raw': '414c49454e5300', 'text': 'ALIENS', 'section': {'start': 5828608, 'end': 8001700, 'raw_end': 6777344, 'flags': 3221225536, 'name': '.data'}, 'root_references': ['SetAlienPedFolderNames'], 'offset': 24992}]
LITERAL_TARGETS={5857120: 704667648, 6679824: 704667680, 6653204: 704667712, 6653396: 704667744, 6679568: 704667776, 6651296: 704667808, 6650392: 704667840, 5857132: 704667872, 6679840: 704667904, 6653216: 704667936, 6653408: 704667968, 6678060: 704668000, 6678572: 704668032, 6678580: 704668064}
INITIAL=[704667840, 0, 0, 0, 0]
BASE=704643072
ARENA=32768
VALUES=[0, 1, 4294967295, 2147483648, 2147483647, 2863311530, 1431655765, 16909060, 704643072, 704675839, 704667648, 704667680, 704667712, 704667744, 704667776, 704667808, 704667840]
def fixtures():
 cases=[];rng=random.Random(0x15e222)
 def add(label,modes,values=None,startup=False,readonly=0):
  assert 0<len(modes)<=16 and all(0<=m<15 for m in modes)and 0<=readonly<32
  memory=bytearray(ARENA)
  for at in range(0,ARENA,128):struct.pack_into('<I',memory,at,(at*71+len(cases))&0xffffffff)
  values=INITIAL if values is None else values
  initial=[v^0xffffffff for v in values]if startup else values
  for row,v in zip(SPECS,initial):struct.pack_into('<I',memory,row[2],v&0xffffffff)
  for literal in LITERALS:
   b=bytes.fromhex(literal['raw']);assert len(b)==literal['bytes']and b[-1]==0
   memory[literal['offset']:literal['offset']+len(b)]=b
  h=struct.pack('<6I',int(startup),len(modes),readonly,rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32))
  commands=struct.pack('<16I',*modes,*([0]*(16-len(modes))))
  case=dict(index=len(cases),label=label,modes=list(modes),startup=startup,readonly_global_mask=readonly,qualified_controlled_permissions=bool(readonly),initial_words=[v&0xffffffff for v in initial],effective_startup_words=INITIAL if startup else[v&0xffffffff for v in values])
  cases.append((case,h+commands+memory))
 for mode in range(15):add('authentic-startup-from-declarations-or-PE',[mode],startup=True)
 for mode in range(15):
  for j in range(len(VALUES)):add('five-independent-raw-pointer-patterns',[mode],[VALUES[(j+i)%len(VALUES)]for i in range(5)])
  add('idempotent-repeat-with-shared-identity',[mode,mode,mode],startup=True)
 for first in range(12,15):
  for second in range(12,15):
   for end in range(12,15):add('three-real-parents-cross-variant-sequence',[first,second,end],startup=True)
 for i in range(64):add('bounded-random-real-entry-sequence',[i%15,*[rng.randrange(15)for _ in range(2+i%8)]],[rng.getrandbits(32)for _ in range(5)])
 for mode in range(15):
  for slot in range(5):add('single-readonly-global-observes-store-prefix',[mode],readonly=1<<slot)
 for mode in range(12,15):
  for mask in range(1,32):add('parent-all-readonly-combinations-first-public-store',[mode],readonly=mask,startup=True)
 return cases

COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "c2_hooks.h"
static unsigned char*arena;static unsigned addresses[15],physical_mode,physical_initial[5],step,current_mode;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern unsigned __cdecl BuilderInvoke(void);
typedef char native_pointer_layout[sizeof(unsigned)==4&&sizeof(void*)==4?1:-1];
/* Owning startup declarations use real native literal storage before projection. */
char* gPedTexturePath="PIXELMAP";
const char* gPed_sound_path;
const char* gPedTextTxtPath;
const char* gPedsFolder;
const char* gPowerup_txt_path;
static void native_initial_words(unsigned*out){
char*texture=*(char* volatile*)&gPedTexturePath;
if(!texture||memcmp(texture,"PIXELMAP",9))exit(18);
if(*(const char* volatile*)&gPed_sound_path||*(const char* volatile*)&gPedTextTxtPath||*(const char* volatile*)&gPedsFolder||*(const char* volatile*)&gPowerup_txt_path)exit(18);
out[0]=__TEXTURE_TARGET__;out[1]=out[2]=out[3]=out[4]=0;
}
#define gPedTexturePath (*(char**)(arena+0))
#define gPed_sound_path (*(const char**)(arena+0x1000))
#define gPedTextTxtPath (*(const char**)(arena+0x2000))
#define gPedsFolder (*(const char**)(arena+0x3000))
#define gPowerup_txt_path (*(const char**)(arena+0x4000))
static void snapshot(unsigned kind,unsigned value,unsigned fault){unsigned h[5];h[0]=kind;h[1]=step;h[2]=current_mode;h[3]=value;h[4]=fault;fwrite(h,20,1,stdout);fwrite(arena,32768,1,stdout);fflush(stdout);}
static LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}
static int run(const char*path){FILE*f;unsigned h[6],modes[16],initial[5],i,mode;DWORD old;FARPROC proc;PVOID(WINAPI*handler)(ULONG,PVOID);
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;
arena=VirtualAlloc((void*)0x2a000000,65536,MEM_RESERVE,PAGE_READWRITE);if(arena!=(void*)0x2a000000)return 11;if(VirtualAlloc(arena,32768,MEM_COMMIT,PAGE_READWRITE)!=arena)return 11;
if(fread(h,sizeof(h),1,f)!=1||fread(modes,sizeof(modes),1,f)!=1||fread(arena,32768,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);if(h[0]>1||h[1]==0||h[1]>16||h[2]>31)return 13;
proc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;
if(h[0]){if(physical_mode)memcpy(initial,physical_initial,sizeof(initial));else native_initial_words(initial);for(i=0;i<5;i++)memcpy(arena+i*4096,initial+i,4);}
for(i=0;i<5;i++)if(h[2]&(1u<<i))if(!VirtualProtect(arena+i*4096,4096,PAGE_READONLY,&old))return 16;
step=0;current_mode=0xffffffff;snapshot(2,0,0);
for(i=0;i<h[1];i++){mode=modes[i];if(mode>14)return 15;current_mode=mode;step=i+1;builder_address=addresses[mode];builder_mode=1;builder_x=h[3];builder_y=h[4];builder_poison=h[5];builder_stack_count=0;_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);}
return 0;}
'''.replace('__TEXTURE_TARGET__',hex(INITIAL[0])+'u')
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[4],offsets[15],i,j,offset,value;unsigned char*image;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]!=15||fread(offsets,sizeof(offsets),1,f)!=1||fread(physical_initial,sizeof(physical_initial),1,f)!=1)return 3;
for(j=0;j<15;j++)if(offsets[j]>=h[0])return 3;
image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;
for(i=0;i<h[2];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[3];memcpy(image+offset,&value,4);}fclose(f);
FlushInstructionCache(GetCurrentProcess(),image,h[0]);for(j=0;j<15;j++)addresses[j]=(unsigned)image+offsets[j];physical_mode=1;return run(argv[1]);}
'''
def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;low=min(a for _,a,_ in ENTRIES);high=max(a+n for _,a,n in ENTRIES);image=bytearray(b'\xcc'*(high-low));bodies=[];patches=[];edges=[]
 for name,address,size in ENTRIES:
  body=raw(binary,address,size);row=next(r for r in PLAN['roots']if r['name']==name);assert sha(body)==row['raw_sha256'];image[address-low:address-low+size]=body
  decoded=list(cs.disasm(body,address));assert sum(i.size for i in decoded)==size
  for i in decoded:
   if i.mnemonic=='mov':
    assert len(i.operands)==2 and i.operands[0].type==CS_OP_MEM and i.operands[1].type==CS_OP_IMM
    op=i.operands[0];assert not op.mem.base and not op.mem.index and not op.mem.segment and op.size==4 and op.mem.disp in GLOBALS and i.disp_size==4 and i.imm_size==4
    target=BASE+GLOBALS[op.mem.disp][0];struct.pack_into('<I',image,i.address+i.disp_offset-low,target);patches.append(dict(site=i.address+i.disp_offset,original=op.mem.disp,target=target,kind='global',instruction_hex=i.bytes.hex()))
    value=i.operands[1].imm;assert value==int(row['literal_original'],16)if row['literal_original']else value==0
    if value:
     literal=next(r for r in LITERALS if int(r['address'],16)==value);assert raw(binary,value,literal['bytes'])==bytes.fromhex(literal['raw']);target=LITERAL_TARGETS[value]
     struct.pack_into('<I',image,i.address+i.imm_offset-low,target);patches.append(dict(site=i.address+i.imm_offset,original=value,target=target,kind='literal',instruction_hex=i.bytes.hex()))
   elif CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups:
    assert i.mnemonic in('call','jmp')and i.operands[0].type==CS_OP_IMM and i.imm_size==4 and i.operands[0].imm in[a for _,a,_ in ENTRIES]
    edges.append(dict(caller=name,site=hex(i.address),target=hex(i.operands[0].imm),kind='call'if i.mnemonic=='call'else'tail-jump'))
   else:assert i.mnemonic=='ret'and not i.operands
  bodies.append(dict(name=name,address=address,bytes=size,raw_sha256=sha(body),instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in decoded]))
 assert edges==PLAN['edges'] and len(bodies)==15 and sum(r['bytes']for r in bodies)==222 and sum(len(r['instructions'])for r in bodies)==39 and len(patches)==29
 # Authentic original initial storage was checked in frozen independently reviewed
 # plan; map only the exact known nonzero PIXELMAP identity to fixed storage.
 for r in PLAN['globals']:
  if r['original_evidence']=='raw-backed pointer bytes':assert struct.unpack('<I',raw(binary,int(r['original'],16),4))[0]==int(r['original_initial_word'],16)
  else:assert r['original_evidence']=='zero-filled virtual tail'and int(r['original_initial_word'],16)==0
 return words([len(image),15,0,low,*[a-low for _,a,_ in ENTRIES],*INITIAL])+image,dict(bodies=bodies,patches=patches,edges=edges,total_bytes=222,total_instructions=39,globals=SPECS,literals=LITERALS,initial_words=INITIAL,tables=[],indirect_calls=[])
def native_source(candidate_path=None,source_dir=None):
 candidate=source_capsule(candidate_path,source_dir);bodies=[];changes=[]
 for row in PLAN['roots']:
  name=row['name'];body=extract(candidate,name);literal=row['literal_original']
  if literal:
   r=next(r for r in LITERALS if r['address']==literal);needle=json.dumps(r['text']);assert body.count(needle)==1
   cast='char*'if row['global_name']=='gPedTexturePath'else'const char*';replacement='('+cast+')'+hex(LITERAL_TARGETS[int(literal,16)])+'u';before=body;body=body.replace(needle,replacement)
   changes.append(dict(name=name,before_sha256=sha(before.encode()),after_sha256=sha(body.encode()),literal_original=literal,raw=r['raw'],expression_before=needle,expression_after=replacement))
  bodies.append(body)
 assert len(changes)==14
 return COMMON+'\n'+'\n'.join(bodies)+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join('addresses[%d]=(unsigned)%s;'%(i,name)for i,(name,_,_)in enumerate(ENTRIES))+'physical_mode=0;return run(argv[1]);}\n',changes
def dependencies():
 includes=[ROOT/p for p in('src/carma2/hooks','src/carma2/include','src/msvc500/include','src/brender/include')];todo=[ROOT/'src/carma2/hooks/c2_hooks.h'];seen=set()
 while todo:
  p=todo.pop().resolve()
  if p in seen:continue
  seen.add(p)
  for name in re.findall(r'^\s*#\s*include\s*[<"]([^>"]+)',p.read_text(),re.M):
   found=[x/name for x in[p.parent,*includes]if(x/name).is_file()]
   if found:todo.append(found[0])
 assert not any(p.name.startswith('72-')for p in seen)
 return sorted(seen)

def source_capsule(candidate_path=None,source_dir=None):
    if candidate_path:return '\n'.join(extract(candidate_path.read_text(),name)for name,_,_ in ENTRIES)
    directory=source_dir or ROOT/'src/carma2/newcommon'
    owners={r['name']:next(g['owner']for g in PLAN['globals']if g['name']==r['global_name'])for r in PLAN['roots']}
    for row in PLAN['globals']:
        name=row['name'];source=(directory/row['owner']).read_text();header=(directory/row['owner']).with_suffix('.h').read_text()
        declaration=re.escape(row['type'])+r'\s+'+re.escape(name)+(r'\s*=\s*"PIXELMAP"'if name=='gPedTexturePath'else'')+r'\s*;'
        assert re.search(r'// GLOBAL: CARMA2_HW '+f"{int(row['original'],16):#010x}"+r'\s+'+declaration,source),('Owning global/initializer',name,row['owner'])
        assert re.search(r'extern\s+'+re.escape(row['type'])+r'\s+'+re.escape(name)+r'\s*;',header),('Owning four-byte global extern',name)
    result=[]
    for name,address,_ in ENTRIES:
        owner=owners[name];source=(directory/owner).read_text();header=(directory/owner).with_suffix('.h').read_text()
        assert re.search(r'// FUNCTION: CARMA2_HW '+f'{address:#010x}'+r'\s+void C2_HOOK_FASTCALL '+re.escape(name)+r'\(void\)',source),('Complete annotated root',name)
        assert re.search(r'void C2_HOOK_FASTCALL '+re.escape(name)+r'\(void\);',header),('Exact noargs prototype',name)
        result.append(extract(source,name))
    return '\n'.join(result)

def matching_image(exe,symbols):
    """Future complete15 literal stores/real child edges; fail closed on new shapes.
    Current literal address classes derive from authenticated store destinations
    and original class authority, never arbitrary string-based normalization.
    This draft has not yet passed future owner-TU/current-PE preflight.
    """
    disk=exe.read_bytes();mapbytes=exe.with_suffix('.map').read_bytes();mapping=json.loads(symbols.read_text())
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(disk)
    mapped={}
    for r in mapping['symbols']:
        if r['orig']in mapped:assert mapped[r['orig']]==r['recomp']
        mapped[r['orig']]=r['recomp']
    funcs={};data={}
    for line in mapbytes.decode('latin1').splitlines():
        f=line.split()
        if len(f)<4 or len(f[0].split(':'))!=2:continue
        try:address=int(f[2],16)
        except ValueError:continue
        if 'f'in f[3:-1]:funcs.setdefault(address,set()).add((f[1],f[-1]))
        else:data.setdefault(f[1],set()).add((address,f[-1]))
    owners={r['name']:(next(g['owner']for g in PLAN['globals']if g['name']==r['global_name'])+'.obj','@'+r['name']+'@0')for r in PLAN['roots']}
    specs={r['name']:r for r in PLAN['roots']}
    def identity(address):
        aliases=funcs.get(address,set());assert len(aliases)==1,('Ambiguous/missing reached identity',hex(address),aliases)
        decorated,owner=next(iter(aliases));name=decorated.lstrip('@').split('@')[0]
        assert name in owners and(owner,decorated)==owners[name],('Unexpected reached callee',hex(address),decorated,owner)
        return name,owner,decorated
    roots=[]
    for name,orig,_ in ENTRIES:
        address=mapped[orig];assert identity(address)[0]==name;roots.append(address)
    assert len(set(roots))==15
    assert len(disk)>=64 and disk[:2]==b'MZ',('DOS header signature/extent')
    header=struct.unpack_from('<I',disk,60)[0];assert header+24<=len(disk)and disk[header:header+4]==b'PE\0\0',('PE signature/extent')
    assert struct.unpack_from('<H',disk,header+4)[0]==0x14c,('Expected genuine I386 code')
    optional=struct.unpack_from('<H',disk,header+20)[0];assert optional>=144 and header+24+optional<=len(disk)
    assert struct.unpack_from('<H',disk,header+24)[0]==0x10b,('Expected genuine PE32 code')
    directory_count=struct.unpack_from('<I',disk,header+24+92)[0];assert 6<=directory_count<=(optional-96)//8,('Data-directory bounds',directory_count,optional)
    nt,base,image_size,tab=pe(disk);sections=[]
    count=struct.unpack_from('<H',disk,nt+6)[0];assert count and tab+40*count<=len(disk)and image_size and base+image_size<=0x100000000
    for index in range(struct.unpack_from('<H',disk,nt+6)[0]):
        at=tab+40*index;v,r,n,file=struct.unpack_from('<4I',disk,at+8)
        assert file+n<=len(disk)and r+max(v,n)<=image_size,('Section file/image bounds',index)
        sections.append(dict(start=base+r,end=base+r+max(v,n),raw_end=base+r+n,flags=struct.unpack_from('<I',disk,at+36)[0],name=disk[at:at+8].split(b'\0')[0].decode('latin1')))
    def section(address,n):
        rows=[s for s in sections if s['start']<=address and address+n<=s['end']];assert len(rows)==1,('Complete PE span',hex(address),n);return rows[0]
    def initial_word(address):
        sec=section(address,4);assert sec['flags']&0xc0000000==0xc0000000 and not sec['flags']&0x20000000
        if address+4<=sec['raw_end']:return struct.unpack('<I',raw(disk,address,4))[0],'raw-backed pointer initializer'
        assert address>=sec['raw_end'];return 0,'zero-filled virtual tail'
    globals_={};byname={}
    for name,orig,offset,typ,sourceowner in SPECS:
        values=data.get('_'+name,set());assert len(values)==1,('Ambiguous named global',name,values)
        address,owner=next(iter(values));expected_owner=sourceowner+'.obj'if name=='gPedTexturePath'else'<common>'
        assert address==mapped[orig]and owner==expected_owner,(name,address,owner,expected_owner)
        value,evidence=initial_word(address)
        if name!='gPedTexturePath':assert value==0,('Non-NULL genuine PE initializer',name,value)
        row=dict(name=name,original=orig,address=address,target=BASE+offset,bytes=4,type=typ,owner=owner,source_owner=sourceowner,initial_word=value,initializer_evidence=evidence)
        globals_[address]=row;byname[name]=row
    assert len(globals_)==5 and all(a['address']+4<=b['address']or b['address']+4<=a['address']for j,a in enumerate(globals_.values())for b in list(globals_.values())[j+1:])
    starts=sorted(funcs);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;active={};bodies={};patches={};edges=[];classes={};literalspans=[]
    byliteral={int(r['address'],16):r for r in LITERALS}
    def literal(original,current,context):
        if original is None:assert current==0,('Expected authentic NULL',context,current);return 0
        original=int(original,16)if isinstance(original,str)else original;r=byliteral[original];blob=bytes.fromhex(r['raw']);sec=section(current,len(blob))
        assert sec['flags']&0x40000000 and not sec['flags']&0x20000000 and current+len(blob)<=sec['raw_end']
        assert all(current+len(blob)<=g['address']or g['address']+4<=current for g in globals_.values()),('Literal overlaps mutable global storage',context)
        assert raw(disk,current,len(blob))==blob,('Literal full bytes',context,hex(original),hex(current))
        if original in classes:assert classes[original]['current']==current,('Lost exact original pointer alias class',context,hex(original),hex(current),classes[original])
        else:
            assert all(current+len(blob)<=r['current']or r['current']+r['bytes']<=current for r in classes.values()),('Distinct original literal classes overlap/current pool coalescing',context)
            classes[original]=dict(original=original,current=current,bytes=len(blob),raw=blob.hex(),target=LITERAL_TARGETS[original],section=sec,contexts=[])
        classes[original]['contexts'].append(context);return LITERAL_TARGETS[original]
    def patch(site,value,target,authority,kind,ins):
        r=dict(site=site,original_value=value,target=target,width=4,authority=authority,kind=kind,instruction_address=ins.address,instruction_hex=ins.bytes.hex())
        if site in patches:assert patches[site]==r
        patches[site]=r
    def decode(address):
        if address in bodies:return bodies[address]
        name,owner,decorated=identity(address);end=next(a for a in starts if a>address);sec=section(address,end-address)
        assert sec['flags']&0x60000020==0x60000020 and not sec['flags']&0x80000000 and end<=sec['raw_end']
        at=address;instructions=[];operations=[];successors={}
        while True:
            assert address<=at<end;i=next(cs.disasm(raw(disk,at,min(15,end-at)),at,count=1));assert at+i.size<=end and not any(i.prefix)
            sec=section(at,i.size);assert sec['flags']&0x60000020==0x60000020 and not sec['flags']&0x80000000 and at+i.size<=sec['raw_end']
            for p in range(at,at+i.size):assert p not in active;active[p]=at
            instructions.append(dict(address=at,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str))
            if i.mnemonic=='mov':
                assert len(i.operands)==2 and i.operands[0].type==CS_OP_MEM and i.operands[1].type==CS_OP_IMM
                op=i.operands[0];assert not op.mem.base and not op.mem.index and not op.mem.segment and op.size==4 and i.disp_size==4 and i.imm_size==4 and op.mem.disp in globals_
                operations.append(dict(kind='store',site=at,destination=globals_[op.mem.disp]['name'],value=i.operands[1].imm&0xffffffff,disp_site=at+i.disp_offset,imm_site=at+i.imm_offset,instruction=i));nexts=[at+i.size]
            elif CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups:
                assert i.mnemonic in('call','jmp')and len(i.operands)==1 and i.operands[0].type==CS_OP_IMM and i.imm_size==4
                target=i.operands[0].imm;targetname,_,_=identity(target)
                expectedchildren={next(r['name']for r in PLAN['roots']if r['original']==edge['target'])for edge in specs[name]['callees']}
                assert targetname in expectedchildren,('Non-genuine root child authority',name,targetname)
                if i.mnemonic=='jmp':assert targetname==next(r['name']for r in PLAN['roots']if r['original']==specs[name]['callees'][-1]['target'])
                operations.append(dict(kind='call'if i.mnemonic=='call'else'tail-jump',site=at,target=target));edges.append(dict(caller=name,site=at,target=target,kind=operations[-1]['kind']));nexts=[at+i.size]if i.mnemonic=='call'else[]
            else:
                assert i.mnemonic=='ret'and not i.operands,('Unexpected current emitted operation; new evidence required',name,hex(at),i.mnemonic,i.op_str);nexts=[]
            successors[at]=nexts
            if not nexts:break
            at=nexts[0]
        row=dict(name=name,owner=owner,decorated=decorated,address=address,end=end,instruction_count=len(instructions),instructions=instructions,successors=successors,operations=operations)
        bodies[address]=row
        for op in operations:
            if op['kind']!='store':decode(op['target'])
        return row
    for address in roots:decode(address)
    # Flatten only authentic complete children. The unbranched actual store trace
    # must equal original folder/Sound/Powerup/Text/Texture schedule exactly.
    def flatten(address,visiting=()):
        assert address not in visiting;out=[]
        for op in bodies[address]['operations']:
            if op['kind']=='store':out.append(op)
            else:out.extend(flatten(op['target'],visiting+(address,)))
        return out
    traces=[]
    for (name,orig,_),address in zip(ENTRIES,roots):
        expected=[specs[name]]+[next(r for r in PLAN['roots']if r['original']==edge['target'])for edge in specs[name]['callees']];actual=flatten(address);assert len(actual)==len(expected)
        for store,r in zip(actual,expected):
            assert store['destination']==r['global_name'],('Current public store order changed',name,store,r)
            target=literal(r['literal_original'],store['value'],name+'/'+r['name'])
            g=byname[store['destination']];patch(store['disp_site'],g['address'],g['target'],g['name'],'global',store['instruction'])
            if store['value']:patch(store['imm_site'],store['value'],target,r['literal_original'],'literal',store['instruction'])
        traces.append(dict(name=name,stores=[dict(site=s['site'],destination=s['destination'],value=s['value'])for s in actual]))
    assert len(classes)==14
    initial=[]
    for row in PLAN['globals']:
        g=byname[row['name']];original=int(row['original_initial_word'],16);initial.append(literal(original if original else None,g['initial_word'],'genuine PE initializer/'+g['name']))
    # Existing accepted defaults supply independent real whole-owner-TU pooling
    # authority. Inspect authenticated first immediate store only; no new credit
    # or claim to a complete current default function's semantics is made here.
    defaults=[]
    for row in PLAN['existing_default_alias_evidence']:
        address=mapped[int(row['original'],16)];assert funcs.get(address)=={('@'+row['name']+'@0',row['owner']+'.obj')}
        end=next(a for a in starts if a>address);sec=section(address,end-address);assert sec['flags']&0x60000020==0x60000020 and not sec['flags']&0x80000000 and end<=sec['raw_end']
        i=next(cs.disasm(raw(disk,address,min(15,end-address)),address,count=1));assert i.address+i.size<=end and not any(i.prefix)and i.mnemonic=='mov'
        assert len(i.operands)==2 and i.operands[0].type==CS_OP_MEM and not i.operands[0].mem.base and not i.operands[0].mem.index and not i.operands[0].mem.segment and i.operands[0].size==4 and i.operands[1].type==CS_OP_IMM
        rootdefault=next(r for r in PLAN['globals']if r['owner']==row['owner']);assert i.operands[0].mem.disp==byname[rootdefault['name']]['address']
        literal(row['original_literal'],i.operands[1].imm&0xffffffff,'accepted default/'+row['name']);defaults.append(dict(name=row['name'],address=address,owner=row['owner'],opcode=i.bytes.hex(),literal=i.operands[1].imm&0xffffffff,whole_default_function_credit=0))
    # Every transported code operand must have an authentic HIGHLOW when the
    # image has relocation records; absent directory requires stripped flag.
    rva,n=struct.unpack_from('<II',disk,nt+24+96+5*8);coff=struct.unpack_from('<H',disk,nt+22)[0];relocs=set()
    if rva or n:
        assert rva and n;cursor=0
        while cursor<n:
            assert cursor+8<=n;page,length=struct.unpack('<II',raw(disk,base+rva+cursor,8));assert page%4096==0 and length>=8 and length%2==0 and cursor+length<=n
            for entry in struct.unpack('<'+'H'*((length-8)//2),raw(disk,base+rva+cursor+8,length-8)):
                kind=entry>>12
                if not kind:continue
                assert kind==3;site=base+page+(entry&4095);assert site+4<=base+image_size and site not in relocs;relocs.add(site)
                if any(site+j in active for j in range(4)):assert site in patches,('Unclassified active HIGHLOW',hex(site))
            cursor+=length
        assert cursor==n and all(site in relocs for site in patches)
        for g in globals_.values():
            if g['initial_word']:assert g['address']in relocs,('Initialized pointer missing HIGHLOW',g)
    else:assert coff&1
    low=min(active);high=max(active)+1;image=bytearray(b'\xcc'*(high-low))
    for row in bodies.values():
        for i in row['instructions']:b=bytes.fromhex(i['hex']);image[i['address']-low:i['address']-low+len(b)]=b
    for site,row in patches.items():struct.pack_into('<I',image,site-low,row['target'])
    graph=[{k:v for k,v in row.items()if k!='operations'}for row in bodies.values()]
    packet=words([len(image),15,0,low,*[a-low for a in roots],*initial])+image
    meta=dict(recompiled_sha256=sha(disk),symbols_sha256=sha(symbols.read_bytes()),map_sha256=sha(mapbytes),roots=roots,original_roots=[hex(a)for _,a,_ in ENTRIES],graph=graph,edges=edges,store_traces=traces,patches=list(patches.values()),globals=list(globals_.values()),literal_classes=list(classes.values()),accepted_default_first_store_alias_evidence=defaults,initial_words=initial,relocation_directory=[rva,n],coff_characteristics=coff,active_highlow=sorted(site for site in relocs if any(site+j in active for j in range(4))),low=low,high=high,indirect_calls=[],tables=[],private_helpers=[],map_aliases=[dict(address=a,aliases=sorted(v),reachable=a in bodies)for a,v in funcs.items()if len(v)>1],limits=['Strict current direct store/call/tail/RET grammar; unanticipated compiler shape must fail preflight and receive genuine evidence review.','Existing accepted defaults inspected only at their first store for exact shared literal identity; no full-default body credit.','Initialized/default/Blood same-address aliases are exact, not normalized by equal string contents. Four NULL initializers,14 distinct literal NUL spans and five globals are fully authenticated.','Packet projects only authenticated named globals and exact literal classes into controlled mirror storage; no real adjacency/native string-pool context equivalence claim.'])
    return packet,meta


def main():
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--limit',type=int);p.add_argument('--package-only',action='store_true');p.add_argument('--candidate',type=Path);p.add_argument('--source-dir',type=Path);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args();assert bool(args.recompiled)==bool(args.symbols);assert not(args.candidate and args.source_dir);assert not(args.recompiled and(args.candidate or args.source_dir));assert not args.source_dir or args.package_only;out=args.output.resolve();assert not out.exists();out.mkdir(parents=True)
 relevant=[Path(__file__),args.original]+[ROOT/'scripts'/n for n in('verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py')]+dependencies()
 if args.candidate:relevant.append(args.candidate)
 else:
  directory=args.source_dir or ROOT/'src/carma2/newcommon'
  relevant +=[directory/(stem+ext)for stem in('10-loading2','27-powerup','61-pedestrn','63-loading3','69-sound')for ext in('.c','.h')]
 if args.recompiled:relevant +=[args.recompiled,args.symbols,args.recompiled.with_suffix('.map')]
 before={str(p.resolve()):sha(p.read_bytes())for p in relevant};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'sources').mkdir();snapshots={}
 for i,p in enumerate(relevant):
  if p.suffix in('.py','.c','.h','.json'):
   dest=out/'sources'/f'{i}-{p.name}';dest.write_bytes(p.read_bytes());snapshots[str(p.resolve())]=str(dest.resolve())
 (out/'source-snapshots.json').write_text(json.dumps(snapshots,indent=2)+'\n');(out/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes())
 try:
  binary=args.original.read_bytes();assert sha(binary)==HASH;packet,graph=original_image(binary);(out/'original-code.bin').write_bytes(packet);native,relocation=native_source(args.candidate,args.source_dir);caller,_,_=registry.literals();registry.COMMON=COMMON
  matching=None
  if args.recompiled:
   matching_packet,matching=matching_image(args.recompiled,args.symbols);assert matching['initial_words']==graph['initial_words'];(out/'matching-code.bin').write_bytes(matching_packet)
  cases=fixtures();assert len(cases)==544 and sha(b''.join(payload for _,payload in cases))=='b91a15eb9bb2de5cca9a7f56032502be3ed83a0e650a4fff615338dbd6033c0f';indices=list(range(len(cases)));indices=indices[:args.limit]if args.limit else indices
  (out/'fixtures.bin').write_bytes(b''.join(cases[i][1]for i in indices));(out/'fixture-metadata.json').write_text(json.dumps([cases[i][0]for i in indices],indent=2)+'\n');(out/'literal-expression-relocation.json').write_text(json.dumps(relocation,indent=2)+'\n')
  if args.package_only:
   for name,data in [('native',native),('physical',COMMON+PHYSICAL),('caller',caller)]:(out/(name+'.c')).write_text(data)
   (out/'package.json').write_text(json.dumps(dict(stage='Package only; no compiler/game execution',graph=graph,matching=matching,fixture_sha256=[sha(cases[i][1])for i in indices],corpus_sha256=sha((out/'fixtures.bin').read_bytes()),source_before=before,relocation=relocation),indent=2)+'\n');print('PACKAGE',len(cases));return
  commands=registry.compile_sources(out,native,PHYSICAL,caller);stride=ARENA+20;results=[];events=[];failures=[];streams={n:(out/(n+'-outputs.bin')).open('wb')for n in('original','msvc5','msvc','mingw')+(('matching',)if matching else())}
  try:
   for index in indices:
    case,payload=cases[index];fixture=out/'case.bin';fixture.write_bytes(payload);reference=subprocess.run([str(out/'physical.exe'),str(fixture),str(out/'original-code.bin')],capture_output=True,timeout=20)
    assert reference.returncode==0 and len(reference.stdout)%stride==0 and len(reference.stdout)>=2*stride,(index,reference.returncode,len(reference.stdout),reference.stderr)
    expected=reference.stdout;streams['original'].write(expected);headers=[struct.unpack_from('<5I',expected,i)for i in range(0,len(expected),stride)]
    assert headers[0]==(2,0,0xffffffff,0,0)
    for event,h in enumerate(headers[1:],1):
     assert h[1]==event and h[2]==case['modes'][event-1] and (h[0]==0 and h[3:]==(0,0)or h[0]==3 and h[3]==0xc0000005 and event==len(headers)-1)
    terminal=headers[-1][0]==3;assert not terminal or case['qualified_controlled_permissions']
    if not terminal:assert len(headers)==len(case['modes'])+1
    events.append(dict(index=index,event_count=len(headers),normal_calls=sum(h[0]==0 for h in headers),terminal_av=terminal,fault_address=headers[-1][4]if terminal else None))
    for compiler in('msvc5','msvc','mingw')+(('matching',)if matching else()):
     command=[str(out/'physical.exe'),str(fixture),str(out/'matching-code.bin')]if compiler=='matching'else[str(out/(compiler+'.exe')),str(fixture)]
     actual=subprocess.run(command,capture_output=True,timeout=20);streams[compiler].write(actual.stdout);same=actual.returncode==0 and actual.stdout==expected
     row=dict(index=index,compiler=compiler,passed=same,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(actual.stdout),exit_code=actual.returncode,event_count=len(headers),terminal_av=terminal);results.append(row)
     if not same:
      prefix=out/f'failure-{compiler}-{index}';prefix.with_suffix('.original.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual.stdout);prefix.with_suffix('.json').write_text(json.dumps(dict(case=case,row=row,stderr=actual.stderr.hex()),indent=2)+'\n');failures.append(row)
    if len(results)%(200 if matching else 150)==0:print('checked',len(results)//(4 if matching else 3),len(results),'failures',len(failures),flush=True)
  finally:
   for s in streams.values():s.close()
  after={p:sha(Path(p).read_bytes())for p in before};assert before==after
  report=dict(passed=not failures,scenes=len(indices),comparisons=len(results),event_comparisons=sum(r['event_count']for r in events)*(4 if matching else 3),normal_scenes=sum(not r['terminal_av']for r in events),controlled_terminal_scenes=sum(r['terminal_av']for r in events),complete_suite=indices==list(range(len(cases))),source_stable=True,source_before=before,source_after=after,original_sha256=HASH,graph=graph,matching=matching,recompiled_sha256=matching["recompiled_sha256"]if matching else None,symbols_sha256=matching["symbols_sha256"]if matching else None,literal_expression_relocation=relocation,commands=commands,fixtures_sha256=sha((out/'fixtures.bin').read_bytes()),fixture_sha256=[sha(cases[i][1])for i in indices],events=events,results=results,failures=failures,limits=__doc__,live_handles=[],new_acceptance_credit=0,artifacts={str(p.resolve()):sha(p.read_bytes())for p in out.iterdir()if p.is_file()})
  (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),flush=True)
  if failures:raise SystemExit(1)
 finally:
  after={p:sha(Path(p).read_bytes())for p in before};(out/'guard-closure.json').write_text(json.dumps(dict(source_before=before,source_after=after,source_stable=before==after),indent=2)+'\n')
if __name__=='__main__':main()
