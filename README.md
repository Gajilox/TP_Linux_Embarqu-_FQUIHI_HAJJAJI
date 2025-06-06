# README – TP Linux Embarqué

Ce depot contient les fichiers relatifs au TP Linux Embarque, incluant la creation de modules noyau, l'acces aux registres materiels, la gestion d'un chenillard via /proc, et la communication entre la carte SoC VEEK-MT2S et une machine virtuelle Ubuntu (VM-SOC-2019).

## Objectif du TP

Ce TP a pour objectifs :
- De comprendre comment fonctionne un module noyau sous Linux embarque.
- D'apprendre a compiler des modules noyaux en cross-compilation.
- De manipuler les interfaces /proc et /sys.
- De decouvrir l'acces direct aux registres materiels via mmap().
- De mettre en oeuvre un chenillard dans un module noyau configurable.

## Structure du depot

| Fichier        | Description |
|----------------|-------------|
| hello.c        | Module noyau simple affichant "Hello world!" lors de son chargement (insmod) et "Goodbye" lors de sa suppression (rmmod). |
| Makefile       | Fichier permettant la compilation croisee du module noyau pour l'architecture ARM a l'aide des sources du noyau Terasic. |
| chenillard.c   | Module noyau avance realisant un chenillard avec possibilite de modifier la vitesse via un parametre au chargement et d'interagir via /proc/ensea_speed. |
| gpio_access.c  | Programme utilisateur utilisant mmap() pour acceder directement aux registres GPIO de la carte SoC et controler les LEDs. |

## Comment les fichiers ont ete crees

### 1. hello.c
Fichier C standard implementant un module noyau basique. Il utilise les macros module_init(), module_exit(), ainsi que printk() pour communiquer avec le noyau.

### 2. Makefile
Cree a partir des templates fournis dans le cours. Il est configure pour pointer vers les sources du noyau Linux embarque sur la VM :

```makefile
obj-m := hello.o

all:
    make -C $(KERNEL_SOURCE) M=$(PWD) modules

clean:
    make -C $(KERNEL_SOURCE) M=$(PWD) clean
```

### 3. chenillard.c
Module noyau etendu qui gere un chenillard logiciel grace a un timer noyau (timer_setup, mod_timer). Il accepte un parametre speed= au chargement et expose une interface /proc/ensea_speed pour lire ou ecrire cette valeur dynamiquement.

### 4. gpio_access.c
Programme utilisateur ecrit en C utilisant mmap() pour acceder aux registres physiques de la carte VEEK-MT2S a l'adresse 0xFF203000. Ce programme permet de manipuler les GPIOs depuis l'espace utilisateur.

## Interactions entre la VM et la carte SoC

### Connexion SSH a la carte SoC :
```bash
ssh root@192.168.0.250
```

### Transfert des fichiers compiles :
```bash
scp hello.ko root@192.168.0.250:/root/
```

### Tests sur la carte :
```bash
insmod hello.ko
dmesg | tail
rmmod hello
```

La carte VEEK-MT2S est alimentee par une image Linux embarquee. Elle possede un systeme de fichiers complet permettant de charger des modules noyau, d'utiliser /proc, et d'acceder aux peripheriques via /sys.

## Compilation croisee sur la VM

### Prérequis :
Installation des outils necessaires sur la VM :
```bash
sudo apt update
sudo apt install bc crossbuild-essential-armhf binutils-multiarch
```

### Recuperation de la configuration noyau depuis la carte :
```bash
scp root@192.168.0.250:/proc/config.gz .
gunzip config.gz
mv config .config
```

### Configuration de l'environnement de cross-compilation :
```bash
export CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf-
export ARCH=arm
make prepare
make scripts
```

### Compilation croisee des modules :
Depuis le dossier contenant les sources du module (ex: hello.c) :
```bash
make
```

Le resultat est un fichier .ko pret a etre charge sur la carte SoC.

## Test des modules noyau

Voici les commandes principales pour tester vos modules noyau sur la carte VEEK-MT2S :

### Chargement du module :
```bash
insmod hello.ko
```

### Verification des messages noyau :
```bash
dmesg | tail
```

### Affichage des modules charges :
```bash
lsmod
```

### Suppression du module :
```bash
rmmod hello
```

### Verification apres suppression :
```bash
dmesg | tail
```

Ces commandes vous permettent de valider que votre module noyau fonctionne correctement et interagit comme prevu avec le noyau.
