#!/bin/bash

sha1sum Texte.txt > Texte.txt.sha
md5sum Texte.txt > Texte.txt.md5

echo "Le manuel disait: Nécessite Windows 7 ou mieux. J’ai donc installé Linux" | sha1sum > Texte2.txt.sha
echo "Le manuel disait: Nécessite Windows 7 ou mieux. J’ai donc installé Linux" | md5sum > Texte2.txt.md5

echo -n "Le manuel disait: Nécessite Windows 7 ou mieux. J’ai donc installé Linux" | sha1sum > Texte3.txt.sha
echo -n "Le manuel disait: Nécessite Windows 7 ou mieux. J’ai donc installé Linux" | md5sum > Texte3.txt.md5

# Le retour à ligne fait de la merde
