/**
 * UQAM - Hiver 2017 - INF3105 - Groupe 10 - TP3
 *
 * inf3105 - Fichier source c++ d'une programme visant a creer un "resume" d'une
 *           histoire contenue dans un fichier xml. 
 *
 *           Le programme utilise un algorithme de partionnement sur un graphe 
 *           des "distances" (mesure inversement proportionnelle a la similarite) 
 *           de chaque paire de phrases du texte donnee pour partionner/regrouper
 *           les phrases qui sont similaires. Apres le partionnement, une phrase
 *           representante est selectionnee pour representer chaque partition. 
 *           La phrase representante choisie est la phrase ayant la moyenne de 
 *           distances avec les autres phrases de la partition la plus basse.
 *           Le programme affiche une phrase representante par ligne
 *           a la sortie standard et se termine.
 *
 *           Utilisation :
 *
 *           ./NOM_EXEC <-MODE> <-n NB_PHRASES> "TITRE_HISTOIRE"
 *
 *           * MODE est la methode utilisee pour calculer les nouvelles distances
 *             apres une fusion de noeuds lors du partionnement. Les options sont :
 *                -s : "Simple Linkage Clustering", mode par defaut.
 *                -c : "Complete Linkage Clustering".
 *                -a : "Average Linkage Clustering". 
 *           * NB_PHRASES est un entier strictement positif suivant -n. Permet de
 *             specifier le nombre de phrases a afficher. Par defaut, le programme
 *             affiche 5 phrases.
 *           * TITRE_HISTOIRE est le titre de l'histoire pour lequel on desir creer
 *             une resume. Doit etre une histoire incluse dans les fichiers xml 
 *             decrits dans "listeDocument.xml" et le fichier doit etre inclus dans
 *             le repertoire courant. 
 *
 *  @Auteurs   Alexis Chretien (CHRA25049209)
 *             Bruno Malenfant
 *  @Version   13 avril 2017 
 */

#include "DocumentXML.h"
#include "Histoire.h"
#include "Phrase.h"

#include <cmath>
#include <algorithm>
#include <utility>
#include <math.h>
#include <string.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

/**
 * Fonction qui appelle la fonction "lireFichierXML" et qui renvoie un pointeur
 * vers un vecteur de pointeur d'histoires permettant d'iterer sur les phrases
 * des differentes histoires. 
 *
 * @param  a_nomFichier   Le nom du fichier XML a lire.
 * @return                Un pointeur sur un vector<Histoire*>
 *
 * @Auteur Bruno Malenfant 
 */
vector< Histoire *> * lireDocuments( string a_nomFichier ) {
    vector<Histoire *> * histoires = new vector< Histoire * >();
    DocumentXML * listeFichiers = lireFichierXML( a_nomFichier );

    Element * courrant = listeFichiers->racine();
    vector< Contenu * >::const_iterator it = courrant->begin();

    // trouver <liste>
    while( ! ( * it )->estElement() ) ++ it;
    courrant = ( Element * )( * it );

    for( Contenu * contenu : * courrant ) {
        if( contenu->estElement() ) {
            Element * element = ( Element * )contenu;

            DocumentXML * doc = lireFichierXML( element->attribut( string( "fichier" ) ) );

            vector<Histoire *> * h = extraireHistoires( * doc );

            histoires->insert( histoires->end(), h->begin(), h->end() );
        }
    }
    return histoires;
}

/**
 *  Fonction affichant un message d'erreur en fonction du numero d'erreur.
 *
 *  @param  erreur    Le numero d'erreur.
 *  @return
 */
void afficherMessageErreur (int const &erreur) {

    if (erreur == 1) {
        cout << "Erreur, specification de plusieurs modes." << endl;
    }
    else if (erreur == 2) {
        cout << "Erreur, specification de plusieurs nombres de lignes a afficher." << endl;
    }
    else if (erreur == 3) {
        cout << "Erreur, nombre de lignes non-specifie ou invalide." << endl;
    }
    else if (erreur == 4) {
        cout << "Erreur, semaphore invalide." << endl;
    }
    else if (erreur == 5) {
        cout << "Erreur, specification de plusieurs titres." << endl;
    }
    else if (erreur == 6) {
        cout << "Erreur, il n'existe aucune histoire satisfaisant le titre en argument." << endl;
    }
}

