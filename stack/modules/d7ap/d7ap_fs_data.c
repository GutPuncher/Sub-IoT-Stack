/*! \file d7ap_fs_data.c
 *

 *  \copyright (C) Copyright 2019 University of Antwerp and others (http://mosaic-lopow.github.io/dash7-ap-open-source-stack/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  \author glenn.ergeerts@uantwerpen.be
 *
 */

#include "d7ap_fs.h"
#include "platform_defs.h"
#include "MODULE_D7AP_defs.h"

#ifdef MODULE_D7AP_USE_DEFAULT_SYSTEMFILES

// The cog section below does not generate code but defines some global variables and functions which are used in subsequent cog sections below,
// which do the actual code generation

/*[[[cog
import cog
from d7a.system_files.system_files import SystemFiles
from d7a.system_files.access_profile import AccessProfileFile
from d7a.system_files.dll_config import DllConfigFile
from d7a.system_files.firmware_version import FirmwareVersionFile
from d7a.system_files.phy_config import PhyConfigFile
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.not_implemented import NotImplementedFile
from d7a.system_files.security_key import SecurityKeyFile
from d7a.system_files.uid import UidFile
from d7a.system_files.engineering_mode import EngineeringModeFile
from d7a.system_files.factory_settings import FactorySettingsFile
from d7a.system_files.vid import VidFile
from d7a.fs.file_permissions import FilePermissions
from d7a.fs.file_properties import FileProperties
from d7a.fs.file_properties import ActionCondition, StorageClass, FileProperties
from d7a.fs.file_header import FileHeader
from d7a.dll.access_profile import AccessProfile, CsmaCaMode, SubBand
from d7a.dll.sub_profile import SubProfile
from d7a.phy.channel_header import ChannelHeader, ChannelBand, ChannelCoding, ChannelClass
from d7a.types.ct import CT

default_channel_header = ChannelHeader(
  channel_class=ChannelClass.LO_RATE,
  channel_coding=ChannelCoding.FEC_PN9,
  channel_band=ChannelBand.BAND_868
)

default_channel_index = 0

# AP used by GW doing continuous FG scan
ap_cont_fg_scan = AccessProfile(
  channel_header=default_channel_header,
  sub_profiles=[SubProfile(subband_bitmap=0x01, scan_automation_period=CT.compress(0))] * 4,
  sub_bands=[SubBand(eirp=14, channel_index_start=default_channel_index, channel_index_end=default_channel_index)] * 8
)

# AP used for scanning for BG request at max allowed rate (maximum 1 second of sending including foreground and background frames)
# 60 BG frames(799.8 ticks) + 14 preamble(11.7 ticks) + FG Frame 255 bytes (212.5 ticks) = 1024 ticks
# 59 BG frames(786.47 ticks) Tsched + 1 BG frame(13.33 ticks) Ttx 
ap_bg_scan = AccessProfile(
  channel_header=default_channel_header,
  sub_profiles=[SubProfile(subband_bitmap=0x01, scan_automation_period=CT.compress(786, ceil=False))] * 4,
  sub_bands=[SubBand(eirp=14, channel_index_start=default_channel_index, channel_index_end=default_channel_index)] * 8
)

# AP used for push only, no scanning
ap_no_scan = AccessProfile(
  channel_header=default_channel_header,
  sub_profiles=[SubProfile(subband_bitmap=0x00, scan_automation_period=CT.compress(0))] * 4,
  sub_bands=[SubBand(eirp=14, channel_index_start=default_channel_index, channel_index_end=default_channel_index)] * 8
)

system_files = [
  UidFile(),
  FactorySettingsFile(),
  FirmwareVersionFile(),
  NotImplementedFile(SystemFileIds.DEVICE_CAPACITY.value, 19),
  NotImplementedFile(SystemFileIds.DEVICE_STATUS.value, 9),
  EngineeringModeFile(),
  VidFile(),
  NotImplementedFile(SystemFileIds.RFU_07.value, 0),
  PhyConfigFile(),
  NotImplementedFile(SystemFileIds.PHY_STATUS.value, 24),  # TODO assuming 3 channels for now
  DllConfigFile(active_access_class=0x21),
  NotImplementedFile(SystemFileIds.DLL_STATUS.value, 12),
  NotImplementedFile(SystemFileIds.NWL_ROUTING.value, 1),  # TODO variable routing table
  NotImplementedFile(SystemFileIds.NWL_SECURITY.value, 5),
  SecurityKeyFile(),
  NotImplementedFile(SystemFileIds.NWL_SSR.value, 4),  # TODO 0 recorded devices
  NotImplementedFile(SystemFileIds.NWL_STATUS.value, 20),
  NotImplementedFile(SystemFileIds.TRL_STATUS.value, 1),  # TODO 0 TRL records
  NotImplementedFile(SystemFileIds.SEL_CONFIG.value, 6),
  NotImplementedFile(SystemFileIds.FOF_STATUS.value, 10),
  NotImplementedFile(SystemFileIds.RFU_14.value, 0),
  NotImplementedFile(SystemFileIds.RFU_15.value, 0),
  NotImplementedFile(SystemFileIds.RFU_16.value, 0),
  NotImplementedFile(SystemFileIds.LOCATION_DATA.value, 1),  # TODO 0 recorded locations
  NotImplementedFile(SystemFileIds.D7AALP_RFU_18.value, 0),
  NotImplementedFile(SystemFileIds.D7AALP_RFU_19.value, 0),
  NotImplementedFile(SystemFileIds.D7AALP_RFU_1A.value, 0),
  NotImplementedFile(SystemFileIds.D7AALP_RFU_1B.value, 0),
  NotImplementedFile(SystemFileIds.D7AALP_RFU_1C.value, 0),
  NotImplementedFile(SystemFileIds.D7AALP_RFU_1D.value, 0),
  NotImplementedFile(SystemFileIds.D7AALP_RFU_1E.value, 0),
  NotImplementedFile(SystemFileIds.D7AALP_RFU_1F.value, 0),
  AccessProfileFile(0, ap_cont_fg_scan),
  AccessProfileFile(1, ap_bg_scan),
  AccessProfileFile(2, ap_no_scan),
  AccessProfileFile(3, ap_no_scan),
  AccessProfileFile(4, ap_no_scan),
  AccessProfileFile(5, ap_no_scan),
  AccessProfileFile(6, ap_no_scan),
  AccessProfileFile(7, ap_no_scan),
  AccessProfileFile(8, ap_no_scan),
  AccessProfileFile(9, ap_no_scan),
  AccessProfileFile(10, ap_no_scan),
  AccessProfileFile(11, ap_no_scan),
  AccessProfileFile(12, ap_no_scan),
  AccessProfileFile(13, ap_no_scan),
  AccessProfileFile(14, ap_no_scan)
]

sys_file_permission_default = FilePermissions(encrypted=False, executeable=False, user_readable=True, user_writeable=False, user_executeable=False,
                   guest_readable=True, guest_writeable=False, guest_executeable=False)
sys_file_permission_non_readable = FilePermissions(encrypted=False, executeable=False, user_readable=False, user_writeable=False, user_executeable=False,
                   guest_readable=False, guest_writeable=False, guest_executeable=False)
sys_file_prop_default = FileProperties(act_enabled=False, act_condition=ActionCondition.WRITE, storage_class=StorageClass.PERMANENT)

def output_file(file):
  file_type = SystemFileIds(file.id)
#  cog.outl("\t// {} - {}".format(file_type.name, file_type.value))
  file_array_elements = "\t"
  for byte in bytearray(file):
    file_array_elements += "{}, ".format(hex(byte))
  
  if(len(file_array_elements) > 1):
    cog.outl(file_array_elements)

def output_fileheader(file):
  file_type = SystemFileIds(system_file.id)
  cog.outl("\t// {} - {} (length {})".format(file_type.name, file_type.value, file.length))
  file_header = FileHeader(permissions=file_permissions, properties=sys_file_prop_default, alp_command_file_id=0xFF, interface_file_id=0xFF, file_size=system_file.length, allocated_size=system_file.length)
  file_header_array_elements = "\t"
  for byte in bytearray(file_header):
    file_header_array_elements += "{}, ".format(hex(byte))

  cog.outl(file_header_array_elements)

def output_system_file_offsets():
  current_offset = 4
  headerlength = 12
  for system_file in system_files:
    file_type = SystemFileIds(system_file.id)
    cog.outl("\t{}, // {} - {} (length {}))".format(hex(current_offset), file_type.name, file_type.value, system_file.length))
    current_offset += system_file.length + headerlength

def output_system_file_length():
  headerlength = 12
  for system_file in system_files:
    file_type = SystemFileIds(system_file.id)
    cog.outl("\t{},\t// {} - {})".format(hex(system_file.length + headerlength), file_type.name, file_type.value))


def output_fs_file_headers():
  current_offset = 4 + 4 + (256 * 9) # magic number + number of files + 256 * fs_file_t
  headerlength = 12
  for system_file in system_files:
    file_type = SystemFileIds(system_file.id)
    cog.outl("\t{{\t//\t{} - {} (length {})".format(file_type.name, file_type.value, system_file.length + headerlength))
    cog.out("\t\t.length = {}, ".format(hex(system_file.length + headerlength)))
    cog.out(".storage = FS_STORAGE_PERMANENT, ")
    cog.out(".rfu = 0, ")
    cog.outl(".addr = {}".format(hex(current_offset)))
    cog.outl("\t},")
    current_offset += system_file.length + headerlength

def output_number_of_files():
  cog.outl("\t.nfiles = {} // Number of files".format(len(system_files)))
]]]*/
//[[[end]]] (checksum: d41d8cd98f00b204e9800998ecf8427e)

