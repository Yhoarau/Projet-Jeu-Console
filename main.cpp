#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>

using namespace std;


namespace
{
    typedef vector <char> CVLine; // un type représentant une ligne de la grille
    typedef vector <CVLine> CMatrix; // un type représentant la grille
    typedef pair   <unsigned, unsigned> CPosition; // un type représentant une coordonnée dans la grille


    void ClearScreen ()
    {
        cout << "\033[H\033[2J";
    }

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

    void Couleur (const string & coul)
    {
        cout << "\033[" << coul <<"m";
    }

    const char KTokenPlayer1 = 'X';
    const char KTokenPlayer2 = 'O';
    const char KTokenPlayerI = 'I';
    const char KTokenPlayerA = 'A';
    const char KEmpty        = ' ';
    const char KTiret        = '-';
    const char KPipe         = '|';
    const char KForbidden    = 'G';

    const unsigned MatSizeH (10);
    const unsigned MatSizeL (10);

    const vector< pair<int,int>> PossibleMove = {{-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1}};



    void  ShowMatrix (const CMatrix & Mat)
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

    void InitMat (CMatrix & Mat, const unsigned NbLine, const unsigned NbColumn, const CPosition & PosPlayer1, const CPosition & PosPlayer2,
                  const CPosition & PosPlayerI, const CPosition & PosPlayerA)
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
    } //InitMat

    bool MoveToken (CMatrix & Mat, const char Move, CPosition  & Pos)
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
        if(Mat[Pos.first][Pos.second] == KForbidden || Pos.first > Mat.size() - 1 || Pos.second > Mat[0].size() - 1 )
            return false;
        Mat[Pos.first][Pos.second] = Player == KTokenPlayer1 ? KTokenPlayer1 : KTokenPlayer2;
        return true;
    } //MoveToken

    void MoveIA(CMatrix & Mat, const CPosition & PosPlayer, CPosition & PosIA, const unsigned & Turn, const unsigned & Diff)
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
                case 0 : // Cas IA facile
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
                case 1 : // Cas IA moyen ( déplacement interdit en diagonale )
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
                case 2 : // Cas IA difficile ( Tout déplacement autorisé )
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

    void DisableCase(CMatrix & Mat, const CPosition & Pos1, const CPosition & Pos2, const CPosition & PosI, const CPosition & PosA)
    {
        CPosition ForbiddenCase;
        do
        {
            srand (time(NULL));
            ForbiddenCase = make_pair(rand() % (Mat.size()-1), rand() % (Mat[0].size()-1));
        } while(ForbiddenCase == Pos1 || ForbiddenCase == Pos2 || ForbiddenCase == PosI || ForbiddenCase == PosA  ||Mat[ForbiddenCase.first][ForbiddenCase.second] == KForbidden
                || (ForbiddenCase.first == 0 && ForbiddenCase.second == MatSizeL-1) || (ForbiddenCase.first == MatSizeH-1 && ForbiddenCase.second == 0) );

        Mat[ForbiddenCase.first][ForbiddenCase.second] = KForbidden;


    } //DisableCase





    bool WinTest(const bool & Move,const CPosition & Pos1,const CPosition & Pos2, const CPosition & PosI,const CPosition & PosA, const unsigned & NbTurn)
    {
        if(!Move)
        {
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 2 : 1) << " est entré sur une case interdite ou est sorti des limites du terrain." << endl;
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 1 : 2) << " a gagné !" << endl;
            return true;
        }
        else if(Pos1 == Pos2)
        {
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 2 : 1) << " a mangé le joueur "  << (NbTurn%2 == 0 ? 1 : 2)  << endl;
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 2 : 1) << " a gagné !" << endl;
            return true;
        }
        else if(Pos1 == PosA || Pos2 == PosA || Pos1 == PosI || Pos2 == PosI )
        {
            cout << " Le joueur " << (NbTurn%2==0 ? (PosA == Pos1 ? 1 : 2) : (PosI == Pos1 ? 1 : 2)) << " s'est fait mangé par l'IA " << (NbTurn%2==0 ? 'A' : 'I') << endl;
            cout << "Le joueur "<< (NbTurn%2==0 ? (PosA == Pos1 ? 2 : 1) : (PosI == Pos1 ? 2 : 1)) << " a gagné !" << endl;
            return true;
        }
        else if ((Pos1.first  == MatSizeH -1 && Pos1.second == 0) || (Pos2.first == 0 && Pos2.second == MatSizeL -1))
        {
            cout << "Le joueur " << (NbTurn%2==0 ? 2 : 1) << " est entré dans le camp de l'adversaire." << endl;
            cout << "Le joueur " << (NbTurn%2==0 ? 2 : 1) << " a gagné !" << endl;
            return true;
        }

        return false;

    } //WinTest

    void CptTurn(unsigned & Turn)
    {
        cout << "Tour : " << ++Turn << endl;
    } //CptTurn

    void Invite( const unsigned & Turn, char & MoveKey)
    {
        cout << "Joueur " << (Turn%2 == 0 ? 2 : 1) << " : Entrez une touche :" << endl;
        cin >> MoveKey;
    } //Invite



    int ppal ()
    {

        CMatrix Mat;
        CPosition Pos1 (0,MatSizeL - 1);
        CPosition Pos2 (MatSizeH -1,0);
        CPosition PosI (0,0);
        CPosition PosA (MatSizeH -1,MatSizeL -1);
        unsigned NbTurn(0);
        unsigned Diff(1);
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
    } //ppal

}

int main()
{
    ppal();
    return 0;
}
