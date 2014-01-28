#ifndef _COMMON_H_
#define _COMMON_H_

/* data type define */
typedef signed	 char				INT8S;
typedef unsigned char				INT8U;
typedef signed	 short int	INT16S;
typedef unsigned short int	INT16U;
typedef signed	 long 			INT32S;
typedef unsigned long 			INT32U;	

/* handy sizes */
#define SZ_1K                           0x00000400
#define SZ_4K                           0x00001000
#define SZ_8K                           0x00002000
#define SZ_16K                          0x00004000
#define SZ_64K                          0x00010000
#define SZ_128K                         0x00020000
#define SZ_256K                         0x00040000
#define SZ_512K                         0x00080000

#define SZ_1M                           0x00100000
#define SZ_2M                           0x00200000
#define SZ_4M                           0x00400000
#define SZ_8M                           0x00800000
#define SZ_16M                          0x01000000
#define SZ_32M                          0x02000000
#define SZ_64M                          0x04000000
#define SZ_128M                         0x08000000
#define SZ_256M                         0x10000000
#define SZ_512M                         0x20000000

#define SZ_1G                           0x40000000
#define SZ_2G                           0x80000000

typedef volatile unsigned long	vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char	vu_char;

/* bit field operation */
#define vi *( volatile unsigned int * ) 
#define set_zero( addr, bit ) ( (vi addr) &= ( ~ ( 1 << (bit) ) ) )
#define set_one( addr, bit ) ( (vi addr) |= ( 1 << ( bit ) ) )
#define set_bit( addr, bit, val ) ( (vi addr) = (( vi addr)&=(~(1<<(bit))) ) | ( (val)<<(bit) ) )
#define set_2bit( addr, bit, val ) ( (vi addr) = (( vi addr)&(~(3<<(bit))) ) | ( (val)<<(bit) ) )
#define set_nbit( addr, bit, len,  val ) \
	( (vi addr) = ((( vi addr)&(~(( ((1<<(len))-1) )<<(bit))))  | ( (val)<<(bit) ) ))
#define get_bit( addr, bit ) ( (( vi addr ) & ( 1 << (bit) )) > 0  )
#define get_val( addr, val ) ( (val) = vi addr )
#define read_val( addr ) ( vi ( addr ) )
#define set_val( addr, val ) ( (vi addr) = (val) )
#define or_val( addr, val ) ( (vi addr) |= (val) ) 

/* function prototype */
/* clock function */
void clock_init(void);
/* uart function */
void uart_init(void);
void uart_putchar(INT8S ch);
INT8S uart_getchar(void);

#endif
