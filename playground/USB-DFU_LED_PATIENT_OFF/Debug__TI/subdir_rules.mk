################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
board.obj: ../board.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --fp_mode=strict --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" --include_path="C:/ti/tirtos_tivac_2_00_02_36/products/TivaWare_C_Series-2.1.0.12573c" -g --define=PART_TM4C1233H4PM --define=TARGET_IS_BLIZZARD_RA3 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ineedmd_bluetooth_radio.obj: ../ineedmd_bluetooth_radio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --fp_mode=strict --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" --include_path="C:/ti/tirtos_tivac_2_00_02_36/products/TivaWare_C_Series-2.1.0.12573c" -g --define=PART_TM4C1233H4PM --define=TARGET_IS_BLIZZARD_RA3 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="ineedmd_bluetooth_radio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ineedmd_led.obj: ../ineedmd_led.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --fp_mode=strict --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" --include_path="C:/ti/tirtos_tivac_2_00_02_36/products/TivaWare_C_Series-2.1.0.12573c" -g --define=PART_TM4C1233H4PM --define=TARGET_IS_BLIZZARD_RA3 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="ineedmd_led.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

innedmd_adc.obj: ../innedmd_adc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --fp_mode=strict --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" --include_path="C:/ti/tirtos_tivac_2_00_02_36/products/TivaWare_C_Series-2.1.0.12573c" -g --define=PART_TM4C1233H4PM --define=TARGET_IS_BLIZZARD_RA3 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="innedmd_adc.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --fp_mode=strict --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" --include_path="C:/ti/tirtos_tivac_2_00_02_36/products/TivaWare_C_Series-2.1.0.12573c" -g --define=PART_TM4C1233H4PM --define=TARGET_IS_BLIZZARD_RA3 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c1233h6pm_startup_ccs.obj: ../tm4c1233h6pm_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --fp_mode=strict --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.5/include" --include_path="C:/ti/tirtos_tivac_2_00_02_36/products/TivaWare_C_Series-2.1.0.12573c" -g --define=PART_TM4C1233H4PM --define=TARGET_IS_BLIZZARD_RA3 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tm4c1233h6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


