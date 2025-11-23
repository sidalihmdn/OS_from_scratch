ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    .text ALIGN(4K) :
    {
        *(.text*)
        *(.rodata*)
    }

    .data ALIGN(4K) :
    {
        *(.data*)
        *(.bss*)
    }

    kernel_end = .;       /* symbol marking end of kernel */

    heap_start = .;       /* heap starts here */
    . = . + 50M;           /* reserve 50 MB for heap */
    heap_end = .;         /* heap ends here */
}
