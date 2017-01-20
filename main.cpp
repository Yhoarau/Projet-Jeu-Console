#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

using namespace std;

struct termios saved_attributes;

/*
 *
 *
 * ------------------------------------------------------------------------ FONCTIONS DE MODIFICATION DE SAISIE CLAVIER ------------------------------------------------------------
 *
 *
 */

void
reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);

}

void set_input_mode (void)
{
  struct termios tattr;

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
  {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
  }

  /* Save the terminal attributes so we can restore them later.*/
  tcgetattr (STDIN_FILENO, &saved_attributes);
  atexit (reset_input_mode);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  tattr.c_cc[VMIN] = 0;
  tattr.c_cc[VTIME] = 3;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}


namespace
{
    void Menu(); // A supprimer à l'ajour du .h

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
    const char KCampPlayer1   = 'B';
    const char KCampPlayer2   = 'C';
    const char KFoW          = 'F';
    const char KWatch        = 'W';
    const char KVictory      = 'V';


    // PossibleMove contient tous les déplacements possible dans la matrice dans le sens des aiguilles d'une montre
    const vector< pair<int,int>> PossibleMove = {{-1,-1},{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1}, {0,0}};
    //PossibleKey contient toutes les touches de déplacement en suivant l'ordre de PossibleMove ( sens des aiguilles d'une montre)
    const vector<char> PossibleKey = {'a', 'z', 'e', 'd', 'c', 'x', 'w', 'q', 's'};


    /*
     *
     *
     * ------------------------------------------------------------------------ FONCTIONS INHERENTES A LA MATRICE ----------------------------------------------------------------
     *
     *
     */


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
                        break;
                    case KCampPlayer1 :
                        Couleur(KRouge);
                        Couleur(KFRouge);
                        break;
                    case KCampPlayer2 :
                        Couleur(KBleu);
                        Couleur(KFBleu);
                    case KFoW :
                        Couleur(KFNoir);
                        break;
                    case KWatch :
                        Couleur(KVert);
                        Couleur(KFVert);
                        break;
                    case KVictory :
                        Couleur(KJaune);
                        Couleur(KFJaune);
                        break;
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

    /*
     *
     *
     * ----------------------------------------------------  FONCTIONS DE TEST ET VERIFICATION -----------------------------------------------------------------------------------
     *
     *
     */

    bool IsInMat(CMatrix & Mat, CPosition & Pos)
    {
        if( Pos.first > Mat.size() - 1 || Pos.second > Mat[0].size() - 1)
            return false;
        return true;
    }


    bool IsAllowed(CMatrix & Mat, CPosition & Pos)
    {
        if( !(IsInMat(Mat, Pos)) || Mat[Pos.first][Pos.second] == KForbidden ||  Mat[Pos.first][Pos.second] == KTokenPlayerA  || Mat[Pos.first][Pos.second] == KTokenPlayerI )
            return false;
        return true;
    }

    bool MoveCheck(char & MoveKey)
    {
        for(unsigned i (0); i < PossibleKey.size()- 1; ++i)
            if (PossibleKey[i] == MoveKey) return true;
        return false;
    }

    /*
     *
     *
     * -------------------------------------------------------------------- FONCTIONS DE MOUVEMENT -----------------------------------------------------------------------------------------
     *
     *
     */