/**
 * Fonction validant un entier sous forme de chaine de caracteres et retournant
 * cet entier sous la forme d'un "int". 
 *
 * @param   chaine       Une chaine de caracteres.
 * @return               Si la chaine de caracteres represente
 *                       un entier, retourne la valeur "int" de cet
 *                       entier. Sinon, retourne -1.
 */
int retournerEntier(char* const &chaine) {

    int entierRetour;
    bool entierValide;
    
    entierRetour = 0;
    entierValide = true;

    for (int i = 0 ; i < strlen(chaine) && entierValide == true ; ++i) {

        if (chaine[i] < '0' || chaine[i] > '9') {
           entierValide = false; 
        } 
    }

    if(entierValide == true) {
        entierRetour = atoi(chaine);
    }
    else {
        entierRetour = -1;
    }
    return entierRetour;
}

/**
 * Fonction transformant la chaine de caracteres referencee en argument
 * par la meme chaine, en minuscules. 
 *
 * @param  chaine     Le chaine a mettre en minuscules.
 * @return            
 */
void enMinuscules(string &chaine) {

    for (int i = 0 ; i < chaine.length() ; ++i) {
        chaine[i] = tolower(chaine[i]);
    }
}

/**
 * Fonction qui valide les arguments du programme. 
 *
 * @param  nbArguments      Le nombre d'arguments
 * @param  arguments        Les arguments
 * @return                  Si les arguments sont valides, retourne 0.
 *                          Si un ou plusieurs argument(s) est/sont invalide(s),
 *                          retourne le numero d'erreur associe.                           
 */
int validerArguments(int const &nbArguments, char** const &arguments, 
                     int &nbLignes, char &mode, string &titreHistoire) {

    int erreur; 
    bool argumentsValides; 

    mode = '\0';
    titreHistoire = "\0";
    erreur = 0;
    nbLignes = 0;
    argumentsValides = true;

    // parcourir les arguments
    for (int i = 1 ; i < nbArguments && argumentsValides == true ; ++i) {

        // si argument commence par '-'   
        if (arguments[i][0] == '-') {   
            // si le semaphore est un sephamore de mode de calcul de distance         
            if ((strcmp(arguments[i], "-c") == 0) 
                    || (strcmp(arguments[i], "-s") == 0)
                    || (strcmp(arguments[i], "-a") == 0)) {
                // si le sempahore de mode de calcul de distance a deja
                // ete specifie.
                if (mode != '\0') {
                    erreur = 1;
                    argumentsValides = false;
                }
                else {
                    mode = arguments[i][1];
                }
            }
            // si le semaphore est l'argument de specification du nombre de lignes 
            // de resume a afficher.
            else if (strcmp(arguments[i], "-n") == 0) {

                if (nbLignes != 0) {
                    erreur = 2;
                   argumentsValides = false; 
                }
                // l'arguments "-n" doit etre au plus l'avant dernier argument, et 
                // l'argument suivant doit etre un entier valide. 
                else if ((i > nbArguments - 2)
                        || (nbLignes = retournerEntier(arguments[i+1])) < 1) {
                    erreur = 3;
                    argumentsValides = false;
                }
                ++i;
            }
           // si le semaphore n'est pas un semaphore valide
            else {
                erreur = 4;
                argumentsValides = false;
            }
        }      
        // si un titre a deja ete specifie
        else if (titreHistoire != "\0") {
            erreur = 5;
            argumentsValides = false;
        }
        else {
            titreHistoire = arguments[i];
            enMinuscules(titreHistoire);
        }
    }    
    // mode par defaut
    if (mode == '\0') {
        mode = 's';
    }
    // nbLignes par defaut
    if (nbLignes == 0) {
        nbLignes = 5;
    }
    return erreur;
}

/**
 * Fonction qui verifie si une histoire peut etre consideree comme une partie
 * d'une autre histoire. 
 *
 * La fonction verifie si la chaine "CHAINE_TITRE_1 (partie " est un prefixe de
 * titre2. La fonction considere que si un titre contient la chaine " (partie ", 
 * cette chaine sera necessairement suivie de la chaine "_NO_)", ou _NO_ est un 
 * entier, et que cette chaine sera la fin du titre. 
 *
 * @param   titre1          Le titre d'une histoire.
 * @param   titre2          Un autre titre d'histoire.
 * @return                  Vrai si titre1 est le titre general (excluant la
 *                          chaine " (partie _NO_)") de titre2. Faux sinon. 
 */
