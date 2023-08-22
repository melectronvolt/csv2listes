//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#include "src_functions.h"
#include "src_filesutils.h"
#include <string>
#include <iostream>
#include <fstream>
#include <complex>
#include <unordered_set>
#include "src_stringutils.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/spdlog.h"

std::stringstream memStreamFile;
unsigned int memStreamLine;
bool hasHeader = true;
std::wstring filenameCSV;
std::vector<std::string> nomsColonnes;
std::vector<std::vector<std::string>> donnees;
std::stringstream sortie;
std::wstringstream wsortie;
std::string separateur = ";";
unsigned int nbColonnes;
unsigned int nbLigneEntete;
unsigned int nbLigneDonnees;
std::shared_ptr<spdlog::logger> logger = spdlog::null_logger_mt("null_logger");


// Fonction pour vérifier si le flux en mémoire "memStreamFile" est vide
bool isMemStreamEmpty() {
    logger->info("isMemStreamEmpty called");
    // Sauvegarde la position actuelle du curseur dans le flux
    std::streampos current = memStreamFile.tellg();
    // Déplace le curseur à la fin du flux
    memStreamFile.seekg(0, std::ios::end);
    // Vérifie si la position actuelle (qui est maintenant à la fin du flux) est égale à la position initiale (c'est-à-dire 0)
    bool isEmpty = (memStreamFile.tellg() == std::streampos(0));
    // Restaure la position du curseur à sa position initiale
    memStreamFile.seekg(current);
    if (isEmpty) logger->debug("Retour : flux vide");
    else logger->debug("Retour : flux non vide");

    // Retourne vrai si le flux est vide, sinon retourne faux
    return isEmpty;
}

// Fonction pour vérifier si les lignes dans le flux en mémoire "memStreamFile" dépasse la taille maximale "MAX_SIZE_LINE"
bool checkMaxSizeLine() {
    logger->info("checkMaxSizeLine called");
    // Variable pour stocker chaque ligne lors de la lecture du flux
    std::string line;

    // Réinitialise l'état du flux (par exemple, efface les indicateurs d'erreur)
    memStreamFile.clear();

    // Positionne le curseur au début du flux pour commencer la lecture depuis le début
    memStreamFile.seekg(0, std::ios::beg);

    // Lit le flux ligne par ligne
    while (std::getline(memStreamFile, line)) {
        // Si la longueur d'une ligne dépasse MAX_SIZE_LINE
        if (line.length() > MAX_SIZE_LINE) {
            // Retourne faux
            logger->error("Valeur : False, longueur de la ligne {} / {}", line.length(), MAX_SIZE_LINE);
            return false;
        }
    }

    // Réinitialise à nouveau l'état du flux
    memStreamFile.clear();

    // Repositionne le curseur au début du flux
    memStreamFile.seekg(0, std::ios::beg);

    // Réinitialise un compteur de ligne à zéro.
    memStreamLine = 0;

    // Si aucune ligne ne dépasse MAX_SIZE_LINE, retourne vrai
    logger->debug("Valeur : True");
    return true;
}


// Fonction pour charger le contenu d'un fichier dans le flux en mémoire "memStreamFile"
bool loadIntoMemStream(const std::wstring& filename) {
    logger->info("loadIntoMemStream called");
    logger->debug("Fichier : {}", wstring_to_string(filename));

    // Ouvre le fichier en lecture
    std::ifstream file(filename);
    // Si l'ouverture du fichier échoue
    if (!file.is_open()) {
        logger->error("Le fichier ne peut pas être ouvert.");

        // Retourne faux
        return false;
    }
    // Réinitialise le flux en mémoire pour s'assurer qu'il est vide
    memStreamFile.str("");
    // Réinitialise l'état du flux en mémoire (par exemple, efface les indicateurs d'erreur)
    memStreamFile.clear();
    // Charge le contenu du fichier dans le flux en mémoire
    memStreamFile << file.rdbuf();
    // Ferme le fichier
    file.close();
    // Réinitialise un compteur de ligne à zéro.
    memStreamLine = 0;
    // Retourne vrai pour indiquer le succès de l'opération
    logger->debug("Le fichier a bien été copié en mémoire.");
    return true;
}


