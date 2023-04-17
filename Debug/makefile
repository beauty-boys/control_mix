################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/software/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./SVPWM.obj" \
"./source/Angle.obj" \
"./source/F2837xD_CodeStartBranch.obj" \
"./source/F2837xD_DefaultISR.obj" \
"./source/F2837xD_EPwm.obj" \
"./source/F2837xD_Emif.obj" \
"./source/F2837xD_GlobalVariableDefs.obj" \
"./source/F2837xD_Gpio.obj" \
"./source/F2837xD_Ipc.obj" \
"./source/F2837xD_PieCtrl.obj" \
"./source/F2837xD_PieVect.obj" \
"./source/F2837xD_Sci.obj" \
"./source/F2837xD_SysCtrl.obj" \
"./source/F2837xD_can.obj" \
"./source/F2837xD_sci_io.obj" \
"./source/F2837xD_usDelay.obj" \
"./source/FPUmathTables.obj" \
"./source/OCB_az1.obj" \
"./source/atan2_f32.obj" \
"./source/atan_f32.obj" \
"./source/az1gen_dp.obj" \
"./source/clarke.obj" \
"./source/cos_f32.obj" \
"./source/div_f32.obj" \
"./source/ipark.obj" \
"./source/isqrt_f32.obj" \
"./source/memcpy.obj" \
"./source/nspwm.obj" \
"./source/park.obj" \
"./source/pid_reg3.obj" \
"./source/rampgen.obj" \
"./source/rmp_cntl.obj" \
"./source/sin_f32.obj" \
"./source/sincos_f32.obj" \
"./source/sqrt_f32.obj" \
"./source/svgen_dq.obj" \
"../CMD/2837xD_FLASH_lnk_cpu1.cmd" \
"../CMD/F2837xD_Headers_nonBIOS_cpu1.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include CMD/subdir_vars.mk
-include source/subdir_vars.mk
-include subdir_rules.mk
-include CMD/subdir_rules.mk
-include source/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
White_Module_SiC_Inverter.out \

EXE_OUTPUTS__QUOTED += \
"White_Module_SiC_Inverter.out" \


# All Target
all: White_Module_SiC_Inverter.out

# Tool invocations
White_Module_SiC_Inverter.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"D:/software/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --fp_mode=relaxed --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"White_Module_SiC_Inverter.map" --stack_size=0x200 --warn_sections -i"D:/software/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/lib" -i"D:/software/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="White_Module_SiC_Inverter_linkInfo.xml" --rom_model -o "White_Module_SiC_Inverter.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "source\FPUmathTables.d" 
	-$(RM) "SVPWM.obj" "source\Angle.obj" "source\F2837xD_CodeStartBranch.obj" "source\F2837xD_DefaultISR.obj" "source\F2837xD_EPwm.obj" "source\F2837xD_Emif.obj" "source\F2837xD_GlobalVariableDefs.obj" "source\F2837xD_Gpio.obj" "source\F2837xD_Ipc.obj" "source\F2837xD_PieCtrl.obj" "source\F2837xD_PieVect.obj" "source\F2837xD_Sci.obj" "source\F2837xD_SysCtrl.obj" "source\F2837xD_can.obj" "source\F2837xD_sci_io.obj" "source\F2837xD_usDelay.obj" "source\FPUmathTables.obj" "source\OCB_az1.obj" "source\atan2_f32.obj" "source\atan_f32.obj" "source\az1gen_dp.obj" "source\clarke.obj" "source\cos_f32.obj" "source\div_f32.obj" "source\ipark.obj" "source\isqrt_f32.obj" "source\memcpy.obj" "source\nspwm.obj" "source\park.obj" "source\pid_reg3.obj" "source\rampgen.obj" "source\rmp_cntl.obj" "source\sin_f32.obj" "source\sincos_f32.obj" "source\sqrt_f32.obj" "source\svgen_dq.obj" 
	-$(RM) "SVPWM.d" "source\Angle.d" "source\F2837xD_DefaultISR.d" "source\F2837xD_EPwm.d" "source\F2837xD_Emif.d" "source\F2837xD_GlobalVariableDefs.d" "source\F2837xD_Gpio.d" "source\F2837xD_Ipc.d" "source\F2837xD_PieCtrl.d" "source\F2837xD_PieVect.d" "source\F2837xD_Sci.d" "source\F2837xD_SysCtrl.d" "source\F2837xD_can.d" "source\F2837xD_sci_io.d" "source\OCB_az1.d" "source\az1gen_dp.d" "source\clarke.d" "source\ipark.d" "source\memcpy.d" "source\nspwm.d" "source\park.d" "source\pid_reg3.d" "source\rampgen.d" "source\rmp_cntl.d" "source\svgen_dq.d" 
	-$(RM) "source\F2837xD_CodeStartBranch.d" "source\F2837xD_usDelay.d" "source\atan2_f32.d" "source\atan_f32.d" "source\cos_f32.d" "source\div_f32.d" "source\isqrt_f32.d" "source\sin_f32.d" "source\sincos_f32.d" "source\sqrt_f32.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

