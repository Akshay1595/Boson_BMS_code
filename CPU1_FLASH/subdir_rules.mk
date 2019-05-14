################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt_c2000_18.1.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt_c2000_18.1.3.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/BMS_Branch/BMS_Launchpad/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --define=CPU1 --define=_FLASH --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt_c2000_18.1.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt_c2000_18.1.3.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/BMS_Branch/BMS_Launchpad/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --define=CPU1 --define=_FLASH --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


