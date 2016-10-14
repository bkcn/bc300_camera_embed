		EXPORT	_udelay
		EXPORT	udelay
		EXPORT	___delay
		EXPORT	__delay
		IMPORT	loops_per_sec

	AREA |C$$code|, CODE, READONLY


		
LC0		
	    DCD loops_per_sec


udelay
_udelay
		mov	r2, #0x1000
		orr	r2, r2, #0x00c6
		mul	r1, r0, r2
		ldr	r2, LC0
		ldr	r2, [r2]
		mov	r1, r1, lsr #11
		mov	r2, r2, lsr #11
		mul	r0, r1, r2
		movs	r0, r0, lsr #10
		moveq pc,lr

; Delay routine

__delay
___delay
		subs	r0, r0, #1
		movcc	pc,lr
		subs	r0, r0, #1
		movcc	pc,lr
		subs	r0, r0, #1
		movcc	pc,lr
		subs	r0, r0, #1
		movcc	pc,lr
		subs	r0, r0, #1
		movcc	pc,lr
		subs	r0, r0, #1
		movcc	pc,lr
		subs	r0, r0, #1
		movcc	pc,lr
		subs	r0, r0, #1
		bcs	___delay
		mov		pc,lr

	END