#ifdef PLATFORM_FS_SYSTEMFILES_IN_SEPARATE_LINKER_SECTION
  #define LINKER_SECTION_FS_METADATA __attribute__((section(".d7ap_fs_metadata")))
  #define LINKER_SECTION_FS_FILE_HEADERS __attribute__((section(".d7ap_fs_file_headers")))
  #define LINKER_SECTION_FS_SYSTEM_FILE __attribute__((section(".d7ap_fs_systemfiles")))
#else
  #define LINKER_SECTION_FS_METADATA
  #define LINKER_SECTION_FS_FILE_HEADERS
  #define LINKER_SECTION_FS_SYSTEM_FILE
#endif

__attribute__((used)) fs_metadata_t fs_metadata LINKER_SECTION_FS_METADATA = {
  .magic_number = {0x34, 0xC2, 0x00, 0x00}, // FS_MAGIC_NUMBER first 2 bytes fixed, last 2 byte for version
  /*[[[cog
  output_number_of_files()
  ]]] */
  .nfiles = 47 // Number of files
  //[[[end]]] (checksum: d54c1927b5f5c8d57eeb6730fc98d49d)
};


__attribute__((used)) fs_file_t fs_file_headers[256] LINKER_SECTION_FS_FILE_HEADERS= {
  /*[[[cog
  output_fs_file_headers()
  ]]] */
  {	//	UID - 0 (length 20)
  	.length = 0x14, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x908
  },
  {	//	FACTORY_SETTINGS - 1 (length 49)
  	.length = 0x31, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x91c
  },
  {	//	FIRMWARE_VERSION - 2 (length 27)
  	.length = 0x1b, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x94d
  },
  {	//	DEVICE_CAPACITY - 3 (length 31)
  	.length = 0x1f, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x968
  },
  {	//	DEVICE_STATUS - 4 (length 21)
  	.length = 0x15, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x987
  },
  {	//	ENGINEERING_MODE - 5 (length 21)
  	.length = 0x15, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x99c
  },
  {	//	VID - 6 (length 15)
  	.length = 0xf, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x9b1
  },
  {	//	RFU_07 - 7 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x9c0
  },
  {	//	PHY_CONFIG - 8 (length 23)
  	.length = 0x17, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x9cc
  },
  {	//	PHY_STATUS - 9 (length 36)
  	.length = 0x24, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0x9e3
  },
  {	//	DLL_CONFIG - 10 (length 19)
  	.length = 0x13, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa07
  },
  {	//	DLL_STATUS - 11 (length 24)
  	.length = 0x18, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa1a
  },
  {	//	NWL_ROUTING - 12 (length 13)
  	.length = 0xd, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa32
  },
  {	//	NWL_SECURITY - 13 (length 17)
  	.length = 0x11, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa3f
  },
  {	//	NWL_SECURITY_KEY - 14 (length 28)
  	.length = 0x1c, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa50
  },
  {	//	NWL_SSR - 15 (length 16)
  	.length = 0x10, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa6c
  },
  {	//	NWL_STATUS - 16 (length 32)
  	.length = 0x20, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa7c
  },
  {	//	TRL_STATUS - 17 (length 13)
  	.length = 0xd, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xa9c
  },
  {	//	SEL_CONFIG - 18 (length 18)
  	.length = 0x12, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xaa9
  },
  {	//	FOF_STATUS - 19 (length 22)
  	.length = 0x16, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xabb
  },
  {	//	RFU_14 - 20 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xad1
  },
  {	//	RFU_15 - 21 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xadd
  },
  {	//	RFU_16 - 22 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xae9
  },
  {	//	LOCATION_DATA - 23 (length 13)
  	.length = 0xd, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xaf5
  },
  {	//	D7AALP_RFU_18 - 24 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb02
  },
  {	//	D7AALP_RFU_19 - 25 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb0e
  },
  {	//	D7AALP_RFU_1A - 26 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb1a
  },
  {	//	D7AALP_RFU_1B - 27 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb26
  },
  {	//	D7AALP_RFU_1C - 28 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb32
  },
  {	//	D7AALP_RFU_1D - 29 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb3e
  },
  {	//	D7AALP_RFU_1E - 30 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb4a
  },
  {	//	D7AALP_RFU_1F - 31 (length 12)
  	.length = 0xc, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb56
  },
  {	//	ACCESS_PROFILE_0 - 32 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xb62
  },
  {	//	ACCESS_PROFILE_1 - 33 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xbaf
  },
  {	//	ACCESS_PROFILE_2 - 34 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xbfc
  },
  {	//	ACCESS_PROFILE_3 - 35 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xc49
  },
  {	//	ACCESS_PROFILE_4 - 36 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xc96
  },
  {	//	ACCESS_PROFILE_5 - 37 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xce3
  },
  {	//	ACCESS_PROFILE_6 - 38 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xd30
  },
  {	//	ACCESS_PROFILE_7 - 39 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xd7d
  },
  {	//	ACCESS_PROFILE_8 - 40 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xdca
  },
  {	//	ACCESS_PROFILE_9 - 41 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xe17
  },
  {	//	ACCESS_PROFILE_10 - 42 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xe64
  },
  {	//	ACCESS_PROFILE_11 - 43 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xeb1
  },
  {	//	ACCESS_PROFILE_12 - 44 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xefe
  },
  {	//	ACCESS_PROFILE_13 - 45 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xf4b
  },
  {	//	ACCESS_PROFILE_14 - 46 (length 77)
  	.length = 0x4d, .storage = FS_STORAGE_PERMANENT, .rfu = 0, .addr = 0xf98
  },
  //[[[end]]] (checksum: a723f5f0ce0173b7155528ccd9757c1d)
};

