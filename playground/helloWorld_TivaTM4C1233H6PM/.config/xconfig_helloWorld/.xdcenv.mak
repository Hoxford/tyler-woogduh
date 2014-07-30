#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_tivac_2_00_02_36/packages;C:/ti/tirtos_tivac_2_00_02_36/products/bios_6_40_02_27/packages;C:/ti/tirtos_tivac_2_00_02_36/products/ndk_2_23_01_01/packages;C:/ti/tirtos_tivac_2_00_02_36/products/uia_2_00_01_34/packages;C:/ti/ccsv6/ccs_base;C:/Users/BrianS/workspace_v6_0/brian_g_aero1/playground/helloWorld_TivaTM4C1233H6PM/.config
override XDCROOT = c:/ti/xdctools_3_30_01_25_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_tivac_2_00_02_36/packages;C:/ti/tirtos_tivac_2_00_02_36/products/bios_6_40_02_27/packages;C:/ti/tirtos_tivac_2_00_02_36/products/ndk_2_23_01_01/packages;C:/ti/tirtos_tivac_2_00_02_36/products/uia_2_00_01_34/packages;C:/ti/ccsv6/ccs_base;C:/Users/BrianS/workspace_v6_0/brian_g_aero1/playground/helloWorld_TivaTM4C1233H6PM/.config;c:/ti/xdctools_3_30_01_25_core/packages;..
HOSTOS = Windows
endif
