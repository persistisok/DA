#
# This file is part of the linux-prinj project.
# https://gitlab.com/pgregoire/linux-prinj/
#
# Copyright 2022 Philippe Grégoire <git@pgregoire.xyz>
#
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

.intel_syntax noprefix
.text
.global _start

_start:
	# debugging
	#jmp	_start
	nop; nop

	jmp	entry
jmpret:
	# The target return address is stored right before this
	# shellcode. Use it as the jmp target.
	# Subtract 2 if the nops above are removed.
	jmp	[rip-10-8]


entry:
	# backup registries
	push    rsp
	push    rbp
	push    rax
	push    rbx
	push    rcx
	push    rdx
	push    rdi
	push    rsi
	push    r8
	push    r9
	push    r10
	push    r11
	push	r12
	push	r13
	push	r14
	push	r15

	call    main

	# restore registries
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop     r11
	pop     r10
	pop     r9
	pop     r8
	pop     rsi
	pop     rdi
	pop     rdx
	pop     rcx
	pop     rbx
	pop     rax
	pop     rbp
	pop     rsp

	jmp     jmpret

main:
	# write(1, "Hello World!\n", 13);
	mov	rax, 0x6e
	push    rax
	mov rax, 0x5c2165646f632079
	push	rax
	mov	rax, 0x6d20732774616854 
			   
	push	rax
	mov	rdi, 1
	mov	rsi, rsp
	mov	rdx, 13
	mov	rax, 1
	syscall

	pop	rax
	pop	rax
	pop	rax
	ret
