bits 64
section .text

global IoOut32 ; void IoOut32(uint16_t addr, uint32_t data);
IoOut32:
  mov dx, di
  mov eax, esi
  out dx, eax
  ret

global IoIn32 ; uint32_t IoIn32(uint16_t addr);
IoIn32:
  mov dx, di ; 引数はRDI, RSI, ...
  in eax, dx ; 戻り値はRAX
  ret