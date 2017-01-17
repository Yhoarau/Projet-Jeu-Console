/**
 *
 * @file    wesh.h
 *
 * @author  Yaël Hoarau, Théo Hébrard, Hugo Fasone, Leo Ferrer Laroche, Kévin Falco
 *
 * @date    16/01/2016
 *
 * @brief   Corps des fonctions de notre projet
 *
 **/

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "wesh.h"

using namespace std;
using namespace Matrix;

typedef vector <char> CVLine; // un type représentant une ligne de la grille
typedef vector <CVLine> CMatrix; // un type représentant la grille
typedef pair   <unsigned, unsigned> CPosition; // un type représentant une coordonnée dans la grille

const string KReset   ("0");
const string KNoir    ("30");
const string KRouge   ("31");
const string KVert    ("32");
const string KJaune   ("33");
const string KBleu    ("34");
const string KMagenta ("35");
const string KCyan    ("36");

const string KFNoir    ("40");
const string KFRouge   ("41");
const string KFVert    ("42");
const string KFJaune   ("43");
const string KFBleu    ("44");
const string KFMagenta ("45");
const string KFCyan    ("46");

const char KTokenPlayer1 = 'X';
const char KTokenPlayer2 = 'O';
const char KTokenPlayerI = 'I';
const char KTokenPlayerA = 'A';
const char KEmpty        = ' ';
const char KTiret        = '-';
const char KPipe         = '|';
const char KForbidden    = 'G';

const vector< pair<int,int>> PossibleMove = {{-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1}};

const unsigned MatSizeH (10);
const unsigned MatSizeL (10);

/*
 * fonction permettant de rafraîchir le terminal
 */

void Matrix::ClearScreen ()
{
    cout << "\033[H\033[2J";
} // ClearScreen

/*
 * fonction permettant de modifier la couleur du fond ou des caractères
 */

void Matrix::Couleur (const string & coul)
{
    cout << "\033[" << coul <<"m";
} // Couleur

/*
 * fonction permettant d'afficher la matrice ainsi que tous ses éléments
 */

void  Matrix::ShowMatrix (const CMatrix & Mat)
{
    const string KTiretsLine = string(4*(Mat[0].size()), KTiret);
    ClearScreen();
    Couleur (KReset);
    cout << KTiretsLine << endl;
    for( unsigned Line(0); Line < Mat.size(); ++Line )
    {
        for( unsigned Col(0); Col < Mat[Line].size(); ++Col)
        {
            cout << KPipe;
            if(Line == 0 && Col == MatSizeL-1) Couleur(KFRouge);
            if(Line == MatSizeH-1 && Col == 0) Couleur (KFBleu);
            switch(Mat[Line][Col])
            {
                case KEmpty:
                    break;
                case KForbidden:
                    Couleur(KNoir);
                    Couleur(KFNoir);
                    break;
                case KTokenPlayer1 :
                    Couleur(KRouge);
                    break;
                case KTokenPlayer2 :
                    Couleur(KBleu);
                    break;
                case KTokenPlayerI :
                    Couleur(KVert);
                    break;
                case KTokenPlayerA :
                    Couleur(KJaune);
                default:
                    break;
            }
            cout << KEmpty << Mat[Line][Col] << KEmpty;
            Couleur(KReset);
        }
        cout << KPipe << endl << KTiretsLine << endl;
    }
} // ShowMatrix

/*
 *  fonction permettant d'initialiser la matrice et ses éléments
 */

void Matrix::InitMat (CMatrix & Mat,
              const unsigned NbLine,
              const unsigned NbColumn,
              const CPosition & PosPlayer1,
              const CPosition & PosPlayer2,
              const CPosition & PosPlayerI,
              const CPosition & PosPlayerA)
{
    Mat.resize(NbLine);
    for( unsigned Line(0); Line < Mat.size(); ++Line )
    {
        Mat[Line].resize(NbColumn);
        for( unsigned Col(0); Col < Mat[Line].size(); ++Col)
        {
            Mat[Line][Col] = KEmpty;
        }
    }
    Mat[PosPlayer1.first][PosPlayer1.second] = KTokenPlayer1;
    Mat[PosPlayer2.first][PosPlayer2.second] = KTokenPlayer2;
    Mat[PosPlayerI.first][PosPlayerI.second] = KTokenPlayerI;
    Mat[PosPlayerA.first][PosPlayerA.second] = KTokenPlayerA;
} // InitMat

