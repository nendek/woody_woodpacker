# woody_woodpacker

# fonctionnement global

Le packing se fait en injectant 2 shellcodes:
- loader, son role est d'appeler le 2eme shellcode 
- woody, son role est de print woody, puis de dechiffrer la section .text puis de l'executer

### Loader
ce shellcode est injecté directement dans la section .text. pour l'injecter, le programme va cherche un segment PT_LOAD qui contient assez de padding pour injecter le shellcode loader. Ensuite, l'entry point est redirigée vers le debut de Loader.

Le fonctionnement du shellcode est le suivant:
Il va passer la permission de la zone où est stocké le shellcode woody en PROT_EXEC, puis va jump dessus
Lorsque le shellcode woody a terminé son boulot, il va rejump sur la 2eme moitiée de ce shellcode qui va consister a remettre la permission de la zone en PROT_WRITE, puis il va jump a l'entry point d'origine du programme.


### woody
ce shellcode est injecté a la fin du programme, il peut potentiellement chevaucher les sections header.

Le fonctionnement du shellcode est le suivant:
Il va ecrire "...WOODY...\n" sur l'entrée standard, 
puis va jump sur la deuxieme moitée du shellcode loader


# Details techniques

Le programme dispose de 4 méthodes d'injection differentes, classées de la plus "discrète" à la moins discrète.

### Méthode 1

* Loader est placé dans le padding du segment contenant la section .text
* Woody est placé dans le padding d'un autre segment PT_LOAD
* Prerequis : 
  * Il faut assez de place dans le padding du segment text pour acceuillir Loader.
  * Il faut assez de place dans le padding d'un autre segment pour acceuillir Woody.
* Conséquences : 
  * L'entry point est modifié
  * La taille du segment .text est modifiée
  * La taille d'un autre segment PT_LOAD est modifiée

### Méthode 2

* Loader est placé dans le padding du segment contenant la section .text
* Woody est ajouté a la fin de la section .bss
* Prerequis :
  * Il faut assez de place dans le padding du segment text pour acceuillir Loader.
* Conséquences : 
  * L'entry point est modifié
  * La taille du segment .text est modifiée
  * La taille du segment contenant la section .bss est modifiée
  * La section .bss est physiquement présente dans le fichier
  * Les sections headers peuvent potentiellement etre ecrasés

### Méthode 3

* Loader est placé dans le padding d'un segment PT_LOAD autre que le segment contenant la section .text
* Woody est placé directement a la suite de Loader
* Prerequis :
  * Il faut assez de place dans le padding d'un segment PT_LOAD pour acceuillir Loader et Woody
* Conséquences :
  * L'entry point est modifié
  * La taille d'un segment PT_LOAD est modifiée
  * Le segment PT_LOAD modifié voit ses droits modifiés pour y ajouter le droit d'execution
  
### Méthode 4

* Loader est ajouté a la fin de la section .bss
* Woody est placé directement a la suite de Loader
* Prérequis : Aucun
* Conséquences :
  * L'entry point est modifié
  * La taille du segment contenant la section .bss est modifiée
  * La section .bss est physiquement présente dans le fichier
  * Les sections headers peuvent potentiellement etre ecrasés
  * Le segment PT_LOAD modifié voit ses droits modifiés pour y ajouter le droit d'execution

# Infinite packing

```
root@kali:~/woody/clean# sh loop.sh /bin/ls 50
rm -rf objs
rm -f woody_woodpacker
mkdir -p objs
gcc -Wall -Wextra -Werror -I includes -c -o objs/woody.o srcs/woody.c
gcc -Wall -Wextra -Werror -I includes -c -o objs/creation.o srcs/creation.c
gcc -Wall -Wextra -Werror -I includes -c -o objs/crypto.o srcs/crypto.c
gcc -Wall -Wextra -Werror -I includes -c -o objs/libft_handler.o srcs/libft_handler.c
gcc -Wall -Wextra -Werror -I includes -c -o objs/get_case64.o srcs/get_case64.c
gcc -Wall -Wextra -Werror -I includes -c -o objs/handle_64.o srcs/handle_64.c
gcc -Wall -Wextra -Werror -I includes -c -o objs/get_case32.o srcs/get_case32.c
gcc -Wall -Wextra -Werror -I includes -c -o objs/handle_32.o srcs/handle_32.c
gcc -Wall -Wextra -Werror -I includes -o woody_woodpacker objs/woody.o objs/creation.o objs/crypto.o objs/libft_handler.o objs/get_case64.o objs/handle_64.o objs/get_case32.o objs/handle_32.o
Encryption Key : 0x7ccf722c
Encryption Key : 0x59913488
1
Encryption Key : 0x3454ff1f
2
Encryption Key : 0x3a5dc83
3
...
...
Encryption Key : 0x3d274a5e
50
root@kali:~/woody/clean# ./woody
....WOODY....
....WOODY....
....WOODY....
....WOODY....
...
...
....WOODY....
....WOODY....
....WOODY....
bytecode  includes  loop.sh  Makefile  objs  README.md	srcs  woody  woody_woodpacker
```
