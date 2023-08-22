#include <windows.h>
#include <CommCtrl.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include "src_functions.h"
#include "src_stringutils.h"
#include "src_clipboardutils.h"
#include "src_windowsutils.h"
#include "src_pythonutils.h"
#include "src_filesutils.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

#define MAINICON 1

const wchar_t CLASS_NAME[] = L"Window Class";
HBRUSH hBrush = NULL;

// Définition des ID pour les contrôles
#define ID_STATUSBAR           1000
#define ID_OPENFILE_BUTTON     1001
#define ID_GROUPBOX_DESC       1002
#define ID_LABEL_DESC          1003
#define ID_GROUPBOX_CSV        1004
#define ID_CHECKBOX_TITLECSV   1005
#define ID_LABEL_NUMCOLUMN     1006
#define ID_EDIT_NUMCOLUMN      1007
#define ID_UPDOWN_NUMCOLUMN    1008
#define ID_GROUPBOX_DELIM      1010
#define ID_RADIO_DELIM1        1011
#define ID_RADIO_DELIM2        1012
#define ID_RADIO_DELIM3        1013
#define ID_RADIO_DELIM4        1014
#define ID_RADIO_DELIM5        1015
#define ID_RADIO_CUSTOMDELIM   1016
#define ID_EDIT_CUSTOMDELIM    1017
#define ID_GROUPBOX_FILE       1021

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * Affiche une boîte de dialogue avec une question et retourne la réponse de l'utilisateur.
 *
 * @param message Le message ou la question à afficher à l'utilisateur.
 * @return Retourne un entier représentant le bouton sur lequel l'utilisateur a cliqué.
 *         Les valeurs possibles peuvent être : IDYES, IDNO, IDCANCEL, etc.
 */
int ShowQuestion(const std::wstring& message) {
    // Affiche une boîte de dialogue avec le message donné, un titre "Question" et des boutons "Oui", "Non" et "Annuler"

    int response = MessageBoxW(NULL,
                               message.c_str(),
                               L"Question",
                               MB_YESNOCANCEL | MB_ICONQUESTION);

    // Retourne la réponse de l'utilisateur
    logger->debug("Réponse : {}", response);
    return response;
}


/**
 * Affiche une boîte de dialogue indiquant une erreur à l'utilisateur.
 *
 * @param message Le message d'erreur à afficher à l'utilisateur.
 */
void ShowError(const std::wstring& message) {
    // Affiche une boîte de dialogue avec le message d'erreur donné, un titre "Error" et un bouton "OK"
    // L'icône de la boîte de dialogue est celle d'une erreur.
    MessageBox(NULL, message.c_str(), L"Error", MB_OK | MB_ICONERROR);
}


/**
 * Récupère la version du produit de l'exécutable courant.
 *
 * @return Une chaîne de caractères formatée contenant la version du produit.
 *         Retourne une chaîne vide si la version ne peut pas être récupérée.
 */
std::wstring GetProductVersion() {
    // Récupère le chemin de l'exécutable courant
    wchar_t path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);

    DWORD dummy;
    // Récupère la taille des informations de version pour l'exécutable
    DWORD size = GetFileVersionInfoSize(path, &dummy);

    // Si la taille est 0, cela signifie qu'il n'y a pas d'informations de version
    if (size == 0) return L"";

    // Crée un vecteur pour stocker les données de version
    std::vector<BYTE> data(size);
    // Tente de récupérer les informations de version de l'exécutable
    if (!GetFileVersionInfo(path, 0, size, &data[0])) return L"";

    VS_FIXEDFILEINFO* vInfo = nullptr;
    UINT len;
    // Tente de récupérer les informations de version fixes de l'exécutable
    if (!VerQueryValue(&data[0], L"\\", (void**) &vInfo, &len)) return L"";

    // Si aucune information de version fixe n'est trouvée, retourne une chaîne vide
    if (!vInfo) return L"";

    // Formate et retourne la version du produit comme une chaîne de caractères
    return std::to_wstring(HIWORD(vInfo->dwProductVersionMS)) + L"." +
           std::to_wstring(LOWORD(vInfo->dwProductVersionMS)) + L"." +
           std::to_wstring(HIWORD(vInfo->dwProductVersionLS)) + L"." +
           std::to_wstring(LOWORD(vInfo->dwProductVersionLS));
}

/**
 * Crée un groupe de contrôles pour le choix d'un délimiteur CSV dans une fenêtre parente.
 *
 * @param hInstance Instance de l'application.
 * @param hwnd Fenêtre parente où seront placés les contrôles.
 */
