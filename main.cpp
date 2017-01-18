#include <iostream>
#include <vector>
#include <string>

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


    void  ShowMatrix (const CMatrix & Mat)
    {
        const string KTiretsLine = string(4*(Mat[0].size()), '-');
        ClearScreen();
        Couleur (KReset);
        cout << KTiretsLine << endl;
        for( unsigned Line(0); Line < Mat.size(); ++Line )
        {
            for( unsigned Col(0); Col < Mat[Line].size(); ++Col)
            {
                if(Mat[Line][Col] != KTokenPlayer1 && Mat[Line][Col] != KTokenPlayer2)
                    cout << '|' << KEmpty << Mat[Line][Col] << KEmpty;
                else
                {
                    cout << '|';
                    Couleur(Mat[Line][Col] == KTokenPlayer1 ? KRouge : KBleu);
                    cout << KEmpty << Mat[Line][Col] << KEmpty;
                    Couleur(KReset);
                }



            }
            cout << '|'<< endl << KTiretsLine << endl;
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
                Mat[Line][Col] = ' ';
            }
        }
        Mat[PosPlayer1.first][PosPlayer1.second] = KTokenPlayer1;
        Mat[PosPlayer2.first][PosPlayer2.second] = KTokenPlayer2;
    } //InitMat

    void MoveToken (CMatrix & Mat, char Move, CPosition  & Pos)
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
        Mat[Pos.first][Pos.second] = Player == KTokenPlayer1 ? KTokenPlayer1 : KTokenPlayer2;
    } //MoveToken

    void Labyrinth(CMatrix & Mat)
    {
        Mat.resize(20);
        for (CVLine & Line : Mat)
            Line.resize(10);
        CPosition Pos1 (0,3);
        CPosition Pos2 (0,3);
        CPosition PosI (0,3);
        CPosition PosA (0,3);
        srand (time(NULL));
        unsigned LabType = 0; //rand()%4;
        if (LabType == 0)
        {
            for (unsigned Line (0); Line < Mat.size(); ++Line)
            {
                for (unsigned i(0); i<Mat[Line].size(); ++i)
                {
                    Mat[Line][i] = KForbidden;
                }
            }
           for (unsigned Line (0); Line <= 3; ++Line)
               Mat[Line][3] = KEmpty;
           for (unsigned Line (3); Line <= 6; ++Line)
               Mat[Line][4] = KEmpty;
           for (unsigned Line (6); Line <= 11; ++Line)
               Mat[Line][3] = KEmpty;
           for (unsigned Line (11); Line <= 13; ++Line)
               Mat[Line][4] = KEmpty;
           for (unsigned Line (13); Line <= 16; ++Line)
               Mat[Line][5] = KEmpty;
           for (unsigned Line (16); Line <= 18; ++Line)
               Mat[Line][4] = KEmpty;
           for (unsigned Column (4); Column <= 8; ++Column)
               Mat[18][Column] = KEmpty;
           for (unsigned Line (18); Line >= 8; --Line)
               Mat[Line][8] = KEmpty;
           for (unsigned Column (8); Column >= 1; --Column)
               Mat[8][Column] = KEmpty;
           for (unsigned Line (8); Line <= 12; ++Line)
               Mat[Line][1] = KEmpty;
           for (unsigned Line (12); Line <= 14; ++Line)
               Mat[Line][0] = KEmpty;
           for (unsigned Line (14); Line <= 16; ++Line)
               Mat[Line][1] = KEmpty;
           for (unsigned Line (16); Line <= 19; ++Line)
               Mat[Line][2] = KEmpty;

        }
    }//Labyrinth

    int ppal ()
    {

        CMatrix Mat;
        CPosition Pos1 (0,9);
        CPosition Pos2 (9,0);
        unsigned NbTurn(10);
        char MoveKey;
        InitMat(Mat, 10, 10, Pos1, Pos2);
        ShowMatrix(Mat);
        while(NbTurn != 0)
        {
            cout << "Joueur " << (NbTurn%2 == 0 ? 1 : 2) << " : Entrez une touche :" << endl;
            cin >> MoveKey;
            MoveToken(Mat, MoveKey, NbTurn%2 == 0 ? Pos1 : Pos2);
            ShowMatrix(Mat);
            if(Pos1 == Pos2)
            {
                cout << "Le joueur "<< (NbTurn%2 == 0 ? 1 : 2) << " a gagne !" << endl;
                break;
            }
            --NbTurn;
            cout << "Il reste " << NbTurn << " tours"<< endl;
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

