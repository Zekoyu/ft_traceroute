#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

// https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h
#include <elf.h>

#include "./libft/libft.h"

void ft_nm(char *filename)
{
	int fd;
	struct stat file_infos;
	char *file_content;

	if ((fd = open(filename, O_RDONLY)) < 0)
	{
		ft_printf("Cannot open file %s\n", filename);
		return;
	}

	if (fstat(fd, &file_infos) < 0)
	{
		ft_printf("Cannot stat file %s\n", filename);
		close(fd);
		return;
	}

	// open read only (and not shared with other processes)
	if((file_content = mmap(NULL, file_infos.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		ft_printf("Cannot map file %s\n", filename);
		close(fd);
		return;
	}

	// from the man, we can close the fd without invalidating the mapping, so close it asap
	close(fd);
	ft_printf("File %s successfully opened and mapped\n", filename);


	// only read first bytes to check magic number and 32/64 bits
	// see https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h
	// and https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
	unsigned char *elf_identification = (unsigned char*)file_content;

	if (elf_identification[EI_MAG0] == ELFMAG0 && elf_identification[EI_MAG1] == ELFMAG1 && elf_identification[EI_MAG2] == ELFMAG2 && elf_identification[EI_MAG3] == ELFMAG3)
	{
		ft_printf("File is an ELF file\n");

		unsigned char class = elf_identification[EI_CLASS];

		if (class == 1)
		{
			ft_printf("File is 32bit\n");
		}
		else if(class == 2)
		{
			ft_printf("File is 64bit\n");
		}
		else
		{
			ft_printf("Invalid TODO\n");
			return;
		}


		if (class == 2)
		{
			Elf64_Ehdr header = *(Elf64_Ehdr *)file_content;

			printf("Test: %x\n", header.e_entry);
		}
	}
	else
	{
		ft_printf("File is not an ELF file\n");
		return;
	}
}

// https://medium.com/a-42-journey/nm-otool-everything-you-need-to-know-to-build-your-own-7d4fef3d7507
int main(int argc, char **argv)
{
	char *filenames[argc];
	int filecount;

	if (argc == 1)
	{
		filecount = 1;
		filenames[0] = "a.out";
	}
	else
	{
		filecount = argc - 1;
		for (int i = 1; i < argc; i++)
			filenames[i - 1] = argv[i];
	}

	for (int i = 0; i < filecount; i++)
	{
		char *file = filenames[i];

		if (filecount != 1) // print filenames
			ft_putstr_fd("test", STDOUT_FILENO);
			// ft_printf("\n%s:\n", file);

		ft_nm(file);
	}
}