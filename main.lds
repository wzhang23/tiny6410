SECTIONS
{
	. = 0x50000000;		/* current address */
	
	.text : {
		startup.o
		* (.startup)
		* (.text)
	}

	.data : {
		* (.data)
	}

	bss_start = .;
	.bss : {
		* (.bss)
	}
	bss_end	  = .;
}
