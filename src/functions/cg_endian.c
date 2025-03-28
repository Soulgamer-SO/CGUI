#include "cg_endian.h"

bool cg_is_little_endian_mode(void) {
	uint16_t num = 0x1234;
	if (*(uint8_t *)&num == 0x34) {
		return true;
	} else if (*(uint8_t *)&num == 0x12) {
		return false;
	}

	return false;
}
