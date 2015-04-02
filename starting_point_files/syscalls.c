/**************************************************************************//*****
 * @file     stdio.c
 * @brief    Implementation of newlib syscall
 ********************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#undef errno
extern int errno;
extern int  _end;

__attribute__ ((used))
caddr_t _sbrk ( int incr )
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&_end;
  }
  prev_heap = heap;

  heap += incr;

  return (caddr_t) prev_heap;
}

__attribute__ ((used))
int link(char *old, char *new) {
return -1;
}

__attribute__ ((used))
int _close(int file)
{
  return -1;
}

__attribute__ ((used))
int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

__attribute__ ((used))
int _isatty(int file)
{
  return 1;
}

__attribute__ ((used))
int _lseek(int file, int ptr, int dir)
{
  return 0;
}

//************************************

char * ConsoleStringIn(void);
static char * InputPointer;
static int ctr;

__attribute__ ((used))
int _read(int file, char *ptr, int len)
{
	InputPointer = ConsoleStringIn(); // input string from console
	for (ctr=0; ctr<len; ctr++) {     // copy it to caller's buffer
		if (*InputPointer == '\0') break;
		*ptr++ = *InputPointer++;
	}
	*ptr++ = '\n';  // terminate it with newline
	return ++ctr;   // count the newline as well
}

//************************************

void USART_Send(unsigned short);
void ConsoleCharOut(char);

__attribute__ ((used))
int _write(int file, char *ptr, int len)
{
	int ctr=0;

	USART_Send(0x00FF);  // insure display support is sync'd
	while ((ctr < len) && (*ptr != '\0')) {
		ConsoleCharOut(*ptr++);
		ctr++;
	}

	return ctr;
}

__attribute__ ((used))
void abort(void)
{
  /* Abort called */
  while(1);
}
          
/* --------------------------------- End Of File ------------------------------ */
