PongPangPing
============

Projet Curiositas 2014



Source et projet Visual Studio

MATERIEL NECESSAIRE
===================

* 2 kinect
* 1 Ordinateur avec Carte Graphique descente (testé avec carte NVIDIA)
* 1 projecteur
* 1 table pour projet le terrain (terrain pas forçément plat)
* 2 raquettes de Ping Pong


INSTALLATION
============

Sous Windows:

Installer OpenFrameworks et les addons OfxBullet, ofxOpenNI2 [a faire liens]
Générer un projet avec ces libs + ofxOpenCv et ofxOpenKinect

Installer les drivers nécessaires: OpenCL (Nvidia = cuda), OpenNi, NiTe, (autres? je sais plus)

Dans Visual Studio, ajouter les includes et libs des drivers, ne pas oubleir de mettre les libs en dependances du projet, et les dll dans le dossier de l'executable.

Enfin il faut dire aux kinects quel driver utilises specifiquement: dans le gestionnaire de peripherique, clique droit sur les composants de la kinect, demander a choisir un driver et donner le chemin: (a reprendre du projet windows)
