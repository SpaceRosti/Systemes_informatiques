#!/bin/bash

clean(){ # nettoie le nom : enlève les espaces, guillemets, apostrophes
  mv "$1" `echo $1 |tr ' ' '_' | tr '"' '_' | tr "'" '_'`
}

makeCopy(){ # copier un fichier (on veut image) dans un autre répertoire
  if [ `file -ib "$1" | cut -d '/' -f1` = "image" ];then # vérifie que c'est une image
    cp -r "$1" $2/  # copie le contenu du repertoire
  fi
}

convertir(){ # converti l'image en png et la redimensionne si demandé
  NAME=`echo "$1" | cut -d'.' -f1` # on prend le nom du fichier, sans l'extension
  if [ ! -z $2 ]; then
    convert -resize $2 $1 $NAME.png
  else
    convert $1 $NAME.png #converti l'image au format png
  fi
}

if [ -z $1 -o -z $2 ];then  # on vérifie si $1 ou $2 sont vides
  echo "Veuillez renseigner tout les arguments"
else
  if [ -d $1 ] ; then # on vérifie que le répertoire de lecture existe
    if [ -d $2 ];then # si le répertoire d'écriture existe
      for fichier in $1/*;do
        makeCopy "$fichier" $2
      done
    else # si le répertoire d'écriture n'existe pas
      mkdir $2  # on crée le répertoire
      for fichier in $1/*;do
        makeCopy "$fichier" $2 # on copie le fichier dans le répertoire $2
      done
    fi

    for fichier in $2/* ; do
      clean "$fichier" # on nettoie le nom
    done

    for fichier in $2/*; do
      if [ ! `file -ib "$fichier" | cut -d ';' -f1` = "image/png" ]; then
        convertir $fichier $3 # on convertit les formats d'images
        rm $fichier
      fi
    done

  else
    echo " '$1' is not a valid directory"
  fi
fi
