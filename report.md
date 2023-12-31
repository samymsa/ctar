# Rapport ctar

Le rapport ci-présent donne un aperçu sur le travail qui a été mené pour l’implémentation d’un générateur/extracteur d’archive `.tar`, similaire à la commande éponyme `tar`, par **Samy Mosa** et **Younes Bokhari**.

## Sommaire

- [Choix de conception](#choix-de-conception)
- [Difficultés rencontrées](#difficultés-rencontrées)
- [Organisation de travail](#organisation-de-travail)

## Choix de conception

### ctar_header

Dans le cadre du parcours d’une archive, il a été nécessaire de lire de façon répétée les entêtes correspondant au format étudié, [Basic Tar Format](https://www.gnu.org/software/tar/manual/html_node/Standard.html), pour chaque item composant l'archive. De ce fait, une structure `ctar_header` a été définie reprenant les différentes propriétés du format :

```c
typedef struct ctar_header
{
  char name[CTAR_NAME_SIZE];
  char mode[CTAR_MODE_SIZE];
  char uid[CTAR_UID_SIZE];
  char gid[CTAR_GID_SIZE];
  char size[CTAR_SIZE_SIZE];
  char mtime[CTAR_MTIME_SIZE];
  char chksum[CTAR_CHKSUM_SIZE];
  char typeflag[CTAR_TYPEFLAG_SIZE];
  char linkname[CTAR_LINKNAME_SIZE];
  char magic[CTAR_MAGIC_SIZE];
  char version[CTAR_VERSION_SIZE];
  char uname[CTAR_UNAME_SIZE];
  char gname[CTAR_GNAME_SIZE];
  char devmajor[CTAR_DEVMAJOR_SIZE];
  char devminor[CTAR_DEVMINOR_SIZE];
  char prefix[CTAR_PREFIX_SIZE];
  char pad[CTAR_PAD_SIZE];
} ctar_header;
```

### Structure du projet

Le projet est composé de plusieurs dossiers :
- **/bin** : contient le binaire `ctar` généré par la commande `make`.
- **/docs** : contient les fichiers `doxygen` pour la documentation de code.
- **/include** : contient les entêtes des fichiers du programme.
- **/src** : contient les fichiers du programme :
  - **argparse.c** : gère le parsing des arguments.
  - **ctar.c** : regroupe les principales fonctionnalités du programme : génération d'archive, extraction, listage des éléments...
  - **ctar_zlib.c** : gère la compression et décompression d'une archive.
  - **main.c** : point d'entrée du programme.
  - **utils.c** : fonctions usuelles utilisées dans le programme.

## Difficultés rencontrées

### Hétérogénéité de niveaux

Dans notre projet, nous avons rencontré une hétérogénéité de niveaux avec d'une part Samy, ayant une bonne maîtrise du langage C, et d'autre part Younes, ayant un niveau intermédiaire. Pour pallier cette hétérogénéité et favoriser un environnement d'apprentissage mutuel, nous avons adopté une approche structurée et collaborative :
- Des sessions de **pair programming** régulières, permettant à Younes d'acquérir des compétences pratiques en observant et en travaillant directement avec Samy.
- Des **points réguliers** pour évaluer l'avancement du projet et identifier les zones de progression pour Younes.
- Des **code reviews** pour discuter des choix de programmation et renforcer la compréhension du code de chacun.

Cette stratégie a permis non seulement de maintenir la qualité et la cohérence du projet, mais aussi de progresser mutuellement d'un point de vue technique.

### Documentation zlib.h

Une autre difficulté rencontrée a été la navigation dans la documentation de la bibliothèque ***zlib***. En effet, cette documentation est concentrée dans un unique fichier volumineux, ***zlib.h***, comptant plus de 1900 lignes. Sa densité et sa structure rendent la lecture et la recherche d'informations spécifiques compliquées. Face à ce défi, nous avons initialement opté pour une approche personnalisée pour la compression et la décompression des données, en s'appuyant sur un [script open-source](https://gist.github.com/arq5x/5315739) pour la compression et la décompression d'une chaîne de caractères. Toutefois, cette décision s'est avérée plus laborieuse que prévu. En effet, la bibliothèque ***zlib*** offre des fonctions natives spécialement conçues pour ces tâches, telles que `gzopen()`, `gzwrite()`, etc., qui auraient facilité notre travail. En optant pour une solution sur mesure, nous avons rendu notre code moins simple et manqué l'opportunité d'exploiter pleinement les fonctionnalités optimisées et éprouvées de ***zlib***. Toutefois, cette expérience a permis à Younes, pour qui la tâche était confiée, de mieux appréhender le langage C et les concepts sous-jacents (typage statique, gestion de la mémoire, pointeurs, gestion des erreurs, etc).

## Organisation de travail

### Gitflow

Nous avons adopté une stratégie de versionning efficace grâce à l'approche **Gitflow** sur GitHub, distinguant le développement de nouvelles fonctionnalités (branches `features`) et la correction de bugs (branches `fix`), avec `main` comme branche principale. Cette méthode a assuré une organisation claire et une bonne traçabilité. Les **code reviews**, utilisées pour les modifications majeures, ont maintenu la qualité du code sans surcharger le processus de développement. Parallèlement, l'intégration de **CI/CD** via GitHub Actions a automatisé la compilation et les tests du code C, améliorant la qualité, facilitant le débogage et accélérant l'amélioration continue.

### Répartition des tâches

Au sein de notre binôme, la répartition des tâches a été planifiée pour exploiter au mieux nos compétences respectives et assurer une progression équilibrée du projet. Samy s'est concentré sur les aspects relatifs à la lecture, l'extraction et la création d'archives. Ces tâches, exigeant une compréhension approfondie du traitement des fichiers et de la manipulation des données, ont donc été assurées par Samy pour un total de **12 heures** de travail.

De son côté, Younes a pris en charge des responsabilités distinctes. Il a travaillé sur l'implémentation de l'option directory `-d`, permettant une gestion plus flexible des chemins de fichiers pour l'archivage et l'extraction. Également, Younes a été en charge des opérations de compression et de décompression, un élément clé du projet nécessitant une bonne compréhension de la bibliothèque ***zlib*** et de ses fonctionnalités. Enfin, il a également rédigé le rapport du projet ci-présent, documentant les processus et les résultats de notre travail. Younes a dédié un total de **11 heures** à ces tâches.

Cette répartition des tâches a permis non seulement une gestion efficace du temps et des ressources, mais aussi une bonne collaboration, permettant à chacun d'apporter ses forces au projet tout en apprenant des aspects gérés par l'autre.

### Tests

Durant le développement, nous avons opté pour une approche pragmatique en matière de tests, en les effectuant à la volée plutôt que de mettre en place des tests unitaires structurés. Cette méthode nous a permis de tester rapidement notre programme dans des conditions réelles d'utilisation, en exécutant manuellement des scénarios d'utilisation variés pour vérifier le bon fonctionnement des différentes fonctionnalités.

Bien que cette stratégie ait offert une flexibilité et une rapidité appréciables dans le processus de développement, elle a eu pour contrepartie de ne pas couvrir systématiquement tous les cas de figure potentiels, comme le ferait une suite de tests unitaires formelle. Néanmoins, cette approche a été suffisante pour répondre aux exigences et à la portée de notre projet, permettant une validation efficace des fonctionnalités clés et une adaptation rapide aux changements et aux corrections nécessaires.

## Conclusion

En conclusion, notre projet, visant à simplifier l'implémentation d'un générateur et extracteur d'archives `.tar`, nous a immergés dans la gestion de fichiers et la compression de données. Grâce à une répartition équilibrée des tâches et une collaboration structurée, nous avons surmonté des défis techniques, avec un accent sur la bibliothèque ***zlib*** et la manipulation de fichiers. Le **pair programming** et les **code reviews** ont assuré une qualité de code constante, tandis que l'intégration de **CI/CD** via GitHub Actions a augmenté notre efficacité de développement. Ce projet a non seulement atteint ses objectifs, mais a également enrichi nos compétences en programmation C, gestion de projet et travail d'équipe, offrant de bonnes leçons pour nos futurs projets.