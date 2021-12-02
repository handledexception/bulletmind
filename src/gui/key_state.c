#include "gui/key_state.h"

#include "core/types.h"

void gui_key_up_win32(int key, struct key_state* state)
{
	if (key < 0 || key > 0xfe)
		return;
	u32* ptr = (u32*)state;
	u32 bf = 1u << (key & 0x1f);
	ptr[(key >> 5)] |= bf;
}

void gui_key_down_win32(int key, struct key_state* state)
{
	if (key < 0 || key > 0xfe)
		return;
	u32* ptr = (u32*)state;
	u32 bf = 1u << (key & 0x1f);
	ptr[(key >> 5)] &= ~bf;
}
