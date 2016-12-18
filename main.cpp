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

    const string KFReset   ("10");
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
    const char KEmpty        = ' ';
    const char KTiret        = '-';
    const char KPipe         = '|';
    const char KForbidden    = 'G';



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
                if(Mat[Line][Col] != KTokenPlayer1 && Mat[Line][Col] != KTokenPlayer2 && Mat[Line][Col] != KForbidden)
                    cout << KPipe << KEmpty << Mat[Line][Col] << KEmpty;
                else if (Mat[Line][Col] == KForbidden)
                {
                    cout << KPipe;
                    Couleur(KNoir);
                    Couleur(KFNoir);
                    cout << KEmpty << Mat[Line][Col] << KEmpty;
                    Couleur(KReset);
                }
                else
                {
                    cout << KPipe;
                    Couleur(Mat[Line][Col] == KTokenPlayer1 ? KRouge : KBleu);
                    cout << KEmpty << Mat[Line][Col] << KEmpty;
                    Couleur(KReset);
                }



            }
            cout << KPipe << endl << KTiretsLine << endl;
        }
    } // ShowMatrix

    void InitMat (CMatrix & Mat, unsigned NbLine, unsigned NbColumn, CPosition & PosPlayer1, CPosition & PosPlayer2)
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
    } //InitMat

    bool MoveToken (CMatrix & Mat, char Move, CPosition  & Pos)
    {
        char Player = Mat[Pos.first][Pos.second];
        Mat[Pos.first][Pos.second] = KEmpty;
        switch(Move)
        {
            case 'z':
                Pos = make_pair(Pos.first -1, Pos.second);
                break;
            case 'a':
                Pos = make_pair(Pos.first -1, Pos.second -1);
                break;
            case 'e':
                Pos = make_pair(Pos.first -1, Pos.second +1);
                break;
            case 'q':
                Pos = make_pair(Pos.first , Pos.second -1);
                break;
            case 'd':
                Pos = make_pair(Pos.first , Pos.second +1);
                break;
            case 'w':
                Pos = make_pair(Pos.first +1 , Pos.second -1);
                break;
            case 'x':
                Pos = make_pair(Pos.first +1 , Pos.second);
                break;
            case 'c':
                Pos = make_pair(Pos.first +1 , Pos.second +1);
                break;
            default:
                break;
        }
        if(Mat[Pos.first][Pos.second] == KForbidden)
            return false;
        Mat[Pos.first][Pos.second] = Player == KTokenPlayer1 ? KTokenPlayer1 : KTokenPlayer2;
        return true;
    } //MoveToken

    void DisableCase(CMatrix & Mat, CPosition & Pos1, CPosition & Pos2)
    {
        CPosition ForbiddenCase;
        do
        {
            srand (time(NULL));
            ForbiddenCase = make_pair(rand() % (Mat.size()-1), rand() % (Mat[0].size()-1));
        } while(ForbiddenCase == Pos1 || ForbiddenCase == Pos2 || Mat[ForbiddenCase.first][ForbiddenCase.second] == KForbidden);

        Mat[ForbiddenCase.first][ForbiddenCase.second] = KForbidden;


    } //DisableCase

    bool WinTest(const bool & Move,const CPosition & Pos1,const CPosition & Pos2, const unsigned & NbTurn)
    {
        if(!Move)
        {
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 1 : 2) << " est entré sur une case interdite." << endl;
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 2 : 1) << " a gagne !" << endl;
            return true;
        }
        else if(Pos1 == Pos2)
        {
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 1 : 2) << " a a mange le joueur "  << (NbTurn%2 == 0 ? 2 : 1)  << endl;
            cout << "Le joueur "<< (NbTurn%2 == 0 ? 1 : 2) << " a gagne !" << endl;
            return true;
        }

        return false;

    } //WinTest

    int ppal ()
    {

        CMatrix Mat;
        CPosition Pos1 (0,9);
        CPosition Pos2 (9,0);
        unsigned NbTurn(0);
        char MoveKey;
        bool Moved;
        bool GameOver(false);
        InitMat(Mat, 10, 10, Pos1, Pos2);
        ShowMatrix(Mat);
        while(!(GameOver))
        {
            cout << "Tour : " << NbTurn << endl;
            cout << "Joueur " << (NbTurn%2 == 0 ? 1 : 2) << " : Entrez une touche :" << endl;
            cin >> MoveKey;
            Moved = MoveToken(Mat, MoveKey, NbTurn%2 == 0 ? Pos1 : Pos2);
            ShowMatrix(Mat);
            GameOver = WinTest(Moved, Pos1, Pos2, NbTurn);
            DisableCase(Mat, Pos1, Pos2);

            ++NbTurn;

        }
        cout << " Partie terminee" << endl;
        return 0;
    } //ppal

}

int main(int argc, char *argv[])
{
    ppal();
    return 0;
}
