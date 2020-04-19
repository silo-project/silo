#ifndef GATES_H
#define GATES_H

#include "Gates/buffer_gate.h"
#include "Gates/not_gate.h"
#include "Gates/and_gate.h"

enum class GATE_ID : uint8_t
{
	BUFFER_GATE = 0,
	NOT_GATE,
	AND_GATE,
	OR_GATE,
	NAND_GATE,
	NOR_GATE,
	XOR_GATE,
	XNOR_GATE,

};

#endif