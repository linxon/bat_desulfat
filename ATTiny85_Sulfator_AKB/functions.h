/*
 * functions.h
 *
 * Created: 08.01.2022 12:01:35
 *  Author: linxon
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <ctype.h>
#include "typedef.h"

#define my_delay_us(us)		({	\
	uint8_t n = us;				\
	while (0 < n) {				\
		_delay_us(1);			\
		--n;					\
	}							\
})

#define my_delay_ms(ms)		({	\
	uint8_t n = ms;				\
	while (0 < n) {				\
		_delay_ms(1);			\
		--n;					\
	}							\
})

#define my_toupper(c)		(((c) >= 'a' && (c) <= 'z')? (c) + 'A' - 'a': (c))
#define my_tolower(c)		(((c) >= 'A' && (c) <= 'Z')? (c) + 'a' - 'A': (c))
#define my_isdigit_asm1(c, r)						\
	__asm__ __volatile__(							\
		"cpi %[sign], 0x30"					"\n\t"	\
		"brlt L1_%="						"\n\t"	\
		"cpi %[sign], 0x3A"					"\n\t"	\
		"brge L1_%="						"\n\t"	\
		"rjmp L2_%="						"\n\t"	\
		"L1_%=: "	"clr %[retval]"			"\n\t"	\
		"L2_%=: "	""								\
													\
		: [retval] "=r" (r)							\
		: [sign] "d" (c)							\
	);

extern bool my_isdigit_asm(char);
extern uint8_t my_atoi_u8(const char *);
extern uint16_t my_atoi_u16(const char *);
extern uint32_t my_atoi_u32(const char *);
extern int8_t my_atoi_8(const char *);
extern int16_t my_atoi_16(const char *);
extern uint32_t my_atoi_u32(const char *);
extern void my_itoa_u8(uint8_t, char *);
extern void my_itoa_u16(uint16_t, char *);
extern void my_itoa_u32(uint32_t, char *);
extern void my_itoa_8(int8_t, char *);
extern void my_itoa_16(int16_t, char *);
extern void my_itoa_32(int32_t, char *);
extern void my_itohex_u8(uint8_t, char *);
extern void my_itohex_u16(uint16_t, char *);
extern void my_itohex_u32(uint32_t, char *);
extern float my_atof(const char *);
extern void my_ftoa(float, char *, uint8_t);
extern void my_strrev(char *);
extern bool my_strcmp(const char *, const char *);
extern uint16_t crc_chk(const byte *, uint8_t);

extern uint16_t conv_to_999(uint16_t);
extern uint8_t count_999(uint16_t range);

extern uint8_t bcd2bin(uint8_t);
extern uint8_t bin2bcd(uint8_t);

#endif /* FUNCTIONS_H_ */