/*
 * fonction permettant aux joueurs de déplacer la position de son pion
 */

bool Matrix::MoveToken (CMatrix & Mat,
                const char Move,
                CPosition  & Pos)
{
    char Player = Mat[Pos.first][Pos.second];
    Mat[Pos.first][Pos.second] = KEmpty;
    switch(Move)
    {
        case 'a':
            Pos = make_pair(Pos.first + PossibleMove[0].first, Pos.second + PossibleMove[0].second);
            break;
        case 'z':
            Pos = make_pair(Pos.first + PossibleMove[1].first, Pos.second + PossibleMove[1].second);
            break;
        case 'e':
            Pos = make_pair(Pos.first + PossibleMove[2].first, Pos.second + PossibleMove[2].second);
            break;
        case 'd':
            Pos = make_pair(Pos.first + PossibleMove[3].first, Pos.second + PossibleMove[3].second);
            break;
        case 'c':
            Pos = make_pair(Pos.first + PossibleMove[4].first, Pos.second + PossibleMove[4].second);
            break;
        case 'x':
            Pos = make_pair(Pos.first + PossibleMove[5].first, Pos.second + PossibleMove[5].second);
            break;
        case 'w':
            Pos = make_pair(Pos.first + PossibleMove[6].first, Pos.second + PossibleMove[6].second);
            break;
        case 'q':
            Pos = make_pair(Pos.first + PossibleMove[7].first, Pos.second + PossibleMove[7].second);
            break;
        default:
            break;
    }
    if(Mat[Pos.first][Pos.second] == KForbidden) return false;
    Mat[Pos.first][Pos.second] = Player == KTokenPlayer1 ? KTokenPlayer1 : KTokenPlayer2;
    return true;
} // MoveToken

/*
 * fonction permettant à l'IA de se déplacer de manière autonome et sécurisée
 */

void Matrix::MoveIA(CMatrix & Mat,
            const CPosition & PosPlayer,
            CPosition & PosIA,
            const unsigned & Turn,
            const unsigned & Diff)
{
    CPosition Dist;
    CPosition PosTemp;
    PosTemp = make_pair(PosIA.first, PosIA.second);
    bool SignX(PosPlayer.first > PosIA.first); // Vrai si le Joueur se trouve en bas de l'IA
    bool SignY(PosPlayer.second > PosIA.second); // Vrai si le Joueur se trouve à droite de l'IA

    Dist = make_pair( SignX ? PosPlayer.first - PosIA.first : PosIA.first - PosPlayer.first, SignY ? PosPlayer.second - PosIA.second : PosIA.second - PosPlayer.second );
    // Dist prend pour valeur la distance qui sépare l'IA du joueur qui le pourchasse en X et en Y

    unsigned Incr(0); // Sers à passer au mouvement suivant si le mouvement courant est impossible


    switch(Diff)
        {
            case 1 : // Cas IA facile
            {

                if((Turn+2)%4 > 1 ) // Condition pour faire jouer les IA un tour sur deux
                {

                    do
                    {
                        if (Dist.first > Dist.second) // Si le joueur est plus loin en hauteur qu'en longueur
                            PosTemp = make_pair(PosIA.first + PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].first, PosIA.second + PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].second);
                        else if (Dist.first < Dist.second) // Si le joueur est plus loin en longueur qu'en hauteur
                            PosTemp = make_pair(PosIA.first + PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].first, PosIA.second + PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].second);
                        Incr += 2;
                        if ( Incr > 8)
                            return;
                    }while(Mat[PosTemp.first][PosTemp.second] == KForbidden || Mat[PosTemp.first][PosTemp.second] == KTokenPlayerA || Mat[PosTemp.first][PosTemp.second] == KTokenPlayerI
                           || PosTemp.first > Mat.size() || PosTemp.second > Mat[0].size());

                }
                else return;
                break;
            }
            case 2 : // Cas IA moyen ( déplacement interdit en diagonale )
            {
                do
                {
                    if (Dist.first > Dist.second) // Si le joueur est plus loin en hauteur qu'en longueur
                        PosTemp = make_pair(PosIA.first + PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].first, PosIA.second + PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].second);
                    else if (Dist.first < Dist.second) // Si le joueur est plus loin en longueur qu'en hauteur
                        PosTemp = make_pair(PosIA.first + PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].first, PosIA.second + PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].second);
                    Incr += 2;
                    if (Incr > 8)
                        return;
                    cout << Incr;
                }while(Mat[PosTemp.first][PosTemp.second] == KForbidden || Mat[PosTemp.first][PosTemp.second] == KTokenPlayerA || Mat[PosTemp.first][PosTemp.second] == KTokenPlayerI
                       || PosTemp.first > Mat.size() || PosTemp.second > Mat[0].size());
                break;
            }
            case 3 : // Cas IA difficile ( Tout déplacement autorisé )
            {
                do
                {
                    if((Dist.first != 0) && (Dist.second != 0)) // Si le joueur n'est ni sur la même ligne ni sur la même colonne
                    {
                        if(!(SignX))
                            PosTemp = make_pair(PosIA.first + PossibleMove[SignY ? ((2+Incr)%8) : ((0+Incr)%8)].first, PosIA.second + PossibleMove[SignY ? ((2+Incr)%8) : ((0+Incr)%8)].second);
                        else if(SignX)
                            PosTemp = make_pair(PosIA.first + PossibleMove[SignY ? ((4+Incr)%8) : ((6+Incr)%8)].first, PosIA.second + PossibleMove[SignY ? ((4+Incr)%8) : ((6+Incr)%8)].second);
                    }
                    else if (Dist.first > Dist.second) // Si le joueur est plus loin en hauteur qu'en longueur
                        PosTemp = make_pair(PosIA.first + PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].first, PosIA.second + PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].second);
                    else if (Dist.first < Dist.second) // Si le joueur est plus loin en longueur qu'en hauteur
                        PosTemp = make_pair(PosIA.first + PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].first, PosIA.second + PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].second);
                    if (++Incr >8)
                        return;
                }while(Mat[PosTemp.first][PosTemp.second] == KForbidden || Mat[PosTemp.first][PosTemp.second] == KTokenPlayerA || Mat[PosTemp.first][PosTemp.second] == KTokenPlayerI
                       || PosTemp.first > Mat.size() || PosTemp.second > Mat[0].size());
                break;
            }
            default :
                break;
    }

    Mat[PosIA.first][PosIA.second] = KEmpty;

    PosIA = make_pair(PosTemp.first, PosTemp.second);


    Mat[PosIA.first][PosIA.second] = Turn%2==0 ? KTokenPlayerA : KTokenPlayerI;
} // MoveIA

