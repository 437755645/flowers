#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/Adc.c ../src/Calendar.c ../src/Comm232.c ../src/DEBUG.c ../src/DS18B20.c ../src/In_Sensor.c ../src/In_Setup.c ../src/In_Sys.c ../src/Init.c ../src/Lcd.c ../src/Main.c ../src/MakeMsg.c ../src/Power.c ../src/R4571.c ../src/ReadPort.c ../src/Record.c ../src/Regist.c ../src/Reports.c ../src/SerialSensor.c ../src/Spi.c ../src/Sub.c ../src/TF_Ch376Func.c ../src/TF_FileSys.c ../src/TF_main.c ../src/Usb_disk.c ../src/cmBeidou.c ../src/cmGprs.c ../src/cmGsmSm.c ../src/DispKey.c ../src/cmWifi.c ezbl_integration/ex_boot_uart.merge.s

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/Adc.o ${OBJECTDIR}/_ext/1360937237/Calendar.o ${OBJECTDIR}/_ext/1360937237/Comm232.o ${OBJECTDIR}/_ext/1360937237/DEBUG.o ${OBJECTDIR}/_ext/1360937237/DS18B20.o ${OBJECTDIR}/_ext/1360937237/In_Sensor.o ${OBJECTDIR}/_ext/1360937237/In_Setup.o ${OBJECTDIR}/_ext/1360937237/In_Sys.o ${OBJECTDIR}/_ext/1360937237/Init.o ${OBJECTDIR}/_ext/1360937237/Lcd.o ${OBJECTDIR}/_ext/1360937237/Main.o ${OBJECTDIR}/_ext/1360937237/MakeMsg.o ${OBJECTDIR}/_ext/1360937237/Power.o ${OBJECTDIR}/_ext/1360937237/R4571.o ${OBJECTDIR}/_ext/1360937237/ReadPort.o ${OBJECTDIR}/_ext/1360937237/Record.o ${OBJECTDIR}/_ext/1360937237/Regist.o ${OBJECTDIR}/_ext/1360937237/Reports.o ${OBJECTDIR}/_ext/1360937237/SerialSensor.o ${OBJECTDIR}/_ext/1360937237/Spi.o ${OBJECTDIR}/_ext/1360937237/Sub.o ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o ${OBJECTDIR}/_ext/1360937237/TF_main.o ${OBJECTDIR}/_ext/1360937237/Usb_disk.o ${OBJECTDIR}/_ext/1360937237/cmBeidou.o ${OBJECTDIR}/_ext/1360937237/cmGprs.o ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o ${OBJECTDIR}/_ext/1360937237/DispKey.o ${OBJECTDIR}/_ext/1360937237/cmWifi.o ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/Adc.o.d ${OBJECTDIR}/_ext/1360937237/Calendar.o.d ${OBJECTDIR}/_ext/1360937237/Comm232.o.d ${OBJECTDIR}/_ext/1360937237/DEBUG.o.d ${OBJECTDIR}/_ext/1360937237/DS18B20.o.d ${OBJECTDIR}/_ext/1360937237/In_Sensor.o.d ${OBJECTDIR}/_ext/1360937237/In_Setup.o.d ${OBJECTDIR}/_ext/1360937237/In_Sys.o.d ${OBJECTDIR}/_ext/1360937237/Init.o.d ${OBJECTDIR}/_ext/1360937237/Lcd.o.d ${OBJECTDIR}/_ext/1360937237/Main.o.d ${OBJECTDIR}/_ext/1360937237/MakeMsg.o.d ${OBJECTDIR}/_ext/1360937237/Power.o.d ${OBJECTDIR}/_ext/1360937237/R4571.o.d ${OBJECTDIR}/_ext/1360937237/ReadPort.o.d ${OBJECTDIR}/_ext/1360937237/Record.o.d ${OBJECTDIR}/_ext/1360937237/Regist.o.d ${OBJECTDIR}/_ext/1360937237/Reports.o.d ${OBJECTDIR}/_ext/1360937237/SerialSensor.o.d ${OBJECTDIR}/_ext/1360937237/Spi.o.d ${OBJECTDIR}/_ext/1360937237/Sub.o.d ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o.d ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o.d ${OBJECTDIR}/_ext/1360937237/TF_main.o.d ${OBJECTDIR}/_ext/1360937237/Usb_disk.o.d ${OBJECTDIR}/_ext/1360937237/cmBeidou.o.d ${OBJECTDIR}/_ext/1360937237/cmGprs.o.d ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o.d ${OBJECTDIR}/_ext/1360937237/DispKey.o.d ${OBJECTDIR}/_ext/1360937237/cmWifi.o.d ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/Adc.o ${OBJECTDIR}/_ext/1360937237/Calendar.o ${OBJECTDIR}/_ext/1360937237/Comm232.o ${OBJECTDIR}/_ext/1360937237/DEBUG.o ${OBJECTDIR}/_ext/1360937237/DS18B20.o ${OBJECTDIR}/_ext/1360937237/In_Sensor.o ${OBJECTDIR}/_ext/1360937237/In_Setup.o ${OBJECTDIR}/_ext/1360937237/In_Sys.o ${OBJECTDIR}/_ext/1360937237/Init.o ${OBJECTDIR}/_ext/1360937237/Lcd.o ${OBJECTDIR}/_ext/1360937237/Main.o ${OBJECTDIR}/_ext/1360937237/MakeMsg.o ${OBJECTDIR}/_ext/1360937237/Power.o ${OBJECTDIR}/_ext/1360937237/R4571.o ${OBJECTDIR}/_ext/1360937237/ReadPort.o ${OBJECTDIR}/_ext/1360937237/Record.o ${OBJECTDIR}/_ext/1360937237/Regist.o ${OBJECTDIR}/_ext/1360937237/Reports.o ${OBJECTDIR}/_ext/1360937237/SerialSensor.o ${OBJECTDIR}/_ext/1360937237/Spi.o ${OBJECTDIR}/_ext/1360937237/Sub.o ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o ${OBJECTDIR}/_ext/1360937237/TF_main.o ${OBJECTDIR}/_ext/1360937237/Usb_disk.o ${OBJECTDIR}/_ext/1360937237/cmBeidou.o ${OBJECTDIR}/_ext/1360937237/cmGprs.o ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o ${OBJECTDIR}/_ext/1360937237/DispKey.o ${OBJECTDIR}/_ext/1360937237/cmWifi.o ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o

