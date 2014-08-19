################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
utils/cmdline.obj: ../utils/cmdline.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/cmdline.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/cpu_usage.obj: ../utils/cpu_usage.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/cpu_usage.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/flash_pb.obj: ../utils/flash_pb.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/flash_pb.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/isqrt.obj: ../utils/isqrt.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/isqrt.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/locator.obj: ../utils/locator.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/locator.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/random.obj: ../utils/random.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/random.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/ringbuf.obj: ../utils/ringbuf.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/ringbuf.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/scheduler.obj: ../utils/scheduler.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/scheduler.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/sine.obj: ../utils/sine.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/sine.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/softi2c.obj: ../utils/softi2c.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/softi2c.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/softssi.obj: ../utils/softssi.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/softssi.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/softuart.obj: ../utils/softuart.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/softuart.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/spi_flash.obj: ../utils/spi_flash.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/spi_flash.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/uartstdio.obj: C:/ti/TivaWare_C_Series-2.1.0.12573/utils/uartstdio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/uartstdio.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/ustdlib.obj: C:/ti/TivaWare_C_Series-2.1.0.12573/utils/ustdlib.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/ustdlib.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/wavfile.obj: ../utils/wavfile.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-lm4f232" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RA3 --define=UART_BUFFERED --define=DEBUG --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="utils/wavfile.pp" --obj_directory="utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