/*
 * fonction permettant de faire apparaître des cases interdites sur la matrice
 */

void Matrix::DisableCase(CMatrix & Mat,
                 const CPosition & Pos1,
                 const CPosition & Pos2,
                 const CPosition & PosI,
                 const CPosition & PosA)
{
    CPosition ForbiddenCase;
    do
    {
        srand (time(NULL));
        ForbiddenCase = make_pair(rand() % (Mat.size()-1), rand() % (Mat[0].size()-1));
    } while(ForbiddenCase == Pos1 || ForbiddenCase == Pos2 || ForbiddenCase == PosI || ForbiddenCase == PosA  ||Mat[ForbiddenCase.first][ForbiddenCase.second] == KForbidden);

    Mat[ForbiddenCase.first][ForbiddenCase.second] = KForbidden;
} // DisableCase

/*
 *  fonction permettant de définir les conditions de victoire et les messages à afficher correspondants
 */

bool Matrix::WinTest(const bool & Move,
             const CPosition & Pos1,
             const CPosition & Pos2,
             const CPosition & PosI,
             const CPosition & PosA,
             const unsigned & NbTurn)
{
    if(!Move)
    {
        cout << "Le joueur "<< (NbTurn%2 == 0 ? 2 : 1) << " est entré sur une case interdite. Il devrait regarder l'écran." << endl;
        cout << "Le joueur "<< (NbTurn%2 == 0 ? 1 : 2) << " dispose de la vue" << endl;
        return true;
    }
    else if(Pos1 == Pos2)
    {
        cout << "Le joueur "<< (NbTurn%2 == 0 ? 2 : 1) << " a mangé le joueur "  << (NbTurn%2 == 0 ? 1 : 2) << endl;
        cout << "Le joueur "<< (NbTurn%2 == 0 ? 1 : 2) << " retarde sa mort !" << endl;
        return true;
    }
    else if(Pos1 == PosA || Pos2 == PosA || Pos1 == PosI || Pos2 == PosI )
    {
        cout << " Le joueur " << (NbTurn%2==0 ? (PosA == Pos1 ? 1 : 2) : (PosI == Pos1 ? 1 : 2)) << " s'est fait mangé par l'IA " << (NbTurn%2==0 ? 'A' : 'I') << endl;
        cout << "Le joueur "<< (NbTurn%2==0 ? (PosA == Pos1 ? 2 : 1) : (PosI == Pos1 ? 2 : 1)) << " était moins appétissant!" << endl;
        return true;
    }

    else if ((Pos1.first  == MatSizeH -1 && Pos1.second == 0) || (Pos2.first == 0 && Pos2.second == MatSizeL -1))
    {
        cout << "Le joueur " << (NbTurn%2==0 ? 2 : 1) << " est entré dans le camp de l'adversaire." << endl;
        cout << "Le joueur " << (NbTurn%2==0 ? 2 : 1) << " découvre un nouveau monde fantastique !" << endl;
        return true;
    }
    return false;
} //WinTest

