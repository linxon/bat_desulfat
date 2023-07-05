/*
 * functions.c
 *
 * Created: 08.01.2022 12:01:24
 *  Author: linxon
 */

/*

	' ' (0x20) space (SPC)

	'\t' (0x09) horizontal tab (TAB)

	'\n' (0x0a) newline (LF)

	'\v' (0x0b) vertical tab (VT)

	'\f' (0x0c) feed (FF)

	'\r' (0x0d) carriage return (CR)

*/

#include "functions.h"


bool my_isdigit_asm(char _c) {
	register bool _ret = TRUE;

	asm volatile(
		"cpi %[sign], 0x30"					"\n\t"
		"brlt L1_%="						"\n\t"
		"cpi %[sign], 0x3A"					"\n\t"
		"brge L1_%="						"\n\t"
		"rjmp L2_%="						"\n\t"

		"L1_%=: "	"clr %[retval]"			"\n\t"
		"L2_%=: "	""

		: [retval] "=r" (_ret)
		: [sign] "d" (_c)
	);

	return _ret;
}

uint8_t my_atoi_u8(const char *s) {
	uint8_t n = 0;

	while (isdigit(*s))
		n = 10 * n + (*(s++) - '0');

	return n;
}

uint16_t my_atoi_u16(const char *s) {
	uint16_t n = 0;

	while (isdigit(*s))
		n = 10 * n + (*(s++) - '0');

	return n;
}

uint32_t my_atoi_u32(const char *s) {
	uint32_t n = 0;

	while (isdigit(*s))
		n = 10 * n + (*(s++) - '0');

	return n;
}

int8_t my_atoi_8(const char *s) {
	register uint8_t i = 0;
	register int8_t sign = 0;
	int8_t n = 0;

	for (; s[i] == 0x20
		|| s[i] == 0x09
		|| s[i] == 0x0A; ++i);

	sign = (s[i] == '-')? -1: 1;
	if (s[i] == '-' || s[i] == '+')
		++i;

	for (; isdigit(s[i]); ++i)
		n = 10 * n + (s[i] - '0');

	return n * sign;
}

int16_t my_atoi_16(const char *s) {
	register uint8_t i = 0;
	register int8_t sign = 0;
	int16_t n = 0;

	for (; s[i] == 0x20
		|| s[i] == 0x09
		|| s[i] == 0x0A; ++i);

	sign = (s[i] == '-')? -1: 1;
	if (s[i] == '-' || s[i] == '+')
		++i;

	for (; isdigit(s[i]); ++i)
		n = 10 * n + (s[i] - '0');

	return n * sign;
}

/*

void my_itoa_u8(uint8_t num, char *dst) {
	register uint8_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	tmp = num;
	do {
		dst++;
		tmp /= 10;
	} while (tmp > 0);

	*dst = '\0';

	for (tmp = num; tmp > 0; tmp /= 10)
		*(--dst) = tmp % 10 + '0';
}

*/

void my_itoa_u8(uint8_t num, char *dst) {
	register uint8_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	tmp = num;
	do {
		dst++;
		tmp /= 10;
	} while (tmp > 0);

	*dst = '\0';

	for (tmp = num; tmp > 0; tmp /= 10)
		*(--dst) = tmp % 10 + '0';
}

void my_itoa_u16(uint16_t num, char *dst) {
	register uint16_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	tmp = num;
	do {
		dst++;
		tmp /= 10;
	} while (tmp > 0);

	*dst = '\0';

	for (tmp = num; tmp > 0; tmp /= 10)
		*(--dst) = tmp % 10 + '0';
}

void my_itoa_u32(uint32_t num, char *dst) {
	register uint32_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	tmp = num;
	do {
		dst++;
		tmp /= 10;
	} while (tmp > 0);

	*dst = '\0';

	for (tmp = num; tmp > 0; tmp /= 10)
		*(--dst) = tmp % 10 + '0';
}

void my_itoa_8(int8_t num, char *dst) {
	register int8_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	else if (num < 0) {
		num *= (-1);
		*dst++ = '-';
	}

	tmp = num;
	do {
		dst++;
		tmp /= 10;
	} while (tmp > 0);

	*dst = '\0';

	for (tmp = num; tmp > 0; tmp /= 10)
		*(--dst) = tmp % 10 + '0';
}

void my_itoa_16(int16_t num, char *dst) {
	register int16_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	else if (num < 0) {
		num *= (-1);
		*dst++ = '-';
	}

	tmp = num;
	do {
		dst++;
		tmp /= 10;
	} while (tmp > 0);

	*dst = '\0';

	for (tmp = num; tmp > 0; tmp /= 10)
		*(--dst) = tmp % 10 + '0';
}

void my_itoa_32(int32_t num, char *dst) {
	register int32_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	else if (num < 0) {
		num *= (-1);
		*dst++ = '-';
	}

	tmp = num;
	do {
		dst++;
		tmp /= 10;
	} while (tmp > 0);

	*dst = '\0';

	for (tmp = num; tmp > 0; tmp /= 10)
		*(--dst) = tmp % 10 + '0';
}