__attribute__((used)) uint8_t d7ap_files_data[FRAMEWORK_FS_PERMANENT_STORAGE_SIZE] LINKER_SECTION_FS_SYSTEM_FILE = {
  
      /*[[[cog
      file_permissions = sys_file_permission_default
      for system_file in system_files:
        output_fileheader(system_file)
        output_file(system_file)
      ]]]*/
      // UID - 0 (length 8)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // FACTORY_SETTINGS - 1 (length 37)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x25, 0x0, 0x0, 0x0, 0x25, 
      0x0, 0x0, 0x0, 0x28, 0xe4, 0x0, 0x1, 0x33, 0x36, 0x0, 0x1, 0xeb, 0xac, 0x0, 0x0, 0x25, 0x80, 0x0, 0x0, 0x12, 0xc0, 0x0, 0x0, 0xd9, 0x3, 0x0, 0x0, 0xc3, 0x50, 0x0, 0x2, 0x8b, 0xb, 0x0, 0x0, 0xa2, 0xc3, 
      // FIRMWARE_VERSION - 2 (length 15)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0x0, 0xf, 
      0x0, 0x0, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
      // DEVICE_CAPACITY - 3 (length 19)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x13, 0x0, 0x0, 0x0, 0x13, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // DEVICE_STATUS - 4 (length 9)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x9, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // ENGINEERING_MODE - 5 (length 9)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x9, 
      0x0, 0x0, 0x0, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // VID - 6 (length 3)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x3, 
      0xff, 0xff, 0x0, 
      // RFU_07 - 7 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // PHY_CONFIG - 8 (length 11)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0xb, 0x0, 0x0, 0x0, 0xb, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // PHY_STATUS - 9 (length 24)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x18, 0x0, 0x0, 0x0, 0x18, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // DLL_CONFIG - 10 (length 7)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0, 0x7, 
      0x21, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // DLL_STATUS - 11 (length 12)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0xc, 0x0, 0x0, 0x0, 0xc, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // NWL_ROUTING - 12 (length 1)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 
      0x0, 
      // NWL_SECURITY - 13 (length 5)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x5, 
      0x0, 0x0, 0x0, 0x0, 0x0, 
      // NWL_SECURITY_KEY - 14 (length 16)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x10, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // NWL_SSR - 15 (length 4)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x4, 
      0x0, 0x0, 0x0, 0x0, 
      // NWL_STATUS - 16 (length 20)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x14, 0x0, 0x0, 0x0, 0x14, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // TRL_STATUS - 17 (length 1)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 
      0x0, 
      // SEL_CONFIG - 18 (length 6)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x6, 0x0, 0x0, 0x0, 0x6, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // FOF_STATUS - 19 (length 10)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0xa, 0x0, 0x0, 0x0, 0xa, 
      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // RFU_14 - 20 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // RFU_15 - 21 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // RFU_16 - 22 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // LOCATION_DATA - 23 (length 1)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 
      0x0, 
      // D7AALP_RFU_18 - 24 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // D7AALP_RFU_19 - 25 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // D7AALP_RFU_1A - 26 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // D7AALP_RFU_1B - 27 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // D7AALP_RFU_1C - 28 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // D7AALP_RFU_1D - 29 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // D7AALP_RFU_1E - 30 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // D7AALP_RFU_1F - 31 (length 0)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
      // ACCESS_PROFILE_0 - 32 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x1, 0x0, 0x1, 0x0, 0x1, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_1 - 33 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x1, 0x6c, 0x1, 0x6c, 0x1, 0x6c, 0x1, 0x6c, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_2 - 34 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_3 - 35 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_4 - 36 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_5 - 37 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_6 - 38 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_7 - 39 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_8 - 40 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_9 - 41 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_10 - 42 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_11 - 43 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_12 - 44 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_13 - 45 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      // ACCESS_PROFILE_14 - 46 (length 65)
      0x24, 0x23, 0xff, 0xff, 0x0, 0x0, 0x0, 0x41, 0x0, 0x0, 0x0, 0x41, 
      0x32, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 0x0, 0x0, 0x0, 0x0, 0xe, 0x56, 0xff, 
      //[[[end]]] (checksum: 7f1ec5e1f61946472da47efd1c0354c6)  
  };

fs_filesystem_t d7ap_filesystem __attribute__((used)) = {
  .metadata = &fs_metadata,
  .file_headers = fs_file_headers,
  .files_data = d7ap_files_data
};
#endif

// The userfiles are only stored in RAM for now
uint8_t d7ap_volatile_files_data[FRAMEWORK_FS_VOLATILE_STORAGE_SIZE];

