/* STM32F051K8Tx memory map */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 64K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 8K
}

ENTRY(Reset_Handler)

SECTIONS
{
  .isr_vector : {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text : {
    . = ALIGN(4);
    *(.text*) 
    *(.rodata*) 
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
  } >FLASH

  .ARM.exidx : {
    *(.ARM.exidx*)
  } >FLASH

  _etext = .;
  
  _sidata = _etext;

  .data : AT ( _etext ) {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM

  .bss (NOLOAD) : {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >RAM

  _estack = ORIGIN(RAM) + LENGTH(RAM);
}