void my_itohex_u8(uint8_t num, char *dst) {
	register uint8_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	while (num != 0) {
		tmp = num % 16;
		tmp = (tmp < 10)? tmp + '0': tmp + 'A' - 10;

		*dst++ = tmp;
		num /= 16;
	}

	*dst = '\0';
}

void my_itohex_u16(uint16_t num, char *dst) {
	register uint16_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	while (num != 0) {
		tmp = num % 16;
		tmp = (tmp < 10)? tmp + '0': tmp + 'A' - 10;

		*dst++ = tmp;
		num /= 16;
	}

	*dst = '\0';
}

void my_itohex_u32(uint32_t num, char *dst) {
	register uint32_t tmp = 0;

	if (num == 0) {

		*dst++ = '0';
		*dst = '\0';

		return;
	}

	while (num != 0) {
		tmp = num % 16;
		tmp = (tmp < 10)? tmp + '0': tmp + 'A' - 10;

		*dst++ = tmp;
		num /= 16;
	}

	*dst = '\0';
}

float my_atof(const char *s) {
	register uint8_t i = 0;
	register int8_t sign = 0;
	float power = 1.0;
	float n = 0.0;

	for (; s[i] == 0x20
		|| s[i] == 0x09
		|| s[i] == 0x0A; ++i);

	sign = (s[i] == '-')? -1: 1;
	if (s[i] == '-' || s[i] == '+')
		++i;

	for (; isdigit(s[i]); ++i)
		n = 10 * n + (s[i] - '0');

	if (s[i] == '.')
		++i;

	for (; isdigit(s[i]); ++i) {
		n = 10 * n + (s[i] - '0');
		power *= 10;
	}

	return sign * n / power;
}

void my_ftoa(float num, char *s, uint8_t apoint) {
	register int16_t tmp = 0;
	register uint8_t i = 0;
	int16_t ipart = (int16_t) num;
	float fpart = num - (float) ipart;

	if (num < 0) {
		ipart *= (-1);
		fpart *= (-1);
		*s++ = '-';
	}

	if (ipart == 0) {
		*s = '0';
		++i;
	}

	else {
		tmp = ipart;

		do {
			s++;
			tmp /= 10;
		} while (tmp > 0);

		for (tmp = ipart; tmp > 0; tmp /= 10) {
			*(--s) = tmp % 10 + '0';
			++i;
		}
	}

	if (apoint != 0) {
		s[i] = '.';
		fpart = fpart * pow(10, apoint);

		my_itoa_u16((uint16_t) fpart, s + i + 1);
	}
}

void my_strrev(char *s) {
	register uint8_t j, i = 0;
	register char tmp_c = '\0';

	while (s[++i] != '\0');

	for (--i, j = 0; TRUE; --i) {
		if (j == i || j == i+1)
			break;

		tmp_c = s[j];
		s[j++] = s[i];
		s[i] = tmp_c;
	}
}

bool my_strcmp(const char *s1, const char *s2) {
	while (*s2 != '\0') {
		if (*s1 != *s2)
			return FALSE;

		s1++;
		s2++;
	}

	return TRUE;
}

// Пример: https://ctlsys.com/support/how_to_compute_the_modbus_rtu_message_crc/
// полином - 16#A001 (CRC16-IBM)
uint16_t crc_chk(const byte *data, uint8_t length) {
	register uint8_t i;
	register uint16_t reg_crc = 0xFFFF;

	while (length--) {
		reg_crc ^= *data++;

		for (i = 0; i < 8; ++i) {
			if(reg_crc & 0x01)
				reg_crc = (reg_crc >> 1) ^ 0xA001;
			else
				reg_crc = reg_crc >> 1;
	   }
	}

	return reg_crc;
}

inline uint16_t conv_to_999(uint16_t range) {
	register uint8_t tmp = range;

	for (uint8_t i = 0; i < tmp; ++i)
		range *= 10;

	return (range / tmp) -1;
}

inline uint8_t count_999(uint16_t value) {
	uint8_t count = 0;

	while (value /= 10)
		count++;

	return count;
}

inline uint8_t bcd2bin(uint8_t val) {
	return val - 6 * (val >> 4);
}

inline uint8_t bin2bcd(uint8_t val) {
	return val + 6 * (val / 10);
}


// TODO: Р”РѕР±Р°РІРёС‚СЊ СЃРІРѕРё РєР°СЃС‚РѕРјРЅС‹Рµ С„СѓРЅРєС†РёРё РґР»СЏ СЂР°Р±РѕС‚С‹ СЃРѕ СЃС‚СЂРѕРєР°РјРё (РїРѕРїСЂРѕР±РѕРІР°С‚СЊ РѕРїС‚РёРјРёР·РёСЂРѕРІР°С‚СЊ РёС…)
/*

char *my_strtok(char *s, char delimiter) {
	char *ptr = s;

	//while (*ptr != )

	return 0;
}

*/