void CreateDelimiteur(HINSTANCE hInstance, HWND hwnd) {
    // Création d'une boîte de groupe (group box) intitulée "Etape 2 : Choix du Délimiteur CSV"
    logger->info("GUI : Création de la groupbox : Etape 2 Choix du Délimiteur CSV");
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Etape 2 : Choix du Délimiteur CSV",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 195, 500, 80,  // x, y, largeur, hauteur
            hwnd,
            (HMENU) ID_GROUPBOX_DELIM,
            hInstance,
            NULL
    );

    // Création des boutons radio pour les choix de délimiteurs

    // Bouton radio "Point virgule"
    HWND hRadioButton1 = CreateWindowEx(
            0,
            L"BUTTON",
            L"Point virgule",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
            20, 220, 100, 20,
            hwnd,
            (HMENU) ID_RADIO_DELIM1,
            hInstance,
            NULL
    );

    // Bouton radio "Virgule"
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Virgule",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            180, 220, 100, 20,
            hwnd,
            (HMENU) ID_RADIO_DELIM2,
            hInstance,
            NULL
    );

    // Bouton radio "Tabulation"
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Tabulation",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            350, 220, 100, 20,
            hwnd,
            (HMENU) ID_RADIO_DELIM3,
            hInstance,
            NULL
    );

    // Bouton radio "Espace"
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Espace",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            20, 240, 100, 20,
            hwnd,
            (HMENU) ID_RADIO_DELIM4,
            hInstance,
            NULL
    );

    // Bouton radio "Barre verticale |"
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Barre verticale |",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            180, 240, 130, 20,
            hwnd,
            (HMENU) ID_RADIO_DELIM5,
            hInstance,
            NULL
    );

    // Bouton radio "Autre :"
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Autre :",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            350, 240, 60, 20,  // x, y, largeur, hauteur
            hwnd,
            (HMENU) ID_RADIO_CUSTOMDELIM,
            hInstance,
            NULL
    );

    // Création d'un champ de saisie (edit control) pour le délimiteur personnalisé
    HWND hEditForRadioButton = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",  // Initialement vide
            WS_CHILD | WS_VISIBLE | ES_LEFT,
            420, 240, 50, 24,  // x, y, largeur, hauteur
            hwnd,
            (HMENU) ID_EDIT_CUSTOMDELIM,
            hInstance,
            NULL
    );

    // Désactivation du champ de saisie pour le délimiteur personnalisé
    EnableWindow(hEditForRadioButton, FALSE);
    // Sélection du bouton radio "Point virgule" par défaut
    SendMessage(hRadioButton1, BM_SETCHECK, BST_CHECKED, 0);
    // Limitation du champ de saisie à 5 caractères
    SendMessage(hEditForRadioButton, EM_LIMITTEXT, 5, 0);
    // Affectation de la valeur ";" au champ de saisie
    SetWindowText(hEditForRadioButton, L";");
}

/**
 * Crée un groupe de contrôles pour la configuration d'un fichier CSV dans une fenêtre parente.
 *
 * @param hInstance Instance de l'application.
 * @param hwnd Fenêtre parente où seront placés les contrôles.
 */
void CreateCSVConfig(HINSTANCE hInstance, HWND hwnd) {
    // Création d'une boîte de groupe (group box) intitulée "Etape 1 : fichier CSV"
    logger->info("GUI : Création de la groupbox : Etape 1 Fichier CSV");
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Etape 1 : fichier CSV",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 110, 500, 80,  // Ajuster la position et la taille si nécessaire
            hwnd,
            (HMENU) ID_GROUPBOX_CSV,
            hInstance,
            NULL
    );

    // Création d'une case à cocher (checkbox) pour indiquer si le CSV contient une ligne de titre
    HWND hTitleCsvCheckbox = CreateWindowEx(
            0,
            L"BUTTON",
            L"Il existe une ligne donnant le titre des colonnes dans le fichier CSV.",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            20, 135, 480, 20,
            hwnd,
            (HMENU) ID_CHECKBOX_TITLECSV,
            hInstance,
            NULL
    );

    // Cocher la case par défaut
    SendMessage(hTitleCsvCheckbox, BM_SETCHECK, BST_CHECKED, 0);

    // Création d'une étiquette (label) et d'un champ de saisie (edit control) pour le nombre de colonnes
    CreateWindowEx(
            0,
            L"STATIC",
            L"Nombre de colonnes (descripteurs), dans le fichier CSV :",
            WS_CHILD | WS_VISIBLE,
            20, 160, 400, 20,
            hwnd,
            (HMENU) ID_LABEL_NUMCOLUMN,
            hInstance,
            NULL
    );

    HWND hNumColumnEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"1",
            WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_RIGHT,
            400, 155, 50, 24,
            hwnd,
            (HMENU) ID_EDIT_NUMCOLUMN,
            hInstance,
            NULL
    );

    // Création d'un contrôle de type "spin control" (ou "up-down control") associé au champ de saisie du nombre de colonnes
    HWND hUpDownControl = CreateWindowEx(
            0,
            UPDOWN_CLASS,
            NULL,
            WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_NOTHOUSANDS | UDS_ALIGNRIGHT,
            0, 0, 0, 0,   // Ces valeurs peuvent être mises à 0 car UDS_SETBUDDYINT ajustera automatiquement la taille
            hwnd,
            (HMENU) ID_UPDOWN_NUMCOLUMN,
            hInstance,
            NULL
    );

    // Associer le champ de saisie au contrôle "up-down"
    SendMessage(hUpDownControl, UDM_SETBUDDY, (WPARAM) hNumColumnEdit, 0);
    // Définir la plage de valeurs pour le contrôle "up-down"
    SendMessage(hUpDownControl, UDM_SETRANGE, 0, MAKELPARAM(999, 1));  // Max: 999, Min: 1
    // Définir la position initiale du contrôle "up-down" à 3
    SendMessage(hUpDownControl, UDM_SETPOS, 0, MAKELPARAM(3, 0));
}

