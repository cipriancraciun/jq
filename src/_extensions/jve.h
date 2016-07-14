#ifndef JVE_H
#define JVE_H


#include "../jv.h"

#include <stdbool.h>


extern jv jve_string_input (jv value, bool encode);
extern jv jve_string_input_1 (jv value, jv encode);

extern jv jve_string_output (jv value, bool decode);
extern jv jve_string_output_1 (jv value, jv decode);


#endif
