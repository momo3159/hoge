clang++ -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17 -c main.cpp
ld.lld --entry KernelMain -z norelro --image-base 0x100000 --static -o kernel.elf main.o





export CPPFLAGS="-I/home/t0mmy/osbook/devenv/x86_64-elf/include/c++/v1 -I/home/t0mmy/osbook/devenv/x86_64-elf/include -I/home/t0mmy/osbook/devenv/x86_64-elf/include/freetype2 -I/home/t0mmy/edk2/MdePkg/Include -I/home/t0mmy/edk2/Include/X64 -nostdlibinc -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS -DEFIAPI='__attribute__((ms_abi))'"
clang++ "${CPPFLAGS}" -O2 --target=x86_64-elf -fno-exceptions -ffreestanding -c main.cpp
clang++ -I/home/t0mmy/osbook/devenv/x86_64-elf/include/c++/v1 -I/home/t0mmy/osbook/devenv/x86_64-elf/include -I/home/t0mmy/osbook/devenv/x86_64-elf/include/freetype2 -I/home/t0mmy/edk2/MdePkg/Include -I/home/t0mmy/edk2/Include/X64 -nostdlibinc -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS -DEFIAPI='__attribute__((ms_abi))' -O2 --target=x86_64-elf -fno-exceptions -ffreestanding -c main.cpp

export LDFLAGS="-L/home/t0mmy/edk2/lib"
ld.lld $LDFLAGS --entry KernelMain -z norelro --image-base 0x100000 --static -o kernel.elf main.o