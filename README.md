
# README – TP Linux Embarqué

Ce dépôt contient les fichiers relatifs au TP Linux Embarqué, incluant la création de modules noyau, l'accès aux registres matériels, la gestion d'un chenillard via `/proc`, et la communication entre la carte SoC VEEK-MT2S et une machine virtuelle Ubuntu (VM-SOC-2019).

## Objectif du TP

Ce TP a pour objectifs :
- De comprendre comment fonctionne un module noyau sous Linux embarqué.
- D'apprendre à compiler des modules noyaux en cross-compilation.
- De manipuler les interfaces `/proc` et `/sys`.
- De découvrir l'accès direct aux registres matériels via `mmap()`.
- De mettre en œuvre un chenillard dans un module noyau configurable.

## Structure du dépôt

| Fichier        | Description |
|----------------|-------------|
| hello.c        | Module noyau simple affichant "Hello world!" lors de son chargement (`insmod`) et "Goodbye" lors de sa suppression (`rmmod`). |
| Makefile       | Fichier permettant la compilation croisée du module noyau pour l'architecture ARM à l'aide des sources du noyau Terasic. |
| chenillard.c   | Module noyau avancé réalisant un chenillard avec possibilité de modifier la vitesse via un paramètre au chargement et d'interagir via `/proc/ensea_speed`. |
| gpio_access.c  | Programme utilisateur utilisant `mmap()` pour accéder directement aux registres GPIO de la carte SoC et contrôler les LEDs. |

---

## Comment les fichiers ont été créés

### 1. hello.c
Fichier C standard implémentant un module noyau basique. Il utilise les macros `module_init()`, `module_exit()`, ainsi que `printk()` pour communiquer avec le noyau.

### 2. Makefile
Créé à partir des templates fournis dans le cours. Il est configuré pour pointer vers les sources du noyau Linux embarqué sur la VM :

```makefile
obj-m := hello.o

all:
    make -C $(KERNEL_SOURCE) M=$(PWD) modules

clean:
    make -C $(KERNEL_SOURCE) M=$(PWD) clean
```

### 3. chenillard.c
Module noyau étendu qui gère un chenillard logiciel grâce à un timer noyau (`timer_setup`, `mod_timer`). Il accepte un paramètre `speed=` au chargement et expose une interface `/proc/ensea_speed` pour lire ou écrire cette valeur dynamiquement.

### 4. gpio_access.c
Programme utilisateur écrit en C utilisant `mmap()` pour accéder aux registres physiques de la carte VEEK-MT2S à l'adresse `0xFF203000`. Ce programme permet de manipuler les GPIOs depuis l'espace utilisateur.

---

## Interactions entre la VM et la carte SoC

### Connexion SSH à la carte SoC :
```bash
ssh root@192.168.0.250
```

### Transfert des fichiers compilés :
```bash
scp hello.ko root@192.168.0.250:/root/
```

### Tests sur la carte :
```bash
insmod hello.ko
dmesg | tail
rmmod hello
```

La carte VEEK-MT2S est alimentée par une image Linux embarquée. Elle possède un système de fichiers complet permettant de charger des modules noyau, d'utiliser `/proc`, et d'accéder aux périphériques via `/sys`.

---

## Compilation croisée sur la VM

### Récupération de la configuration noyau depuis la carte :
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

### Compilation croisée des modules :
Depuis le dossier contenant les sources du module (ex: `hello.c`) :
```bash
make
```

Le résultat est un fichier `.ko` prêt à être chargé sur la carte SoC.

---

## Test des modules noyau

Voici les commandes principales pour tester vos modules noyau sur la carte VEEK-MT2S :

### Chargement du module :
```bash
insmod hello.ko
```

### Vérification des messages noyau :
```bash
dmesg | tail
```

### Affichage des modules chargés :
```bash
lsmod
```

### Suppression du module :
```bash
rmmod hello
```

### Vérification après suppression :
```bash
dmesg | tail
```

Ces commandes vous permettent de valider que votre module noyau fonctionne correctement et interagit comme prévu avec le noyau.

---

## Prise en main de la carte VEEK-MT2S

### Identifier la carte SD sous Linux
Avant et après insertion de la carte SD :
```bash
ls /dev
```
Exemple de sortie après insertion :
```
sdb sdb1 sdb2
```

### Flasher l’image sur la carte SD
```bash
cd <chemin_vers_image>
sudo dd if=VEEK_MT2S_LXDE.img of=/dev/sdX bs=4k status=progress
sync
```

### Configurer le réseau (DHCP)
Édition du fichier :
```bash
vim /etc/network/interfaces
```
Contenu :
```
auto eth0
iface eth0 inet dhcp
allow-hotplug eth0
```
Puis :
```bash
reboot
```

### Vérifier l'adresse IP
```bash
ip addr show
```
ou
```bash
ifconfig
```

### Autoriser la connexion SSH sans mot de passe
Ajouter dans `/etc/ssh/sshd_config` :
```
PermitEmptyPasswords yes
```

---

## Accès au matériel

### Contrôler les LEDs via sysfs
Allumer une LED :
```bash
echo 1 > /sys/class/leds/fpga_led1/brightness
```
Éteindre une LED :
```bash
echo 0 > /sys/class/leds/fpga_led1/brightness
```

### Chenillard simple en espace utilisateur
Voir code exemple dans `chenillard.c` ci-dessus.

---

## Modules noyau

### Créer un module basique
Voir exemple `hello.c` ci-dessus.

### Charger/décharger un module
```bash
insmod hello.ko
rmmod hello
```

### Vérifier les logs noyau
```bash
dmesg | tail
```
![image](https://github.com/user-attachments/assets/63128b90-d598-4dc0-9553-34e77dae38df)
---

## Cross-compilation de modules noyau

### Préparer la compilation croisée
```bash
sudo apt install bc crossbuild-essential-armhf binutils-multiarch
export CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf-
export ARCH=arm
make prepare
make scripts
```

### Compiler un module noyau
Depuis le répertoire du module :
```bash
make
```

---