/*
 * fonction servant de compteur de tours
 */

void Matrix::CptTurn (unsigned & Turn)
{
    ++Turn;
    cout << "Tour : " << Turn << endl;
} // CptTurn

/*
 * fonction permettant d'afficher une invite de commande pour permettre au joueur de déplacer son pion
 */

void Matrix::Invite(const unsigned & Turn,
            char & MoveKey)
{
    cout << "Joueur " << (Turn%2 == 0 ? 2 : 1) << " : Entrez une touche :" << endl;
    cin >> MoveKey;
} // Invite

/*
 * fonction de changement de difficulté
 */

unsigned Matrix::Difficulty()
{
     unsigned Choix;
     cout << "-----------------DIFFICULTÉ-----------------" << "\n\n\n";
     cout << "1 - Facile" << endl;
     cout << "2 - Moyen" << endl;
     cout << "3 - Difficile" << "\n\n\n";
     cout << "Votre choix : ";
     cin >> Choix;
     switch(Choix)
     {
         case 1:
             return 1;
             break;
         case 2:
             return 2;
             break;
         case 3:
             return 3;
             break;
         default:
             break;
     }
     return 0;
} // Difficulty

/*
 * programme du Mange moi
 */

int Matrix::MangeMoi ()
{

    CMatrix Mat;
    CPosition Pos1 (0,MatSizeL - 1);
    CPosition Pos2 (MatSizeH -1,0);
    CPosition PosI (0,0);
    CPosition PosA (MatSizeH -1,MatSizeL -1);
    unsigned NbTurn(0);
    unsigned Diff(Difficulty());
    char MoveKey;
    bool Moved;
    bool GameOver(false);

    InitMat(Mat, 10, 10, Pos1, Pos2, PosI, PosA);

    while(!(GameOver))
    {
        ShowMatrix(Mat);
        CptTurn(NbTurn);
        Invite(NbTurn, MoveKey);

        Moved = MoveToken(Mat, MoveKey, NbTurn%2 == 0 ? Pos2 : Pos1);
        ShowMatrix(Mat);
        GameOver = WinTest(Moved, Pos1, Pos2, PosI, PosA, NbTurn);

        MoveIA(Mat, NbTurn%2 == 0 ? Pos2 : Pos1, NbTurn%2 == 0 ? PosA : PosI, NbTurn, Diff);
        ShowMatrix(Mat);

        GameOver = WinTest(Moved, Pos1, Pos2, PosI, PosA, NbTurn);

        DisableCase(Mat, Pos1, Pos2, PosI, PosA);
    }
    cout << "Partie terminee" << endl;
    return 0;
} // MangeMoi

/*
 * fonction du menu du jeu
 */

void Matrix::Menu()
{
        unsigned Choix;
        cout << "-----------------MENU-----------------" << "\n\n\n";
        cout << "1 - Mode \" Mange moi ! \"" << endl;
        cout << "2 - Mode Labyrinthe" << endl;
        cout << "3 - Mode Pièges" << "\n\n\n";
        cout << "Votre choix : ";
        cin >> Choix;
        switch(Choix)
        {
            case 1:
                ClearScreen ();
                MangeMoi();
                break;
            case 2:
                ClearScreen ();
                //FaireMode
                break;
            case 3:
                ClearScreen ();
                //FaireMode
                break;
            default:
                //FaireMode
                break;
        }
    }
