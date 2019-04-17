/*
 * read_test.c
 * Test bed for demo drivers
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define READ	0
#define WRITE	1

static inline void usage(char *prg)
{
	fprintf(stderr,"Usage: %s opt=read/write device_file num_bytes_to_read\n"
			" opt = '0'  => we shall issue the read(2)\n"
			" opt = '1' => we shall issue the write(2)\n",
		       prg);
}

int main(int argc, char **argv)
{
	int fd, opt = READ, flags = O_RDONLY;
	ssize_t n;
	char *buf = NULL;
	size_t num = 0;
	
	if( argc != 4 ) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	opt = atoi(argv[1]);
	if (opt != 0 && opt != 1) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (1 == opt)
		flags = O_WRONLY;

	if( (fd=open(argv[2], flags, 0)) == -1)
		perror("open"),exit(1);
	printf("Device file %s opened (in %s mode): fd=%d\n",
		       argv[2], (flags == O_RDONLY ? "read-only" : "write-only"), fd);

	num = atoi(argv[3]);
	if ((num < 0) || (num > INT_MAX)) {
		fprintf(stderr,"%s: number of bytes '%ld' invalid.\n", argv[0], num);
		close(fd);
		exit(EXIT_FAILURE);
	}

	buf = malloc(num);
	if (!buf) {
		fprintf(stderr,"%s: out of memory!\n", argv[0]);
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (0 == opt) { // test reading..
		n = read(fd, buf, num);
		if( n < 0 ) {
			perror("read failed");
			fprintf(stderr, "Tip: see kernel log\n");
			free(buf); close(fd);
			exit(EXIT_FAILURE);
		}
		printf("%s: read %ld bytes from %s\n", argv[0], n, argv[2]);
		printf(" Data read:\n\"%.*s\"\n", (int)n, buf);
	} else { // test writing ..
		n = write(fd, buf, num);
		if( n < 0 ) {
			perror("write failed");
			fprintf(stderr, "Tip: see kernel log\n");
			free(buf); close(fd);
			exit(EXIT_FAILURE);
		}
		printf("%s: wrote %ld bytes to %s\n", argv[0], n, argv[2]);
	}
	//buf[n]='\0'; 
	/*
	Interesting! If the above line is compiled in, we get this error:
	*** glibc detected *** ./rd_tst: double free or corruption (!prev): 0x097e6008 ***
	======= Backtrace: =========
	/lib/libc.so.6(+0x6c501)[0x505501]
	/lib/libc.so.6(+0x6dd70)[0x506d70]
	...

	Actually, there is a bug if we keep that line: a buffer overrun by 1 byte (valgrind caught it!)
	*/

	free(buf);
	close(fd);
	exit(EXIT_SUCCESS);       
}