    void MoveToken (CMatrix & Mat, char & Move, CPosition  & Pos)
    {
        char Player = Mat[Pos.first][Pos.second];
        Mat[Pos.first][Pos.second] = KEmpty;
        //Move = tolower(Move);
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
        Move = 's';

        Mat[Pos.first][Pos.second] = Player == KTokenPlayer1 ? KTokenPlayer1 : KTokenPlayer2;
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

        /*
         * L'IA comprend 3 difficultés mais repose sur le même principe :
         *  - S'il peut se déplacer en diagonale (en Difficile) Il repère si le joueur ne se trouve ni sur la même ligne, ni sur la même colonne,
         *    et se sert ensuite des signes pour déterminer dans quel sens se trouve le joueur à la fois en horizontal et vertical.
         *  - Sinon il repère si le joueur est plus loin en hauteur ou en longueur et se déplace selon la distance la plus éloignée.
         *  - Si le déplacement qu'il souhaite réaliser n'est pas possible ( Case interdite, Hors Matrice, ou Autre IA sur la case) alors il prendra
         *    le prochain mouvement possible dans le sens des aiguilles d'une montre, qui est l'ordre des mouvements dans le vecteur PossibleMove
         * Je détaille plus précisément le comportement de l'algorithme sur l'IA en mode difficile (case 2) car les deux premiers modes reprennent simplement un bout de cette IA
         * avec quelques subtilités que je détaille également
         */
        switch(Diff)
            {
                case 0 : // Cas IA facile ( déplacement interdit en diagonale + joue un tour sur deux)
                {

                    if((Turn+2)%4 > 1 ) // Condition pour faire jouer les IA un tour sur deux
                    {

                        do
                        {
                            if (Dist.first > Dist.second)
                                PosTemp = make_pair(PosIA.first +
                                                    PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].first,
                                                    PosIA.second +
                                                    PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].second);
                            else if (Dist.first < Dist.second)
                                PosTemp = make_pair(PosIA.first +
                                                    PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].first,
                                                    PosIA.second +
                                                    PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].second);
                            Incr += 2; // Il saute les déplacements en diagonale
                            if ( Incr > 8)
                                return;
                        }while(!(IsAllowed(Mat, PosTemp)));

                    }
                    else return;
                    break;
                }
                case 1 : // Cas IA moyen ( déplacement interdit en diagonale )
                {
                    do
                    {
                        if (Dist.first > Dist.second)
                            PosTemp = make_pair(PosIA.first +
                                                PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].first,
                                                PosIA.second +
                                                PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].second);
                        else if (Dist.first < Dist.second)
                            PosTemp = make_pair(PosIA.first +
                                                PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].first,
                                                PosIA.second +
                                                PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].second);
                        Incr += 2;  // Il saute les déplacements en diagonale
                        if (Incr > 8)
                            return;
                    }while(!(IsAllowed(Mat, PosTemp)));
                    break;
                }
                case 2 : // Cas IA difficile ( Tout déplacement autorisé )
                {
                    do
                    {
                        if((Dist.first != 0) && (Dist.second != 0))                                             // Si le joueur n'est ni sur la même ligne ni sur la même colonne
                        {
                            if(!(SignX))                                                                        // Si le joueur se trouve plus haut dans la matrice que l'IA
                                PosTemp = make_pair(PosIA.first +                                               // La prochaine coordonnée de l'IA sera en premier membre son placement vertical +
                                                    PossibleMove[SignY ? ((2+Incr)%8) : ((0+Incr)%8)].first,    // Le premier membre du prochain mouvement possible en essayant la diagonale haute
                                                                                                                // gauche ou droite (selon le signe horizontal) en premier
                                                    PosIA.second +                                              // Le second membre de la prochaine coordonnée de l'IA sera son placement horizontal +
                                                    PossibleMove[SignY ? ((2+Incr)%8) : ((0+Incr)%8)].second);  // Le premier membre du prochain mouvement possible en essayant la diagonale haute
                                                                                                                // gauche ou droite (selon le signe horizontal) en premier

                            else if(SignX)                                                                      // Si le joueur se trouve plus haut dans la matrice que l'IA
                                PosTemp = make_pair(PosIA.first +                                               // Exactement le même déroulement avec la diagonale basse gauche ou droite testée en premier
                                                    PossibleMove[SignY ? ((4+Incr)%8) : ((6+Incr)%8)].first,
                                                    PosIA.second +
                                                    PossibleMove[SignY ? ((4+Incr)%8) : ((6+Incr)%8)].second);
                        }
                        else if (Dist.first > Dist.second)                                                     // Si le joueur est plus loin en hauteur qu'en longueur
                            PosTemp = make_pair(PosIA.first +                                                  // La prochaine coordonnée de l'IA sera en premier membre son placement vertical +
                                                PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].first,       // Le premier membre du prochain mouvement possible en essayant vertical en premier  selon le signe
                                                PosIA.second +                                                 // Le second membre de la prochaine coordonnée de l'IA sera son placement horizontal +
                                                PossibleMove[SignX ? ((5+Incr)%8) : ((1+Incr)%8)].second);     // Le second membre du prochain mouvement possible en essayant vertical en premier selon le signe

                        else if (Dist.first < Dist.second)                                                     // Si le joueur est plus loin en longueur qu'en hauteur
                            PosTemp = make_pair(PosIA.first +                                                  // Exactement le même déroulement avec le déplacement horizontal selon le signe testée en premier
                                                PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].first,
                                                PosIA.second +
                                                PossibleMove[SignY ? ((3+Incr)%8) : ((7+Incr)%8)].second);
                        if (++Incr >8) // Si on fait le tour des mouvements possible sans succès, on ne bouge pas
                            return;
                    }while(!(IsAllowed(Mat, PosTemp)));
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
     *
     *
     * ---------------------------------------------------------------   FONCTIONS SPECIFIQUES, DECHARGE DU MAIN ---------------------------------------------------------------------
     *
     *
     */


    bool CptTurn(unsigned & Turn, unsigned & sec, char & MoveKey)
    {
        bool TurnPass(false);
        if(--sec == 0)
        {
            cout << "Votre tour est terminé, c'est au tour du joueur suivant." << endl;
            sleep(2);
            sec = 10;
            ++Turn;
            TurnPass = true;
        }
        else if (MoveCheck(MoveKey))
        {
            sec = 10;
            ++Turn;
            TurnPass = true;
        }
        return TurnPass;
    } //CptTurn



    unsigned NbKey(char & Key) // ( NbKey(MoveKey) revoie la position de la touche choisie dans les vecteur PossibleKey et PosssibleMove)
    {
        for(unsigned i(0); i < PossibleKey.size() -1; ++i)
            if (Key == PossibleKey[i]) return i;
        return 8;
    } //NbKey

    /*
     *
     *
     * ------------------------------------------------------ FONCTION DE CAPTURE DU CLAVIER -------------------------------------------------------------------------------------
     *
     *
     */


    void Input(char & MoveKey)
    {
        do
        {
        read (STDIN_FILENO, &MoveKey, 1);
        }while(!(MoveCheck(MoveKey)));

    } //Input

    /*
     *
     *
     *------------------------------------------------   PARTIE DES FONCTIONS RESERVEES AUX AFFICHAGES ECRAN    -------------------------------------------------------------------------
     *
     *
     */

    void AffichTimer(unsigned & sec)
    {
        cout << "Il vous reste " << sec << " secondes pour jouer." << endl;
    }

    void AffichTurn(unsigned & Turn)
    {
        cout << "Tour : " << Turn << endl;
    }

    void Invite( const unsigned & Turn)
    {
        cout << "Joueur " << (Turn%2 == 0 ? "Rouge" : "Bleu") << " : Entrez une touche :" << endl;
    } //Invite


    /*
     *
     *
     *--------------------------------------------------------      PARTIE DES FONCTIONS UNIQUEMENT UTILES AU MANGE MOI    --------------------------------------------------------------------------
     *
     *
     */

    void DisableCase(CMatrix & Mat)
    {
        CPosition ForbiddenCase;
        do
        {
            srand (time(NULL));
            ForbiddenCase = make_pair(rand() % (Mat.size()-1), rand() % (Mat[0].size()-1)); // Génère un position aléatoire dans toute la matrice
        } while(Mat[ForbiddenCase.first][ForbiddenCase.second] != KEmpty);

        Mat[ForbiddenCase.first][ForbiddenCase.second] = KForbidden;


    } //DisableCase





    bool WinTestMangeMoi(const bool & Move,const CPosition & Pos1,const CPosition & Pos2, const CPosition & Camp1,const CPosition & Camp2, const CPosition & PosI,const CPosition & PosA, const unsigned & NbTurn)
    {
        if(!Move) // Si le joueur est sorti des limites du terrain, a essayé de manger un IA, ou est entré dans une case interdite
        {
            cout << "Le joueur "<< (NbTurn%2 == 0 ? "Bleu" : "Rouge") << " est entré sur une case interdite ou est sorti des limites du terrain." << endl;
            cout << "Le joueur "<< (NbTurn%2 == 0 ? "Rouge" : "Bleu") << " a gagné !" << endl;
            return true;
        }
        else if(Pos1 == Pos2) // Si un joueur à mangé l'autre
        {
            cout << "Le joueur "<< (NbTurn%2 == 0 ? "Bleu" : "Rouge") << " a mangé le joueur "  << (NbTurn%2 == 0 ? "Rouge" : "Bleu")  << endl;
            cout << "Le joueur "<< (NbTurn%2 == 0 ? "Bleu" : "Rouge") << " a gagné !" << endl;
            return true;
        }
        else if(Pos1 == PosA || Pos2 == PosA || Pos1 == PosI || Pos2 == PosI ) // Si un IA a mangé un joueur
        {
            cout << " Le joueur " << (NbTurn%2==0 ? (PosA == Pos1 ? "Rouge" : "Bleu") : (PosI == Pos1 ? "Rouge" : "Bleu")) << " s'est fait mangé par l'IA "
                                  << (NbTurn%2==0 ? ((PosA == Pos2) ? 'A' : 'I') : (PosA == Pos1) ? 'A' : 'I') << endl;
            cout << "Le joueur "<< (NbTurn%2==0 ? (PosA == Pos1 ? "Bleu" : "Rouge") : (PosI == Pos1 ? "Bleu" : "Rouge")) << " a gagné !" << endl;
            return true;
        }
        else if ((Pos1 == Camp2) || (Pos2 == Camp1)) // Si un joueur est entré dans le camp de l'autre
        {
            cout << "Le joueur " << (NbTurn%2==0 ? "Bleu" : "Rouge") << " est entré dans le camp de l'adversaire." << endl;
            cout << "Le joueur " << (NbTurn%2==0 ? "Bleu" : "Rouge") << " a gagné !" << endl;
            return true;
        }

        return false;

    } //WinTestMangeMoi

    void SetCasesMangeMoi(CMatrix & Mat, CPosition & Camp1, CPosition & Camp2)
    {
        if(Mat[Camp1.first][Camp1.second] != KTokenPlayer1 && Mat[Camp1.first][Camp1.second] != KTokenPlayer2 ) Mat[Camp1.first][Camp1.second] = KCampPlayer1;

        if(Mat[Camp2.first][Camp2.second] != KTokenPlayer1 && Mat[Camp2.first][Camp2.second] != KTokenPlayer2 ) Mat[Camp2.first][Camp2.second] = KCampPlayer2;
    } //SetCasesMangeMoi


    unsigned Difficulty()
    {
        unsigned Choix;
        do
        {
         ClearScreen();

         cout << "-----------------DIFFICULTÉ-----------------" << "\n\n\n";
         cout << "1 - Facile" << endl;
         cout << "2 - Moyen" << endl;
         cout << "3 - Difficile" << "\n\n\n";
         cout << "Votre choix : ";
         cin >> Choix;

        }
         while (Choix < 1 || Choix > 3);
         return Choix;
    } // Difficulty


    int MangeMoi ()
    {

        CMatrix Mat;
        const unsigned MatSizeH = 10;
        const unsigned MatSizeL = 10;

        CPosition Pos1 (0,MatSizeL - 1);
        CPosition Pos2 (MatSizeH -1,0);
        CPosition Camp1 (Pos1);
        CPosition Camp2 (Pos2);
        CPosition PosI (0,0);
        CPosition PosA (MatSizeH -1,MatSizeL -1);
        unsigned NbTurn(1);
        unsigned Sec(10);
        unsigned Diff(Difficulty() - 1);

        char MoveKey('s');
        bool AllowedMove(true); // Défini si le mouvement que tente de faire le joueur est valide
        bool TurnPass(false); // Défini si on passe au tour suivant
        bool GameOver(false); // Défini si la partie est terminée

        InitMat(Mat, MatSizeH, MatSizeL, Pos1, Pos2, PosI, PosA);
        SetCasesMangeMoi(Mat, Camp1, Camp2);


        set_input_mode();

        while(!(GameOver))
        {

            do
            {
                ShowMatrix(Mat);
                AffichTurn(NbTurn);
                Invite(NbTurn);
                AffichTimer(Sec);

                sleep(1);

                boost::thread ThInput {boost::bind(Input, boost::ref(MoveKey))};   // Lancement du thread qui récupère les input des utilisateurs

                TurnPass = CptTurn(NbTurn, Sec, MoveKey);

            } while ( !(TurnPass) ); // On répète et rafraichi tant que le tour d'un joueur n'est pas passé

            // NextPos indique l'endroit où le joueur souhaite se rendre

            CPosition NextPos = make_pair (                                 // La position où le joueur souhaite se rendre est définie par une pair qui :
                        (NbTurn%2 == 0 ? (Pos2.first) : (Pos1.first)) +     // Prend en premier membre la position actuelle du joueur en vertical +
                        (PossibleMove[NbKey(MoveKey)]).first,               // Le déplacement vertical (.first) du mouvement (PossibleMove) selon la touche choisie (MoveKey)
                        (NbTurn%2 == 0 ? (Pos2.second) : (Pos1.second)) +   // Prend en second membre la position actuelle du joueur en horizontal +
                        (PossibleMove[NbKey(MoveKey)]).second);             // Le déplacement horizontal (.second) du mouvement (PossibleMove) selon la touche choisie (MoveKey)

            AllowedMove = IsAllowed(Mat, NextPos);
            GameOver = WinTestMangeMoi(AllowedMove, Pos1, Pos2, Camp1, Camp2, PosI, PosA, NbTurn); // La partie prend fin ici si c'est du à un mouvement de joueur ( qui le fait mourir ou qui le fait manger l'autre joueur )
            if(!(GameOver))
            {
                MoveToken(Mat, MoveKey, NbTurn%2 == 0 ? Pos2 : Pos1);
                SetCasesMangeMoi(Mat, Camp1, Camp2);
                MoveIA(Mat, NbTurn%2 == 0 ? Pos2 : Pos1, NbTurn%2 == 0 ? PosA : PosI, NbTurn, Diff);
                ShowMatrix(Mat);
                GameOver = WinTestMangeMoi(AllowedMove, Pos1, Pos2, Camp1, Camp2, PosI, PosA, NbTurn); // La partie se termine ici si l'IA mange un joueur
                if(!(GameOver))
                    DisableCase(Mat);
            }
        }
        reset_input_mode();
        Sec = 10;

        cout << "Partie terminee" << endl
             << "Vous serez redirigé dans le Menu dans " << Sec <<  " secondes." << endl;
        cout << endl;
        for(; Sec > 0; Sec--)
        {
            cout << endl << Sec << " ";
            sleep(1);
        }
        Menu();
        return 0;
    } //MangeMoi

    /*
     *
     *
     *------------------------------------------------------     PARTIE DES FONTIONS UNIQUEMENT UTILES AU LABYRINTHE    ----------------------------------------------------------------
     *
     *
     *
     */

    void FogOfWar(CMatrix & Mat,const CPosition & Pos)
    {
        for(CVLine & Line : Mat)
            for(char & Case : Line)
                if(Case == KEmpty) Case = KFoW;
        for(unsigned i (0); i < PossibleMove.size(); ++i)
        {
            pair<int,int> AroundPosInt;
            AroundPosInt = make_pair(Pos.first + PossibleMove[i].first, Pos.second + PossibleMove[i].second);
            CPosition AroundPos = CPosition(AroundPosInt);
            if (!(IsInMat(Mat, AroundPos))) continue;
            if( Mat[AroundPos.first][AroundPos.second] == KFoW) Mat[AroundPos.first][AroundPos.second] = KEmpty;
            for(unsigned j(0); j < 3; ++j)
            {
                CPosition PosTemp = make_pair(AroundPos.first - 1,AroundPos.second);
                if (!(IsInMat(Mat, PosTemp))) continue;
                if(i < 3 && IsInMat(Mat, PosTemp) && Mat[AroundPos.first - 1][AroundPos.second] == KFoW ) Mat[AroundPos.first - 1][AroundPos.second] = KEmpty;
                PosTemp = make_pair(AroundPos.first,AroundPos.second + 1);
                if (!(IsInMat(Mat, PosTemp))) continue;
                if(i > 1 && i < 5 && IsInMat(Mat, PosTemp) && Mat[AroundPos.first][AroundPos.second + 1] == KFoW) Mat[AroundPos.first][AroundPos.second + 1] = KEmpty;
                PosTemp = make_pair(AroundPos.first + 1,AroundPos.second);
                if (!(IsInMat(Mat, PosTemp))) continue;
                if(i > 3 && i < 7 && IsInMat(Mat, PosTemp) && Mat[AroundPos.first + 1][AroundPos.second] == KFoW) Mat[AroundPos.first + 1][AroundPos.second] = KEmpty;
                PosTemp = make_pair(AroundPos.first,AroundPos.second - 1);
                if (!(IsInMat(Mat, PosTemp))) continue;
                if((i == 0 || (i > 5  && i <= 8)) && IsInMat(Mat, PosTemp) && Mat[AroundPos.first][AroundPos.second - 1] == KFoW) Mat[AroundPos.first][AroundPos.second - 1] = KEmpty;

            }
        }

    } //FogOfWar

    void SetCasesLab(CMatrix & Mat, CPosition & Victory, CPosition & Watch)
    {
        if(Mat[Victory.first][Victory.second] != KTokenPlayer1 && Mat[Victory.first][Victory.second] != KTokenPlayer2 ) Mat[Victory.first][Victory.second] = KVictory;

        if(Mat[Watch.first][Watch.second] != KTokenPlayer1 && Mat[Watch.first][Watch.second] != KTokenPlayer2 ) Mat[Watch.first][Watch.second] = KWatch;
    } //SetCasesLab

    void UnFog(CMatrix & Mat, CPosition & Watch)
    {
        if(Mat[Watch.first][Watch.second] == KTokenPlayer1 || Mat[Watch.first][Watch.second] == KTokenPlayer2)
            for(CVLine & Line : Mat)
                for(char & Case : Line)
                    if(Case == KFoW) Case = KEmpty;
    } //UnFog

    bool WinTestLab(const bool & Move,const CPosition & Pos1,const CPosition & Pos2, const CPosition & PosVict, const unsigned & NbTurn)
    {
        if(!Move)
        {
            cout << "Le joueur "<< (NbTurn%2 == 0 ? "Proie" : "Chasseur") << " est entré sur une case interdite ou est sorti des limites du terrain." << endl;
            cout << "Le joueur "<< (NbTurn%2 == 0 ? "Chasseur" : "Proie") << " a gagné !" << endl;
            return true;
        }
        else if (Pos1 == Pos2)
        {
            cout << "Le chasseur a tué sa proie, il a donc gagné." << endl;
            return true;
        }
        else if (Pos2 == PosVict)
        {
            cout << "La proie s'est échappée, bravo à elle" << endl;
            return true;
        }
        return false;
    }//WinTestLab

    void InitLabyrinthe(CMatrix & Mat, CPosition & Pos1, CPosition & Pos2, CPosition & Victory, CPosition & Watch)
    {
        Mat.resize(20);
        for (CVLine & Line : Mat)
            Line.resize(10);

        srand (time(NULL));
        unsigned LabType = 2; //rand()%4;
        if (LabType == 0)
        {

            for (unsigned Line (0); Line < Mat.size(); ++Line)
            {
                for (unsigned Col(0); Col<Mat[Line].size(); ++Col)
                {

                    Mat[Line][Col] = KForbidden;
                }
            }


               for (unsigned Line (0); Line <= 3; ++Line)
                   if(Mat[Line][3] == KForbidden) Mat[Line][3] = KEmpty;
               for (unsigned Line (3); Line <= 6; ++Line)
                   if(Mat[Line][4] == KForbidden) Mat[Line][4] = KEmpty;
               for (unsigned Line (6); Line <= 11; ++Line)
                   if(Mat[Line][3] == KForbidden) Mat[Line][3] = KEmpty;
               for (unsigned Line (11); Line <= 13; ++Line)
                   if(Mat[Line][4] == KForbidden) Mat[Line][4] = KEmpty;
               for (unsigned Line (13); Line <= 16; ++Line)
                   if(Mat[Line][5] == KForbidden) Mat[Line][5] = KEmpty;
               for (unsigned Line (16); Line <= 18; ++Line)
                   if(Mat[Line][4] == KForbidden) Mat[Line][4] = KEmpty;
               for (unsigned Column (4); Column <= 8; ++Column)
                   if(Mat[18][Column] == KForbidden) Mat[18][Column] = KEmpty;
               for (unsigned Column (6); Column <= 7; ++Column)
                   if(Mat[10][Column] == KForbidden) Mat[10][Column] = KEmpty;
               for (unsigned Line (9); Line <= 10; ++Line)
                   if(Mat[Line][6] == KForbidden) Mat[Line][6] = KEmpty;
               for (unsigned Line (18); Line >= 8; --Line)
                   if(Mat[Line][8] == KForbidden) Mat[Line][8] = KEmpty;
               for (unsigned Column (8); Column >= 1; --Column)
                   if(Mat[8][Column] == KForbidden) Mat[8][Column] = KEmpty;
               for (unsigned Line (8); Line <= 12; ++Line)
                   if(Mat[Line][1] == KForbidden) Mat[Line][1] = KEmpty;
               for (unsigned Line (12); Line <= 14; ++Line)
                   if(Mat[Line][0] == KForbidden) Mat[Line][0] = KEmpty;
               for (unsigned Line (14); Line <= 16; ++Line)
                   if(Mat[Line][1] == KForbidden) Mat[Line][1] = KEmpty;
               for (unsigned Line (16); Line <= 19; ++Line)
                   if(Mat[Line][2] == KForbidden) Mat[Line][2] = KEmpty;
               Mat[0][3] = KTokenPlayer1;
               Mat[3][4] = KTokenPlayer2;
               Pos1 = make_pair(0,3);
               Pos2 = make_pair(3,4);
               Victory = make_pair(19, 2);
               Watch = make_pair(10, 6);
        }
        else if (LabType == 1)
        {
            for (unsigned Line (0); Line < Mat.size(); ++Line)
            {
                for (unsigned Col(0); Col<Mat[Line].size(); ++Col)
                {
                    Mat[Line][Col] = KEmpty;
                }
            }
            for (unsigned Column (1); Column <= 8; ++Column)
                if(Mat[1][Column] == KEmpty) Mat[1][Column] = KForbidden;
            for (unsigned Column (1); Column <= 3; ++Column)
                if(Mat[3][Column] == KEmpty) Mat[3][Column] = KForbidden;
            for (unsigned Column (5); Column <= 9; ++Column)
                if(Mat[3][Column] == KEmpty) Mat[3][Column] = KForbidden;
            for (unsigned Line (1); Line <= 5; ++Line)
                if(Mat[Line][1] == KEmpty) Mat[Line][1] = KForbidden;
            for (unsigned Column (1); Column <= 6; ++Column)
                if(Mat[5][Column] == KEmpty) Mat[5][Column] = KForbidden;
            for (unsigned Line (5); Line <= 18; ++Line)
                if(Mat[Line][6] == KEmpty) Mat[Line][6] = KForbidden;
            for (unsigned Column (8); Column <= 9; ++Column)
                if(Mat[5][Column] == KEmpty) Mat[5][Column] = KForbidden;
            for (unsigned Column (6); Column <= 8; ++Column)
                if(Mat[7][Column] == KEmpty) Mat[7][Column] = KForbidden;
            for (unsigned Column (8); Column <= 9; ++Column)
                if(Mat[9][Column] == KEmpty) Mat[9][Column] = KForbidden;
            for (unsigned Column (6); Column <= 8; ++Column)
                if(Mat[11][Column] == KEmpty) Mat[11][Column] = KForbidden;
            for (unsigned Column (8); Column <= 9; ++Column)
                if(Mat[13][Column] == KEmpty) Mat[13][Column] = KForbidden;
            for (unsigned Column (6); Column <= 8; ++Column)
                if(Mat[15][Column] == KEmpty) Mat[15][Column] = KForbidden;
            for (unsigned Column (8); Column <= 9; ++Column)
                if(Mat[17][Column] == KEmpty) Mat[17][Column] = KForbidden;
            for (unsigned Column (0); Column <= 4; ++Column)
                if(Mat[8][Column] == KEmpty) Mat[8][Column] = KForbidden;
            for (unsigned Column (0); Column <= 4; ++Column)
                if(Mat[11][Column] == KEmpty) Mat[11][Column] = KForbidden;
            for (unsigned Column (0); Column <= 4; ++Column)
                if(Mat[14][Column] == KEmpty) Mat[14][Column] = KForbidden;
            for (unsigned Column (0); Column <= 4; ++Column)
                if(Mat[16][Column] == KEmpty) Mat[16][Column] = KForbidden;
            for (unsigned Column (1); Column <= 6; ++Column)
                if(Mat[18][Column] == KEmpty) Mat[18][Column] = KForbidden;
            for (unsigned Line (18); Line <= 19; ++Line)
                if(Mat[Line][3] == KEmpty) Mat[Line][3] = KForbidden;
            Mat[0][3] = KTokenPlayer1;
            Mat[0][7] = KTokenPlayer2;
            Pos1 = make_pair(0,3);
            Pos2 = make_pair(0,7);
            Victory = make_pair(19, 4);
            Watch = make_pair(19, 2);
        }
        else if (LabType == 2)
        {
            for (unsigned Line (0); Line < Mat.size(); ++Line)
            {
                for (unsigned Col(0); Col<Mat[Line].size(); ++Col)
                {
                    Mat[Line][Col] = KEmpty;
                }
            }
            for (unsigned Column(0); Column <= 9;  ++Column)
                if (Mat[0][Column]  == KEmpty) Mat[0][Column]  = KForbidden;
            for (unsigned Line(0);     Line <= 1;    ++Line)
                if (Mat[Line][5]    == KEmpty) Mat[Line][5]    = KForbidden;
            for (unsigned Line(0);     Line <= 5;    ++Line)
                if (Mat[Line][9]    == KEmpty) Mat[Line][9]    = KForbidden;
            for (unsigned Column(7); Column <= 9;  ++Column)
                if (Mat[5][Column]  == KEmpty) Mat[5][Column]  = KForbidden;
            for (unsigned Column(0); Column <= 1;  ++Column)
                if (Mat[3][Column]  == KEmpty) Mat[3][Column]  = KForbidden;
            for (unsigned Line(2);     Line <= 3;    ++Line)
                if (Mat[Line][1]    == KEmpty) Mat[Line][1]    = KForbidden;
            for (unsigned Line(2);     Line <= 3;    ++Line)
                if (Mat[Line][3]    == KEmpty) Mat[Line][3]    = KForbidden;
            for (unsigned Line(2);     Line <= 3;    ++Line)
                if (Mat[Line][7]    == KEmpty) Mat[Line][7]    = KForbidden;
            for (unsigned Column(5); Column <= 7;  ++Column)
                if (Mat[3][Column]  == KEmpty) Mat[3][Column]  = KForbidden;
            for (unsigned Line(3);     Line <= 5;    ++Line)
                if (Mat[Line][5]    == KEmpty) Mat[Line][5]    = KForbidden;
            for (unsigned Column(4); Column <= 5;  ++Column)
                if (Mat[5][Column]  == KEmpty) Mat[5][Column]  = KForbidden;
            for (unsigned Column(1); Column <= 2;  ++Column)
                if (Mat[5][Column]  == KEmpty) Mat[5][Column]  = KForbidden;
            for (unsigned Line(5);     Line <= 6;    ++Line)
                if (Mat[Line][1]    == KEmpty) Mat[Line][1]    = KForbidden;
            for (unsigned Line(8);     Line <= 9;    ++Line)
                if (Mat[Line][1]    == KEmpty) Mat[Line][1]    = KForbidden;
            for (unsigned Column(3); Column <= 5;  ++Column)
                if (Mat[7][Column]  == KEmpty) Mat[7][Column]  = KForbidden;
            for (unsigned Line(7);     Line <= 9;    ++Line)
                if (Mat[Line][3]    == KEmpty) Mat[Line][3]    = KForbidden;
            for (unsigned Column(5); Column <= 5;  ++Column)
                if (Mat[9][Column]  == KEmpty) Mat[9][Column]  = KForbidden;
            for (unsigned Column(8); Column <= 9;  ++Column)
                if (Mat[9][Column]  == KEmpty) Mat[9][Column]  = KForbidden;
            for (unsigned Column(7); Column <= 8;  ++Column)
                if (Mat[7][Column]  == KEmpty) Mat[7][Column]  = KForbidden;
            for (unsigned Line(7);    Line <= 11;   ++Line)
                if (Mat[Line][7]    == KEmpty) Mat[Line][7]    = KForbidden;
            for (unsigned Column(7); Column <= 8;  ++Column)
                if (Mat[11][Column] == KEmpty) Mat[11][Column] = KForbidden;
            for (unsigned Column(2); Column <= 5;  ++Column)
                if (Mat[11][Column] == KEmpty) Mat[11][Column] = KForbidden;
            for (unsigned Line(11);   Line <= 13;   ++Line)
                if (Mat[Line][5]    == KEmpty) Mat[Line][5]    = KForbidden;
            for (unsigned Column(5); Column <= 7;  ++Column)
                if (Mat[13][Column] == KEmpty) Mat[13][Column] = KForbidden;
            for (unsigned Column(9); Column <= 9;  ++Column)
                if (Mat[13][Column] == KEmpty) Mat[13][Column] = KForbidden;
            for (unsigned Line(11);   Line <= 13;   ++Line)
                if (Mat[Line][0]    == KEmpty) Mat[Line][0]    = KForbidden;
            for (unsigned Column(1); Column <= 3;  ++Column)
                if (Mat[13][Column] == KEmpty) Mat[13][Column] = KForbidden;
            for (unsigned Line(13);   Line <= 15;   ++Line)
                if (Mat[Line][1]    == KEmpty) Mat[Line][1]    = KForbidden;
            for (unsigned Column(3); Column <= 6;  ++Column)
                if (Mat[15][Column] == KEmpty) Mat[15][Column] = KForbidden;
            for (unsigned Column(1); Column <= 2;  ++Column)
                if (Mat[17][Column] == KEmpty) Mat[17][Column] = KForbidden;
            for (unsigned Line(15);   Line <= 17;   ++Line)
                if (Mat[Line][8]    == KEmpty) Mat[Line][8]    = KForbidden;
            for (unsigned Column(6); Column <= 9;  ++Column)
                if (Mat[17][Column] == KEmpty) Mat[17][Column] = KForbidden;
            for (unsigned Line(17);   Line <= 19;   ++Line)
                if (Mat[Line][9]    == KEmpty) Mat[Line][9]    = KForbidden;
            for (unsigned Line(17);   Line <= 19;   ++Line)
                if (Mat[Line][4]    == KEmpty) Mat[Line][4]    = KForbidden;
            for (unsigned Column(0); Column <= 9;  ++Column)
                if (Mat[19][Column] == KEmpty) Mat[19][Column] = KForbidden;
            Mat[2][0] = KTokenPlayer1;
            Mat[4][3] = KTokenPlayer2;
            Pos1 = make_pair(2,0);
            Pos2 = make_pair(4,3);
            Victory = make_pair(18,2);
            Watch   = make_pair(8,8);
    }
    }//InitLabyrinthe

    int Labyrinthe ()
    {

        CMatrix Mat;
        unsigned MatSizeH =20;
        unsigned MatSizeL =10;
        CPosition Pos1 (4,4);
        CPosition Pos2 (19,2);
        CPosition PosI (0,0);
        CPosition PosA (MatSizeH -1,MatSizeL -1);
        unsigned NbTurn(1);
        unsigned Diff(4);
        unsigned Sec(10);

        char MoveKey('s');
        bool AllowedMove(true);
        bool TurnPass(false);
        bool GameOver(false);

        InitMat(Mat, MatSizeH, MatSizeL, Pos1, Pos2, PosI, PosA);

        set_input_mode();

        CPosition Victory;
        CPosition Watch;
        InitLabyrinthe(Mat,Pos1, Pos2, Victory, Watch);
        FogOfWar(Mat, Pos2);
        SetCasesLab(Mat, Victory, Watch);

        while(!(GameOver))
                {

                    ShowMatrix(Mat);
                    Invite(NbTurn);
                    AffichTimer(Sec);
                    sleep(1);
                    boost::thread ThInput{boost::bind(Input, boost::ref(MoveKey))};
                    TurnPass = CptTurn(NbTurn, Sec, MoveKey);

                    if(!(TurnPass)) continue;

                    CPosition NextPos = make_pair (                                 // La position où le joueur souhaite se rendre est définie par une pair qui :
                                (NbTurn%2 == 0 ? (Pos2.first) : (Pos1.first)) +     // Prend en premier membre la position actuelle du joueur en vertical +
                                (PossibleMove[NbKey(MoveKey)]).first,               // Le déplacement vertical (.first) du mouvement (PossibleMove) selon la touche choisie (MoveKey)
                                (NbTurn%2 == 0 ? (Pos2.second) : (Pos1.second)) +   // Prend en second membre la position actuelle du joueur en horizontal +
                                (PossibleMove[NbKey(MoveKey)]).second);             // Le déplacement horizontal (.second) du mouvement (PossibleMove) selon la touche choisie (MoveKey)

                    AllowedMove = IsAllowed(Mat, NextPos);
                    MoveToken(Mat, MoveKey, NbTurn%2 == 0 ? Pos2 : Pos1);
                    FogOfWar(Mat, NbTurn%2 == 0 ? Pos1 : Pos2);
                    SetCasesLab(Mat, Victory, Watch);
                    UnFog(Mat, Watch);
                    GameOver = WinTestLab(AllowedMove, Pos1, Pos2, Victory, NbTurn);

                }
        reset_input_mode();
        Sec = 10;

        cout << "Partie terminee" << endl
             << "Vous serez redirigé dans le Menu dans " << Sec <<  " secondes." << endl;
        cout << endl;
        for(; Sec > 0; Sec--)
        {
            cout << endl << Sec << " ";
            sleep(1);
        }
        Menu();
        return 0;
    } //Labyrinthe

    /*
     *
     *
     * ------------------------------------------------------------------------ FONCTION DU MENU ---------------------------------------------------------------------
     *
     *
     */

    void Menu()
    {
        unsigned Choix;
        do
        {
        ClearScreen();

            cout << "-----------------MENU-----------------" << "\n\n\n";
            cout << "1 - Mode \" Mange moi ! \"" << endl;
            cout << "2 - Mode Labyrinthe" << "\n\n\n";
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
                    cout << "coucou";
                    Labyrinthe();
                    break;
                default:
                    //FaireMode
                    break;
            }
        }while(Choix <1 || Choix > 2);
    }

}

int main()
{
    Menu();
    return 0;
}
