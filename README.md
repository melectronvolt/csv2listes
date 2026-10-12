# csv2listes 📲

## Description 📑
- Logiciel pédagogique pour Windows 64 bits qui permet d'extraire des listes Python à partir de fichiers CSV.
- En tant que professeur de sciences physiques, j'ai constaté que les élèves consacraient une grande partie des travaux pratiques de mécanique à essayer de récupérer les données de leurs smartphones ou des autres logiciels ; finissant souvent par les extraire une à une pour obtenir une liste Python. Ce processus est très chronophage et n'apporte pas de valeur pédagogique.
- Le code C++ est un exemple d'utilisation de l'API WIN32 en C++. Il est bien commenté et accompagné de tests.

## Utilisation 🦾
1. Dans l'étape 1, cochez la case permettant de définir le nom des listes en Python à partir des entêtes.
2. Toujours dans l'étape 1, indiquez le nombre de colonnes du fichier CSV que vous souhaitez utiliser.
3. Dans l'étape 2, indiquez le délimiteur utilisé pour générer le fichier CSV (cela dépend du logiciel utilisé et de votre choix).
4. Cliquez sur le bouton "Ouvrir le fichier CSV" et répondez aux questions posées par le logiciel.
5. Les listes sont générées et copiées dans le presse-papiers. Vous pouvez ensuite les coller avec Ctrl + V.
6. Pour obtenir un journal d'activité de l'application, ajoutez l'argument `log` lors du lancement du programme. Un fichier `csv2listes_log.txt` sera créé dans votre dossier `Mes documents`.

## Compilation ⚙️
- Utilisation de CMake.
- Compilation avec MSVC V17.7.0 en date du 21/08/2023.
- J'utilise les packages catch2 🕸️ et spdlog 📄. Vous devrez indiquer dans les `CMakeLists.txt` où CMake doit les localiser.
- Modifier les variables dans `src_functions.h` à votre guise :

```cpp
#define MAX_SIZE_LINE 1024
#define MIN_LINE_COUNT 2
#define MAX_LINE_COUNT 67000
#define MAX_SIZE_FILE 1000000
```

## Version Linux 🐧
- Un portage en Rust pour Linux se trouve dans le dossier `rust/` : voir [rust/README.md](rust/README.md).
- Lancement : `cd rust && cargo run --release`

## Licence 🔓
[MIT](https://fr.wikipedia.org/wiki/Licence_MIT) : Cette licence autorise quiconque reçoit le logiciel (et ses fichiers associés) à l'utiliser, le copier, le modifier, le fusionner, le publier, le distribuer, le vendre et à le « sous-licencier » (l'intégrer dans une autre licence). La seule obligation est d'inclure la notice de licence et de copyright dans toutes les copies.


## À faire 🔨
- Corriger les bugs 🪲
- Corriger les fautes d'orthographe ✒️
- Améliorer la cohérence des noms de variable, français / anglais actuellement
- Réécrire les commentaires décrivant les fonctions

## Remerciements 🙏🏻
- [spdlog 📄](https://github.com/gabime/spdlog)
- [catch2 🕸️](https://github.com/catchorg/Catch2)
