"""Complete107B activation consumer and genuine20-body6259B lower graph,
plus96 whole-transform translation and72 direct59B Translate cases.
Only two external activation callbacks are controlled; geometry/math callbacks
are actual original/production bodies. Full raw64KiB arenas and callback events,
normal ABI or real AV prefix, PC53 masked. No output normalization or partial
acceptance; private stack/sticky flags/concurrency/unmasked/exhaustive excluded.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG,CS_GRP_JUMP
from verify_opponent_route_reconstruction import raw,HASH,sha,words,pe
from verify_tinted_native_reconstruction import extract
from inventory_reconstruction_backlog import mask_c
import verify_phil_registry_reconstruction as registry
import verify_finteray_reconstruction as geometry
BASE=0x22000000;SIZE=65536
ENTRIES=[{'address': '0x4b71b0', 'name': 'PHILActivatePassive', 'bytes': 107, 'body_sha256': '4c205ee733bbc73da4d31466e02a2771bc0bd94909a6e1c03f06c01d124cd9c5', 'contiguous_decode': True, 'published': False, 'direct_calls': [{'site': '0x4b71f1', 'target': '0x4b6090'}, {'site': '0x4b7201', 'target': '0x4b6090'}], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [{'site': '0x4b71d1', 'instruction': 'call eax'}, {'site': '0x4b71e3', 'instruction': 'call eax'}], 'absolute_memory': [{'site': '0x4b71c6', 'target': '0x692db8', 'width': 4, 'instruction': 'mov eax, dword ptr [0x692db8]'}, {'site': '0x4b71d3', 'target': '0x6923e8', 'width': 4, 'instruction': 'mov eax, dword ptr [0x6923e8]'}], 'returns': [{'site': '0x4b71f8', 'cleanup': ''}, {'site': '0x4b7208', 'cleanup': ''}, {'site': '0x4b721a', 'cleanup': ''}]}, {'address': '0x4b6090', 'name': 'PHILMakeObjectActive', 'bytes': 259, 'body_sha256': 'd9c28c8d0afb60c423fa522bb576a487197b1ef7cdb30b652045deeb1ad7f1a1', 'contiguous_decode': True, 'published': True, 'direct_calls': [{'site': '0x4b617d', 'target': '0x4ff530'}], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x4b6090', 'target': '0x65d004', 'width': 4, 'instruction': 'mov eax, dword ptr [0x65d004]'}], 'returns': [{'site': '0x4b609f', 'cleanup': '8'}, {'site': '0x4b610f', 'cleanup': '8'}, {'site': '0x4b611c', 'cleanup': '8'}, {'site': '0x4b6186', 'cleanup': '8'}, {'site': '0x4b6190', 'cleanup': '8'}]}, {'address': '0x4ff530', 'name': 'ResetCarSpecialVolume', 'bytes': 152, 'body_sha256': '6f03662afaf51279ca5c799f223bc6cde639059782f3153b4abb5fb6c0495f4c', 'contiguous_decode': True, 'published': True, 'direct_calls': [{'site': '0x4ff54a', 'target': '0x45d5d0'}, {'site': '0x4ff567', 'target': '0x41e340'}, {'site': '0x4ff56c', 'target': '0x45d5c0'}], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x4ff575', 'target': '0x58b158', 'width': 4, 'instruction': 'fcomp dword ptr [0x58b158]'}, {'site': '0x4ff5ab', 'target': '0x761f44', 'width': 4, 'instruction': 'mov ebx, dword ptr [0x761f44]'}], 'returns': [{'site': '0x4ff5c7', 'cleanup': ''}]}, {'address': '0x45d5d0', 'name': 'DisablePlingMaterials', 'bytes': 11, 'body_sha256': '0c139e0a0136ce2237eebf00da617f72b27ba994b5df0ef653daf846e1cd468d', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x45d5d0', 'target': '0x5964c0', 'width': 4, 'instruction': 'mov dword ptr [0x5964c0], 0'}], 'returns': [{'site': '0x45d5da', 'cleanup': ''}]}, {'address': '0x41e340', 'name': 'FindFace', 'bytes': 31, 'body_sha256': '592403f8de970b7166610b28bfadf9acadd095a1cf4e72f3e989c1c03906fb72', 'contiguous_decode': True, 'published': True, 'direct_calls': [{'site': '0x41e357', 'target': '0x45ca60'}], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x41e351', 'target': '0x75bb60', 'width': 4, 'instruction': 'mov eax, dword ptr [0x75bb60]'}], 'returns': [{'site': '0x41e35c', 'cleanup': '0xc'}]}, {'address': '0x45d5c0', 'name': 'EnablePlingMaterials', 'bytes': 11, 'body_sha256': '27dd29bcb786726fabcbf209f701218f551bdc6f942efdfa6882ed2adb901001', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x45d5c0', 'target': '0x5964c0', 'width': 4, 'instruction': 'mov dword ptr [0x5964c0], 1'}], 'returns': [{'site': '0x45d5ca', 'cleanup': ''}]}, {'address': '0x45ca60', 'name': 'ActorFindFace', 'bytes': 262, 'body_sha256': 'ff7eabf5d868fcddb527b6e199f9c026fa4ead969208d6d1e94a544cff236f79', 'contiguous_decode': True, 'published': True, 'direct_calls': [{'site': '0x45ca7e', 'target': '0x532bd0'}, {'site': '0x45ca96', 'target': '0x45cb70'}], 'literal_code_pointers': [{'site': '0x45ca8a', 'target': '0x45cf20', 'instruction': 'push 0x45cf20'}], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x45ca74', 'target': '0x686190', 'width': 4, 'instruction': 'mov dword ptr [0x686190], 0x42c80000'}, {'site': '0x45ca9b', 'target': '0x686190', 'width': 4, 'instruction': 'fld dword ptr [0x686190]'}, {'site': '0x45caa7', 'target': '0x686190', 'width': 4, 'instruction': 'fld dword ptr [0x686190]'}, {'site': '0x45caad', 'target': '0x589fa8', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fa8]'}, {'site': '0x45cabe', 'target': '0x68618c', 'width': 4, 'instruction': 'mov edx, dword ptr [0x68618c]'}, {'site': '0x45cac4', 'target': '0x686194', 'width': 4, 'instruction': 'mov eax, dword ptr [0x686194]'}, {'site': '0x45cad5', 'target': '0x686188', 'width': 4, 'instruction': 'mov ecx, dword ptr [0x686188]'}, {'site': '0x45caf0', 'target': '0x68618c', 'width': 4, 'instruction': 'mov edx, dword ptr [0x68618c]'}, {'site': '0x45cafc', 'target': '0x686188', 'width': 4, 'instruction': 'mov edx, dword ptr [0x686188]'}, {'site': '0x45cb14', 'target': '0x68618c', 'width': 4, 'instruction': 'mov edx, dword ptr [0x68618c]'}, {'site': '0x45cb20', 'target': '0x686188', 'width': 4, 'instruction': 'mov edx, dword ptr [0x686188]'}, {'site': '0x45cb38', 'target': '0x68618c', 'width': 4, 'instruction': 'mov ecx, dword ptr [0x68618c]'}, {'site': '0x45cb58', 'target': '0x6861cc', 'width': 4, 'instruction': 'mov edx, dword ptr [0x6861cc]'}], 'returns': [{'site': '0x45cb63', 'cleanup': '0x14'}]}, {'address': '0x532bd0', 'name': 'BrMatrix34Inverse', 'bytes': 725, 'body_sha256': '4e540d0946543f2752c12f0bf0789d81d030a2c0bff39de571fbe8ebb466a8da', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x532bdd', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fld dword ptr [0x58b8bc]'}, {'site': '0x532be3', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fld dword ptr [0x58b8bc]'}, {'site': '0x532bef', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcomp dword ptr [0x58b8bc]'}, {'site': '0x532c1b', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcom dword ptr [0x58b8bc]'}, {'site': '0x532c37', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcom dword ptr [0x58b8bc]'}, {'site': '0x532c57', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcom dword ptr [0x58b8bc]'}, {'site': '0x532c77', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcom dword ptr [0x58b8bc]'}, {'site': '0x532c94', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcom dword ptr [0x58b8bc]'}, {'site': '0x532caf', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcomp dword ptr [0x58b8bc]'}, {'site': '0x532cc2', 'target': '0x58b8c0', 'width': 4, 'instruction': 'fcomp dword ptr [0x58b8c0]'}, {'site': '0x532cd3', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fld dword ptr [0x58b8bc]'}, {'site': '0x532ce1', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fcom dword ptr [0x58b8bc]'}, {'site': '0x532cf0', 'target': '0x58b8c4', 'width': 4, 'instruction': 'fcomp dword ptr [0x58b8c4]'}, {'site': '0x532cfd', 'target': '0x58b8bc', 'width': 4, 'instruction': 'fld dword ptr [0x58b8bc]'}, {'site': '0x532d05', 'target': '0x58b8b8', 'width': 4, 'instruction': 'fld dword ptr [0x58b8b8]'}], 'returns': [{'site': '0x532cda', 'cleanup': ''}, {'site': '0x532d04', 'cleanup': ''}, {'site': '0x532ea4', 'cleanup': ''}]}, {'address': '0x45cf20', 'name': 'FindHighestCallBack__finteray', 'bytes': 51, 'body_sha256': 'c2497326f1838afc899e62f03f8dcb95e36697841ae2bdfe025d7194f9e5c79d', 'contiguous_decode': True, 'published': True, 'direct_calls': [{'site': '0x45cf4b', 'target': '0x45cf60'}], 'literal_code_pointers': [{'site': '0x45cf31', 'target': '0x45d570', 'instruction': 'push 0x45d570'}], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [], 'returns': [{'site': '0x45cf52', 'cleanup': ''}]}, {'address': '0x45cb70', 'name': 'ActorRayPick2D', 'bytes': 531, 'body_sha256': '9699349f248c5c66a285c91f68cce3c4df242cb64b1f285d63e6d1ca845901b8', 'contiguous_decode': True, 'published': True, 'direct_calls': [{'site': '0x45cbe4', 'target': '0x531870'}, {'site': '0x45cbf6', 'target': '0x532bd0'}, {'site': '0x45cc0b', 'target': '0x5334a0'}, {'site': '0x45cc25', 'target': '0x533520'}, {'site': '0x45cc6f', 'target': '0x45cd90'}, {'site': '0x45cc9e', 'target': '0x45cb70'}, {'site': '0x45cce3', 'target': '0x45cd90'}, {'site': '0x45cd62', 'target': '0x45cb70'}], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [{'site': '0x45cd30', 'instruction': 'call dword ptr [esp + 0xc8]'}], 'absolute_memory': [{'site': '0x45ccec', 'target': '0x686190', 'width': 4, 'instruction': 'fld dword ptr [0x686190]'}, {'site': '0x45ccf2', 'target': '0x589fb0', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fb0]'}, {'site': '0x45cd11', 'target': '0x686190', 'width': 4, 'instruction': 'mov eax, dword ptr [0x686190]'}], 'returns': [{'site': '0x45cbcc', 'cleanup': '0x10'}, {'site': '0x45ccbe', 'cleanup': '0x10'}, {'site': '0x45cd80', 'cleanup': '0x10'}]}, {'address': '0x45d570', 'name': 'FindHighestPolyCallBack__finteray', 'bytes': 70, 'body_sha256': 'a13c45d82ce4aecde84e485232b96f007eea772c07d05509fc8627a87263a6c8', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x45d574', 'target': '0x686190', 'width': 4, 'instruction': 'fcomp dword ptr [0x686190]'}, {'site': '0x45d58d', 'target': '0x686190', 'width': 4, 'instruction': 'mov dword ptr [0x686190], eax'}, {'site': '0x45d596', 'target': '0x68618c', 'width': 4, 'instruction': 'mov dword ptr [0x68618c], ecx'}, {'site': '0x45d59c', 'target': '0x6861c8', 'width': 4, 'instruction': 'mov ecx, dword ptr [0x6861c8]'}, {'site': '0x45d5a2', 'target': '0x6861cc', 'width': 4, 'instruction': 'mov dword ptr [0x6861cc], edx'}, {'site': '0x45d5a8', 'target': '0x686188', 'width': 4, 'instruction': 'mov dword ptr [0x686188], eax'}, {'site': '0x45d5ad', 'target': '0x686194', 'width': 4, 'instruction': 'mov dword ptr [0x686194], ecx'}], 'returns': [{'site': '0x45d5b5', 'cleanup': ''}]}, {'address': '0x45cf60', 'name': 'DRModelPick2D__finteray', 'bytes': 1548, 'body_sha256': 'f5773f79d71ef6e575d2e3c436bce724d5d676005ac07020818e2e094084026c', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [{'site': '0x45d4e9', 'instruction': 'call dword ptr [esp + 0xb8]'}], 'absolute_memory': [{'site': '0x45cf67', 'target': '0x589fc8', 'width': 4, 'instruction': 'fsub dword ptr [0x589fc8]'}, {'site': '0x45cf84', 'target': '0x589fcc', 'width': 4, 'instruction': 'fsub dword ptr [0x589fcc]'}, {'site': '0x45d00b', 'target': '0x589fc4', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fc4]'}, {'site': '0x45d035', 'target': '0x5964c0', 'width': 4, 'instruction': 'mov eax, dword ptr [0x5964c0]'}, {'site': '0x45d052', 'target': '0x589fac', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fac]'}, {'site': '0x45d08e', 'target': '0x589fb0', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fb0]'}, {'site': '0x45d0a4', 'target': '0x589fbc', 'width': 4, 'instruction': 'fmul dword ptr [0x589fbc]'}, {'site': '0x45d28f', 'target': '0x589fac', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fac]'}, {'site': '0x45d2ac', 'target': '0x589fac', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fac]'}, {'site': '0x45d300', 'target': '0x589fac', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fac]'}, {'site': '0x45d31d', 'target': '0x589fac', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fac]'}, {'site': '0x45d336', 'target': '0x589fb0', 'width': 4, 'instruction': 'fcom dword ptr [0x589fb0]'}, {'site': '0x45d37a', 'target': '0x589fb0', 'width': 4, 'instruction': 'fld dword ptr [0x589fb0]'}, {'site': '0x45d413', 'target': '0x589fb0', 'width': 4, 'instruction': 'fsubr dword ptr [0x589fb0]'}, {'site': '0x45d431', 'target': '0x589fd4', 'width': 4, 'instruction': 'fmul dword ptr [0x589fd4]'}, {'site': '0x45d43d', 'target': '0x589fd4', 'width': 4, 'instruction': 'fsubr dword ptr [0x589fd4]'}, {'site': '0x45d455', 'target': '0x589fd4', 'width': 4, 'instruction': 'fmul dword ptr [0x589fd4]'}, {'site': '0x45d461', 'target': '0x589fd4', 'width': 4, 'instruction': 'fsubr dword ptr [0x589fd4]'}, {'site': '0x45d486', 'target': '0x589fb0', 'width': 4, 'instruction': 'fsubr dword ptr [0x589fb0]'}, {'site': '0x45d4aa', 'target': '0x6861c8', 'width': 4, 'instruction': 'mov dword ptr [0x6861c8], edx'}], 'returns': [{'site': '0x45d565', 'cleanup': '0x1c'}]}, {'address': '0x531870', 'name': 'BrTransformToMatrix34', 'bytes': 722, 'body_sha256': 'c35f0c591dd19ab2a4e14369aaaa949d4e38e2d160f3536e2b00e2d6cd644da5', 'contiguous_decode': True, 'published': True, 'direct_calls': [{'site': '0x5318ab', 'target': '0x5359a0'}, {'site': '0x5318d4', 'target': '0x531de0'}, {'site': '0x531905', 'target': '0x532a90'}, {'site': '0x531b34', 'target': '0x5327c0'}], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [{'site': '0x531887', 'instruction': 'jmp dword ptr [eax*4 + 0x531b44]'}], 'absolute_memory': [{'site': '0x531946', 'target': '0x58b880', 'width': 4, 'instruction': 'fcom dword ptr [0x58b880]'}, {'site': '0x531953', 'target': '0x58b888', 'width': 8, 'instruction': 'fdivr qword ptr [0x58b888]'}, {'site': '0x53197e', 'target': '0x58b890', 'width': 4, 'instruction': 'fld dword ptr [0x58b890]'}, {'site': '0x531984', 'target': '0x58b894', 'width': 4, 'instruction': 'fld dword ptr [0x58b894]'}, {'site': '0x531a28', 'target': '0x58b880', 'width': 4, 'instruction': 'fcom dword ptr [0x58b880]'}, {'site': '0x531a35', 'target': '0x58b888', 'width': 8, 'instruction': 'fdivr qword ptr [0x58b888]'}], 'returns': [{'site': '0x5318a1', 'cleanup': ''}, {'site': '0x5318ca', 'cleanup': ''}, {'site': '0x5318f3', 'cleanup': ''}, {'site': '0x531912', 'cleanup': ''}, {'site': '0x531b2e', 'cleanup': ''}, {'site': '0x531b41', 'cleanup': ''}]}, {'address': '0x5334a0', 'name': 'BrMatrix34ApplyP', 'bytes': 116, 'body_sha256': '0684a171263ff17a75be9f00d3afb3a41bc8fc558600c6e72da0453446b6999d', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [], 'returns': [{'site': '0x533513', 'cleanup': ''}]}, {'address': '0x533520', 'name': 'BrMatrix34ApplyV', 'bytes': 107, 'body_sha256': 'fa01f280a97449df3505c6bf2e6364541d2a94e3f7c04308df4d03754f06eee6', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [], 'returns': [{'site': '0x53358a', 'cleanup': ''}]}, {'address': '0x45cd90', 'name': 'PickBoundsTestRay__finteray', 'bytes': 391, 'body_sha256': '07edd5d2ee81eda1d210f27edb96828e67623af030907850fba595dca3aed11d', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x45cda3', 'target': '0x589fb4', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fb4]'}, {'site': '0x45cdb7', 'target': '0x589fb8', 'width': 4, 'instruction': 'fdivr dword ptr [0x589fb8]'}, {'site': '0x45cdd1', 'target': '0x589fbc', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fbc]'}, {'site': '0x45cde0', 'target': '0x589fbc', 'width': 4, 'instruction': 'fld dword ptr [0x589fbc]'}, {'site': '0x45ce04', 'target': '0x589fc0', 'width': 4, 'instruction': 'fcom dword ptr [0x589fc0]'}, {'site': '0x45ce38', 'target': '0x589fc4', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fc4]'}, {'site': '0x45ce48', 'target': '0x589fb8', 'width': 4, 'instruction': 'fdivr dword ptr [0x589fb8]'}, {'site': '0x45ce56', 'target': '0x589fc0', 'width': 4, 'instruction': 'fcom dword ptr [0x589fc0]'}, {'site': '0x45ce93', 'target': '0x589fbc', 'width': 4, 'instruction': 'fcomp dword ptr [0x589fbc]'}, {'site': '0x45cea2', 'target': '0x589fbc', 'width': 4, 'instruction': 'fld dword ptr [0x589fbc]'}], 'returns': [{'site': '0x45cf0b', 'cleanup': '0x14'}, {'site': '0x45cf14', 'cleanup': '0x14'}]}, {'address': '0x5359a0', 'name': 'BrQuatToMatrix34', 'bytes': 308, 'body_sha256': 'c068b5d37357b6a7a70c82858740ec93719bbd9b4907188c27d8f7bfb8869ed1', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x535a4d', 'target': '0x58b91c', 'width': 4, 'instruction': 'fsubr dword ptr [0x58b91c]'}, {'site': '0x535aa6', 'target': '0x58b91c', 'width': 4, 'instruction': 'fsubr dword ptr [0x58b91c]'}, {'site': '0x535aae', 'target': '0x58b91c', 'width': 4, 'instruction': 'fsubr dword ptr [0x58b91c]'}], 'returns': [{'site': '0x535ad3', 'cleanup': ''}]}, {'address': '0x531de0', 'name': 'BrEulerToMatrix34', 'bytes': 751, 'body_sha256': '0a833cd99564e3e7800fd5fc8a3b569e0c88effd1aa032936e598e0b2074b4d9', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [{'site': '0x531e49', 'target': '0x58b898', 'width': 8, 'instruction': 'fmul qword ptr [0x58b898]'}, {'site': '0x531e51', 'target': '0x58b898', 'width': 8, 'instruction': 'fmul qword ptr [0x58b898]'}, {'site': '0x531e69', 'target': '0x58b898', 'width': 8, 'instruction': 'fmul qword ptr [0x58b898]'}], 'returns': [{'site': '0x5320ce', 'cleanup': ''}]}, {'address': '0x532a90', 'name': 'BrMatrix34Translate', 'bytes': 59, 'body_sha256': 'ad54c35d3e1a77b5c4401d2102eb9b2f19b228c90ac9c12dcd18593df3d879fc', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [], 'returns': [{'site': '0x532aca', 'cleanup': ''}]}, {'address': '0x5327c0', 'name': 'BrMatrix34Identity', 'bytes': 47, 'body_sha256': 'f0f96092bf21eda049cd479b491502b3eb16594e19f4990e74fced2abdafcf9b', 'contiguous_decode': True, 'published': True, 'direct_calls': [], 'literal_code_pointers': [], 'external_branches': [], 'indirect_sites': [], 'absolute_memory': [], 'returns': [{'site': '0x5327ee', 'cleanup': ''}]}]
GLOBALS={0x6923e0:('gPHIL_registry','tPHIL_registry_storage',0x4000,2532),0x65d004:('gPHIL_disabled','int',0x5000,4),0x5964c0:('gPling_materials','int',0x5004,4),0x761f44:('gDefault_water_spec_vol_real','tSpecial_volume*',0x5008,4),0x75bb60:('gTrack_actor','br_actor*',0x500c,4),0x686188:('gNearest_face','int',0x5010,4),0x68618c:('gNearest_model','br_model*',0x5014,4),0x686190:('gNearest_T','br_scalar',0x5018,4),0x686194:('gNearest_face_group','int',0x501c,4),0x686198:('gPick_model_to_view__finteray','br_matrix34',0x5020,48),0x6861c8:('gTemp_group','int',0x5050,4),0x6861cc:('gNearest_actor','br_actor*',0x5054,4),0x66cba0:('OrderAxes','order_info',0x5100,128)}
OWNERS=['src/carma2/newcommon/60-phil.c','src/carma2/newcommon/55-volume.c','src/carma2/newcommon/00-car.c','src/carma2/newcommon/34-finteray.c','src/brender/core/math/matrix34.c','src/brender/core/math/transfrm.c','src/brender/core/math/quat.c','src/brender/core/math/angles.c','src/brender/core/math/vector.c']
NATIVE_ORDER=['PhilCopyWords', 'PHILMakeObjectActive', 'VolumeIdentifierHasLength', 'VolumeBelowSurfaceLimit', 'ResetCarSpecialVolume', 'FindFace', 'FinterayScalar', 'PickBoundsTestRay__finteray', 'ActorRayPick2D', 'DRModelPick2D__finteray', 'FindHighestPolyCallBack__finteray', 'FindHighestCallBack__finteray', 'ActorFindFace', 'EnablePlingMaterials', 'DisablePlingMaterials', 'BrMatrix34Identity', 'BrMatrix34Translate', 'Matrix34ApplyAdd', 'BrMatrix34ApplyP', 'BrMatrix34ApplyV', 'QuaternionScaleRaw', 'QuaternionMultiply', 'QuaternionAddScalar', 'QuaternionSubtractScalar', 'QuaternionScalar', 'BrQuatToMatrix34', 'BrMatrix34Inverse', 'ActivationInverseNaN', 'ActivationInverseAdd', 'ActivationInverseRead', 'ActivationInverseScalar', 'ActivationInverseWrite', 'BrEulerToMatrix34', 'ActivationEulerScalar', 'ActivationEulerCosine', 'ActivationEulerSine', 'BrTransformToMatrix34', 'ActivationTransformWord', 'ActivationTransformCopy', 'ActivationTransformRead', 'ActivationTransformScalar', 'ActivationTransformNegativeZero']
CONSUMER_CAPSULE='/* Full107B activation callback consumer; zero credit until full closure proof. */\n#include "60-phil.h"\n#include <stdint.h>\n#include <string.h>\n\ntypedef void (C2_HOOK_FASTCALL *tActivation_original_call)(tPhysics_object*, undefined4);\ntypedef int (C2_HOOK_FASTCALL *tActivation_permission_call)(tPhysics_object*, undefined4);\n#if defined(__GNUC__)\ntypedef tU32 tActivation_raw_word __attribute__((__may_alias__));\n#else\ntypedef tU32 tActivation_raw_word;\n#endif\n\nstatic tU32 C2_HOOK_CDECL ActivationWord(const void* base, tU32 offset) {\n    return *(volatile const tActivation_raw_word*)((tU32)(uintptr_t)base + offset);\n}\n\nvoid C2_HOOK_FASTCALL PHILActivatePassive(tPhysics_object* object, undefined4 captured_edx) {\n    tU32 header = ActivationWord(object, 0x240);\n    tU32 address;\n    tU32 callbacks;\n    tActivation_original_call original;\n    tActivation_permission_call permission;\n\n    if (header != 0 && ActivationWord((const void*)(uintptr_t)header, 8) == 1) {\n        address = *(volatile const tActivation_raw_word*)(const void*)&gPHIL_registry.original_activate_passive;\n        if (address != 0) {\n            memcpy(&original, &address, sizeof(original));\n            /* EDX is still the entry word at the first original call. */\n            original(object, captured_edx);\n        }\n        callbacks = *(volatile const tActivation_raw_word*)(const void*)&gPHIL_registry.callbacks;\n        address = ActivationWord((const void*)(uintptr_t)callbacks, 0x14);\n        if (address != 0) {\n            memcpy(&permission, &address, sizeof(permission));\n            /* Original explicitly restores the captured entry EDX here. */\n            if (permission(object, captured_edx) == 0) {\n                return;\n            }\n        }\n        PHILMakeObjectActive(object, NULL, NULL, 1);\n    } else {\n        *(volatile tActivation_raw_word*)((tU32)(uintptr_t)object + 0x19c) = ActivationWord(object, 0x19c) | 0x1000u;\n    }\n}\n'
PRODUCTION_CAPSULES=[{'owner': 'src/brender/core/math/matrix34.c', 'functions': ['BrMatrix34Inverse', 'ActivationInverseNaN', 'ActivationInverseAdd', 'ActivationInverseRead', 'ActivationInverseScalar', 'ActivationInverseWrite'], 'text': "/* Full original725B BrMatrix34Inverse candidate. C-first, zero credit.\n * Inputs remain live across column-major cofactor stores. The first product\n * alone is rounded before entering its accumulator; determinant is scalar,\n * subsequent arithmetic is PC53 wide. Original sign changes precede products.\n * Native IEEE/alias proof required before any production correction. */\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif\nstatic double C2_HOOK_CDECL Matrix34ApplyAdd(double a, double b);\n\nstatic int C2_HOOK_CDECL ActivationInverseNaN(double value) {\n    br_uint_32 bits[2];\n\n    memcpy(bits, &value, 8);\n    return (bits[1] & 0x7ff00000u) == 0x7ff00000u\n        && ((bits[1] & 0xfffffu) != 0 || bits[0] != 0);\n}\n\nstatic double C2_HOOK_CDECL ActivationInverseAdd(double left, double right) {\n    /* Root's complete816-case compound corpus retains326 modern SSE NaN\n     * preference failures. Both operands have already passed their original\n     * FLD/float arithmetic load. Use the existing elementary x87 add only\n     * for a paired unordered sum; finite C arithmetic remains PC53 exact. */\n    if (ActivationInverseNaN(left) && ActivationInverseNaN(right)) {\n        return Matrix34ApplyAdd(left, right);\n    }\n    return left + right;\n}\nstatic double C2_HOOK_CDECL ActivationInverseRead(const br_matrix34* matrix, unsigned word) {\n    const volatile br_scalar* scalar = (const volatile br_scalar*)((const char*)matrix + word * 4u);\n\n    return (double)*scalar;\n}\n\nstatic double C2_HOOK_CDECL ActivationInverseScalar(double value) {\n    union { br_scalar scalar; br_uint_32 bits; } rounded;\n    volatile br_uint_32 stored;\n\n    rounded.scalar = (br_scalar)value;\n    stored = rounded.bits;\n    rounded.bits = stored;\n    return rounded.scalar;\n}\n\nstatic void C2_HOOK_CDECL ActivationInverseWrite(br_matrix34* matrix, unsigned word, double value) {\n    volatile br_scalar* scalar = (volatile br_scalar*)((char*)matrix + word * 4u);\n\n    *scalar = (br_scalar)value;\n}\n\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif\nbr_scalar C2_HOOK_CDECL BrMatrix34Inverse(br_matrix34* B, const br_matrix34* A) {\n    double a, b, c, d;\n    double term, rounded_first, positive = 0.0, negative = 0.0;\n    volatile double determinant;\n    double absolute, reciprocal;\n    double x, y, z, first, second, third;\n    double held_b02, held_b12;\n\n    a = ActivationInverseRead(A, 4);\n    b = ActivationInverseRead(A, 0);\n    c = ActivationInverseRead(A, 8);\n    term = (a * b) * c;\n    rounded_first = ActivationInverseScalar(term);\n    if (term >= 0.0) {\n        positive = rounded_first;\n    } else {\n        negative = rounded_first;\n    }\n    a = ActivationInverseRead(A, 5);\n    b = ActivationInverseRead(A, 1);\n    c = ActivationInverseRead(A, 6);\n    term = (a * b) * c;\n    if (term >= 0.0) positive = ActivationInverseAdd(positive, term); else negative = ActivationInverseAdd(negative, term);\n    a = ActivationInverseRead(A, 2);\n    b = ActivationInverseRead(A, 3);\n    c = ActivationInverseRead(A, 7);\n    term = (a * b) * c;\n    if (term >= 0.0) positive = ActivationInverseAdd(positive, term); else negative = ActivationInverseAdd(negative, term);\n    a = ActivationInverseRead(A, 2);\n    b = ActivationInverseRead(A, 4);\n    c = ActivationInverseRead(A, 6);\n    term = -((a * b) * c);\n    if (term >= 0.0) positive = ActivationInverseAdd(positive, term); else negative = ActivationInverseAdd(negative, term);\n    a = ActivationInverseRead(A, 3);\n    b = ActivationInverseRead(A, 1);\n    c = ActivationInverseRead(A, 8);\n    term = -((a * b) * c);\n    if (term >= 0.0) positive = ActivationInverseAdd(positive, term); else negative = ActivationInverseAdd(negative, term);\n    a = ActivationInverseRead(A, 5);\n    b = ActivationInverseRead(A, 0);\n    c = ActivationInverseRead(A, 7);\n    term = -((a * b) * c);\n    if (term >= 0.0) positive = ActivationInverseAdd(positive, term); else negative = ActivationInverseAdd(negative, term);\n    determinant = ActivationInverseScalar(ActivationInverseAdd(positive, negative));\n    absolute = determinant;\n    if (!(absolute >= 0.0)) absolute = -absolute;\n    if (!(absolute > 2.384185791015625e-7)) return 0.f;\n    absolute = determinant / (positive - negative);\n    if (!(absolute >= 0.0)) absolute = -absolute;\n    if (!(absolute >= (double)(br_scalar)1.0e-15)) return 0.f;\n    reciprocal = 1.0 / determinant;\n\n    a = ActivationInverseRead(A, 5); b = ActivationInverseRead(A, 4);\n    c = ActivationInverseRead(A, 8); d = ActivationInverseRead(A, 7);\n    ActivationInverseWrite(B, 0, ActivationInverseAdd(b * c, (-a) * d) * reciprocal);\n    a = ActivationInverseRead(A, 3); b = ActivationInverseRead(A, 5);\n    c = ActivationInverseRead(A, 6); d = ActivationInverseRead(A, 8);\n    ActivationInverseWrite(B, 3, ActivationInverseAdd(b * c, (-a) * d) * reciprocal);\n    a = ActivationInverseRead(A, 4); b = ActivationInverseRead(A, 3);\n    c = ActivationInverseRead(A, 7); d = ActivationInverseRead(A, 6);\n    ActivationInverseWrite(B, 6, ActivationInverseAdd(b * c, (-a) * d) * reciprocal);\n    a = ActivationInverseRead(A, 1); b = ActivationInverseRead(A, 2);\n    c = ActivationInverseRead(A, 7); d = ActivationInverseRead(A, 8);\n    ActivationInverseWrite(B, 1, ActivationInverseAdd(b * c, (-a) * d) * reciprocal);\n    a = ActivationInverseRead(A, 2); b = ActivationInverseRead(A, 8);\n    c = ActivationInverseRead(A, 0); d = ActivationInverseRead(A, 6);\n    ActivationInverseWrite(B, 4, ActivationInverseAdd(b * c, (-a) * d) * reciprocal);\n    a = ActivationInverseRead(A, 0); b = ActivationInverseRead(A, 7);\n    c = ActivationInverseRead(A, 1); d = ActivationInverseRead(A, 6);\n    ActivationInverseWrite(B, 7, ActivationInverseAdd(c * d, (-a) * b) * reciprocal);\n    a = ActivationInverseRead(A, 2); b = ActivationInverseRead(A, 5);\n    c = ActivationInverseRead(A, 1); d = ActivationInverseRead(A, 4);\n    ActivationInverseWrite(B, 2, ActivationInverseAdd(c * b, (-a) * d) * reciprocal);\n    a = ActivationInverseRead(A, 0); b = ActivationInverseRead(A, 2);\n    c = ActivationInverseRead(A, 3); d = ActivationInverseRead(A, 5);\n    ActivationInverseWrite(B, 5, ActivationInverseAdd(c * b, (-a) * d) * reciprocal);\n    a = ActivationInverseRead(A, 1); b = ActivationInverseRead(A, 4);\n    c = ActivationInverseRead(A, 0); d = ActivationInverseRead(A, 3);\n    ActivationInverseWrite(B, 8, ActivationInverseAdd(b * c, (-a) * d) * reciprocal);\n\n    x = ActivationInverseRead(A, 9); z = ActivationInverseRead(A, 11); y = ActivationInverseRead(A, 10);\n    first = x * ActivationInverseRead(B, 0);\n    third = z * ActivationInverseRead(B, 6);\n    second = y * ActivationInverseRead(B, 3);\n    ActivationInverseWrite(B, 9, -ActivationInverseAdd(ActivationInverseAdd(first, third), second));\n    x = ActivationInverseRead(A, 9); a = ActivationInverseRead(B, 1);\n    y = ActivationInverseRead(A, 10); b = ActivationInverseRead(B, 4);\n    z = ActivationInverseRead(A, 11);\n    first = x * a; second = y * b; third = z * ActivationInverseRead(B, 7);\n    held_b02 = ActivationInverseRead(B, 2);\n    absolute = ActivationInverseAdd(ActivationInverseAdd(first, second), third);\n    held_b12 = ActivationInverseRead(B, 5);\n    ActivationInverseWrite(B, 10, -absolute);\n    x = ActivationInverseRead(A, 9); y = ActivationInverseRead(A, 10); z = ActivationInverseRead(A, 11);\n    first = x * held_b02; second = y * held_b12;\n    third = z * ActivationInverseRead(B, 8);\n    ActivationInverseWrite(B, 11, -ActivationInverseAdd(ActivationInverseAdd(first, second), third));\n    return (br_scalar)determinant;\n}\n"}, {'owner': 'src/brender/core/math/angles.c', 'functions': ['BrEulerToMatrix34', 'ActivationEulerScalar', 'ActivationEulerCosine', 'ActivationEulerSine'], 'text': "/* Complete751B original Euler conversion, C-first draft. Explicit scalar\n * slots reflect FST/FSTP instructions; the cosine of rounded i survives wide\n * through the final matrix product. Raw angle wrapping is unsigned16.\n * Actual FSIN/FCOS versus CRT precision remains a physical proof obligation. */\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif\nstatic double C2_HOOK_CDECL ActivationEulerScalar(double value) {\n    union { br_scalar scalar; br_uint_32 bits; } rounded;\n    volatile br_uint_32 stored;\n\n    rounded.scalar = (br_scalar)value;\n    stored = rounded.bits;\n    rounded.bits = stored;\n    return rounded.scalar;\n}\n\n/* C-first physical evidence euler-c-first-focus retains five modern-only\n * quarter-turn differences. The original FSIN/FCOS tiny residue differs\n * from the modern CRT. Keep only these elementary transcendental operations. */\nstatic double C2_HOOK_CDECL ActivationEulerCosine(double radians) {\n#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)\n    double result;\n    __asm {\n        fld qword ptr radians\n        fcos\n        fstp qword ptr result\n    }\n    return result;\n#else\n    return cos(radians);\n#endif\n}\n\nstatic double C2_HOOK_CDECL ActivationEulerSine(double radians) {\n#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)\n    double result;\n    __asm {\n        fld qword ptr radians\n        fsin\n        fstp qword ptr result\n    }\n    return result;\n#else\n    return sin(radians);\n#endif\n}\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif\n\nbr_matrix34* C2_HOOK_CDECL BrEulerToMatrix34(br_matrix34* mat, const br_euler* euler) {\n    const volatile br_euler* input = euler;\n    volatile br_matrix34* output = mat;\n    unsigned order = input->order;\n    unsigned i, j, h;\n    unsigned a0, a1, a2;\n    double ri, rj, rh;\n    double ci, cj, ch, si, sj, sh;\n    double ci_wide, ch_wide, si_wide, sh_wide;\n    double cc, cs, sc, ss;\n    const double radians_per_angle = 9.587379924285257e-5;\n\n    if (order & 0x10) {\n        i = input->c;\n        j = input->b;\n        h = input->a;\n    } else {\n        i = input->a;\n        j = input->b;\n        h = input->c;\n    }\n    if (order & 4) {\n        i = (0u - i) & 0xffffu;\n        j = (0u - j) & 0xffffu;\n        h = (0u - h) & 0xffffu;\n    }\n    ri = i * radians_per_angle;\n    rh = h * radians_per_angle;\n    rj = j * radians_per_angle;\n    ch_wide = ActivationEulerCosine(rh);\n    si_wide = ActivationEulerSine(ri);\n    sh_wide = ActivationEulerSine(rh);\n    ci_wide = ActivationEulerCosine(ActivationEulerScalar(ri));\n    cj = ActivationEulerScalar(ActivationEulerCosine(rj));\n    sj = ActivationEulerScalar(ActivationEulerSine(ActivationEulerScalar(rj)));\n    ci = ActivationEulerScalar(ci_wide);\n    ch = ActivationEulerScalar(ch_wide);\n    si = ActivationEulerScalar(si_wide);\n    sh = ActivationEulerScalar(sh_wide);\n    cc = ActivationEulerScalar(ch_wide * ci_wide);\n    cs = ActivationEulerScalar(sh_wide * ci_wide);\n    sc = ActivationEulerScalar(si_wide * ch);\n    ss = ActivationEulerScalar(sh * si);\n    /* ci's scalar spill is dead, while its wide register value remains live. */\n    (void)ci;\n    a0 = OrderAxes[order].a0;\n    a1 = OrderAxes[order].a1;\n    a2 = OrderAxes[order].a2;\n    if (order & 8) {\n        output->m[a0][a0] = (br_scalar)cj;\n        output->m[a1][a0] = (br_scalar)(sj * si);\n        output->m[a2][a0] = (br_scalar)(sj * ci_wide);\n        output->m[a0][a1] = (br_scalar)(sh * sj);\n        output->m[a1][a1] = (br_scalar)(cc - ss * cj);\n        output->m[a2][a1] = (br_scalar)(-(cs * cj) - sc);\n        output->m[a0][a2] = (br_scalar)(-(sj * ch));\n        output->m[a1][a2] = (br_scalar)(sc * cj + cs);\n        output->m[a2][a2] = (br_scalar)(cc * cj - ss);\n    } else {\n        output->m[a0][a0] = (br_scalar)(ch * cj);\n        output->m[a1][a0] = (br_scalar)(sc * sj - cs);\n        output->m[a2][a0] = (br_scalar)(cc * sj + ss);\n        output->m[a0][a1] = (br_scalar)(sh * cj);\n        output->m[a1][a1] = (br_scalar)(ss * sj + cc);\n        output->m[a2][a1] = (br_scalar)(cs * sj - sc);\n        output->m[a0][a2] = (br_scalar)(-sj);\n        output->m[a1][a2] = (br_scalar)(si * cj);\n        output->m[a2][a2] = (br_scalar)(cj * ci_wide);\n    }\n    output->m[3][2] = 0.f;\n    output->m[3][1] = 0.f;\n    output->m[3][0] = 0.f;\n    return mat;\n}\n"}, {'owner': 'src/brender/core/math/transfrm.c', 'functions': ['BrTransformToMatrix34', 'ActivationTransformWord', 'ActivationTransformCopy', 'ActivationTransformRead', 'ActivationTransformScalar', 'ActivationTransformNegativeZero'], 'text': '/* Full722B transform dispatch. C-first recovery of forward DWORD copy and\n * original LOOK_UP inline precision; no vector/math boundary substitute. */\n#if defined(__GNUC__)\ntypedef br_uint_32 tActivation_transform_word __attribute__((__may_alias__));\n#else\ntypedef br_uint_32 tActivation_transform_word;\n#endif\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif\nstatic br_uint_32 C2_HOOK_CDECL ActivationTransformWord(const void* base, unsigned offset) {\n    return *(volatile const tActivation_transform_word*)((br_uint_32)(uintptr_t)base + offset);\n}\nstatic void C2_HOOK_CDECL ActivationTransformCopy(br_matrix34* mat, const br_transform* xform, unsigned output_word, unsigned input_word) {\n    br_uint_32 value = ActivationTransformWord(xform, input_word * 4u);\n\n    *(volatile tActivation_transform_word*)((br_uint_32)(uintptr_t)mat + output_word * 4u) = value;\n}\nstatic double C2_HOOK_CDECL ActivationTransformRead(const br_transform* xform, unsigned word) {\n    return *(volatile const br_scalar*)((br_uint_32)(uintptr_t)xform + word * 4u);\n}\nstatic double C2_HOOK_CDECL ActivationTransformScalar(double value) {\n    union { br_scalar scalar; br_uint_32 bits; } rounded;\n    volatile br_uint_32 stored;\n\n    rounded.scalar = (br_scalar)value;\n    stored = rounded.bits;\n    rounded.bits = stored;\n    return rounded.scalar;\n}\nstatic double C2_HOOK_CDECL ActivationTransformNegativeZero(void) {\n    union { br_scalar scalar; br_uint_32 bits; } value;\n    volatile br_uint_32 stored = 0x80000000u;\n\n    /* MSVC5 folds the literal -0.0 to positive zero. Original FCHS changes\n     * both zero components of the fallback basis before their scalar stores. */\n    value.bits = stored;\n    return value.scalar;\n}\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif\n\nvoid C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34* mat, const br_transform* xform) {\n    volatile br_matrix34* output = mat;\n    unsigned type = *(volatile const br_uint_16*)xform;\n    unsigned word;\n    double x, y, z, norm, reciprocal;\n    double zx, zy, zz, ux_first, ux_second, uy_first, uy_second, uz_first, uz_second;\n    double cross_x, cross_y, cross_z, round_x, round_y, round_z;\n    double xx, xy, xz, yx, yy, yz;\n\n    switch (type) {\n    case BR_TRANSFORM_MATRIX34:\n    case BR_TRANSFORM_MATRIX34_LP:\n        for (word = 0; word < 12; word++) {\n            ActivationTransformCopy(mat, xform, word, word + 1);\n        }\n        return;\n    case BR_TRANSFORM_QUAT:\n        BrQuatToMatrix34(mat, &xform->t.quat.q);\n        ActivationTransformCopy(mat, xform, 9, 10);\n        ActivationTransformCopy(mat, xform, 10, 11);\n        ActivationTransformCopy(mat, xform, 11, 12);\n        return;\n    case BR_TRANSFORM_EULER:\n        BrEulerToMatrix34(mat, &xform->t.euler.e);\n        ActivationTransformCopy(mat, xform, 9, 10);\n        ActivationTransformCopy(mat, xform, 10, 11);\n        ActivationTransformCopy(mat, xform, 11, 12);\n        return;\n    case BR_TRANSFORM_TRANSLATION:\n        /* Original three PUSHes capture raw Z/Y/X before any output store. */\n        {\n            br_uint_32 bx, by, bz;\n            br_scalar tx, ty, tz;\n            bz = ActivationTransformWord(xform, 48);\n            by = ActivationTransformWord(xform, 44);\n            bx = ActivationTransformWord(xform, 40);\n            memcpy(&tx, &bx, 4); memcpy(&ty, &by, 4); memcpy(&tz, &bz, 4);\n            BrMatrix34Translate(mat, tx, ty, tz);\n        }\n        return;\n    case BR_TRANSFORM_LOOK_UP:\n        x = ActivationTransformRead(xform, 1);\n        y = ActivationTransformRead(xform, 2);\n        z = ActivationTransformRead(xform, 3);\n        norm = sqrt((x * ActivationTransformScalar(x) + y * y) + z * ActivationTransformScalar(z));\n        if (norm > 2.384185791015625e-7) {\n            reciprocal = 1.0 / norm;\n            x = ActivationTransformRead(xform, 1);\n            y = ActivationTransformRead(xform, 2);\n            z = ActivationTransformRead(xform, 3);\n            zx = ActivationTransformScalar(-(x * reciprocal));\n            zy = ActivationTransformScalar(-(y * reciprocal));\n            /* Original Z component is rounded before its sign change. */\n            zz = ActivationTransformScalar(-ActivationTransformScalar(z * reciprocal));\n        } else {\n            zx = -1.0;\n            zy = ActivationTransformNegativeZero();\n            zz = ActivationTransformNegativeZero();\n        }\n        /* Retain the original first-fault address as well as the individual\n         * loads before any public store. The duplicate operands serve different\n         * cross products in the x87 stack, and are all loaded while input is live. */\n        uy_first = ActivationTransformRead(xform, 5);\n        uz_first = ActivationTransformRead(xform, 6);\n        uz_second = ActivationTransformRead(xform, 6);\n        ux_first = ActivationTransformRead(xform, 4);\n        ux_second = ActivationTransformRead(xform, 4);\n        uy_second = ActivationTransformRead(xform, 5);\n        cross_x = uy_first * zz - uz_first * zy;\n        round_x = ActivationTransformScalar(cross_x);\n        cross_y = uz_second * zx - ux_first * zz;\n        round_y = ActivationTransformScalar(cross_y);\n        cross_z = ux_second * zy - uy_second * zx;\n        round_z = ActivationTransformScalar(cross_z);\n        norm = ActivationTransformScalar(sqrt((cross_x * round_x + cross_y * round_y) + cross_z * round_z));\n        if (norm > 2.384185791015625e-7) {\n            reciprocal = 1.0 / norm;\n            xx = ActivationTransformScalar(reciprocal * round_x);\n            xy = ActivationTransformScalar(reciprocal * round_y);\n            xz = ActivationTransformScalar(reciprocal * round_z);\n        } else {\n            xx = 1.0; xy = 0.0; xz = 0.0;\n        }\n        yx = zy * xz - zz * xy;\n        yy = zz * xx - zx * xz;\n        yz = zx * xy - zy * xx;\n        output->m[0][0] = (br_scalar)xx;\n        output->m[1][1] = (br_scalar)yy;\n        output->m[0][1] = (br_scalar)xy;\n        output->m[0][2] = (br_scalar)xz;\n        output->m[1][2] = (br_scalar)yz;\n        output->m[1][0] = (br_scalar)yx;\n        output->m[2][0] = (br_scalar)zx;\n        output->m[2][1] = (br_scalar)zy;\n        output->m[2][2] = (br_scalar)zz;\n        ActivationTransformCopy(mat, xform, 9, 10);\n        ActivationTransformCopy(mat, xform, 10, 11);\n        ActivationTransformCopy(mat, xform, 11, 12);\n        return;\n    case BR_TRANSFORM_IDENTITY:\n        BrMatrix34Identity(mat);\n        return;\n    }\n}\n'}]
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
/* Public brender.h still declares non-const input; own production transfrm.c
 * uses const. Rename only that unused header declaration, preserving body ABI. */
#define BrTransformToMatrix34 ActivationUnusedLegacyTransformDeclaration
#include "60-phil.h"
#include "55-volume.h"
#include "00-car.h"
#include "34-finteray.h"
#include "globvars.h"
#include "brender/br_defs.h"
#include "rec2_macros.h"
#include "c2_math.h"
#undef BrTransformToMatrix34
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#define M(x,y) mat->m[x][y]
#undef BR_MAC3
#undef BR_MAC4
#define BR_MAC3(a,b,c,d,e,f) ((a)*(b)+(c)*(d)+(e)*(f))
#define BR_MAC4(a,b,c,d,e,f,g,h) ((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))
static unsigned char*arena;static unsigned addresses[5],header[9];
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern double builder_wide;
extern unsigned __cdecl BuilderInvoke(void);
typedef char activation_layout[sizeof(void*)==4&&sizeof(tPHIL_registry_storage)==2532&&offsetof(tPHIL_registry_storage,callbacks)==8&&offsetof(tPHIL_registry_storage,original_activate_passive)==2520&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,field_0x240)==0x240&&sizeof(br_actor)==100&&sizeof(br_model)==92&&sizeof(v11group)==36&&sizeof(DR_FACE)==28&&sizeof(DR_VERTEX)==32?1:-1];
'''
for a,(n,t,o,z)in GLOBALS.items():COMMON+=f'#define {n} '+(f'((order_info*)(arena+{o}))'if n=='OrderAxes'else f'(*({t}*)(arena+{o}))')+'\n'
COMMON+=r'''
static void writeword(unsigned o,unsigned v){memcpy(arena+o,&v,4);}
static void event(unsigned code,unsigned object,unsigned edx,unsigned result){unsigned h[4];h[0]=code;h[1]=object;h[2]=edx;h[3]=result;fwrite(h,16,1,stdout);fwrite(arena,65536,1,stdout);fflush(stdout);}
static LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;fprintf(stderr,"AV access-kind %lu\n",(unsigned long)e->ExceptionRecord->ExceptionInformation[0]);event(9,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1],0);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}
static void mutate(unsigned mask,tPhysics_object*object){unsigned o=(unsigned)object-0x22000000;
if(mask&1)writeword(0x5000,1);
if(mask&2)writeword(o+0x240,0x22002100);
if(mask&4)arena[o+0x239]=2;
if(mask&8)writeword(o+0x240,0);
if(mask&16)writeword(0x4008,0x22002840);
if(mask&32)writeword(0x49d8,0);
if(mask&64)writeword(o+0x18,0);
if(mask&128)writeword(0x4008,0);
if(mask&256)writeword(0x2814,0);
if(mask&512)writeword(0x500c,0);
if(mask&1024)writeword(0x5008,0x22005a00);
if(mask&2048)writeword(0x2008,0);
if(mask&4096)writeword(o+0x19c,0x7f812345);
if(mask&8192)writeword(0x2814,3);
}
static void C2_HOOK_FASTCALL WorldBoundary(tPhysics_object*object,unsigned edx){event(1,(unsigned)object,edx,0);mutate(header[4],object);event(2,(unsigned)object,edx,0);}
static int C2_HOOK_FASTCALL PermissionBoundary1(tPhysics_object*object,unsigned edx){unsigned result=header[3];event(3,(unsigned)object,edx,1);mutate(header[5],object);event(4,(unsigned)object,edx,result);return(int)result;}
static int C2_HOOK_FASTCALL PermissionBoundary2(tPhysics_object*object,unsigned edx){unsigned result=header[3]^0x80000000u;event(3,(unsigned)object,edx,2);mutate(header[5],object);event(4,(unsigned)object,edx,result);return(int)result;}
static int run(const char*path){FILE*f;FARPROC p;unsigned char*callback;unsigned i,v,target,relative,result;volatile float scalar_result;typedef void*(WINAPI*add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));add_handler handler;
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;
if(fread(header,sizeof(header),1,f)!=1||fread(arena,65536,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);
callback=VirtualAlloc((void*)0x22100000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(callback!=(void*)0x22100000)return 13;
for(i=0;i<3;i++){target=i==0?(unsigned)WorldBoundary:i==1?(unsigned)PermissionBoundary1:(unsigned)PermissionBoundary2;callback[i*16]=0xe9;relative=target-((unsigned)callback+i*16+5);memcpy(callback+i*16+1,&relative,4);}FlushInstructionCache(GetCurrentProcess(),callback,4096);
p=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&p,4);if(!handler||!handler(1,observe_fault))return 14;
/* Public pointers already contain canonical callback addresses in fixture. */
if(header[6]>4)return 16;v=header[0];builder_address=addresses[header[6]];builder_mode=header[6]==3?8:header[6]==1?0:1;builder_x=v?0x22000000+v:0;builder_y=header[6]?(header[1]?0x22000000+header[1]:0):header[1];builder_poison=header[8];builder_wide=0.0;
#if defined(REC2_MATCHING)
builder_cdecl=0;builder_stack_count=0;
#else
builder_cdecl=1;builder_stack_count=2;builder_stack[0]=builder_x;builder_stack[1]=builder_y;
#endif
if(header[6]){builder_cdecl=1;builder_stack_count=2;builder_stack[0]=builder_x;builder_stack[1]=builder_y;}
if(header[6]==4){builder_cdecl=1;builder_stack_count=4;builder_stack[0]=builder_x;builder_stack[1]=header[1];builder_stack[2]=header[2];builder_stack[3]=header[3];}
_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 15;result=builder_result;if(header[6]==3){scalar_result=(float)builder_wide;memcpy(&result,(const void*)&scalar_result,4);}event(0,0,0,result);return 0;}
'''
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[8],i,offset,value;unsigned char*image;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0])return 3;
image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;
for(i=0;i<h[6];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[7];memcpy(image+offset,&value,4);}fclose(f);
FlushInstructionCache(GetCurrentProcess(),image,h[0]);for(i=0;i<5;i++)addresses[i]=(unsigned)image+h[i+1];return run(argv[1]);}
'''

def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;roots={int(r['address'],16):r for r in ENTRIES};bodies=[];refs=[];globalrefs=[];constants={};literalrefs=[];indirect=[]
 def mutable(a,z):
  return next(((base,n,t,o,size)for base,(n,t,o,size)in GLOBALS.items()if base<=a and a+z<=base+size),None)
 for a,r in roots.items():
  code=raw(binary,a,r['bytes']);assert sha(code)==r['body_sha256'];ins=list(cs.disasm(code,a));assert sum(i.size for i in ins)==len(code);starts={i.address for i in ins}
  for i in ins:
   if i.mnemonic=='call':
    if i.operands[0].type==CS_OP_IMM:assert i.operands[0].imm in roots
    else:assert i.address in(0x4b71d1,0x4b71e3,0x45cd30,0x45d4e9);indirect.append(dict(site=i.address,instruction=i.mnemonic+' '+i.op_str))
   if CS_GRP_JUMP in i.groups:
    if i.operands[0].type==CS_OP_IMM:assert i.operands[0].imm in starts
    else:assert i.address==0x531887
   for op in i.operands:
    if op.type==CS_OP_IMM and i.mnemonic in('mov','push'):
     if op.imm in roots:assert i.imm_size==4;refs.append(i.address+i.imm_offset);literalrefs.append(dict(site=i.address,target=op.imm))
     m=mutable(op.imm,1)
     if m:
      assert i.imm_size==4;base,n,t,o,size=m;globalrefs.append(dict(site=i.address+i.imm_offset,original=op.imm,canonical=BASE+o+op.imm-base,name=n,form='immediate-address'))
    if op.type==CS_OP_MEM and (not op.mem.base or mutable(op.mem.disp,op.size)):
     address=op.mem.disp;assert i.disp_size==4
     if op.mem.index and address==0x531b44:assert i.address==0x531887;refs.append(i.address+i.disp_offset);continue
     m=mutable(address,op.size)
     if m:
      base,n,t,o,size=m;globalrefs.append(dict(site=i.address+i.disp_offset,original=address,canonical=BASE+o+address-base,name=n))
     else:
      assert address not in roots;constants[(address,op.size)]=raw(binary,address,op.size);refs.append(i.address+i.disp_offset)
  bodies.append(dict(name=r['name'],address=a,bytes=len(code),sha256=sha(code),instructions=[[i.address,i.bytes.hex(),i.mnemonic,i.op_str]for i in ins]))
 table=raw(binary,0x531b44,28);targets=struct.unpack('<7I',table);assert targets==(0x53188e,0x53188e,0x5318a2,0x5318cb,0x531913,0x5318f4,0x531b2f)
 low=min(roots);high=max([a+r['bytes']for a,r in roots.items()]+[a+z for a,z in constants]+[0x531b60]);image=bytearray(b'\xcc'*(high-low))
 for a,r in roots.items():image[a-low:a-low+r['bytes']]=raw(binary,a,r['bytes'])
 for(a,z),v in constants.items():image[a-low:a-low+z]=v
 image[0x531b44-low:0x531b60-low]=table;refs.extend(0x531b44+4*i for i in range(7))
 for r in globalrefs:struct.pack_into('<I',image,r['site']-low,r['canonical'])
 assert len(set(refs))==len(refs)
 return words([len(image),0x4b71b0-low,0x531de0-low,0x531870-low,0x532bd0-low,0x532a90-low,len(refs),low])+image+words([a-low for a in refs]),dict(bodies=bodies,total_bytes=6259,readonly=[dict(address=a,bytes=z,hex=v.hex())for(a,z),v in constants.items()],globals=globalrefs,absolute_relocations=refs,literal_code_pointers=literalrefs,indirect_sites=indirect,jump_table=dict(address=0x531b44,targets=targets),low=low,high=high)

def native_source(candidate,overrides):
 files={p:(ROOT/p).read_text()for p in OWNERS};definitions={};order=[]
 for path in overrides:files[str(path)]=path.read_text()
 if True:
  assert all(set(re.findall(r"\bC2_HOOK_\w+\s+(\w+)\(",path.read_text()))=={"BrMatrix34Translate"}for path in overrides),"Only complete Translate override allowed"
  # Keep the frozen conditional typedef/inline policy while materializing all
  # arithmetic/public bodies from the current genuine production definitions.
  for row in PRODUCTION_CAPSULES:
   material=row['text'];production=files[row['owner']]
   for name in row['functions']:
    old=extract(row['text'],name);current=extract(production,name);assert material.count(old)==1
    material=material.replace(old,current)
   if 'tActivation_transform_word'in material:
    typedef=material[material.index('#if defined(__GNUC__)'):material.index('#if defined(_MSC_VER)')]
    assert typedef in production,'Production raw-word alias policy differs'
   files['capsule:'+row['owner']]=material

 for p,s in files.items():
  for m in re.finditer(r'^[^\n;{}]*\bC2_HOOK_\w+\s+(\w+)\([^;{}\n]*\)\s*\{',mask_c(s),re.M):
   name=m[1];assert name not in definitions or p in {str(x)for x in overrides}or p.startswith('capsule:');definitions[name]=(p,extract(s,name))
 needed={r['name']for r in ENTRIES if r['name']!='PHILActivatePassive'}
 todo=list(needed)
 while todo:
  name=todo.pop();p,body=definitions[name]
  for called in re.findall(r'\b(\w+)\s*\(',mask_c(body)):
   if called in definitions and called not in needed:needed.add(called);todo.append(called)
 # Genuine additional C helper boundaries, no rewritten lower semantic code.
 for p in list(files):
  for name,(owner,body)in definitions.items():
   if owner==p and name in needed:order.append(name)
 order=[name for name in NATIVE_ORDER if name in needed]+[name for name in order if name not in NATIVE_ORDER]
 prototypes=[];blocks=[];bindings=[]
 for name in order:
  owner,body=definitions[name];prototypes.append(body[:body.index('{')].rstrip()+';')
  if owner not in {str(x)for x in overrides}and not owner.startswith('capsule:'):blocks.append(body)
  bindings.append(dict(name=name,owner=owner,sha256=sha(body.encode())))
 blocks.extend(path.read_text()for path in overrides)
 blocks.extend(files['capsule:'+row['owner']]for row in PRODUCTION_CAPSULES)
 if candidate.name=='candidate.c':consumer=candidate.read_text()
 else:
  consumer=CONSUMER_CAPSULE
  for name in ['ActivationWord','PHILActivatePassive']:
   consumer=consumer.replace(extract(CONSUMER_CAPSULE,name),extract(candidate.read_text(),name))
  declarations=CONSUMER_CAPSULE[CONSUMER_CAPSULE.index('typedef void'):CONSUMER_CAPSULE.index('static tU32')]
  assert declarations in candidate.read_text(),'Production activation raw callback/word policies differ'
 return COMMON+'\n'+'\n'.join('#undef '+n for n in order)+'\n'+'\n'.join(prototypes)+'\n'+'\n'.join(blocks)+'\n'+consumer+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)PHILActivatePassive;addresses[1]=(unsigned)BrEulerToMatrix34;addresses[2]=(unsigned)BrTransformToMatrix34;addresses[3]=(unsigned)BrMatrix34Inverse;addresses[4]=(unsigned)BrMatrix34Translate;return run(argv[1]);}\n',bindings

def fixtures(binary):
 cases=[]
 def add(status=1,info=0x2000,type=1,worldcb=1,permission=1,result=1,disabled=0,m1=0,m2=0,shape=None,edx=0x7f812345,object=0x100,flags=0xa5a5a5a5,patches=(),label='normal'):
  opts=dict(empty=True,no_model=True,axis=1,sign=1,planes=[50,100],plane_constant=50,vertices3d=[[-5,50,-5],[5,50,-5],[-5,50,5]],min=[-100,-100,-100],max=[100,100,100])
  if shape:opts.update(shape)
  old=geometry.make_case(opts)['data'];arena=bytearray(random.Random(len(cases)+1).randbytes(SIZE));arena[0x6000:0xb000]=old
  def put(o,*v):struct.pack_into('<'+'I'*len(v),arena,o,*[x&0xffffffff for x in v])
  # Rebase the authentic retained geometry fixture, preserving all raw values.
  for o in geometry.pointer_slots():
   value=struct.unpack_from('<I',old,o)[0]
   if geometry.HEAP<=value<geometry.HEAP+len(old):put(0x6000+o,BASE+0x6000+value-geometry.HEAP)
  arena[0x100:0x100+1240]=bytes(1240);put(0x118,BASE+0x5c00);put(0x29c,flags);arena[0x339]=type;put(0x340,BASE+info if info else 0)
  arena[0x5c00:0x5c64]=bytes(100);put(0x5c00+80,0,0,0)
  for h in(0x2000,0x2100):arena[h:h+160]=bytes(160);put(h+4,0xa5a5a5a5);put(h+8,status)
  arena[0x4000:0x49e4]=bytes(2532);put(0x4008,BASE+0x2800);put(0x49d8,0x22100000 if worldcb else 0)
  arena[0x2800:0x2880]=bytes(128);put(0x2814,0x22100010 if permission else 0);put(0x2854,0x22100020)
  put(0x5000,disabled,1,BASE+0x5800,BASE+0x6100,0xdec00001,0xdec00002,0xdec00003,0xdec00004)
  arena[0x5020:0x5050]=b'\xa5'*48;put(0x5050,0xdec00005,0xdec00006);arena[0x5100:0x5180]=raw(binary,0x66cba0,128)
  for at,v in patches:put(at,v)
  case=dict(status=status,info=info,type=type,worldcb=worldcb,permission=permission,result=result,disabled=disabled,m1=m1,m2=m2,shape=shape,edx=edx,object=object,flags=flags,patches=patches,label=label)
  cases.append((case,words([object,edx,worldcb,result,m1,m2,0,len(cases)+1,0xcccccccc])+arena))
 for status in(0,1,2,3,0xffffffff,0x7f800001):
  for info in(0,0x2000):
   for type in(0,1,2,255):add(status,info,type,label='entry-gates')
 for worldcb in(0,1):
  for permission in(0,1):
   for result in(0,1,2,0x80000000,0xffffffff):
    for disabled in(0,1):add(worldcb=worldcb,permission=permission,result=result,disabled=disabled,label='callback-return-disabled')
 for m in[1<<i for i in range(14)]+[3,6,12,17,24,65,129,160,257,4097]:
  for result in(0,1):
   for stage in(1,2):add(m1=m if stage==1 else 0,m2=m if stage==2 else 0,result=result,label='callback-live-mutation')
 for edx in(0,1,3,0xffffffff,0x22000100,0x22004008,0x7f800001,0xffc12345):add(edx=edx,label='captured-opaque-EDX')
 for shape in[dict(empty=False,no_model=False),dict(empty=False,no_model=False,identifier='!water'),dict(empty=False,no_model=False,identifier='#water'),dict(empty=False,no_model=False,identifier=''),dict(empty=False,no_model=False,none=True),dict(empty=False,no_model=False,nested=True),dict(empty=False,no_model=False,null_material=True),dict(empty=False,no_model=False,no_groups=True)]:add(shape=shape,label='genuine-model-world')
 for transform_type in range(7):add(shape=dict(empty=False,no_model=False,transform=True,transform_type=transform_type),label='genuine-transform-'+str(transform_type))
 add(object=0,label='authentic-null-object');add(patches=((0x340,3),),label='authentic-invalid-header');add(patches=((0x4008,0),),label='authentic-null-callback-table');add(patches=((0x2814,3),),label='authentic-invalid-callback');add(patches=((0x500c,0),),label='authentic-null-world')
 def leaf(mode,x,y,data,label):
  add(label=label);case,payload=cases[-1];h=bytearray(payload);struct.pack_into('<I',h,0,x);struct.pack_into('<I',h,4,y);struct.pack_into('<I',h,24,mode)
  for at,v in data:h[36+at:36+at+len(v)]=v
  case.update(mode=mode,destination=x,input=y,raw_data=[(o,v.hex())for o,v in data]);cases[-1]=(case,bytes(h))
 for order in range(32):
  for angles in[(0,0,0),(1000,2000,7000),(0,0x4000,0x8000),(0xffff,1,0x1234)]:leaf(1,0xc000,0xc100,[(0xc100,struct.pack('<3HBx',*angles,order))],'whole-Euler')
 for transform in range(7):
  data=bytearray(52);struct.pack_into('<H',data,0,transform);data[4:52]=struct.pack('<12f',1,0,0,0,1,0,0,0,1,0,0,0)
  if transform==3:data[4:12]=struct.pack('<3HBx',1000,2000,7000,0)
  leaf(2,0xc000,0xc100,[(0xc100,bytes(data))],'whole-transform')
 identity=struct.pack('<12f',1,0,0,0,1,0,0,0,1,0,0,0)
 for delta in[0,1,4,8,12,-4,-1,48]:leaf(3,0xc100+delta,0xc100,[(0xc100,identity)],'whole-inverse-alias')
 for scale in[0,1e-8,2.384185791015625e-7,2.384185933124172e-7,1e-6,1]:leaf(3,0xc000,0xc100,[(0xc100,struct.pack('<12f',scale,0,0,0,1,0,0,0,1,0,0,0))],'whole-inverse-cutoff')
 # Strengthening appends preserve every initial fixture index and payload seed.
 for header_alias in[0x294,0x5000-8,0x4000,0x280c]:
  for type in(1,2):add(info=header_alias,type=type,patches=((header_alias+8,1),),label='consumer-header-status-alias')
 for mutation in[0,16,256,4096]:add(flags=0x22100010,patches=((0x4008,BASE+0x100+0x188),),m1=mutation,label='consumer-permission-slot-object-flags-alias')
 for at in[0x140,0x4000,0x5000,0x2800,0x2000]:add(object=at,label='consumer-raw-object-global-alias')
 for field,values in[(0x5c00+80,[0,0x80000000,0x7f800001,0x7fc12345,0x7f800000,0xff800000]),(0x6000+geometry.FACES+16,[0,0x80000000,0x7f800001,0x7fc12345,0x7f800000]),(0x6000+geometry.FACES+24,[0x42480000,0x42c80000,0x7f800001,0x7fc12345])]:
  for value in values:add(shape=dict(empty=False,no_model=False,identifier='!water'),patches=((field,value),),label='consumer-real-world-IEEE')
 for angles in[(1,2,3),(1000,2000,7000),(0x4000,0x8000,0xc000)]:
  for order in[0,4,8,16,24,31]:
   patch=struct.pack('<3HBx',*angles,order)
   add(shape=dict(empty=False,no_model=False,transform=True,transform_type=3,identifier='!water'),patches=tuple((0x61ac+4*j,struct.unpack_from('<I',patch,4*j)[0])for j in range(2)),label='consumer-real-Euler-world')
 special=[0,0x80000000,1,0x80000001,0x00800000,0x3f800000,0xbf800000,0x40000000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345]
 for word in range(12):
  for value in special:
   matrix=bytearray(identity);struct.pack_into('<I',matrix,word*4,value);leaf(3,0xc000,0xc100,[(0xc100,bytes(matrix))],'whole-inverse-singleIEEE')
 for delta in range(-47,48):
  if delta in[0,1,4,8,12,-4,-1]:continue
  leaf(3,0xc100+delta,0xc100,[(0xc100,struct.pack('<12f',2,.25,-.125,-.5,1,.375,.125,-.25,1,4,-3,2))],'whole-inverse-partial-byte-alias')
 for seed in range(96):
  rng=random.Random(0x725000+seed);values=[rng.randint(-32,32)/8 for j in range(12)];leaf(3,0xc000,0xc100,[(0xc100,struct.pack('<12f',*values))],'whole-inverse-finite-cancellation')
 for delta in range(-7,8):
  for order in[0,8,16,24,31]:leaf(1,0xc100+delta,0xc100,[(0xc100,struct.pack('<3HBx',0x1234,0xfedc,0xabcd,order))],'whole-Euler-byte-alias')
 for seed in range(128):
  rng=random.Random(0x751000+seed);angles=[rng.randrange(65536)for j in range(3)];leaf(1,0xc000,0xc100,[(0xc100,struct.pack('<3HBx',*angles,seed%32))],'whole-Euler-random-angles')
 for kind in[2,4]:
  count=4 if kind==2 else 9
  for word in range(count):
   for value in special:
    transform=bytearray(52);struct.pack_into('<H',transform,0,kind)
    if kind==2:struct.pack_into('<4f',transform,4,0,0,0,1)
    else:struct.pack_into('<9f',transform,4,0,0,1,0,1,0,0,0,0)
    struct.pack_into('<I',transform,4+word*4,value);leaf(2,0xc000,0xc100,[(0xc100,bytes(transform))],'whole-transform-IEEE-'+str(kind))
 for kind in range(7):
  for delta in[-47,-4,-1,0,1,4,12,47]:
   transform=bytearray(52);struct.pack_into('<H',transform,0,kind);transform[4:52]=identity
   if kind==3:transform[4:12]=struct.pack('<3HBx',1000,2000,7000,0)
   leaf(2,0xc100+4+delta,0xc100,[(0xc100,bytes(transform))],'whole-transform-partial-byte-alias')
 leaf(2,0xc000,0xfff0,[(0xfff0,struct.pack('<H',4)),(0xfff4,struct.pack('<3f',0,0,1))],'whole-transform-authentic-first-up-fault')
 # Exact816 root-probe payloads, appended after all1255 historical indices.
 compound_deltas=[None,-47,-36,-12,-4,-1,0,1,4,8,12,24,32,36,40,44,47]
 compound_linear=[(1,0,0,0,1,0,0,0,1),(2,.25,-.125,-.5,1,.375,.125,-.25,1)]
 compound_pairs=[(0x7fc12345,0x7f800001),(0x7f800001,0x7fc12345),(0xffc23456,0x7fc12345),(0x7fc12345,0xffc23456),(0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7f800000,0xff800000),(0xff800000,0x7f800000)]
 compound_index=0
 for matrix_index in range(2):
  for slots in[(9,10),(9,11),(10,11)]:
   for pair in compound_pairs:
    for delta in compound_deltas:
     arena=bytearray(random.Random(0x725c000+compound_index).randbytes(SIZE));struct.pack_into('<12f',arena,0xc100,*compound_linear[matrix_index],4,-3,2)
     for slot,value in zip(slots,pair):struct.pack_into('<I',arena,0xc100+4*slot,value)
     output=0xc000 if delta is None else 0xc100+delta
     case=dict(mode=3,label='whole-inverse-root-compound',upstream_index=compound_index,matrix_index=matrix_index,slots=slots,pair=pair,delta=delta)
     payload=words([output,0xc100,0,0,0,0,3,compound_index,0xcccccccc])+arena
     cases.append((case,payload));compound_index+=1
 assert compound_index==816
 return cases

def matching_image(exe,symbols_path,entries,globals_spec,native_capsules):
 data=exe.read_bytes();symbols=json.loads(symbols_path.read_text());assert symbols['original_sha256']==HASH and symbols['recompiled_sha256']==sha(data)
 mapped={}
 for row in symbols['symbols']:
  if row['orig']in mapped:assert mapped[row['orig']]==row['recomp'],row
  mapped[row['orig']]=row['recomp']
 mapbytes=exe.with_suffix('.map').read_bytes();text=mapbytes.decode('latin1');functions={};variables={}
 for section,name,a,flag,owner in re.findall(r'^\s*(\d{4}):[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(f\s+)?(.+)$',text,re.M):
  (functions if flag else variables).setdefault(int(a,16),[]).append((name,owner.strip()))
 owners={'PHILActivatePassive':'60-phil.c.obj','PHILMakeObjectActive':'60-phil.c.obj','ResetCarSpecialVolume':'55-volume.c.obj','FindFace':'00-car.c.obj','DisablePlingMaterials':'34-finteray.c.obj','EnablePlingMaterials':'34-finteray.c.obj','ActorFindFace':'34-finteray.c.obj','ActorRayPick2D':'34-finteray.c.obj','DRModelPick2D__finteray':'34-finteray.c.obj','FindHighestCallBack__finteray':'34-finteray.c.obj','FindHighestPolyCallBack__finteray':'34-finteray.c.obj','PickBoundsTestRay__finteray':'34-finteray.c.obj','BrMatrix34Inverse':'libbrender:matrix34.c.obj','BrMatrix34ApplyP':'libbrender:matrix34.c.obj','BrMatrix34ApplyV':'libbrender:matrix34.c.obj','BrMatrix34Translate':'libbrender:matrix34.c.obj','BrMatrix34Identity':'libbrender:matrix34.c.obj','BrTransformToMatrix34':'libbrender:transfrm.c.obj','BrEulerToMatrix34':'libbrender:angles.c.obj','BrQuatToMatrix34':'libbrender:quat.c.obj'}
 public={mapped[int(row['address'],16)]:(row['name'],owners[row['name']])for row in entries};assert len(public)==20 and set(public)<=set(functions)
 root_orig=[0x4b71b0,0x531de0,0x531870,0x532bd0,0x532a90];roots=[mapped[a]for a in root_orig]
 def undecorate(name):return name.lstrip('_@').split('@')[0]
 private_owners={'ActivationWord':'60-phil.c.obj','PhilCopyWords':'60-phil.c.obj','VolumeIdentifierHasLength':'55-volume.c.obj','VolumeBelowSurfaceLimit':'55-volume.c.obj','FinterayScalar':'34-finteray.c.obj','Matrix34ApplyAdd':'libbrender:matrix34.c.obj'}
 for prefix,owner in [('ActivationInverse','libbrender:matrix34.c.obj'),('ActivationEuler','libbrender:angles.c.obj'),('ActivationTransform','libbrender:transfrm.c.obj'),('Quaternion','libbrender:quat.c.obj')]:
  for row in native_capsules:
   if row['name'].startswith(prefix):private_owners[row['name']]=owner
 nt,base,size,tab=pe(data);regions=[]
 for k in range(struct.unpack_from('<H',data,nt+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',data,tab+40*k+8);flags=struct.unpack_from('<I',data,tab+40*k+36)[0];regions.append((base+rva,max(vs,n),flags))
 globalmap={};fallback=[]
 for old,(name,ctype,offset,width)in globals_spec.items():
  candidates=[a for a,aliases in variables.items()if any(undecorate(n)==name for n,o in aliases)]
  if old in mapped:address=mapped[old];assert not candidates or address in candidates,(name,candidates,address)
  else:assert len(candidates)==1,(name,candidates);address=candidates[0];fallback.append(dict(name=name,original=old,address=address,map_aliases=variables[address]))
  assert any(lo<=address and address+width<=lo+n and not flags&0x20000000 for lo,n,flags in regions),(name,hex(address),width)
  globalmap[address]=(old,name,offset,width)
 # Only fixture-backed mutable ranges are remapped; unknown static objects reject.
 def mutable(address,width):
  matches=[(lo,*row)for lo,row in globalmap.items()if lo<=address and address+width<=lo+row[3]];assert len(matches)<=1
  return matches[0]if matches else None
 def canonical(address,width):
  m=mutable(address,width);assert m
  lo,old,name,offset,size=m;return 0x22000000+offset+address-lo
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 todo=list(public);seen=set();instructions={};refs=set();constants={};mutations={};graph=[];edges=[];tables=[];callbacks=[];literal=[]
 cleanup={mapped[int(row['address'],16)]:next(iter({int(r['cleanup'],0)if r['cleanup']else 0 for r in row['returns']}))for row in entries}
 for row in entries:assert len({int(r['cleanup'],0)if r['cleanup']else 0 for r in row['returns']})==1,row['name']
 table_targets=set()
 def family(register):
  n=cs.reg_name(register)
  for key,aliases in [('eax',['eax','ax','al','ah']),('ebx',['ebx','bx','bl','bh']),('ecx',['ecx','cx','cl','ch']),('edx',['edx','dx','dl','dh']),('esi',['esi','si']),('edi',['edi','di']),('ebp',['ebp','bp']),('esp',['esp','sp'])]:
   if n in aliases:return key
  return n
 while todo:
  start=todo.pop()
  if start in seen:continue
  seen.add(start);aliases=functions[start]
  if start in public:
   name,owner=public[start];assert any(undecorate(n)==name and o==owner for n,o in aliases),(name,aliases)
  else:
   allowed=[(n,o)for n,o in aliases if private_owners.get(undecorate(n))==o or o.startswith('LIBC:')and undecorate(n)in {'sqrt','CIsqrt','cos','sin','CIcos','CIsin','memcpy'}]
   assert allowed,('Unclassified actual callee',hex(start),aliases);name=undecorate(allowed[0][0]);owner=allowed[0][1];cleanup[start]=0
  end=min(a for a in functions if a>start);pending=[start];local={};decoded={};indirect=[]
  while pending:
   at=pending.pop();known={};previous=[]
   while at not in local:
    assert start<=at<end,(name,hex(at));i=next(cs.disasm(raw(data,at,15),at));assert at+i.size<=end
    assert not any(at<a<at+i.size or a<at<a+len(b)for a,b in local.items()),('Overlapping actual instructions',name,hex(at))
    local[at]=bytes(i.bytes);decoded[at]=i;instructions[at]=bytes(i.bytes)
    writes={family(r)for r in i.regs_access()[1]};tag=None
    if i.mnemonic=='mov'and i.operands[0].type==CS_OP_REG and i.operands[0].size==4 and i.operands[1].type==CS_OP_MEM:
     op=i.operands[1];m=mutable(op.mem.disp,4)if not op.mem.base and not op.mem.index else None
     if m and m[2]=='gPHIL_registry':
      delta=op.mem.disp-m[0]
      if delta==2520:tag=('world',at,[(at,i.op_str)])
      if delta==8:tag=('registry-table',at,[(at,i.op_str)])
     elif op.mem.base and not op.mem.index and op.mem.disp==20 and known.get(family(op.mem.base),('',))[0]=='registry-table':
      old=known[family(op.mem.base)];tag=('permission',old[1],old[2]+[(at,i.op_str)])
    for reg in writes:known.pop(reg,None)
    if tag:known[family(i.operands[0].reg)]=tag
    if i.mnemonic=='call'or i.group(CS_GRP_JUMP):
     op=i.operands[0]
     if op.type==CS_OP_IMM:
      target=op.imm
      if start<=target<end:
       if i.mnemonic=='call':assert target==start;todo.append(target)
       else:pending.append(target)
      else:assert target in functions,(name,i.op_str);todo.append(target);edges.append(dict(function=start,site=at,target=target,kind=i.mnemonic))
     elif i.mnemonic=='jmp':
      assert name=='BrTransformToMatrix34'and op.type==CS_OP_MEM and not op.mem.base and op.mem.index and op.mem.scale==4 and i.disp_size==4
      assert len(previous)>=2;compare,branch=previous[-2:]
      assert compare.mnemonic=='cmp'and compare.operands[0].type==CS_OP_REG and compare.operands[0].reg==op.mem.index and compare.operands[1].type==CS_OP_IMM and compare.operands[1].imm==6
      assert branch.mnemonic=='ja'and branch.operands[0].type==CS_OP_IMM and start<=branch.operands[0].imm<end
      targets=list(struct.unpack('<7I',raw(data,op.mem.disp,28)));assert all(start<=t<end for t in targets)
      refs.add(at+i.disp_offset);constants[op.mem.disp,28]=raw(data,op.mem.disp,28)
      for j,target in enumerate(targets):refs.add(op.mem.disp+j*4);table_targets.add(op.mem.disp+j*4);pending.append(target)
      tables.append(dict(function=start,site=at,compare=compare.address,guard=branch.address,address=op.mem.disp,count=7,targets=targets))
     elif op.type==CS_OP_REG:
      assert name=='PHILActivatePassive'and family(op.reg)in known,(name,hex(at),i.op_str,known)
      value=known[family(op.reg)];assert value[0]in('world','permission')
      indirect.append(dict(function=start,site=at,kind=value[0],load=value[1],load_chain=value[2],register=family(op.reg),contract='Live canonical registry world/permission callback; genuine fixture callback pointer and call retained'))
     else:
      assert name in('ActorRayPick2D','DRModelPick2D__finteray')and op.type==CS_OP_MEM and not op.mem.index and cs.reg_name(op.mem.base)in('esp','ebp'),(name,hex(at),i.op_str)
      indirect.append(dict(function=start,site=at,kind='genuine-geometry-callback-parameter',base=cs.reg_name(op.mem.base),disp=op.mem.disp,entry_stack_offset=16 if name=='ActorRayPick2D'else 24,contract='Actual callback parameter; incoming immutable geometry callback targets are transported'))
    for op in i.operands:
     value=None
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      if i.mnemonic=='jmp'and name=='BrTransformToMatrix34':continue
      assert i.disp_size==4;value=op.mem.disp;site=at+i.disp_offset;width=op.size
     elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
      assert i.imm_size==4;value=op.imm;site=at+i.imm_offset;width=1
     if value is not None:
      m=mutable(value,width)
      if m:mutations[site]=canonical(value,width)
      elif value in functions:
       assert op.type==CS_OP_IMM and value in public and public[value][0]in('FindHighestCallBack__finteray','FindHighestPolyCallBack__finteray'),(name,i.op_str)
       refs.add(site);todo.append(value);literal.append(dict(function=start,site=at,patch=site,target=value))
      else:
       region=next(r for r in regions if r[0]<=value and value+width<=r[0]+r[1]);assert not region[2]&0xa0000020,(name,i.op_str)
       assert op.type==CS_OP_MEM and not op.mem.base and not op.mem.index and width in(4,8,10),('Unclassified readonly operand',name,i.op_str)
       constants[value,width]=raw(data,value,width);refs.add(site)
    if i.mnemonic=='call':known.clear()
    if i.mnemonic.startswith('ret'):
     assert ([o.imm for o in i.operands]or[0])==[cleanup[start]],(name,i.op_str,cleanup[start]);break
    assert i.mnemonic not in('int','int3'),('Unexpected terminal instruction',name,hex(at))
    previous.append(i)
    if i.mnemonic=='jmp':break
    at+=i.size
  # Prove canonical callback load dominance; branch-entry bypasses reject.
  for call in indirect:
   if 'load'in call:
    for i in decoded.values():
     if i.group(CS_GRP_JUMP)and i.operands[0].type==CS_OP_IMM and call['load']<i.operands[0].imm<=call['site']:assert call['load']<=i.address<=call['site'],('Registry load bypass',name,call,i.op_str)
  callbacks.extend(indirect)
  graph.append(dict(name=name,address=start,end=end,map_aliases=aliases,instructions=[[a,b.hex()]for a,b in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(b for a,b in sorted(local.items())))))
 # Stack provenance is evaluated after every callee and RET cleanup is known.
 frame_states={}
 for call in callbacks:
  if call['kind']!='genuine-geometry-callback-parameter':continue
  row=next(r for r in graph if r['address']==call['function']);ins={a:next(cs.disasm(bytes.fromhex(b),a))for a,b in row['instructions']}
  states={};queue=[(row['address'],0,None)]
  while queue:
   at,sp,ebp=queue.pop()
   if at in states:assert states[at]==(sp,ebp),('Inconsistent callback frame',row['name'],hex(at),states[at],(sp,ebp));continue
   states[at]=(sp,ebp);i=ins[at]
   if i.mnemonic=='push':sp=None if sp is None else sp-4
   elif i.mnemonic=='pop':
    if cs.reg_name(i.operands[0].reg)=='ebp':ebp=None
    sp=None if sp is None else sp+4
   elif i.mnemonic in('sub','add')and i.operands[0].type==CS_OP_REG and cs.reg_name(i.operands[0].reg)=='esp':
    assert i.operands[1].type==CS_OP_IMM;sp=None if sp is None else sp+(-1 if i.mnemonic=='sub'else 1)*i.operands[1].imm
   elif i.mnemonic=='mov'and i.operands[0].type==CS_OP_REG:
    dest=cs.reg_name(i.operands[0].reg);source=cs.reg_name(i.operands[1].reg)if i.operands[1].type==CS_OP_REG else''
    if dest=='ebp':ebp=sp if source=='esp'else None
    if dest=='esp':sp=ebp if source=='ebp'else None
   elif i.mnemonic=='lea'and i.operands[0].type==CS_OP_REG and cs.reg_name(i.operands[0].reg)=='esp':
    op=i.operands[1];anchor=ebp if cs.reg_name(op.mem.base)=='ebp'else sp if cs.reg_name(op.mem.base)=='esp'else None;sp=anchor+op.mem.disp if anchor is not None and not op.mem.index else None
   elif 'esp'in {cs.reg_name(r)for r in i.regs_access()[1]}and i.mnemonic not in('call','ret'):sp=None
   if at==call['site']:
    anchor=sp if call['base']=='esp'else ebp;assert anchor is not None and anchor+call['disp']==call['entry_stack_offset'],('Callback does not load genuine argument',row['name'],call,sp,ebp)
    call['frame_provenance']=dict(sp_delta=sp,ebp_delta=ebp,computed_entry_offset=anchor+call['disp'])
   if i.mnemonic=='call'and i.operands[0].type==CS_OP_IMM:sp=None if sp is None else sp+cleanup[i.operands[0].imm]
   if i.mnemonic.startswith('ret'):continue
   if i.group(CS_GRP_JUMP):
    assert i.operands[0].type==CS_OP_IMM
    if row['address']<=i.operands[0].imm<row['end']:queue.append((i.operands[0].imm,sp,ebp))
    if i.mnemonic=='jmp':continue
   nxt=at+i.size;assert nxt in ins;queue.append((nxt,sp,ebp))
  assert 'frame_provenance'in call
  frame_states[call['function']]=states
 assert {public[r['target']][0]for r in literal}=={'FindHighestCallBack__finteray','FindHighestPolyCallBack__finteray'}
 geometry_provenance=[];proved_incoming=set()
 # Authenticate literal callback argument slots and recursive raw forwarding.
 for pointer in literal:
  row=next(r for r in graph if r['address']==pointer['function']);ins={a:next(cs.disasm(bytes.fromhex(b),a))for a,b in row['instructions']};load=ins[pointer['site']]
  assert load.mnemonic=='push'and load.operands[0].type==CS_OP_IMM and load.operands[0].imm==pointer['target']
  target_name='ActorRayPick2D'if public[pointer['target']][0]=='FindHighestCallBack__finteray'else'DRModelPick2D__finteray';target=next(a for a,(n,o)in public.items()if n==target_name)
  at=load.address+load.size;pushes=0;span=[]
  while True:
   i=ins[at];span.append(at)
   if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm==target;break
   assert not i.group(CS_GRP_JUMP)and i.mnemonic not in('pop','ret')
   if i.mnemonic=='push':pushes+=1
   else:
    assert 'esp'not in{cs.reg_name(r)for r in i.regs_access()[1]}
    assert not any(op.type==CS_OP_MEM and cs.reg_name(op.mem.base)=='esp'and operand_index==0 and i.mnemonic in('mov','fst','fstp')for operand_index,op in enumerate(i.operands))
   at+=i.size
  offset=12 if target_name=='ActorRayPick2D'else 20;assert pushes*4==offset
  for i in ins.values():
   if i.group(CS_GRP_JUMP)and i.operands[0].type==CS_OP_IMM and load.address<i.operands[0].imm<=at:assert load.address<=i.address<=at,('Bypassed literal callback PUSH',row['name'],i.op_str)
  proved_incoming.add(at);geometry_provenance.append(dict(kind='immutable-literal-call-argument',function=row['address'],load=load.address,call=at,target=target,callback_target=pointer['target'],caller_stack_offset=offset,intervening_pushes=pushes))
 actor=next(a for a,(n,o)in public.items()if n=='ActorRayPick2D');row=next(r for r in graph if r['address']==actor);ins={a:next(cs.disasm(bytes.fromhex(b),a))for a,b in row['instructions']}
 for at,i in ins.items():
  if i.mnemonic!='call'or i.operands[0].type!=CS_OP_IMM or i.operands[0].imm!=actor:continue
  preceding=[ins[a]for a in sorted(ins)if a<at];pushes=[j for j in preceding[-12:]if j.mnemonic=='push'];assert len(pushes)==4
  push=pushes[0];assert push.operands[0].type==CS_OP_REG;reg=family(push.operands[0].reg)
  prior=[j for j in preceding if j.address<push.address];load=next(j for j in reversed(prior)if reg in{family(r)for r in j.regs_access()[1]})
  assert load.mnemonic=='mov'and load.operands[0].type==CS_OP_REG and family(load.operands[0].reg)==reg and load.operands[1].type==CS_OP_MEM
  op=load.operands[1];assert not op.mem.index and cs.reg_name(op.mem.base)=='esp';sp,_=frame_states[actor][load.address];assert sp is not None and sp+op.mem.disp==16
  for j in ins.values():
   if j.group(CS_GRP_JUMP)and j.operands[0].type==CS_OP_IMM and load.address<j.operands[0].imm<=at:assert load.address<=j.address<=at,('Bypassed recursive callback load',j.op_str)
  proved_incoming.add(at);geometry_provenance.append(dict(kind='recursive-raw-callback-forward',function=actor,load=load.address,push=push.address,call=at,entry_stack_offset=16,source_sp_delta=sp,source_disp=op.mem.disp,caller_stack_offset=12))
 geometry_targets={actor,next(a for a,(n,o)in public.items()if n=='DRModelPick2D__finteray')}
 incoming={a for row in graph for a,b in row['instructions']for i in[ next(cs.disasm(bytes.fromhex(b),a)) ]if i.mnemonic=='call'and i.operands[0].type==CS_OP_IMM and i.operands[0].imm in geometry_targets}
 assert incoming==proved_incoming and len(geometry_provenance)==4,('Unproved incoming geometry callback',incoming-proved_incoming)
 for table in tables:
  row=next(r for r in graph if r['address']==table['function']);starts={a for a,b in row['instructions']};assert set(table['targets'])<=starts
 assert len(callbacks)==4 and sorted(r['kind']for r in callbacks)==['genuine-geometry-callback-parameter','genuine-geometry-callback-parameter','permission','world']
 assert len(tables)==1 and tables[0]['count']==7
 active={a+k for a,b in instructions.items()for k in range(len(b))};rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);characteristics=struct.unpack_from('<H',data,nt+22)[0];cursor=base+rr;checked=[]
 while cursor<base+rr+rz:
  page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8 and n%2==0 and cursor+n<=base+rr+rz
  for w in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
   site=base+page+(w&4095);kind=w>>12
   if kind and(any(site+k in active for k in range(4))or site in table_targets):
    assert kind==3 and site in refs|set(mutations),('Unclassified actual relocation',hex(site),kind)
    assert site in table_targets or all(site+k in active for k in range(4)),('Partial active operand relocation',hex(site));checked.append(site)
  cursor+=n
 if rz:assert rr and not characteristics&1 and set(checked)==refs|set(mutations);relocation_contract='Complete bidirectional HIGHLOW equality against all classified instruction/table patch sites'
 else:assert (rr,rz)==(0,0)and characteristics&1;relocation_contract='COFF RELOCS_STRIPPED plus exact empty directory: no HIGHLOW coverage claim. Every patch is classified decoded operand/table entry with actual owner/range; reverse relocation equality applies only to relocatable images.'
 for site in refs|set(mutations):assert site in table_targets or all(site+k in active for k in range(4)),('Patch outside decoded operand',hex(site))
 low=min([*instructions,*[a for a,n in constants]]);high=max([a+len(b)for a,b in instructions.items()]+[a+n for a,n in constants]);image=bytearray(b'\xcc'*(high-low))
 for a,b in instructions.items():image[a-low:a-low+len(b)]=b
 for(a,n),blob in constants.items():assert not any(a+k in active for k in range(n));image[a-low:a-low+n]=blob
 for site,value in mutations.items():struct.pack_into('<I',image,site-low,value)
 refs=sorted(refs);packet=words([len(image),*[a-low for a in roots],len(refs),low])+image+words([a-low for a in refs])
 return packet,dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(mapbytes),original_roots=root_orig,roots=roots,public_entries=[dict(address=a,name=n,owner=o)for a,(n,o)in public.items()],graph=graph,edges=edges,callbacks=callbacks,geometry_callback_provenance=geometry_provenance,jump_tables=tables,literal_code_pointers=literal,global_fallbacks=fallback,globals=[dict(address=lo,original=row[0],name=row[1],canonical_base=0x22000000+row[2],bytes=row[3])for lo,row in globalmap.items()],global_patches=[dict(site=site,canonical=value)for site,value in sorted(mutations.items())],constants=[dict(address=a,bytes=n,hex=b.hex())for(a,n),b in sorted(constants.items())],absolute_relocations=refs,active_relocations_checked=checked,coff_characteristics=characteristics,relocation_directory=[rr,rz],relocation_contract=relocation_contract,normalization=[],low=low,high=high)

def translate_upper_cases():
 special=[0,0x80000000,1,0x80000001,0x00800000,0x3f800001,0xbf800001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345]
 cases=[]
 def add(label,words=(0x3f800000,0xc0000000,0x40400000),destination=0xc000,input=0xc100):
  arena=bytearray(random.Random(0x532a9000+len(cases)).randbytes(65536));transform=bytearray(52);struct.pack_into('<H',transform,0,5);struct.pack_into('<3I',transform,40,*words)
  if input:
   for j,value in enumerate(transform):
    if 0<=input+j<65536:arena[input+j]=value
  payload=struct.pack('<9I',destination&0xffffffff,input&0xffffffff,0,0,0,0,2,len(cases),0xcccccccc)+arena
  cases.append((dict(label=label,words=list(words),destination=destination,input=input),payload))
 for axis in range(3):
  for value in special:
   words=[0x3f800000,0xc0000000,0x40400000];words[axis]=value;add('raw-translation-IEEE',tuple(words))
 for delta in [-47,-36,-12,-4,-1,0,1,4,12,24,32,36,40,44,47]:
  for words in[(0x3f800000,0xc0000000,0x40400000),(0x7f800001,0xff800002,0x7fc12345)]:add('raw-translation-overlap',words,0xc100+4+delta)
 for output in[0,0xfffffff8,0xfffffffc,0xffc0,0xffd0,0xffd4,0xffd8,0xffdc,0xffe0,0xffe4,0xffe8,0xffec,0xfff0,0xfff4,0xfff8,0xfffc]:add('authentic-output-fault',destination=output)
 for input in[0,0xffd0,0xffd4,0xfff0,0xfffc]:add('authentic-translation-input-fault',input=input)
 return cases

def translate_direct_cases():
 cases=[]
 def add(label,bits=(0x3f800000,0xc0000000,0x40400000),destination=0xc000):
  arena=random.Random(0x59c200+len(cases)).randbytes(65536);payload=words([destination&0xffffffff,*bits,0,0,4,len(cases),0xcccccccc])+arena;cases.append((dict(label=label,bits=list(bits),destination=destination),payload))
 special=[0,0x80000000,1,0x80000001,0x00800000,0x3f800001,0xbf800001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345]
 for lane in range(3):
  for value in special:
   bits=[0x3f800000,0xc0000000,0x40400000];bits[lane]=value;add('direct-raw-IEEE',tuple(bits))
 for bits in[(0x7f800001,0xff800002,0x7fc12345),(0xffc12345,0x7f800001,0x80000000)]:add('direct-paired-raw-IEEE',bits)
 for offset in[1,3,4,7,12,16,32,47]:add('direct-unaligned-ordinary',destination=0xc000+offset)
 for output in[0,0xffffffe0,0xfffffff8,0xfffffffc,0xffc0,0xffd0,0xffd4,0xffd8,0xffdc,0xffe0,0xffe4,0xffe8,0xffec,0xfff0,0xfff4,0xfff8,0xfffc]:add('direct-authentic-output-fault',destination=output)
 return cases

baseline_fixtures=fixtures
def fixtures(binary):
 cases=baseline_fixtures(binary);assert len(cases)==2071
 upper=translate_upper_cases();direct=translate_direct_cases()
 assert(len(upper),len(direct))==(96,72)
 cases+=upper+direct;assert len(cases)==2239
 return cases

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--lower-override',type=Path,action='append',default=[]);p.add_argument('--candidate-source',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols);out=a.output.resolve();assert not out.exists();out.mkdir(parents=True)
 source=a.candidate_source.resolve()if a.candidate_source else ROOT/'src/carma2/newcommon/60-phil.c';script=Path(__file__).resolve();paths=[source,script]+[ROOT/p for p in OWNERS]+[ROOT/'src/carma2/newcommon/49-physics.c']+list((ROOT/'src').rglob('*.h'))+[ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/inventory_reconstruction_backlog.py',ROOT/'scripts/verify_finteray_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py']
 paths+=[ROOT/'build-reference/originals/CARMA2_HW.EXE']
 paths+=a.lower_override
 if a.recompiled:paths += [a.recompiled,a.symbols,a.recompiled.with_suffix('.map')]
 before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'candidate-snapshot.c').write_bytes(source.read_bytes());(out/'verifier-snapshot.py').write_bytes(script.read_bytes())
 binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;blob,graph=original_image(binary);(out/'original-code.bin').write_bytes(blob);native,capsules=native_source(source,a.lower_override)
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je activation_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n activation_stackready:\n mov ecx,esp')
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 cases=fixtures(binary);indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices;results=[];failures=[];classification={};matching=None
 if a.recompiled:
  packet,matching=matching_image(a.recompiled,a.symbols,ENTRIES,GLOBALS,capsules);(out/'matching-code.bin').write_bytes(packet)
 for index in indices:
  case,payload=cases[index];f=out/'case.bin';f.write_bytes(payload);r=subprocess.run([str(out/'physical.exe'),str(f),str(out/'original-code.bin')],capture_output=True,timeout=30);expected=r.stdout
  assert r.returncode==0 and len(expected)>=SIZE+16 and len(expected)%(SIZE+16)==0,('original',index,r.returncode,len(expected),r.stderr)
  events=[struct.unpack_from('<4I',expected,j)for j in range(0,len(expected),SIZE+16)];assert events[-1][0]in(0,9);classification[index]=events
  for compiler in['msvc5','msvc','mingw']+(['matching']if matching else[]):
   command=[str(out/('physical.exe'if compiler=='matching'else compiler+'.exe')),str(f)];command += [str(out/'matching-code.bin')]if compiler=='matching'else []
   r=subprocess.run(command,capture_output=True,timeout=30);passed=r.returncode==0 and r.stdout==expected
   results.append(dict(index=index,compiler=compiler,passed=passed,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),original_bytes=len(expected),actual_bytes=len(r.stdout),exit=r.returncode,normal_return_abi_checked=events[-1][0]==0))
   if not passed:
    stem=out/f'failure-{compiler}-{index}';stem.with_suffix('.original.bin').write_bytes(expected);stem.with_suffix('.actual.bin').write_bytes(r.stdout);stem.with_suffix('.json').write_text(json.dumps(dict(case=case,stderr=r.stderr.hex(),exit=r.returncode),indent=2)+'\n');failures.append(dict(index=index,compiler=compiler,case=case,path=str(stem)))
  if index%50==0:print('checked',index,len(results),'failures',len(failures),flush=True)
 after={f:sha(Path(f).read_bytes())for f in before};assert before==after,'Source guard changed';report=dict(passed=not failures,scenes=len(indices),comparisons=len(results),total_scenes=len(cases),complete_suite=indices==list(range(len(cases))),selected_indices=indices,source_stable=True,source_before=before,source_after=after,original_sha256=HASH,original_graph=graph,native_capsules=capsules,fixtures_sha256=sha(b''.join(cases[i][1]for i in indices)),fixture_sha256=[sha(cases[i][1])for i in indices],classification=classification,results=results,failures=failures,commands=commands,artifacts={str(f):sha(f.read_bytes())for f in out.iterdir()if f.is_file()},limits=__doc__,live_handles=[])
 report.update(baseline_suite_scenes=2071,translate_upper_scenes=96,translate_direct_scenes=72)
 if matching:report.update(matching=matching,recompiled_sha256=sha(a.recompiled.read_bytes()),symbols_sha256=sha(a.symbols.read_bytes()))
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),len(failures),flush=True);assert not failures,'Failures retained'
if __name__=='__main__':main()
