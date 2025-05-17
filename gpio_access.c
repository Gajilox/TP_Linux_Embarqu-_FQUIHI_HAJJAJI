#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

//Adresse physique du registre GPIO_LED
#define LED_BASE 0xFF203000
//Taile de la plage mémoire
#define MAP_SIZE 4096UL

int main() {
	int fd;
	void *gpio_map;
	volatile unsigned int *leds;

	//Ouvrir /dev/mem
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1) {
		perror("open");
		exit(-1);
	}

	//Mapper la région mémoire à partir de LED_BASE
	gpio_map = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, LED_BASE);
	if (gpio_map == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(-1);
	}

	//On pointe leds sur le registre de sortie du GPIO
	leds = (volatile unsigned int *)gpio_map;

	printf("Adresse virtuelle mappée: %p\n", gpio_map);

	// Allumer toutes les LEDS
	*leds = 0xFF;
	printf("LEDs allumées \n");

	sleep(2);

	//Eteindre toutes les LEDs
	*leds = 0x00;
	printf("LEDs éteintes\n");

	//Nettoyage
	munmap(gpio_map, MAP_SIZE);
	close(fd);

	return 0;
}

