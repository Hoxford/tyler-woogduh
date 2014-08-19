################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
drivers/LED.obj: ../drivers/LED.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/LED.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/SDcard.obj: ../drivers/SDcard.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/SDcard.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/UI.obj: ../drivers/UI.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/UI.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/battery.obj: ../drivers/battery.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/battery.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/bluetooth.obj: ../drivers/bluetooth.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/bluetooth.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/ineedmd_adc.obj: ../drivers/ineedmd_adc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/ineedmd_adc.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/ineedmd_bluetooth_radio.obj: ../drivers/ineedmd_bluetooth_radio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/ineedmd_bluetooth_radio.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/ineedmd_led.obj: ../drivers/ineedmd_led.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/ineedmd_led.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


