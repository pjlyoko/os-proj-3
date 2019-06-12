#include "Tool.h"

Tool::Tool() {
	taken = ATOMIC_VAR_INIT(false);
}

void Tool::take(int no) {
	this->taken = true;
	this->pizzaioloNo = no;
}

void Tool::leave() {
	this->taken = false;
	this->pizzaioloNo = -1;
}