//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//
#include "src_functions.h"
#include "src_filesutils.h"
#include "src_stringutils.h"
#include "spdlog/spdlog.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

// Fonction qui vérifie si un fichier spécifié existe et est lisible
bool fileExistsAndReadable(const std::wstring& filename) {
    logger->info("fileExistsAndReadable called");
    logger->debug("Fichier : {}", wstring_to_string(filename));
    // Tente d'ouvrir le fichier en mode lecture avec des caractères larges
    std::wifstream file(filename);

    // Si le fichier est correctement ouvert et prêt à être lu
    if (file.good()) {
        logger->debug("Le fichier est lisible");
        // Ferme le fichier
        file.close();
        // Retourne vrai, indiquant que le fichier existe et est lisible
        return true;
    }

    // Ferme le fichier (bien que non nécessaire car le fichier serait déjà fermé si l'ouverture échoue)
    file.close();
    logger->error("Le fichier est illisible");
    // Retourne faux, indiquant que le fichier n'existe pas ou n'est pas lisible
    return false;
}

// Fonction pour obtenir la taille d'un fichier spécifié
double getFileSize(const std::wstring& filename) {
    logger->info("getFileSize called");
    logger->debug("Fichier : {}", wstring_to_string(filename));
    // Tente d'ouvrir le fichier en mode binaire et positionne le curseur à la fin du fichier
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    // Si le fichier n'est pas correctement ouvert
    if (!file.is_open()) {
        logger->error("Impossible d'ouvrir le fichier");
        // Retourne -1 pour indiquer une erreur
        return -1;
    }

    logger->debug("Récupération de la position du curseur");
    // Récupère la position actuelle du curseur, qui est égale à la taille du fichier car le curseur est à la fin
    std::streampos fileSize = file.tellg();

    // Ferme le fichier
    file.close();

    // Si la récupération de la taille échoue, tellg() renvoie -1
    if (fileSize == -1) {
        logger->error("Impossible de récupérer la position du curseur");
        return -1;
    }

    logger->debug("Taille du fichier : {} bits", static_cast<double>(fileSize));
    // Convertit la taille du fichier (en octets) en double et la retourne
    return static_cast<double>(fileSize);
}

// Fonction pour vérifier si un fichier spécifié par son nom est un fichier binaire
bool isBinary(const std::wstring& filename) {
    logger->info("isBinary called");
    logger->debug("Fichier : {}", wstring_to_string(filename));
    // Tente d'ouvrir le fichier en mode binaire
    std::ifstream file(filename, std::ios::binary);

    // Si le fichier n'est pas correctement ouvert
    if (!file.is_open()) {
        logger->error("Le fichier ne peut pas être ouvert.");
        // Retourne vrai, supposant que c'est un fichier binaire (ce choix est discutable)
        return true;
    }

    char byte;
    // Lit le fichier octet par octet
    logger->debug("Lecture du fichier en cours");
    while (file.read(&byte, 1)) {
        // Si un octet nul '\0' ou un octet de fin de fichier '\x1A' est rencontré
        if (byte == '\0' || byte == '\x1A') {
            // Ferme le fichier
            file.close();
            // Retourne vrai car ses octets sont typiques des fichiers binaires
            logger->debug("Le fichier est binaire.");
            return true;
        }
    }

    // Ferme le fichier
    file.close();

    // Si aucun octet typique d'un fichier binaire n'a été trouvé, retourne faux, supposant que c'est un fichier texte
    logger->debug("Le fichier n'est pas binaire.");
    return false;
}

// Fonction pour vérifier si un fichier est lisible par l'homme
bool isFileHumanReadable(const std::wstring& filename) {
    logger->info("isFileHumanReadable called");
    logger->debug("Fichier : {}", wstring_to_string(filename));
    // Tente d'ouvrir le fichier en mode binaire
    std::ifstream file(filename, std::ios::binary);

    // Si le fichier n'est pas correctement ouvert
    if (!file.is_open()) {
        logger->error("Le fichier ne peut pas être ouvert.");
        // Retourne faux
        return false;
    }

    // Crée une carte pour compter la fréquence de chaque caractère dans le fichier
    std::unordered_map<char, int> charFrequency;
    char byte;

    // Lit le fichier octet par octet
    logger->debug("Lecture du fichier en cours.");
    while (file.read(&byte, 1)) {
        charFrequency[byte]++;
    }

    // Ferme le fichier
    file.close();

    int totalChars = 0;
    int nonPrintableChars = 0;

    // Parcourt chaque entrée de la carte de fréquence des caractères
    logger->debug("Traitement des données.");
    for (const auto& entry: charFrequency) {
        char ch = entry.first;

        // Si le caractère est non affichable (et n'est pas une nouvelle ligne, un retour chariot ou une tabulation)
        if (ch < 32 && ch != '\n' && ch != '\r' && ch != '\t') {
            nonPrintableChars += entry.second;
        }
        totalChars += entry.second;
    }

    // Calcule le ratio de caractères non imprimables
    double nonPrintableRatio = static_cast<double>(nonPrintableChars) / totalChars;
    logger->debug("Ratio des caractères non affichable : {}%", nonPrintableRatio*100);
    // Retourne vrai si le ratio de caractères non imprimables est inférieur à 10%, sinon retourne faux
    return nonPrintableRatio < 0.1;
}
