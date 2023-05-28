# Description for the IR format

## Syntax
- The IR format is a text format, with one instruction per line.
- Very similar to intel flavor x86 assembly
- The IR format is case sensitive
- The IR format is whitespace sensitive
- Format is `instruction [operand1, operand2, ...]`
- Operands are prefixed to indicate their type
	- no prefix for register
	- `C` for constant
	- `S` for sp (stack pointer) relative
	- `V` for variable in bss or data
- Operands have their size (in bytes) in parentheses if it is not inferrable from the instruction
    - e.g. mov 0 V(4)a where `a` is a variable
- Comments begin with a `;` and end at the end of the line

## Instructions
- `prologue` - function prologue
- `epilogue` - function epilogue
- `mov dst src` (dst = destination; src = source)
- `(arithmetic) dst(src1) src2` (dst = dst op src2)
- `push src` (push src on to stack)
- `pop dst` (pop src from stack)
- `call func` (call function)
- `ret a` (a = return value)
- `jmp label` (jump to label)
- `jcc label` (pop from stack and jump to label if true)
    - `jcc` instructions: https://www.felixcloutier.com/x86/jcc.html
- `cmpcc` (compare r0 and r1, push result on to stack)
	- `cc` has same meaning as in `jcc`
- `label:` (label)
	- labels beginning with . are local labels