// Fonction pour lire la ligne suivante du flux en mémoire "memStreamFile"
bool readNextLineMemStream(std::string& line) {

    // Si la lecture de la ligne suivante du flux en mémoire est réussie
    if (std::getline(memStreamFile, line)) {

        // Incrémente le compteur de ligne
        memStreamLine++;
        logger->info("readNextLineMemStream called - Nouvelle ligne lue : {}", memStreamLine);

        if ((nbLigneDonnees != 0) && (trim(line).empty())) {
            logger->info("Ligne vide -> Arrêt de la lecture à la ligne {}", memStreamLine);
            return false;
        }

        // Retourne vrai pour indiquer le succès de l'opération
        return true;
    }

    // Si la fin du flux est atteinte ou s'il y a une erreur, retourne faux
    logger->info("readNextLineMemStream called - Fin du fichier");
    return false;
}


// Fonction pour obtenir le nombre de lignes dans le flux en mémoire "memStreamFile"
double getLineCount() {
    // Variable pour stocker chaque ligne lors de la lecture du flux
    std::string line;

    // Réinitialise l'état du flux (par exemple, efface les indicateurs d'erreur)
    memStreamFile.clear();

    // Positionne le curseur au début du flux pour commencer la lecture depuis le début
    memStreamFile.seekg(0, std::ios::beg);

    // Variable pour compter le nombre de lignes
    double count = 0;

    // Lit le flux ligne par ligne
    while (std::getline(memStreamFile, line)) {
        count++;
    }
    logger->info("getLineCount called - nombre de lignes : {}", count);
    // Réinitialise à nouveau l'état du flux
    memStreamFile.clear();

    // Repositionne le curseur au début du flux
    memStreamFile.seekg(0, std::ios::beg);

    // Réinitialise un compteur de ligne (supposé) à zéro. Note: "memStreamLine" n'est pas défini dans le code fourni,
    // mais on peut supposer qu'il s'agit d'une variable globale ou d'un membre de classe.
    memStreamLine = 0;

    // Retourne le nombre total de lignes dans le flux
    return count;
}


// Fonction pour obtenir une ligne spécifique du flux en mémoire "memStreamFile"
std::string getOneLine(int LineNumber) {
    logger->info("getOneLine - Ligne : {}", LineNumber);
    // Variable pour stocker la ligne lue
    std::string line;

    // Réinitialise l'état du flux (par exemple, efface les indicateurs d'erreur)
    memStreamFile.clear();

    // Positionne le curseur au début du flux pour commencer la lecture depuis le début
    memStreamFile.seekg(0, std::ios::beg);

    // Lit les lignes jusqu'à atteindre la ligne demandée
    for (int i = 0; i < LineNumber; ++i) {
        std::getline(memStreamFile, line);
    }

    // Réinitialise à nouveau l'état du flux
    memStreamFile.clear();

    // Repositionne le curseur au début du flux
    memStreamFile.seekg(0, std::ios::beg);

    // Réinitialise un compteur de ligne (supposé) à zéro. Note: "memStreamLine" n'est pas défini dans le code fourni,
    // mais on peut supposer qu'il s'agit d'une variable globale ou d'un membre de classe.
    memStreamLine = 0;

    // Retourne la ligne demandée
    return line;
}

