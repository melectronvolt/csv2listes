//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#include "src_stringutils.h"
#include "src_functions.h"
#include <string>
#include <windows.h>
#include <iostream>

#include <complex>
#include <unordered_set>

#include "spdlog/spdlog.h"

// Fonction pour convertir une std::string (UTF-8) en std::wstring (UTF-16)
std::wstring string_to_wstring(const std::string& str) {

    // Si la chaîne d'entrée est vide, retourne une chaîne large vide
    if (str.empty()) {
        return L"";
    }

    // Détermine la taille nécessaire pour la conversion
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
    if (requiredSize == 0) {
        return L"";
    }

    // Convertit la chaîne UTF-8 en UTF-16
    std::wstring wstr(static_cast<size_t>(requiredSize), 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], requiredSize);

    // Retourne la chaîne convertie
    return wstr;
}


// Fonction pour convertir une std::wstring (UTF-16) en std::string (UTF-8)
std::string wstring_to_string(const std::wstring& wstr) {
    // Détermine la taille nécessaire pour la conversion
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr,
                                           nullptr);
    if (requiredSize == 0) {
        return "";
    }

    // Convertit la chaîne UTF-16 en UTF-8
    std::string result(requiredSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], requiredSize, nullptr,
                        nullptr);

    // Retourne la chaîne convertie
    return result;
}

// Fonction pour convertir un flux std::stringstream (UTF-8) en flux std::wstringstream (UTF-16)
void ConvertUTF8StreamToWideStream(const std::stringstream& sortie, std::wstringstream& wsortie) {
    logger->info("ConvertUTF8StreamToWideStream called");
    // Obtient la chaîne du flux UTF-8
    std::string utf8String = sortie.str();

    // Détermine la taille nécessaire pour la conversion
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, nullptr, 0);
    if (wlen == 0) {
        return;
    }

    // Convertit la chaîne UTF-8 en UTF-16
    std::vector<wchar_t> wideString(wlen);
    MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, &wideString[0], wlen);

    // Charge la chaîne convertie dans le flux de sortie
    wsortie << &wideString[0];
}

// Fonction pour remplacer toutes les occurrences d'un caractère par un autre dans une chaîne
std::string replaceAllChars(const std::string& input, char oldChar, char newChar) {
    logger->info("replaceAllChars called. Paramètres '{}' vers '{}'.", oldChar,newChar);
    // Copie la chaîne d'entrée
    std::string result = input;

    // parcourt chaque caractère de la chaîne
    for (char& c: result) {
        // Si le caractère correspond à l'ancien caractère, le remplace par le nouveau
        if (c == oldChar) {
            c = newChar;
        }
    }
    // Retourne la chaîne modifiée
    return result;
}

