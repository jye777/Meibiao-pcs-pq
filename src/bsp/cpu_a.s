
		EXPORT  CPU_SR_Save
        EXPORT  CPU_SR_Restore
		
		AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

CPU_SR_Save
        MRS     R0, PRIMASK                     ; Set prio int mask to mask all (except faults)
        CPSID   I
        BX      LR


CPU_SR_Restore                                  ; See Note #2.
        MSR     PRIMASK, R0
        BX      LR

		END