/**
 * Crée un groupe de contrôles pour afficher une description dans une fenêtre parente.
 *
 * @param hInstance Instance de l'application.
 * @param hwnd Fenêtre parente où seront placés les contrôles.
 */
void CreateDescription(HINSTANCE hInstance, HWND hwnd) {
    logger->info("GUI : Création de la groupbox : Description");
    // Création d'une boîte de groupe (group box) intitulée "Description :"
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Description :",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 10, 500, 94,  // x, y, largeur, hauteur
            hwnd,
            (HMENU) ID_GROUPBOX_DESC,
            hInstance,
            NULL
    );

    // Création d'un contrôle statique (label) multiligne à l'intérieur de la boîte de groupe pour afficher une description
    CreateWindowEx(
            0,
            L"STATIC",
            L"- Permet de convertir les colonnes d'un fichier CSV en listes pour Python ;\r\n"
            L"- Pour l'export : choisir un séparateur décimal différent de celui du CSV ;\r\n"
            L"- Suivre les étapes puis ouvrir le fichier désiré ;\r\n"
            L"- Les listes en Python seront stockées dans le presse papier.",
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
            20, 30, 480, 65,
            hwnd,
            (HMENU) ID_LABEL_DESC,
            hInstance,
            NULL
    );
}

/**
 * Crée une barre de statut (status bar) dans une fenêtre parente.
 *
 * @param hInstance Instance de l'application.
 * @param hwnd Fenêtre parente où la barre de statut sera placée.
 */
void CreateStatusBar(HINSTANCE hInstance, HWND hwnd) {
    logger->info("GUI : Création de la barre de statut");
    // Construit le texte à afficher dans la barre de statut.
    // Il contient le copyright, le nom de l'auteur, l'année, ainsi que la version du produit.
    std::wstring Informations = L"Copyright Rémi MEVAERE - 2023\u00A9 - " + GetProductVersion();
    logger->info("Version du logiciel : " + wstring_to_string(GetProductVersion()));
    // Création de la barre de statut
    CreateWindowEx(
            0,                            // Styles étendus (aucun spécifié ici)
            STATUSCLASSNAME,              // Nom de la classe de la barre de statut
            Informations.c_str(),         // Texte à afficher dans la barre
            WS_CHILD | WS_VISIBLE,        // Styles de fenêtre : enfant et visible
            0, 0, 0,
            0,                   // x, y, largeur, hauteur (les dimensions sont gérées automatiquement par la barre de statut)
            hwnd,                         // Fenêtre parente
            (HMENU) ID_STATUSBAR,         // Identifiant de la barre de statut
            hInstance,                    // Instance de l'application
            NULL                          // Pointeur vers les données de création de la fenêtre (non utilisé ici)
    );
}

/**
 * Modifie l'état et le texte du contrôle d'édition (edit control) pour le délimiteur personnalisé.
 *
 * @param hwnd Fenêtre parente contenant le contrôle d'édition.
 * @param valeur Valeur à définir dans le contrôle d'édition (par défaut ";").
 */
void modEditCustomDelim(HWND hwnd, std::wstring valeur = L";") {
    logger->info("modEditCustomDelim called");
    // Désactive le contrôle d'édition pour le délimiteur personnalisé.
    EnableWindow(GetDlgItem(hwnd, ID_EDIT_CUSTOMDELIM), FALSE);

    // Définit le texte du contrôle d'édition pour le délimiteur personnalisé.
    SetWindowText(GetDlgItem(hwnd, ID_EDIT_CUSTOMDELIM), valeur.c_str());

    // Convertit la valeur (délimiteur) en chaîne de caractères standard et stocke dans la variable 'separateur'.
    separateur = wstring_to_string(valeur);
    logger->debug("Séparateur : {}", separateur);
}

/**
 * Crée un groupe et un bouton pour permettre à l'utilisateur de choisir un fichier.
 *
 * @param hInstance Instance du programme courant.
 * @param hwnd Fenêtre parente où le groupe et le bouton seront ajoutés.
 */
void CreateBouton(HINSTANCE hInstance, HWND hwnd) {
    // Crée un groupe intitulé "Etape 3 : Choisir un fichier".
    logger->info("GUI : Création de la groupbox : Etape 3 Choisir un fichier");
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Etape 3 : Choisir un fichier",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 280, 500, 80,  // x, y, largeur, hauteur - ajuster la position si nécessaire
            hwnd,
            (HMENU) ID_GROUPBOX_FILE,
            hInstance,
            NULL
    );

    // Crée un bouton avec le texte "Ouvrir le fichier CSV et convertir en listes pour python."
    CreateWindowEx(
            0,
            L"BUTTON",
            L"Ouvrir le fichier CSV et convertir en listes pour python.",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 300, 480, 50,  // x, y, largeur, hauteur
            hwnd,
            (HMENU) ID_OPENFILE_BUTTON,  // Assignation d'un ID unique à ce bouton
            hInstance,
            NULL
    );
}

