/*
 * Blinkt! LED flickering control for the IT arch. class, FUN.
 * Katsuya Matsubara and Takuya Shizukuishi
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>

int
main (int argc, char* argv[]) {

	if ((argc < 3) || (argc > 5))
		goto show_usage;

	int n_flickers, bit_shift;
	if (isdigit(argv[1][strlen(argv[1]) - 1])) {
		bit_shift = 0;
		n_flickers = atoi(argv[2]);
	} else {
		if (argc == 3)
			goto show_usage;
		bit_shift = atoi(argv[2]) - 1;
		n_flickers = atoi(argv[3]);
		if (((bit_shift < 0) && (n_flickers > 0)) || (bit_shift > 7))
			goto show_usage;
	}
	int use_flock = (argc == 5) && (strcmp(argv[4], "-x") == 0);

	int device_fd = open(argv[1], O_RDWR);
	if (device_fd < 0) {
		perror(argv[1]);
		goto show_usage;
	}

	unsigned char led_state = 0U;
	int ret;

	/* Show the current LED status as bit digits */
	if (n_flickers <= 0) {
		ret = read(device_fd, &led_state, 1);
		if (ret != 1)
			perror("read");
		else for (int i = 7; i >= 0; i--)
			putchar((led_state & (1 << i)) ? '1' : '0');
		printf(" = 0x%02x\n", led_state);
	}

	/* Flicker the specified LED iteratively */
	for (int i = 0; i < n_flickers; i++) {
		if (use_flock)
			flock(device_fd, LOCK_EX);
		ret = read(device_fd, &led_state, 1);
		if (ret != 1) {
			perror("read");
			break;
		}
		led_state ^= 1 << bit_shift;
		ret = write(device_fd, &led_state, 1);
		if (ret != 1) {
			perror("write");
			break;
		}
		if (use_flock)
			flock(device_fd, LOCK_UN);
		usleep(30000);
	}

	close(device_fd);
	return 0;
show_usage:
	printf("Usage: %s <DEVICE FILE PATH> "
		"[LED NUMBER] <TOGLLE NUM> [-x]\n", argv[0]);
	return -1;
}
