################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_CodeStartBranch.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_CodeStartBranch.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_DefaultISR.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_DefaultISR.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_DefaultISR.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_GlobalVariableDefs.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/source/F2837xD_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_GlobalVariableDefs.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_Gpio.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_Gpio.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_Gpio.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_Ipc.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_Ipc.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_Ipc.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_PieCtrl.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_PieCtrl.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_PieCtrl.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_PieVect.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_PieVect.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_PieVect.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_SysCtrl.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_SysCtrl.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_SysCtrl.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2837xD_usDelay.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/source/F2837xD_usDelay.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="F2837xD_usDelay.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

can.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated/driverlib/can.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="can.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

interrupt.obj: C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated/driverlib/interrupt.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.5.LTS/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/examples/cpu1/can_external_transmit/cpu01_Working_code_edited_for_project - Copy/ccs/includes" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/deprecated" --advice:performance=all --define=CPU1 --preproc_with_compile --preproc_dependency="interrupt.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