/**
 * Point d'entrée principal de l'application Windows.
 *
 * @param hInstance     Instance actuelle de l'application.
 * @param hPrevInstance Toujours NULL (non utilisé).
 * @param pCmdLine      Ligne de commande en tant que string large.
 * @param nCmdShow      Comment afficher la fenêtre au démarrage.
 *
 * @return Code de sortie de l'application.
 */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {

    try {
        int argc;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argv) {
            if ((argc > 1) && (wcscmp(argv[1], L"debug"))) {
                // Initialisation du système de journalisation
                logger = spdlog::rotating_logger_mt("basic_logger",
                                                    getDocumentsPath() +
                                                    "csv2listes_log.txt", 1048576 * 5, 3);

                // Configuration du niveau et du format du journal
                logger->set_level(spdlog::level::debug);
                logger->set_pattern("%d/%m/%Y %H:%M:%F %z\t%^%-l%$\t%P\t%t\t%v");
                spdlog::flush_every(std::chrono::seconds(3));

                logger->info("MAX_SIZE_LINE : {}", MAX_SIZE_LINE);
                logger->info("MAX_SIZE_FILE : {}", MAX_SIZE_FILE);
                logger->info("MIN_LINE_COUNT : {}", MIN_LINE_COUNT);
                logger->info("MAX_LINE_COUNT : {}", MAX_LINE_COUNT);

            } else {
                logger = spdlog::null_logger_mt("null_logger");
            }
            LocalFree(argv);
        }

    }
    catch (const std::exception& e) {
        // Erreur non bloquante pour le programme
    }

    try {

        // Écriture de quelques messages de test
        logger->info("Démarrage du logiciel!");

        // Récupération et journalisation des arguments de la ligne de commande
        int argc;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argv) {
            for (int i = 0; i < argc; i++) {
                std::wstring warg(argv[i]);
                logger->info("Arguments {}: {}", i, wstring_to_string(warg));

            }
            LocalFree(argv);
        }
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }

    // Initialisation de la fenêtre principale
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    logger->info("GUI : Création de la fenêtre principale");
    HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Convertisseur : fichier CSV -> listes en Python",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT, 535, 430,
            NULL,
            NULL,
            hInstance,
            NULL
    );
    if (hwnd == NULL) {
        logger->error("GUI : Impossible de créer la fenêtre");
        return 0;
    }

    logger->info("GUI : Définition de l'îcone pour la fenêtre");
    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAINICON));
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) hIcon);

    CreateStatusBar(hInstance, hwnd);
    CreateDescription(hInstance, hwnd);
    CreateCSVConfig(hInstance, hwnd);
    CreateDelimiteur(hInstance, hwnd);
    CreateBouton(hInstance, hwnd);

    logger->info("GUI : Affichage de la fenêtre");
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    ShowWindow(hwnd, nCmdShow);

    // Boucle principale pour gérer les messages de la fenêtre
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


/**
 * Extraire les données d'un fichier CSV, les traiter et les convertir en listes Python.
 */