# Source Files
SOURCEFILES=../src/Adc.c ../src/Calendar.c ../src/Comm232.c ../src/DEBUG.c ../src/DS18B20.c ../src/In_Sensor.c ../src/In_Setup.c ../src/In_Sys.c ../src/Init.c ../src/Lcd.c ../src/Main.c ../src/MakeMsg.c ../src/Power.c ../src/R4571.c ../src/ReadPort.c ../src/Record.c ../src/Regist.c ../src/Reports.c ../src/SerialSensor.c ../src/Spi.c ../src/Sub.c ../src/TF_Ch376Func.c ../src/TF_FileSys.c ../src/TF_main.c ../src/Usb_disk.c ../src/cmBeidou.c ../src/cmGprs.c ../src/cmGsmSm.c ../src/DispKey.c ../src/cmWifi.c ezbl_integration/ex_boot_uart.merge.s


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ256GA106
MP_LINKER_FILE_OPTION=,--script=ezbl_integration/ex_boot_uart.merge.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/Adc.o: ../src/Adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Adc.c  -o ${OBJECTDIR}/_ext/1360937237/Adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Adc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Calendar.o: ../src/Calendar.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Calendar.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Calendar.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Calendar.c  -o ${OBJECTDIR}/_ext/1360937237/Calendar.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Calendar.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Calendar.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Comm232.o: ../src/Comm232.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Comm232.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Comm232.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Comm232.c  -o ${OBJECTDIR}/_ext/1360937237/Comm232.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Comm232.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Comm232.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DEBUG.o: ../src/DEBUG.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DEBUG.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DEBUG.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DEBUG.c  -o ${OBJECTDIR}/_ext/1360937237/DEBUG.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DEBUG.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DEBUG.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DS18B20.o: ../src/DS18B20.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DS18B20.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DS18B20.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DS18B20.c  -o ${OBJECTDIR}/_ext/1360937237/DS18B20.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DS18B20.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DS18B20.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/In_Sensor.o: ../src/In_Sensor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sensor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sensor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/In_Sensor.c  -o ${OBJECTDIR}/_ext/1360937237/In_Sensor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/In_Sensor.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/In_Sensor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/In_Setup.o: ../src/In_Setup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Setup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Setup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/In_Setup.c  -o ${OBJECTDIR}/_ext/1360937237/In_Setup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/In_Setup.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/In_Setup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/In_Sys.o: ../src/In_Sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/In_Sys.c  -o ${OBJECTDIR}/_ext/1360937237/In_Sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/In_Sys.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/In_Sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Init.o: ../src/Init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Init.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Init.c  -o ${OBJECTDIR}/_ext/1360937237/Init.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Init.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Init.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Lcd.o: ../src/Lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Lcd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Lcd.c  -o ${OBJECTDIR}/_ext/1360937237/Lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Lcd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Main.o: ../src/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main.c  -o ${OBJECTDIR}/_ext/1360937237/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/MakeMsg.o: ../src/MakeMsg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/MakeMsg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/MakeMsg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/MakeMsg.c  -o ${OBJECTDIR}/_ext/1360937237/MakeMsg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/MakeMsg.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/MakeMsg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Power.o: ../src/Power.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Power.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Power.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Power.c  -o ${OBJECTDIR}/_ext/1360937237/Power.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Power.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Power.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/R4571.o: ../src/R4571.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/R4571.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/R4571.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/R4571.c  -o ${OBJECTDIR}/_ext/1360937237/R4571.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/R4571.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/R4571.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/ReadPort.o: ../src/ReadPort.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ReadPort.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ReadPort.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ReadPort.c  -o ${OBJECTDIR}/_ext/1360937237/ReadPort.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/ReadPort.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/ReadPort.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Record.o: ../src/Record.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Record.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Record.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Record.c  -o ${OBJECTDIR}/_ext/1360937237/Record.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Record.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Record.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Regist.o: ../src/Regist.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Regist.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Regist.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Regist.c  -o ${OBJECTDIR}/_ext/1360937237/Regist.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Regist.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Regist.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Reports.o: ../src/Reports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Reports.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Reports.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Reports.c  -o ${OBJECTDIR}/_ext/1360937237/Reports.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Reports.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Reports.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/SerialSensor.o: ../src/SerialSensor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SerialSensor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SerialSensor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/SerialSensor.c  -o ${OBJECTDIR}/_ext/1360937237/SerialSensor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/SerialSensor.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/SerialSensor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Spi.o: ../src/Spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Spi.c  -o ${OBJECTDIR}/_ext/1360937237/Spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Spi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Sub.o: ../src/Sub.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Sub.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Sub.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Sub.c  -o ${OBJECTDIR}/_ext/1360937237/Sub.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Sub.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Sub.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o: ../src/TF_Ch376Func.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/TF_Ch376Func.c  -o ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/TF_FileSys.o: ../src/TF_FileSys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/TF_FileSys.c  -o ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/TF_FileSys.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/TF_FileSys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/TF_main.o: ../src/TF_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/TF_main.c  -o ${OBJECTDIR}/_ext/1360937237/TF_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/TF_main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/TF_main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Usb_disk.o: ../src/Usb_disk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Usb_disk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Usb_disk.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Usb_disk.c  -o ${OBJECTDIR}/_ext/1360937237/Usb_disk.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Usb_disk.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Usb_disk.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmBeidou.o: ../src/cmBeidou.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmBeidou.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmBeidou.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmBeidou.c  -o ${OBJECTDIR}/_ext/1360937237/cmBeidou.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmBeidou.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmBeidou.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmGprs.o: ../src/cmGprs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGprs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGprs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmGprs.c  -o ${OBJECTDIR}/_ext/1360937237/cmGprs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmGprs.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmGprs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmGsmSm.o: ../src/cmGsmSm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmGsmSm.c  -o ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmGsmSm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmGsmSm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DispKey.o: ../src/DispKey.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DispKey.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DispKey.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DispKey.c  -o ${OBJECTDIR}/_ext/1360937237/DispKey.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DispKey.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DispKey.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmWifi.o: ../src/cmWifi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmWifi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmWifi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmWifi.c  -o ${OBJECTDIR}/_ext/1360937237/cmWifi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmWifi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmWifi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1360937237/Adc.o: ../src/Adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Adc.c  -o ${OBJECTDIR}/_ext/1360937237/Adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Adc.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Calendar.o: ../src/Calendar.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Calendar.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Calendar.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Calendar.c  -o ${OBJECTDIR}/_ext/1360937237/Calendar.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Calendar.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Calendar.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Comm232.o: ../src/Comm232.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Comm232.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Comm232.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Comm232.c  -o ${OBJECTDIR}/_ext/1360937237/Comm232.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Comm232.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Comm232.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DEBUG.o: ../src/DEBUG.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DEBUG.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DEBUG.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DEBUG.c  -o ${OBJECTDIR}/_ext/1360937237/DEBUG.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DEBUG.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DEBUG.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DS18B20.o: ../src/DS18B20.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DS18B20.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DS18B20.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DS18B20.c  -o ${OBJECTDIR}/_ext/1360937237/DS18B20.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DS18B20.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DS18B20.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/In_Sensor.o: ../src/In_Sensor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sensor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sensor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/In_Sensor.c  -o ${OBJECTDIR}/_ext/1360937237/In_Sensor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/In_Sensor.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/In_Sensor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/In_Setup.o: ../src/In_Setup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Setup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Setup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/In_Setup.c  -o ${OBJECTDIR}/_ext/1360937237/In_Setup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/In_Setup.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/In_Setup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/In_Sys.o: ../src/In_Sys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/In_Sys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/In_Sys.c  -o ${OBJECTDIR}/_ext/1360937237/In_Sys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/In_Sys.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/In_Sys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Init.o: ../src/Init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Init.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Init.c  -o ${OBJECTDIR}/_ext/1360937237/Init.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Init.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Init.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Lcd.o: ../src/Lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Lcd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Lcd.c  -o ${OBJECTDIR}/_ext/1360937237/Lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Lcd.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Main.o: ../src/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main.c  -o ${OBJECTDIR}/_ext/1360937237/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Main.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/MakeMsg.o: ../src/MakeMsg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/MakeMsg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/MakeMsg.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/MakeMsg.c  -o ${OBJECTDIR}/_ext/1360937237/MakeMsg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/MakeMsg.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/MakeMsg.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Power.o: ../src/Power.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Power.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Power.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Power.c  -o ${OBJECTDIR}/_ext/1360937237/Power.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Power.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Power.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/R4571.o: ../src/R4571.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/R4571.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/R4571.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/R4571.c  -o ${OBJECTDIR}/_ext/1360937237/R4571.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/R4571.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/R4571.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/ReadPort.o: ../src/ReadPort.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ReadPort.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ReadPort.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ReadPort.c  -o ${OBJECTDIR}/_ext/1360937237/ReadPort.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/ReadPort.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/ReadPort.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Record.o: ../src/Record.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Record.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Record.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Record.c  -o ${OBJECTDIR}/_ext/1360937237/Record.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Record.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Record.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Regist.o: ../src/Regist.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Regist.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Regist.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Regist.c  -o ${OBJECTDIR}/_ext/1360937237/Regist.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Regist.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Regist.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Reports.o: ../src/Reports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Reports.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Reports.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Reports.c  -o ${OBJECTDIR}/_ext/1360937237/Reports.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Reports.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Reports.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/SerialSensor.o: ../src/SerialSensor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SerialSensor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/SerialSensor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/SerialSensor.c  -o ${OBJECTDIR}/_ext/1360937237/SerialSensor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/SerialSensor.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/SerialSensor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Spi.o: ../src/Spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Spi.c  -o ${OBJECTDIR}/_ext/1360937237/Spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Spi.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Sub.o: ../src/Sub.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Sub.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Sub.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Sub.c  -o ${OBJECTDIR}/_ext/1360937237/Sub.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Sub.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Sub.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o: ../src/TF_Ch376Func.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/TF_Ch376Func.c  -o ${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/TF_Ch376Func.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/TF_FileSys.o: ../src/TF_FileSys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/TF_FileSys.c  -o ${OBJECTDIR}/_ext/1360937237/TF_FileSys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/TF_FileSys.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/TF_FileSys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/TF_main.o: ../src/TF_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/TF_main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/TF_main.c  -o ${OBJECTDIR}/_ext/1360937237/TF_main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/TF_main.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/TF_main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/Usb_disk.o: ../src/Usb_disk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Usb_disk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/Usb_disk.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Usb_disk.c  -o ${OBJECTDIR}/_ext/1360937237/Usb_disk.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/Usb_disk.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/Usb_disk.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmBeidou.o: ../src/cmBeidou.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmBeidou.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmBeidou.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmBeidou.c  -o ${OBJECTDIR}/_ext/1360937237/cmBeidou.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmBeidou.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmBeidou.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmGprs.o: ../src/cmGprs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGprs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGprs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmGprs.c  -o ${OBJECTDIR}/_ext/1360937237/cmGprs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmGprs.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmGprs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmGsmSm.o: ../src/cmGsmSm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmGsmSm.c  -o ${OBJECTDIR}/_ext/1360937237/cmGsmSm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmGsmSm.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmGsmSm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/DispKey.o: ../src/DispKey.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DispKey.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/DispKey.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DispKey.c  -o ${OBJECTDIR}/_ext/1360937237/DispKey.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/DispKey.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/DispKey.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1360937237/cmWifi.o: ../src/cmWifi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmWifi.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cmWifi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/cmWifi.c  -o ${OBJECTDIR}/_ext/1360937237/cmWifi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360937237/cmWifi.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -I"../../../TRANS/test/inc" -I"../inc" -I"." -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/cmWifi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o: ezbl_integration/ex_boot_uart.merge.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/ezbl_integration" 
	@${RM} ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o.d 
	@${RM} ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ezbl_integration/ex_boot_uart.merge.s  -o ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -omf=elf -DXPRJ_default=$(CND_CONF)    -I".." -I"." -Wa,-MD,"${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o: ezbl_integration/ex_boot_uart.merge.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/ezbl_integration" 
	@${RM} ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o.d 
	@${RM} ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ezbl_integration/ex_boot_uart.merge.s  -o ${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DXPRJ_default=$(CND_CONF)    -I".." -I"." -Wa,-MD,"${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/ezbl_integration/ex_boot_uart.merge.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ezbl_integration/ex_boot_uart.merge.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="../../../mcc30/support/PIC24F/gld",--library-path=".",--no-force-link,--smart-io,-Map="${DISTDIR}/Mix.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ezbl_integration/ex_boot_uart.merge.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="../../../mcc30/support/PIC24F/gld",--library-path=".",--no-force-link,--smart-io,-Map="${DISTDIR}/Mix.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Mix.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
