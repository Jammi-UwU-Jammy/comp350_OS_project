#cp diskc_backup.img diskc.img

bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o

bcc -ansi -c -o shell_c.o shell.c
as86 userlib.asm -o userlib_asm.o
ld86 -o shell -d shell_c.o userlib_asm.o

dd if=kernel of=diskc.img bs=512 conv=notrunc seek=3

#./loadFile shell
#./loadFile tstpr1
#./loadFile tstpr2
#./loadFile message.txt
