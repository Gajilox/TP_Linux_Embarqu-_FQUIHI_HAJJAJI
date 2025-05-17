#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LED_BASE_PATH "/sys/class/leds/fpga_led%d/brightness"
#define NUM_LEDS 10
#define DELAY_MS 200000 // 200 ms en microsecondes

void set_led(int led_num, int state) {
	char path[128];
	snprintf(path, sizeof(path), LED_BASE_PATH, led_num);

	int fd = open(path,O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Erreur lors de l'ouverture de %s\n", path);
		return;
	}

	const char *value = state ? "1" : "0";
	write(fd, value, strlen(value));
	close(fd);
}

int main() {
	while(1) {
		for (int i = 1; i < NUM_LEDS; i++) {
			set_led(i,1); // Allumer la led  i
			usleep(DELAY_MS);
			set_led(i,0); // Eteindre la LED i
		}
	}
}
