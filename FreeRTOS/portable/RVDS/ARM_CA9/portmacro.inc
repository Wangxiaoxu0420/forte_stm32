;

SYS_MODE			EQU		0x1f
SVC_MODE			EQU		0x13
IRQ_MODE			EQU		0x12

	IMPORT  ulCriticalNesting
	IMPORT	pxCurrentTCB
	IMPORT	ulPortTaskHasFPUContext
	IMPORT  ulAsmAPIPriorityMask
	IMPORT	ulICCPMR


	MACRO
	portSAVE_CONTEXT

	; Save the LR and SPSR onto the system mode stack before switching to
	; system mode to save the remaining system mode registers
	SRSDB	sp!, #SYS_MODE
	CPS		#SYS_MODE
	PUSH	{R0-R12, R14}

	; Push the critical nesting count
	LDR		R2, =ulCriticalNesting
	LDR		R1, [R2]
	PUSH	{R1}

	; Does the task have a floating point context that needs saving?  If
	; ulPortTaskHasFPUContext is 0 then no.
	LDR		R2, =ulPortTaskHasFPUContext
	LDR		R3, [R2]
	CMP		R3, #0

	; Save the floating point context, if any
	FMRXNE  R1,  FPSCR
	VPUSHNE {D0-D15}
	VPUSHNE	{D16-D31}
	PUSHNE	{R1}

	; Save ulPortTaskHasFPUContext itself
	PUSH	{R3}

	; Save the stack pointer in the TCB
	LDR		R0, =pxCurrentTCB
	LDR		R1, [R0]
	STR		SP, [R1]

	MEND

; /**********************************************************************/

	MACRO
	portRESTORE_CONTEXT

	; Set the SP to point to the stack of the task being restored.
	LDR		R0, =pxCurrentTCB
	LDR		R1, [R0]
	LDR		SP, [R1]

	; Is there a floating point context to restore?  If the restored
	; ulPortTaskHasFPUContext is zero then no.
	LDR		R0, =ulPortTaskHasFPUContext
	POP		{R1}
	STR		R1, [R0]
	CMP		R1, #0

	; Restore the floating point context, if any
	POPNE	{R0}
	VPOPNE	{D16-D31}
	VPOPNE	{D0-D15}
	VMSRNE  FPSCR, R0

	; Restore the critical section nesting depth
	LDR		R0, =ulCriticalNesting
	POP		{R1}
	STR		R1, [R0]

	; Ensure the priority mask is correct for the critical nesting depth
	LDR		R2, =ulICCPMR
	CMP		R1, #0
	MOVEQ	R4, #255
	LDRNE	R4, =ulAsmAPIPriorityMask
	STR		R4, [r2]

	; Restore all system mode registers other than the SP (which is already
	; being used)
	POP		{R0-R12, R14}

	; Return to the task code, loading CPSR on the way.
	RFEIA	sp!

	MEND

	END