bool estMemeHistoire(string titre1, string const &titre2) {

    bool memeHistoire = false;    
    titre1 += " (partie "; 
    
    if ((titre1.length() < titre2.length())
            && (titre2.substr(0, titre1.length()) == titre1)) {
        memeHistoire = true;
    }
    return memeHistoire;
}

/**
 * Fonction retournant un vecteur de phrases, contenant toutes les phrases de
 * l'histoire en argument. 
 *
 * @param  histoire          Un vecteur d'histoire, contenant toutes les histoires
 *                           de tous les fichiers XML contenu dans 
 *                           "listeDocument.xml".
 * @param  titreHistoire     Le titre de l'histoire de laquelle on souhaite 
 *                           extraire les phrases. 
 * @return                   Un vecteur de phrases. 
 */
vector<Phrase> retournerPhrases(vector<Histoire*>  const &histoires, 
                                string const &titreHistoire) {

    string unTitre;
    vector<Phrase> phrases;


    for (Histoire* histoire : histoires) {
        
        unTitre = histoire->titre();
        enMinuscules(unTitre);

        if (unTitre == titreHistoire
                || estMemeHistoire(titreHistoire, unTitre)) {
 
            vector<Phrase>::const_iterator iter    = histoire->begin();
            vector<Phrase>::const_iterator iterFin = histoire->end();
            for ( ; iter != iterFin ; ++iter) {
                phrases.push_back(*iter);    
            }
            // l'iteration sur les phrases des histoires retourne toujours une 
            // derniere phrase eronnee, constituee d'un caractere espace et d'un
            // point. Doit enlever cette phrase.          
            phrases.pop_back();
            
        }
    }
    return phrases;
}

/**
 * Fonction qui instancie des vecteurs de pair de phrases et d'entier et retournant
 * un pointeur sur ceux-ci. 
 *
 * Chaque vecteur instancie contient une seule pair : une phrase de l'histoire a
 * traiter et son index (commence a 0). Ces vecteurs joueront le role de noeuds
 * lors du partionnement. Conserver l'index de leur position initiale 
 * est necessaire afin d'eviter de devoir refaire des calcul de distances 
 * couteuses en terme de performences apres avoir effectue le partionnement. 
 *
 * @param  phrases   Un vecteur de phrases contenant toutes les phrases de 
 *                   l'histoire a traiter
 * @return           Un pointeur pointant sur des vecteurs de pair de phrase 
 *                   et d'entier contenant chacun une seule pair <Phrase, int>.
 */
vector< pair<Phrase, int> >* retournerNoeuds(vector<Phrase> const &phrases) {
    
    vector< pair<Phrase, int> >* noeuds;
    vector<Phrase>::const_iterator iter    = phrases.begin();
    vector<Phrase>::const_iterator iterFin = phrases.end();

    noeuds = new vector< pair<Phrase, int> >[phrases.size()];

    for (int i = 0 ; iter != iterFin ; ++iter, ++i) {
        noeuds[i].push_back(make_pair(*iter, i)); 
    }
    return noeuds;
}

/**
 * Fonction qui recherche la plus petite valeur contenu dans un graphe de distances. 
 *
 * Place les coordonnees du graphe contenant la plus petite valeur de distance
 * dans les variables "coord_i" et "coord_j" referencees en arguments. 
 *
 * @param   graphe    Le graphe des distances sous forme de table 2D de doubles.
 * @param   nbNoeuds  Le nombre de noeuds du graphe
 * @param   coord_i   La variable ou placer la coordonnee en i de la plus petite 
 *                    valeur de distance.
 * @param   coord_j   La variable ou placer la doordonnee en j de la plus petite
 *                    valeur de distance.
 * @return
 */
void trouverPlusPetiteDistance(double** const &graphe, int const &nbNoeuds, 
                               int &coord_i, int &coord_j) {

    double minimum = INFINITY;

    for (int i = 0 ; i < nbNoeuds ; ++i) {
        for (int j = i + 1 ; j < nbNoeuds ; ++j) {
            if (graphe[i][j] < minimum) {
                coord_i = i;
                coord_j = j;
                minimum = graphe[i][j];
            }
        }
    }
}

