dd if=/dev/zero of=diskc.img bs=512 count=256

dd if=bootload of=diskc.img bs=512 seek=0 conv=notrunc

bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o

bcc -ansi -c -o shell_c.o shell.c
as86 userlib.asm -o userlib_asm.o
ld86 -o shell -d shell_c.o userlib_asm.o

#cp diskc_backup.img diskc.img
<<<<<<< HEAD
#./loadFile shell
=======
./loadFile kernel
./loadFile shell
./loadFile message.txt
./loadFile tstpr1
./loadFile tstpr2
>>>>>>> 4ceac34 (added listing files and sizes)

#dd if=kernel of=diskc.img bs=512 conv=notrunc seek=3
