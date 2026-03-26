#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>

#ifndef NULL
#define NULL 0
#endif

enum { STDIN  = 0, STDOUT, STDERR};

ssize_t print_error(const char* err)
{
	int write_status = write(STDERR, err, strlen(err));

	return write_status;
}

enum Error { READ_ERROR = 0, WRITE_ERROR, UNEXPECTED_WRITE_RESULT};

/* Safely writes data from fd_from to fd_to. If error code is not needed, pass NULL */
static ssize_t try_write(int fd_from, int fd_to, enum Error *err)
{
	char buf[512];

	while (true)
	{
		ssize_t read_count;
	
		read_count = read(fd_from, buf, sizeof(buf));

		if (read_count == 0)
		{
			return 0;	/* EOF */
		}

		if (read_count < 0) /* syscall interuption */
		{
			if (errno == EINTR) continue;	/* try read again */
			if (err != NULL) *err = READ_ERROR;

			return -1;
		}

		ssize_t total_written = 0;

		while (total_written < read_count)
		{
			ssize_t written = write(fd_to, buf + total_written, read_count - total_written);

			if (written < 0)
			{
				if (errno == EINTR) continue;

				if (err != NULL) *err = WRITE_ERROR;

				return -1;
			}

			if (written == 0)
			{
				if (err != NULL) *err = UNEXPECTED_WRITE_RESULT;

				return -1;
			}

			total_written += written;
		}
	}
}

int mycat(const char *path, int fd_to)
{
	/* opening the file for read */
	int fd = open(path, O_RDONLY);

	if (fd == -1)
	{
		print_error("Could not open the file '");
		print_error(path);
		print_error("'\n");

		return 2;
	}
	
	/* Reading the file */
	enum Error error_code;
	ssize_t write_status = try_write(fd, fd_to, &error_code);

	if (write_status < 0)
	{
		switch (error_code)
		{
			case READ_ERROR:
				print_error("Read error\n");
				return 3;
			case WRITE_ERROR:
				print_error("Write error\n");
				return 4;
			case UNEXPECTED_WRITE_RESULT:
				print_error("Unexpected write result\n");
				return 5;
		}
	}

	if (close(fd) < 0)
	{
		print_error("Error with closing file\n");
		return 6;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		print_error("Improper argument count for 'cat'\n");

		return 1;
	}

	return mycat(argv[1], STDOUT);
}