/**
 * Fonction qui calcule une nouvelle valeur de distance entre un noeud k
 * et un noeud nouvellement cree lors de la fusion des noeuds i et j.
 *
 * @param  distance_ki   La distance entre le noeud k et l'ancien noeud i.
 * @param  distance_kj   La distance entre le noeud k et l'ancien noeud j.
 * @param  nbPhrases_i   Le nombre de phrases que contenait le noeud i.
 * @param  nbPhrases_j   Le nombre de phrases que contenait le noeud j.
 * @param  mode          Le mode de calcul de la nouvelle distance. 
 * @return               La distance entre k et le noeud i-j.
 */
double nouvelleDistance(double const &distance_ki, 
                        double const &distance_kj, 
                        int    const &nbPhrases_i,
                        int    const &nbPhrases_j,
                        char   const &mode) {

    double distance = 0;

    // complete-linkage clustering
    if (mode == 'c') {
        distance = max(distance_ki, distance_kj);    
    }
    // simple-linkage clustering
    else if (mode == 's') {
        distance = min(distance_ki, distance_kj);
    }
    // average-linkage clustering
    // le mode a deja ete valide en debut de programme, si mode != 'c' et != 's', 
    // sera necessaire 'a'.
    else {
        distance = (distance_ki * nbPhrases_i 
                  + distance_kj * nbPhrases_j) / (double) (nbPhrases_i + nbPhrases_j);    
    }

    return distance;
}

/**
 * Fonction calculant les distances entre les phrases d'une histoire, place
 * les valeurs dans un graphe et effectue un algorithme de partionnement pour
 * regrouper les phrases semblables de l'histoire dans les meme noeuds. 
 *
 * Une fois le partionnement termine, la fonction selectionne une phrase 
 * representante pour chaque partition. La phrase representante est la phrase
 * ayant la plus petite moyenne de distances avec les autres phrases de la partition.
 *
 * La fonction renvoie un pointeur de Phrase, pointant sur les phrases representantes. 
 *
 * Noter que puisqu'un graphe de distances est necessairement symetrique, l'algorithme
 * peut etre effectue sur seulement une des deux moitiers du tableau 2D. 
 *
 * @param  noeuds              Les noeuds du graphe, contenant initialement une 
 *                             seule pair<Phrase, int>.
 * @param  nbNoeuds            Le nombre de noeuds du graphe.
 * @param  nbPhrasesAfficher   Le nombre de phrases que doit contenu le resume, soit
 *                             le nombre final de partitions. 
 * @param  mode                Le mode de calcul de distances suivant une fusion de noeuds. 
 * @return                     Un pointeur pointant sur les phrases representantes. 
 */
