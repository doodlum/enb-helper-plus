#include "PCH.h"

void JumpProcAddress(FARPROC procAddress, int64_t address)
{
	AbsoluteJump patch;
	patch.absolute64 = address;
	REL::safe_write(reinterpret_cast<std::uintptr_t>(procAddress), &patch, sizeof(AbsoluteJump));
}
