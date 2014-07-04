#include <xc.inc>
GLOBAL _trig ; make _add globally accessible
SIGNAT _trig,4217 ; tell the linker how it should be called
; everything following will be placed into the mytext psect
PSECT mytext,local,class=CODE,delta=2
; our routine to add to ints and return the result

;***** VARIABLE DEFINITIONS
pulsecount	equ		0x08
times		equ		0x09
delay		equ		0x0A

movlw	0x01
movwf	times

_trig:
    movlw	77
	movwf	pulsecount
	call 	pulsehi
	movlw	0xff
	movwf	pulsecount
	call	pulselo
	call	pulselo
	call	pulselo
	call	pulselo
	movlw	48
	movwf	pulsecount
	call	pulselo
	movlw	19
	movwf	pulsecount
	call	pulsehi
	movlw	58
	movwf	pulsecount
	call	pulselo
	movlw	19
	movwf	pulsecount
	call	pulsehi
	movlw	135
	movwf	pulsecount
	call	pulselo
	movlw	19
	movwf	pulsecount
	call	pulsehi
	movlw	0xff
	movwf	pulsecount
	call	pulselo
	call	pulselo
	call	pulselo
	call	pulselo
	call	pulselo
	call	pulselo
	call	pulselo
	call	pulselo
	call	pulselo
	movlw	125
	movwf	pulsecount
	call	pulselo
	decfsz	times,1
	goto 	_trig

	movlw	0xff
	movwf	pulsecount
	call	pulselo

	sleep


pulsehi
	bsf		GPIO,2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop							;10
	nop
	nop
	nop
	bcf		GPIO,2
	nop
	nop
	nop
	nop
	nop
	nop							;20
	nop
	nop
	nop
	decfsz	pulsecount,1
	goto pulsehi
	retlw	0

pulselo
	bcf		GPIO,2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop							;10
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop							;20
	nop
	nop
	nop
	decfsz	pulsecount,1
	goto pulselo
	retlw	0

RETURN