// Fonction pour découper une chaîne "s" en fonction d'un délimiteur "delimiter"
std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    logger->info("split called with parameter {} : {}", s, delimiter);
    // Crée un vecteur pour stocker les sous-chaînes obtenues
    std::vector<std::string> tokens;

    // Position de départ pour la recherche du délimiteur
    size_t start = 0;

    // Trouve la première occurrence du délimiteur
    size_t end = s.find(delimiter);

    // Boucle tant que le délimiteur est trouvé
    while (end != std::string::npos) {
        // Ajoute la sous-chaîne (avant le délimiteur) au vecteur
        tokens.push_back(s.substr(start, end - start));

        // Met à jour la position de départ après le délimiteur
        start = end + delimiter.length();

        // Recherche la prochaine occurrence du délimiteur
        end = s.find(delimiter, start);

        // Si le délimiteur n'est pas trouvé et qu'il reste des caractères dans la chaîne
        if (end == std::string::npos && start < s.length()) {
            // Ajoute le reste de la chaîne au vecteur
            tokens.push_back(s.substr(start));
        }
    }

    // Si aucun délimiteur n'a été trouvé (et donc le vecteur est vide)
    if (tokens.empty()) {
        // Ajoute la chaîne entière au vecteur
        tokens.push_back(s);
    }

    // Retourne le vecteur des sous-chaînes
    logger->info("Nombre de découpage : {}", tokens.size());
    return tokens;
}

// Fonction pour vérifier si la taille du fichier n'est pas trop grande
bool isNotTooBig(const std::wstring& filename) {
    logger->info("isFileHumanReadable called");
    logger->debug("Fichier : {}", wstring_to_string(filename));

    // Obtient la taille du fichier
    double fileSize = getFileSize(filename);
    // Vérifie si la taille du fichier est positive et inférieure à la taille maximale définie
    if (fileSize > 0 && fileSize < MAX_SIZE_FILE) {
        logger->debug("Fichier de taille convenable {} / {}.", fileSize, MAX_SIZE_FILE);
    } else {
        logger->debug("Fichier trop gros {} / {}.", fileSize, MAX_SIZE_FILE);
    }
    return (fileSize > 0 && fileSize < MAX_SIZE_FILE);
}


// Fonction pour vérifier si le fichier a suffisamment de lignes
bool notTooLessLines(const std::wstring& filename) {
    logger->info("notTooLessLines called");
    logger->debug("Fichier : {}", wstring_to_string(filename));
    // Ouvre le fichier en lecture
    std::wifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    size_t lineCount = 0;
    std::wstring line;
    while (std::getline(file, line)) {
        // Ignore les lignes vides
        if (line.empty()) {
            continue;
        }
        ++lineCount;
        if (lineCount >= MIN_LINE_COUNT) {
            logger->debug("Valeur : true {} / {}", lineCount, MIN_LINE_COUNT);
            return true;
        }
    }

    logger->debug("Valeur : false {} / {}", lineCount, MIN_LINE_COUNT);
    return false;
}


// Fonction pour vérifier si le fichier n'a pas trop de lignes
bool isNotTooManyLines(const std::wstring& filename) {
    logger->info("isNotTooManyLines called");
    logger->debug("Fichier : {}", wstring_to_string(filename));
    // Ouvre le fichier en lecture
    std::wifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    size_t lineCount = 0;
    std::wstring line;
    while (std::getline(file, line)) {
        ++lineCount;
        if (lineCount >= MAX_LINE_COUNT) {
            logger->debug("Valeur : false {} / {}", lineCount, MAX_LINE_COUNT);
            return false;
        }
    }
    logger->debug("Valeur : true {} / {}", lineCount, MAX_LINE_COUNT);
    return true;
}


// Fonction pour vérifier si le fichier est un fichier texte et lisible par l'homme
bool isFileText(const std::wstring& filename) {
    if (!isBinary(filename) && isFileHumanReadable(filename)) {
        logger->info("isFileText called : Détection d'un fichier texte");
    } else {
        logger->info("isFileText called : Détection d'un fichier non texte");
    }
    return (!isBinary(filename) && isFileHumanReadable(filename));
}
