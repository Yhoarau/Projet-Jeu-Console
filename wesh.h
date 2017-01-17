/**
 *
 * @file    wesh.h
 *
 * @author  Yaël Hoarau, Théo Hébrard, Hugo Fasone, Leo Ferrer Laroche, Kévin Falco
 *
 * @date    16/01/2016
 *
 * @brief   Fonctions de notre projet
 *
 **/

#ifndef WESH_H
#define WESH_H

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>

namespace Matrix
{
    typedef std::vector <char> CVLine; // un type représentant une ligne de la grille
    typedef std::vector <CVLine> CMatrix; // un type représentant la grille
    typedef std::pair   <unsigned, unsigned> CPosition; // un type représentant une coordonnée dans la grille

    /*
     * fonction permettant de rafraîchir le terminal
     */

    void ClearScreen ();

    /*
     * fonction permettant de modifier la couleur du fond ou des caractères
     */

    void Couleur (const std::string & coul);

    /*
     * fonction permettant d'afficher la matrice ainsi que tous ses éléments
     */

    void  ShowMatrix (const CMatrix & Mat);

    /*
     *  fonction permettant d'initialiser la matrice et ses éléments
     */

    void InitMat (CMatrix & Mat,
                  const unsigned NbLine,
                  const unsigned NbColumn,
                  const CPosition & PosPlayer1,
                  const CPosition & PosPlayer2,
                  const CPosition & PosPlayerI,
                  const CPosition & PosPlayerA);

    /*
     * fonction permettant aux joueurs de déplacer la position de son pion
     */

    bool MoveToken (CMatrix & Mat,
                    const char Move,
                    CPosition  & Pos);

    /*
     * fonction permettant à l'IA de se déplacer de manière autonome et sécurisée
     */

    void MoveIA(CMatrix & Mat,
                const CPosition & PosPlayer,
                CPosition & PosIA,
                const unsigned & Turn,
                const unsigned & Diff);

    /*
     * fonction permettant de faire apparaître des cases interdites sur la matrice
     */

    void DisableCase(CMatrix & Mat,
                     const CPosition & Pos1,
                     const CPosition & Pos2,
                     const CPosition & PosI,
                     const CPosition & PosA);

    /*
     *  fonction permettant de définir les conditions de victoire et les messages à afficher correspondants
     */

    bool WinTest(const bool & Move,
                 const CPosition & Pos1,
                 const CPosition & Pos2,
                 const CPosition & PosI,
                 const CPosition & PosA,
                 const unsigned & NbTurn);

    /*
     * fonction servant de compteur de tours
     */

    void CptTurn (unsigned & Turn);

    /*
     * fonction permettant d'afficher une invite de commande pour permettre au joueur de déplacer son pion
     */

    void Invite(const unsigned & Turn,
                char & MoveKey);

    /*
     * fonction de changement de difficulté
     */

    unsigned Difficulty();

    /*
     * Programme du Mange moi
     */

    int MangeMoi ();

    /*
     * fonction du menu du jeu
     */

    void Menu();
}
#endif // WESH_H