Phrase* retournerRepresentantes(vector< pair<Phrase, int> >* &noeuds,  
                                int  &nbNoeuds,
                                int  const &nbPhrasesAfficher,
                                char const &mode) {

    int nbPhrases_i;
    int nbPhrases_j;
    int index_i;
    int index_j;
    int index_k;
    int nbPhrasesPartition;
    double distance;
    double somme;
    double moyenne;
    double moyenneMin;
    double** graphe;
    double** grapheInit;
    Phrase* phrasesRep;

    moyenne = 0;
    somme   = 0;   

    graphe     = new double*[nbNoeuds];
    grapheInit = new double*[nbNoeuds];

    // Initialisation du graphe et de sa copie
    for (int i = 0 ; i < nbNoeuds ; ++i) {
        graphe[i]     = new double[nbNoeuds];
        grapheInit[i] = new double[nbNoeuds];
    }
    for (int i = 0 ; i < nbNoeuds ; ++i) {

        for (int j = i + 1; j < nbNoeuds ; ++j) {

            distance = (noeuds[i].at(0).first).distance(noeuds[j].at(0).first);
         
            graphe[i][j]     = distance;
            grapheInit[i][j] = distance;
        }
    }
    // partitionner tant qu'on n'a pas un certain nombre de partitions
    while (nbNoeuds > nbPhrasesAfficher) {

        // trouver l'index des noeuds a fusionner.
        trouverPlusPetiteDistance(graphe, nbNoeuds, index_i, index_j);

        nbPhrases_i = noeuds[index_i].size();
        nbPhrases_j = noeuds[index_j].size();

        // fusion des noeuds i et j. Le nouveau noeud est place en position du noeud i.
        // Le dernier noeud est avance en position j. 
        noeuds[index_i].insert(noeuds[index_i].end(), 
                               noeuds[index_j].begin(),
                               noeuds[index_j].end());
        noeuds[index_j] = noeuds[nbNoeuds - 1];
        --nbNoeuds; 

        // mise a jour des distances 
        for (int i = 0 ; i < index_i ; ++i) {
            graphe[i][index_i] = nouvelleDistance(graphe[i][index_i], graphe[i][index_j],
                                                  nbPhrases_i, nbPhrases_j, mode);
            graphe[i][index_j] = graphe[i][nbNoeuds];
        }
        for (int j = index_i + 1 ; j < index_j ; ++j) {
            graphe[index_i][j] = nouvelleDistance(graphe[index_i][j], graphe[j][index_j],
                                                  nbPhrases_i, nbPhrases_j, mode);
            graphe[j][index_j] = graphe[j][nbNoeuds];
        } 
        for (int j = index_j + 1 ; j < nbNoeuds; ++j) {
            graphe[index_i][j] = nouvelleDistance(graphe[index_i][j], graphe[index_j][j], 
                                                  nbPhrases_i, nbPhrases_j, mode);
            graphe[index_j][j] = graphe[j][nbNoeuds];
        }

        graphe[index_i][index_j] = nouvelleDistance(graphe[index_i][nbNoeuds], 
                                                    graphe[index_j][nbNoeuds],
                                                    nbPhrases_i, nbPhrases_j, mode);
    }   
    phrasesRep = new Phrase[nbNoeuds];
    
    // pour toutes les partitions
    for (int i = 0 ; i < nbNoeuds ; ++i) {
        
        moyenneMin = INFINITY;
        nbPhrasesPartition = noeuds[i].size();
        phrasesRep[i] = noeuds[i].at(0).first;

        // pour chaque phrase j de la partition i, calculer la moyenne
        // des distances avec toutes autres phrases de la partition. 
        // Conserver la phrase avec la moyenne la plus basse.
        for (int j = 0 ; j < nbPhrasesPartition ; ++j) {
            
            index_j = noeuds[i].at(j).second;
            moyenne = 0;
            somme = 0;
            for (int k = 0 ; k < nbPhrasesPartition ; ++k) {

                if (j != k) {
                    index_k = noeuds[i].at(k).second;
                    
                    if (index_j < index_k) {
                        somme += grapheInit[index_j][index_k];
                    }
                    else {
                        somme += grapheInit[index_k][index_j];
                    }
                }
            }
            moyenne = somme/(nbPhrasesPartition - 1);

            if(moyenne < moyenneMin) {
                moyenneMin = moyenne;
                phrasesRep[i] = noeuds[i].at(j).first;
            }   
        }
    }
    return phrasesRep;
}

/**
 * Fonction affichant les phrases representantes a la sortie standard.
 *
 * @param  phrasesRep   Pointeur pointant sur les phrases representantes
 * @param  nbPhrases    Le nombre de phrases. 
 * @return 
 */
void afficherRepresentantes(Phrase* const &phrasesRep, int const &nbPhrases) {

    for (int i = 0 ; i < nbPhrases ; ++i) {
        cout << phrasesRep[i] << endl;
    }
}

/**
 * Main
 */
int main(int argc, char** argv) {

    int nbPhrasesAfficher;
    int nbNoeuds;
    int erreur;
    int nbHistoires;
    double** graphe;
    char mode;
    vector<Phrase> phrases;
    vector< pair<Phrase, int> >* noeuds;
    string titreHistoire;
    Phrase* phrasesRep;
    
    vector<Histoire *> * histoires = lireDocuments( string( "listeDocument.xml" ) );  
    erreur = validerArguments(argc, argv, nbPhrasesAfficher, mode, titreHistoire);
       
    if (erreur != 0) {
        afficherMessageErreur(erreur);
    }
    else {
        phrases  = retournerPhrases(*histoires, titreHistoire);
        nbNoeuds = phrases.size();

        if (nbNoeuds < 1) {
            afficherMessageErreur(6);
        } 
        else {
            noeuds = retournerNoeuds(phrases);
            phrasesRep = retournerRepresentantes(noeuds, nbNoeuds, nbPhrasesAfficher, mode); 
            afficherRepresentantes(phrasesRep, nbPhrasesAfficher);
        }
    }
    return 0;
}
