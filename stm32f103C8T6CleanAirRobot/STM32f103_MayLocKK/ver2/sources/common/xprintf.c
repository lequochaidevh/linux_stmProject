/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   13/08/2016
 ******************************************************************************
**/
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "utils.h"
#include "xprintf.h"

#define CR_CRLF								(1)

#define DYMC_BUFFER_SIZE					(256)

#define DYMC_UART_SHELL_TYPE				(0x00)
#define DYMC_RF_REMOTE_TYPE					(0x01)

typedef void (*xiostream)(char* str, uint8_t c);

/* pointer to the output stream (external) */
void (*xfunc_out)(uint8_t);

static uint8_t dymc_print_type = DYMC_UART_SHELL_TYPE;

void xputc(char* str, uint8_t c) {
	(void)str;
	if (CR_CRLF && (c == (uint8_t)'\n')) {
		xfunc_out('\r');
	}
	if (xfunc_out) {
		xfunc_out(c);
	}
}

void sxputc(char* str, uint8_t c) {
	*str = c;
}

static int xstream(xiostream stream, char* string, const char *fmt, va_list va_args) {
	uint32_t index = 0;
	uint32_t num, digit;
	int32_t i;

	uint32_t zero_padding = 0;
	int32_t format_lenght = 0;
	uint32_t base;
	uint32_t minus;
	uint8_t num_stack[11];
	uint8_t* ps;

	while (*fmt) {
		switch (*fmt) {
		case '%':
			zero_padding = 0;
			if (fmt[1] == '0') {
				zero_padding = 1;
				++fmt;
			}
			format_lenght = 0;
			while (*++fmt) {
				switch (*fmt) {
				case '%':
					stream((string + index), *fmt);
					index++;
					goto next_loop;

				case 'c':
					stream((string + index), va_arg(va_args, int32_t));
					index++;
					goto next_loop;

				case 'd':
				case 'X':
				case 'x':
					minus = 0;
					num = va_arg(va_args, uint32_t);
					if (*fmt == 'd') {
						base = 10;
						if (num & (uint32_t)0x80000000) {
							num = -(int32_t)num;
							minus = 1;
						}
					} else {
						base = 16;
					}
					for (digit = 0; digit < sizeof(num_stack);) {
						num_stack[digit++] = num%base;
						num /= base;
						if (num == 0) break;
					}
					if (minus) num_stack[digit++] = 0x7F;
					if (format_lenght > digit) {
						int8_t paddingint8_t = ' ';
						format_lenght -= digit;
						if (zero_padding)
							paddingint8_t = '0';
						while (format_lenght--) {
							stream((string + index), paddingint8_t);
							index++;
						}
					}
					for (i = digit-1; i >= 0; i--) {
						if (num_stack[i] == 0x7F) {
							stream((string + index), '-');
						} else if (num_stack[i] > 9) {
							stream((string + index), num_stack[i]-10 + 'A');
						} else {
							stream((string + index), num_stack[i] + '0');
						}
						index++;
					}
					goto next_loop;

				case 's':
					ps = va_arg(va_args, uint8_t*);
					while(*ps) {
						stream((string + index), *ps++);
						index++;
					}
					goto next_loop;

				default:
					if (*fmt >= '0' && *fmt <= '9') {
						format_lenght = format_lenght*10 + (*fmt-'0');
					} else {
						goto exit;
					}
				}

			}
			if (*fmt == 0) {
				goto exit;
			}

		default:
			stream((string + index), *fmt);
			index++;
			break;
		}
next_loop:
		fmt++;
	}
exit:
	va_end(va_args);
	return index;
}

void xprintf(const char *fmt, ...) {
	va_list va;
	va_start(va,fmt);
	xstream(xputc, NULL, fmt, va);
	va_end(va);
}

int xsprintf(char* str,const char* fmt, ...) {
	int index;
	va_list va;
	va_start(va,fmt);
	index = xstream(sxputc, str, fmt, va);
	va_end(va);
	return index;
}

int set_dymc_output_type(uint8_t type) {
	int ret = -1;
	switch (type) {
	case DYMC_UART_SHELL_TYPE:
	case DYMC_RF_REMOTE_TYPE:
		dymc_print_type = type;
		ret = 0;
		break;
	default:
		break;
	}
	return ret;
}

void xdymcprintf(const char* fmt, ...) {
	va_list va;

	va_start(va,fmt);

	switch (dymc_print_type) {
	case DYMC_UART_SHELL_TYPE:
		xstream(xputc, NULL, fmt, va);
		break;

	default:
		break;
	}

	va_end(va);
}
