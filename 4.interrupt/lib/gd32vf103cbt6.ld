MEMORY{
    flash (rxai!w) : ORIGIN = 0x08000000, LENGTH = 128K
    ram (wxa!ri) : ORIGIN = 0x20000000, LENGTH = 32K
}

SECTIONS{
  .text : {
    _vector_table = .;
    KEEP(*(.isr_vectors))
    . = ALIGN(2);
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
  } > flash
  
  .data : AT(ADDR(.text) + SIZEOF(.text)){
    _data_start = .;
    *(.data*)
    . = ALIGN(4);
    _data_end = .;
  } > ram
  
  .bss : {
    _bss_start = .;
    *(.bss*)
    . = ALIGN(4);
    _bss_end = .;
  } > ram
}

PROVIDE(_stack_end = ORIGIN(ram) + LENGTH(ram));
PROVIDE(_data_load = LOADADDR(.data));