void ExtractData() {

    // Étape 1 : Vérifier si le fichier est accessible et lisible.
    if (fileExistsAndReadable(filenameCSV)) {

        // Étape 2 : Vérifier la taille du fichier pour s'assurer qu'elle est gérable.
        double filesizeCSV = getFileSize(filenameCSV);
        if (filesizeCSV == -1) {
            // Erreur de détection de la taille du fichier
            logger->error("Erreur : Impossible de détecter la taille du fichier.");
            ShowError(L"Impossible de détecter la taille du fichier.");
        } else if (filesizeCSV > MAX_SIZE_FILE) {
            // Le fichier est trop grand pour le traitement
            logger->error("Erreur : La taille du fichier est trop importante pour ce programme pédagogique.");
            ShowError(L"La taille du fichier est trop importante pour ce programme pédagogique.");
        } else {

            // Étape 3 : Valider que le fichier est bien un fichier texte.
            if (isFileText(filenameCSV)) {

                // Étape 4 : Vérifier si le fichier a un nombre minimal de lignes.
                if (notTooLessLines(filenameCSV)) {

                    // Étape 5 : Vérifier si le fichier n'a pas trop de lignes.
                    if (isNotTooManyLines(filenameCSV)) {


                        // Étape 6 : Charger le fichier en mémoire pour un traitement plus rapide.
                        if (loadIntoMemStream(filenameCSV)) {
                            // Étape 7 : Vérification qu'il n'y a pas une trop longue ligne
                            if (checkMaxSizeLine()) {
                                // Initialisation de variables locales
                                bool pasAnnule = true;
                                bool donneesTrouvees = false;
                                bool headerTrouves = false;
                                std::string ligne = "";
                                nbLigneDonnees = 0;
                                nbLigneEntete = 0 ;
                                sortie.str("");
                                sortie.clear();
                                wsortie.str(L"");
                                wsortie.clear();
                                nomsColonnes.clear();
                                donnees.clear();
                                std::vector<std::string> donneesAAjouter;

                                // Étape 8 : Traitement des entêtes
                                // Si le fichier contient des en-têtes, nous devons les détecter et les traiter.
                                if (hasHeader && pasAnnule) {
                                    // Tant que les en-têtes ne sont pas trouvés et que l'utilisateur n'a pas annulé
                                    while (!(headerTrouves) && pasAnnule) {

                                        // Liser la ligne suivante du flux mémoire
                                        if (readNextLineMemStream(ligne)) {
                                            std::wstring message;
                                            // Poser une question à l'utilisateur pour savoir si la ligne actuelle est une ligne d'en-tête
                                            message = L"Est-ce que la ligne n°" + std::to_wstring(memStreamLine) +
                                                      L" correspond aux entêtes :\n \"" + string_to_wstring(ligne) +
                                                      L"\"";
                                            std::string message_ansi;
                                            message_ansi = "Est-ce que la ligne n°" + std::to_string(memStreamLine) +
                                                           " correspond aux entêtes :\n \"" + ligne + "\"";
                                            logger->info("Question : " + message_ansi);
                                            switch (ShowQuestion(message)) {
                                                case IDYES:
                                                    // Si l'utilisateur confirme que c'est une ligne d'en-tête
                                                    headerTrouves = true;
                                                    nbLigneEntete = memStreamLine;
                                                    logger->debug("Entêtes selectionnés pour la ligne {}", nbLigneEntete);
                                                    // Séparer la ligne pour obtenir les noms des colonnes
                                                    nomsColonnes = split(ligne, separateur);
                                                    // Vérifier si le nombre de colonnes correspond à ce qui est attendu
                                                    if (nomsColonnes.size() != nbColonnes) {
                                                        pasAnnule = false;
                                                        std::wstring message_erreur;
                                                        message_erreur =
                                                                L"Le nombre de colonnes ne correspond pas ! \nColonnes comptées : " +
                                                                std::to_wstring(nomsColonnes.size()) +
                                                                L" \nColonnes indiquées : " +
                                                                std::to_wstring(nbColonnes);
                                                        std::string message_erreur_ansi;
                                                        message_erreur_ansi =
                                                                "Le nombre de colonnes ne correspond pas ! \nColonnes comptées : " +
                                                                std::to_string(nomsColonnes.size()) +
                                                                " \nColonnes indiquées : " +
                                                                std::to_string(nbColonnes);
                                                        logger->error("Erreur : " + message_erreur_ansi);
                                                        ShowError(message_erreur);
                                                    } else transformToPythonVar(nomsColonnes);
                                                    break;

                                                case IDCANCEL:
                                                    // Si l'utilisateur annule la sélection des en-têtes
                                                    pasAnnule = false;
                                                    break;
                                                default:
                                                    break;
                                            }
                                        } else {
                                            // Si la fin du fichier est atteinte sans trouver d'en-tête
                                            logger->error("Erreur : Fin du fichier, aucun entête choisi.");
                                            ShowError(L"Fin du fichier, aucun entête choisi.");
                                        }
                                    }

                                } else {
                                    // Si le fichier n'a pas d'en-têtes, donner un nom par défaut aux colonnes
                                    for (int i = 1; i <= nbColonnes; i = i + 1) {
                                        nomsColonnes.push_back("Liste_" + std::to_string(i));
                                    };
                                }


                                // Étape 9 : Traitement des données
                                // On commence par lire et traiter chaque ligne de données du fichier.
                                if (pasAnnule) {
                                    // Tant qu'il reste des lignes dans le flux mémoire et que l'utilisateur n'a pas annulé
                                    while (readNextLineMemStream(ligne) && pasAnnule) {
                                        // Remplacer chaque virgule par un point (pour gérer les délimiteurs décimaux)
                                        ligne = replaceAllChars(ligne, ',', '.');

                                        // Si les données n'ont pas encore été trouvées
                                        if (!donneesTrouvees) {
                                            std::wstring message;
                                            // Poser une question à l'utilisateur pour savoir si la ligne actuelle est la première ligne de données
                                            message = L"Est-ce que la ligne n°" + std::to_wstring(memStreamLine) +
                                                      L" correspond à la première ligne de données :\n \"" +
                                                      string_to_wstring(ligne) + L"\"";
                                            std::string message_ansi;
                                            message_ansi = "Est-ce que la ligne n°" + std::to_string(memStreamLine) +
                                                           " correspond à la première ligne de données :\n \"" +
                                                           ligne + "\"";
                                            logger->info("Question : " + message_ansi);
                                            switch (ShowQuestion(message)) {
                                                case IDYES:
                                                    donneesTrouvees = true;
                                                    nbLigneDonnees = memStreamLine;
                                                    logger->debug("Première ligne de données sélectionnée pour la ligne {}", nbLigneDonnees);
                                                    donneesAAjouter.clear();
                                                    // Découper la ligne en utilisant le délimiteur spécifié
                                                    donneesAAjouter = split(ligne, separateur);
                                                    // Vérifier si le nombre de colonnes correspond à ce qui est attendu
                                                    if (donneesAAjouter.size() != nbColonnes) {
                                                        pasAnnule = false;
                                                        std::wstring message_erreur;
                                                        message_erreur =
                                                                L"Ligne " + std::to_wstring(memStreamLine) + L" : " +
                                                                string_to_wstring(ligne) + L"\n" +
                                                                L"Le nombre de colonnes ne correspond pas ! \nColonnes comptées : " +
                                                                std::to_wstring(donneesAAjouter.size()) +
                                                                L" \nColonnes indiquées : " +
                                                                std::to_wstring(nbColonnes);

                                                        std::string message_erreur_ansi;
                                                        message_erreur_ansi =
                                                                "Ligne " + std::to_string(memStreamLine) + " : " +
                                                                ligne + "\n" +
                                                                "Le nombre de colonnes ne correspond pas ! \nColonnes comptées : " +
                                                                std::to_string(donneesAAjouter.size()) +
                                                                " \nColonnes indiquées : " +
                                                                std::to_string(nbColonnes);
                                                        logger->error("Erreur : " + message_erreur_ansi);

                                                        ShowError(message_erreur);
                                                    } else {
                                                        // Transformer les données pour les rendre compatibles avec Python
                                                        transformThePythonList(donneesAAjouter);
                                                        // Ajouter les données transformées à la liste principale des données
                                                        donnees.push_back(donneesAAjouter);
                                                    }
                                                    break;
                                                case IDCANCEL:
                                                    // Si l'utilisateur annule la sélection des données
                                                    pasAnnule = false;
                                                    break;
                                                default:
                                                    break;
                                            }
                                        } else {
                                            donneesAAjouter.clear();
                                            // Découper la ligne en utilisant le délimiteur spécifié
                                            donneesAAjouter = split(ligne, separateur);
                                            // Vérifier si le nombre de colonnes correspond à ce qui est attendu
                                            if (donneesAAjouter.size() != nbColonnes) {
                                                pasAnnule = false;
                                                std::wstring message_erreur;
                                                message_erreur =
                                                        L"Ligne " + std::to_wstring(memStreamLine) + L" : " +
                                                        string_to_wstring(ligne) + L"\n" +
                                                        L"Le nombre de colonnes ne correspond pas ! \nColonnes comptées : " +
                                                        std::to_wstring(donneesAAjouter.size()) +
                                                        L" \nColonnes indiquées : " + std::to_wstring(nbColonnes);

                                                std::string message_erreur_ansi;
                                                message_erreur_ansi =
                                                        "Ligne " + std::to_string(memStreamLine) + " : " +
                                                        ligne + "\n" +
                                                        "Le nombre de colonnes ne correspond pas ! \nColonnes comptées : " +
                                                        std::to_string(donneesAAjouter.size()) +
                                                        " \nColonnes indiquées : " + std::to_string(nbColonnes);
                                                logger->error("Erreur : " + message_erreur_ansi);
                                                ShowError(message_erreur);
                                            } else {
                                                // Transformer les données pour les rendre compatibles avec Python
                                                transformThePythonList(donneesAAjouter);
                                                // Ajouter les données transformées à la liste principale des données
                                                donnees.push_back(donneesAAjouter);

                                            }
                                        }
                                    }


                                    // Étape 10 : Préparation des listes
                                    // Cette étape consiste à convertir les données préalablement lues et traitées en un format de liste Python.
                                    try {
                                        // Parcourir chaque colonne
                                        logger->info("Préparation des listes. Nbcolonnes {}. Taille {}", nbColonnes,
                                                     donnees.size());
                                        for (int j = 0; j < nbColonnes; j++) {
                                            // Commencer à construire la liste Python avec le nom de la colonne suivi de " = ["
                                            std::string uneliste = nomsColonnes[j] + " = [";

                                            // Parcourir chaque ligne de données pour cette colonne
                                            for (int i = 0; i < donnees.size(); i++) {
                                                // Ajouter la donnée à la liste
                                                uneliste += donnees.at(i).at(j);

                                                // Si ce n'est pas la dernière donnée, ajouter une virgule pour séparer les éléments
                                                if (i < donnees.size() - 1) uneliste += ", ";
                                            }

                                            // Fermer la liste
                                            uneliste += "]";
                                            // Transformer la liste au format PEP8 (norme de style Python) et l'ajouter à la sortie
                                            sortie << transformToPEP8(uneliste) << std::endl;
                                        }

                                    } catch (const std::exception& e) {
                                        // En cas d'erreur lors de la transformation des données, afficher un message d'erreur
                                        pasAnnule = false;
                                        logger->error("Erreur : Impossible de traiter les données : {}\n", e.what());
                                        ShowError(L"Impossible de traiter les données : \n" +
                                                  string_to_wstring(e.what()));
                                    }


                                    // Étape 11 : Copier les listes Python dans le presse-papiers
                                    // Si le traitement précédent n'a pas été annulé
                                    if (pasAnnule) {
                                        // Convertir la sortie (au format UTF-8) en format large (UTF-16 ou wchar_t)
                                        ConvertUTF8StreamToWideStream(sortie, wsortie);

                                        // Tenter de copier le contenu de la sortie (listes Python) dans le presse-papiers
                                        if (PasteClipboardFromStreamUnicode(wsortie)) {
                                            // Si la copie est réussie, informer l'utilisateur via une boîte de message
                                            std::wstring message;
                                            message = L"Le programme a copié dans le presse papiers :\n" +
                                                      std::to_wstring(nbColonnes) + L" colonnes de " +
                                                      std::to_wstring(donnees.size()) + L" valeurs. ";
                                            MessageBox(NULL, message.c_str(), L"Error", MB_OK | MB_ICONINFORMATION);
                                        } else {
                                            // Si la copie a échoué, afficher un message d'erreur
                                            logger->error("Impossible de copier les listes dans le presse-papiers.");
                                            ShowError(L"Impossible de copier les listes dans le presse-papiers.");
                                        }
                                    }

                                }
                            } else {
                                // Erreur il y a une ligne trop longue
                                logger->error("Une ligne est plus longue que prévue par ce programme pédagogique.");
                                ShowError(L"Une ligne est plus longue que prévue par ce programme pédagogique.");
                            }
                        } else {
                            // Erreur lors du chargement du fichier en mémoire
                            logger->error("Impossible de charger en mémoire le fichier.");
                            ShowError(L"Impossible de charger en mémoire le fichier.");
                        }
                    } else {
                        // Le fichier a trop de lignes pour être traité
                        logger->error("Le fichier texte contient trop de lignes pour ce programme pédagogique.");
                        ShowError(L"Le fichier texte contient trop de lignes pour ce programme pédagogique.");
                    }
                } else {
                    // Le fichier n'a pas asser de lignes pour être traité
                    logger->error("Le fichier texte ne contient pas asser de ligne.");
                    ShowError(L"Le fichier texte ne contient pas asser de ligne.");
                }
            } else {
                // Le fichier n'est pas un fichier texte valide
                logger->error("Le fichier n'est pas un fichier texte.");
                ShowError(L"Le fichier n'est pas un fichier texte.");
            }
        }
    } else {
        // Erreur d'accès au fichier
        logger->error("Le fichier n'est pas accessible ou n'est pas lisible.");
        ShowError(L"Le fichier n'est pas accessible ou n'est pas lisible.");
    }
}


