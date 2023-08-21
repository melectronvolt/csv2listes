//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#include "src_stringutils.h"
#include "src_functions.h"
#include "src_windowsutils.h"
#include <string>
#include <windows.h>
#include <cctype>
#include <algorithm>
#include <shlobj.h>
#include "spdlog/spdlog.h"

// Fonction pour obtenir le chemin du dossier "Mes documents" de l'utilisateur
std::string getDocumentsPath() {
    logger->info("getDocumentsPath called");
    // Déclare un tableau de caractères larges pour stocker le chemin
    wchar_t path[MAX_PATH];

    // Essaie de récupérer le chemin du dossier "Mes documents" à l'aide de la fonction SHGetFolderPathW
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path))) {
        // Convertit le tableau de caractères larges en std::wstring
        std::wstring wpath(path);

        // Ajoute un backslash à la fin du chemin
        wpath = wpath + L'\\';
        logger->debug("Valeur : {}", wstring_to_string(wpath));
        // Convertit le chemin de std::wstring en std::string et le retourne
        return wstring_to_string(wpath);
    } else {
        logger->error("Impossible de récupérer le chemin Documents de l'utilisateur");
        // Si la récupération échoue, retourne une chaîne vide
        return "";
    }
}


// Fonction pour obtenir le chemin du répertoire courant (où l'exécutable est en cours d'exécution)
std::wstring getCurrentDirectory() {
    logger->info("getCurrentDirectory called");
    // Déclare un tableau de caractères larges pour stocker le chemin
    wchar_t buffer[MAX_PATH];

    // Récupère le chemin complet de l'exécutable en cours d'exécution
    GetModuleFileNameW(NULL, buffer, MAX_PATH);

    // Trouve le dernier backslash (séparateur de répertoire) dans le chemin
    wchar_t* lastBackslash = wcsrchr(buffer, L'\\');
    if (lastBackslash) {
        // Met un caractère de fin de chaîne juste après le dernier backslash pour obtenir uniquement le chemin du répertoire
        *(lastBackslash + 1) = L'\0';
    }

    // Convertit le tableau de caractères larges en std::wstring et le retourne
    logger->debug("Valeur : {}", wstring_to_string(std::wstring(buffer)));
    return std::wstring(buffer);
}

