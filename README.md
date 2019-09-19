# woody_woodpacker

# fonctionnement de woody_woodpacker

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