/**
 * Fonction de rappel pour gérer les messages Windows envoyés à la fenêtre principale.
 * @param hwnd : Handle de la fenêtre qui reçoit le message.
 * @param uMsg : Le message Windows reçu.
 * @param wParam : Paramètre supplémentaire pour le message.
 * @param lParam : Autre paramètre supplémentaire pour le message.
 * @return : Résultat du traitement du message.
 */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Traitement des différents messages Windows
    switch (uMsg) {
        // Message reçu lorsqu'une action se produit sur un contrôle ou un menu
        case WM_COMMAND: {
            // Si un bouton a été cliqué
            if (HIWORD(wParam) == BN_CLICKED) {
                logger->info("GUI : Un bouton a été cliqué par l'utilisateur.");
                // Traitement spécifique en fonction de l'identifiant du bouton
                switch (LOWORD(wParam)) {
                    case ID_CHECKBOX_TITLECSV:
                        logger->debug("GUI : ID_CHECKBOX_TITLECSV a été cliqué.");
                        // Gestion de la case à cocher pour les entêtes
                        if (SendMessage(GetDlgItem(hwnd, ID_CHECKBOX_TITLECSV), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                            hasHeader = true;
                            logger->debug("hasHeader = true");
                        } else {
                            hasHeader = false;
                            logger->debug("hasHeader = false");
                        }
                        break;

                    case ID_RADIO_CUSTOMDELIM:
                        logger->debug("GUI : ID_RADIO_CUSTOMDELIM a été cliqué.");
                        EnableWindow(GetDlgItem(hwnd, ID_EDIT_CUSTOMDELIM), TRUE);
                        break;

                        // ... autres cas pour les boutons radios ...
                    case ID_RADIO_DELIM1:
                        modEditCustomDelim(hwnd, L";");
                        break;
                    case ID_RADIO_DELIM2:
                        modEditCustomDelim(hwnd, L",");
                        break;
                    case ID_RADIO_DELIM3:
                        modEditCustomDelim(hwnd, L"TAB");
                        separateur = "\t";
                        break;
                    case ID_RADIO_DELIM4:
                        modEditCustomDelim(hwnd, L"SPACE");
                        separateur = " ";
                        break;
                    case ID_RADIO_DELIM5:
                        modEditCustomDelim(hwnd, L"|");
                        break;


                }
            }

            // Vérifie si le message reçu provient d'une mise à jour de contenu d'un contrôle d'édition (EN_UPDATE)
            // et spécifiquement du contrôle ID_EDIT_CUSTOMDELIM
            if (HIWORD(wParam) == EN_UPDATE && LOWORD(wParam) == ID_EDIT_CUSTOMDELIM) {
                logger->debug("GUI : ID_EDIT_CUSTOMDELIM a été modifié.");
                // Teste si le contrôle (dans ce cas, une zone d'édition) est activé
                if (IsWindowEnabled(GetDlgItem(hwnd, ID_EDIT_CUSTOMDELIM))) {
                    // Récupère le texte présent dans le contrôle d'édition
                    wchar_t buffer[10];
                    GetWindowText((HWND) lParam, buffer, sizeof(buffer) / sizeof(wchar_t));

                    // Convertit le texte récupéré (format wchar_t) en string et l'assigne à la variable "separateur"
                    separateur = wstring_to_string(buffer);
                    logger->debug("Séparateur : {}", separateur);
                }
            }


            // Vérifie si le message reçu provient d'une mise à jour de contenu d'un contrôle d'édition (EN_UPDATE)
            // et spécifiquement du contrôle ID_EDIT_NUMCOLUMN
            if (HIWORD(wParam) == EN_UPDATE && LOWORD(wParam) == ID_EDIT_NUMCOLUMN) {
                // Récupère le texte présent dans le contrôle d'édition
                logger->info("GUI : ID_EDIT_NUMCOLUMN a été modifié");
                wchar_t buffer[10];
                GetWindowText((HWND) lParam, buffer, sizeof(buffer) / sizeof(wchar_t));

                // Convertit le texte récupéré (format wchar_t) en un entier
                int value = _wtoi(buffer);

                // Assigner la valeur convertie à la variable "nbColonnes"
                nbColonnes = value;
                logger->debug("nbColonnes {}", nbColonnes);
                // Si la valeur est égale à 0 ou négative
                if (value <= 0) {
                    // Metter à jour le texte du contrôle d'édition pour qu'il soit "1"
                    logger->error("Le nombre de colonne ne peut être inf à 1. ({})", value);
                    SetWindowText((HWND) lParam, L"1");
                }
            }


            // Vérifie si le bouton avec l'ID "ID_OPENFILE_BUTTON" a été cliqué
            if (LOWORD(wParam) == ID_OPENFILE_BUTTON) {
                logger->info("GUI : Le bouton ID_OPENFILE_BUTTON a été cliqué.");
                // Initialisation des paramètres pour la boîte de dialogue de sélection de fichier
                OPENFILENAME ofn;
                wchar_t szFileName[MAX_PATH] = L"";  // Variable pour stocker le chemin du fichier sélectionné

                ZeroMemory(&ofn, sizeof(ofn));  // Initialise à zéro la structure "ofn"
                ofn.lStructSize = sizeof(ofn);  // Taille de la structure
                ofn.hwndOwner = hwnd;  // Fenêtre parente
                // Filtres pour les types de fichiers à afficher dans la boîte de dialogue
                ofn.lpstrFilter = L"Fichiers CSV et Texte (*.csv;*.txt)\0*.csv;*.txt\0Fichiers CSV (*.csv)\0*.csv\0Fichiers Texte (*.txt)\0*.txt\0Tous les Fichiers (*.*)\0*.*\0";
                ofn.lpstrFile = szFileName;  // Variable pour stocker le chemin du fichier sélectionné
                ofn.nMaxFile = MAX_PATH;  // Taille maximale du chemin du fichier
                // Flags pour spécifier certaines propriétés de la boîte de dialogue
                ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                ofn.lpstrDefExt = L"csv";  // Extension de fichier par défaut

                // Affiche la boîte de dialogue de sélection de fichier
                if (GetOpenFileName(&ofn)) {
                    // Si un fichier est sélectionné, stocke son chemin et extrait ses données
                    filenameCSV = szFileName;
                    logger->info("Fichier sélectionné : {}", wstring_to_string(filenameCSV));
                    ExtractData();
                }
            }
            break;

        }

            // Message reçu lorsqu'une notification est envoyée par un contrôle
        case WM_NOTIFY: {
            logger->info("GUI : La barre de statut a été cliquée.");
            LPNMHDR lpnmh = (LPNMHDR) lParam;  // Pointeur vers la structure de notification
            // Vérifie si la notification provient de la barre de statut et que le code est un clic
            if (lpnmh->idFrom == ID_STATUSBAR && lpnmh->code == NM_CLICK) {
                // Demande à l'utilisateur s'il souhaite ouvrir le site web "sciences-physiques.net"
                if (MessageBox(hwnd, L"Souhaiter-vous ouvrir sciences-physiques.net ?", L"Plus d'informations",
                               MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    // Si l'utilisateur répond "Oui", ouvre le site web
                    ShellExecute(NULL, L"open", L"https://sciences-physiques.net/softwares/csv2listes", NULL, NULL, SW_SHOWNORMAL);
                    logger->debug("Ouverture du site https://sciences-physiques.net.");
                }
                return 0;  // Interrompt le traitement du message
            }
            break;
        }

            // Message reçu lorsqu'un contrôle statique demande sa couleur de fond
        case WM_CTLCOLORSTATIC: {
            logger->info("GUI : Correction des couleurs.");
            // Si le pinceau (hBrush) n'a pas encore été créé, le créer avec la couleur de fenêtre système
            if (!hBrush) {
                hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
            }
            HDC hdcStatic = (HDC) wParam;  // Pointeur vers le contexte de périphérique du contrôle statique
            // Définir la couleur du texte et de fond du contrôle statique
            SetTextColor(hdcStatic, GetSysColor(COLOR_WINDOWTEXT));
            SetBkColor(hdcStatic, GetSysColor(COLOR_WINDOW));
            return (INT_PTR) hBrush;  // Retourne le pinceau pour peindre le fond du contrôle
        }


            // Message reçu lorsque la fenêtre est sur le point d'être détruite
        case WM_DESTROY:
            logger->info("GUI : Fermeture de la fenêtre.");
            // Nettoyage avant la fermeture de la fenêtre
            if (hBrush) {
                logger->debug("Libération de la mémoire de l'objet hBrush.");
                DeleteObject(hBrush);
                hBrush = NULL;
            }
            PostQuitMessage(0);
            return 0;

            // Message reçu lorsque le système veut arrêter ou redémarrer
        case WM_QUERYENDSESSION:
            logger->info("GUI : Autorisation de la fermeture de l'application.");
            // Autoriser l'arrêt ou le redémarrage
            return TRUE;

            // Message reçu lorsque la session se termine effectivement
        case WM_ENDSESSION:
            // Traiter la fin de la session
            logger->info("GUI : Fin de session.");
            if (wParam)  // wParam is TRUE if the session is ending.
            {
                // Handle the end of the session (e.g., save data, etc.)
            }
            return 0;
    }

    // Si le message n'a pas été traité, appeler la procédure de fenêtre par défaut
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
