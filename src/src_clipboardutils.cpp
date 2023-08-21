//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#include "src_functions.h"
#include "src_clipboardutils.h"
#include <string>
#include <windows.h>
#include <iostream>
#include <complex>
#include <algorithm>
#include "spdlog/spdlog.h"

// Fonction qui tente de copier le contenu du presse-papiers vers un flux de caractères en Unicode
bool CopyClipboardToStreamUnicode(std::wstringstream& wss) {
    logger->info("CopyClipboardToStreamUnicode called.");
    // Essaie d'ouvrir le presse-papiers, retourne faux si échec
    if (!OpenClipboard(nullptr)) {
        logger->error("Impossible d'ouvrir le presse-papiers.");
        return false;
    }

    // Tente d'obtenir des données du presse-papiers au format de texte Unicode
    logger->debug("Obtention des données dans le presse papier.");
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    // Si aucune donnée n'est obtenue, ferme le presse-papiers et retourne faux
    if (hData == nullptr) {
        logger->error("Impossible de récupérer des données dans le presse-papiers.");
        CloseClipboard();
        return false;
    }

    // Verrouille les données du presse-papiers et obtient un pointeur vers elles
    logger->debug("Verrouillage des données du presse-papiers.");
    wchar_t* pszDataW = static_cast<wchar_t*>(GlobalLock(hData));
    // Si le verrouillage échoue, ferme le presse-papiers et retourne faux
    if (pszDataW == nullptr) {
        logger->error("Le verrouillage a échoué.");
        CloseClipboard();
        return false;
    }

    // Copie le contenu pointé par pszDataW vers le flux de caractères <wchar_t*>
    wss << pszDataW;

    // Déverrouille les données du presse-papiers
    logger->debug("Déverrouillage des données.");
    GlobalUnlock(hData);
    logger->debug("Ferme le presse-papiers.");
    // Ferme le presse-papiers
    CloseClipboard();

    // Retourne vrai pour indiquer le succès
    return true;
}

// Fonction qui tente de copier le contenu d'un flux de caractères en Unicode vers le presse-papiers
bool PasteClipboardFromStreamUnicode(const std::wstringstream& wss) {
    logger->info("PasteClipboardFromStreamUnicode called.");
    // Essaie d'ouvrir le presse-papiers, retourne faux si échec
    logger->debug("Ouverture du presse-papiers");
    if (!OpenClipboard(nullptr)) {
        logger->error("Impossible d'ouvrir le presse papier");
        return false;
    }

    // Vide le contenu actuel du presse-papiers
    logger->debug("Nettoyage du presse-papiers");
    EmptyClipboard();

    // Récupère le contenu du flux sous forme de chaîne de caractères larges
    std::wstring ws = wss.str();
    // Calcule la taille nécessaire pour stocker la chaîne (y compris le caractère de terminaison null)
    size_t size = (ws.size() + 1) * sizeof(wchar_t);  // +1 pour le terminateur null

    // Alloue de la mémoire pour stocker la chaîne
    logger->debug("Allocation de la mémoire");
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    // Si l'allocation échoue, ferme le presse-papiers et retourne faux
    if (hMem == nullptr) {
        logger->error("Impossible d'allouer de la mémoire");
        CloseClipboard();
        return false;
    }

    // Verrouille la mémoire allouée pour accéder à son contenu
    logger->debug("Verrouillage des données du presse-papiers.");
    wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hMem));
    // Si le verrouillage échoue, libère la mémoire, ferme le presse-papiers et retourne faux
    if (pMem == nullptr) {
        logger->error("Le verrouillage a échoué.");
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    // Copie le contenu de la chaîne dans la mémoire allouée
    wcscpy_s(pMem, ws.size() + 1, ws.c_str());

    // Déverrouille la mémoire
    GlobalUnlock(hMem);

    logger->debug("Copiage des données.");
    // Tente de définir les données du presse-papiers au format de texte Unicode
    if (SetClipboardData(CF_UNICODETEXT, hMem) == nullptr) {
        // Si l'échec, libère la mémoire, ferme le presse-papiers et retourne faux
        logger->error("Impossible d'affecter les données au presse-papiers.");
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }
    logger->debug("Ferme le presse-papiers.");
    // Ferme le presse-papiers
    CloseClipboard();

    // Retourne vrai pour indiquer le succès
    return true;
}

