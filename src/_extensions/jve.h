#ifndef JVE_H
#define JVE_H


#include "../jv.h"

#include <stdbool.h>
#include <stdint.h>


extern jv jve_string_input (jv value, bool encode);
extern jv jve_string_input_1 (jv value, jv encode);

extern jv jve_string_output (jv value, bool decode);
extern jv jve_string_output_1 (jv value, jv decode);


extern jv jve_string_hex (void const * data, size_t size);


typedef union jve_hash_128 {
	uint8_t uint8[16];
	uint16_t uint16[8];
	uint32_t uint32[4];
	uint64_t uint64[2];
} jve_hash_128_t;

extern jv jve_hash_128 (jv value, jve_hash_128_t * hash);


#endif
