#include "cg_endian.h"

bool_t cg_is_little_endian_mode(void) {
	uint16_t num = 0x1234;
	if (*(uint8_t *)&num == 0x34) {
		return TRUE;
	} else if (*(uint8_t *)&num == 0x12) {
		return FALSE;
	}

	return FALSE;
}
