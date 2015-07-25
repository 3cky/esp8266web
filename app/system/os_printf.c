/******************************************************************************
 * FileName: os_printf.c
 * Description: Alternate SDK (libmain.a)
 * Author: PV`
 * (c) PV` 2015
*******************************************************************************/
#include "bios.h"
#include <stdarg.h>
#include "rom2ram.h"
#include "os_printf.h"

extern char * _sprintf_buf;  // 0x3FFFE360
extern char print_mem_buf[1024]; // 0x3FFFE364..0x3FFFEA80 max 1820 bytes
extern bool system_get_os_print(void);
//=============================================================================
// FLASH code (ICACHE_FLASH_ATTR)
//=============================================================================
//=============================================================================
// int os_printf_plus(const char *format, ...)
// Использует буфер в области RAM-BIOS
//-----------------------------------------------------------------------------
int ICACHE_FLASH_ATTR rom_printf(const char *format, ...)
{
	int i = 0;
	if(system_get_os_print()) {
		va_list args;
		va_start(args, format);
		i = ets_vprintf(ets_write_char, ((uint32)format >> 30)? rom_strcpy(print_mem_buf, (void *)format, sizeof(print_mem_buf)-1) : format, args);
		va_end (args);
	}
	return i;
}
//=============================================================================
// int os_sprintf_plus(char *str, const char *format, ...)
// Использует буфер в области RAM-BIOS
// Вывод может быть расположен в IRAM
//-----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR _sprintf_out(char c)
{
	if(_sprintf_buf != NULL) {
		// *_sprintf_buf++ = c;
		// write_iram_chr(_sprintf_buf++, c);
		union {
			uint8 uc[4];
			uint32 ud;
		}tmp;
  	  	register uint32 *p = (uint32 *)((uint32)_sprintf_buf & (~3));
		tmp.ud = *p;
		tmp.uc[(uint32)_sprintf_buf & 3] = c;
		_sprintf_buf++;
		*p = tmp.ud;
	}
}

int ICACHE_FLASH_ATTR ets_sprintf(char *str, const char *format, ...)
{
	_sprintf_buf = str;
	va_list args;
	va_start(args, format);
	int i = ets_vprintf(_sprintf_out, ((uint32)format >> 30)? rom_strcpy(print_mem_buf, (void *)format, sizeof(print_mem_buf)-1) : format, args);
	va_end (args);
	return i;
}
