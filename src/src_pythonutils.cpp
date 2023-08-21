//
// Created by Rémi MEVAERE on 19/08/2023.
// Copyright (c) 2023.
//
// Github : mevaere
// Twitter : MelectronVolt
//

#include "src_functions.h"

#include "src_pythonutils.h"
#include <string>
#include "src_functions.h"
#include <iostream>

#include <complex>
#include <unordered_set>
#include <cctype>
#include <algorithm>

#include "spdlog/spdlog.h"

// Transforme une chaîne de caractères en respectant la norme PEP8 pour la longueur des lignes
std::string transformToPEP8(const std::string& input) {
    unsigned int nbr_decoupage = 0;
    logger->info("transformToPEP8 called");
    logger->debug("input : {}", input);
    // Si la taille de la chaîne d'entrée est inférieure ou égale à 79 caractères
    if (input.size() <= 79) {
        // Retourne la chaîne d'entrée telle quelle
        return input;
    }

    // Initialise une chaîne vide pour le résultat
    std::string result;
    // Trouve la première occurrence du caractère '[' dans la chaîne d'entrée
    size_t start = input.find('[');
    // Trouve la première occurrence du caractère ']' dans la chaîne d'entrée
    size_t end = input.find(']');

    // Si les caractères '[' et ']' sont présents dans la chaîne d'entrée
    if (start != std::string::npos && end != std::string::npos) {
        // Extrait la partie de la chaîne avant '[' et ajoute un saut de ligne avec une indentation de 4 espaces
        result = input.substr(0, start + 1) + "\n    ";
        nbr_decoupage += 1;
        // Extrait le contenu entre les caractères '[' et ']'
        std::string listContent = input.substr(start + 1, end - start - 1);

        // Utilise un flux de chaînes pour parcourir le contenu de la liste
        std::stringstream ss(listContent);
        std::string item;
        // Tant qu'il y a des éléments dans la liste, séparés par des virgules
        while (std::getline(ss, item, ',')) {
            // Si l'ajout de l'élément courant à la chaîne résultante dépasse 79 caractères pour la ligne en cours
            if (result.size() - result.rfind('\n') + item.size() > 79) {
                // Ajoute un nouveau saut de ligne avec une indentation de 4 espaces
                result += "\n    ";
                nbr_decoupage += 1;
            }
            // Ajoute l'élément courant suivi d'une virgule à la chaîne résultante
            result += item + ",";
        }
        // Supprime la dernière virgule ajoutée
        result.pop_back();
        // Ajoute le reste de la chaîne d'entrée après le caractère ']'
        result += input.substr(end);
    } else {
        // Si '[' et ']' ne sont pas présents, la chaîne d'entrée est retournée telle quelle
        result = input;
    }

    logger->debug("Nombre de découpages : {}", nbr_decoupage);
    logger->debug("output : {}", result);
    // Retourne la chaîne transformée
    return result;
}


// Transforme chaque élément d'un vecteur de chaînes en une variable Python valide
void transformToPythonVar(std::vector<std::string>& vec) {
    logger->info("transformToPythonVar called");
    // Crée un ensemble non ordonné pour suivre les chaînes déjà vues
    std::unordered_set<std::string> seen;

    // Parcourt chaque chaîne dans le vecteur
    for (std::string& str: vec) {
        // Si le premier caractère de la chaîne est un chiffre
        if (isdigit(str[0])) {
            // Préfixe la chaîne avec un '_'
            str = "_" + str;
        }
        // Parcourt chaque caractère de la chaîne
        for (char& c: str) {
            // Si le caractère n'est pas alphanumérique (ni lettre, ni chiffre)
            if (!isalnum(c)) {
                // Remplace le caractère par un '_'
                c = '_';
            }
        }

        // Stocke la chaîne originale après les modifications précédentes
        std::string original = str;
        // Initialise un compteur pour gérer les doublons
        int counter = 1;
        // Tant que la chaîne est déjà présente dans l'ensemble "seen"
        while (seen.find(str) != seen.end()) {
            // Ajoute un suffixe à la chaîne originale avec le compteur actuel (ex: "var_1", "var_2", ...)
            str = original + "_" + std::to_string(counter);
            // Incrémente le compteur
            counter++;
        }
        // Ajoute la chaîne transformée à l'ensemble "seen"
        seen.insert(str);
    }
}


// Transformer chaque élément d'un vecteur de chaînes en élément de liste Python valide
void transformThePythonList(std::vector<std::string>& vec) {
    logger->info("transformThePythonList called");
    // parcourt chaque chaîne du vecteur
    for (std::string& s: vec) {
        // Si la chaîne n'est pas un nombre Python valide
        if (!isValidPythonNumber(s)) {
            // Entoure la chaîne de guillemets pour la transformer en une chaîne de caractères Python
            s = "\"" + s + "\"";
        }
    }
}

// Vérifier si une chaîne est un nombre Python valide (entier, flottant ou complexe)
bool isValidPythonNumber(const std::string& s) {

    // Crée une chaîne pour stocker la version sans espaces de la chaîne d'entrée
    std::string noSpaces;
    // Copie la chaîne d'entrée dans "noSpaces" en omettant les espaces
    std::remove_copy(s.begin(), s.end(), std::back_inserter(noSpaces), ' ');

    // Utilise un flux de chaînes pour vérifier si "noSpaces" est un entier
    std::stringstream ss1(noSpaces);
    int intVal;
    // Si la conversion en entier réussit et qu'il n'y a rien après l'entier, retourne vrai
    if (ss1 >> intVal && ss1.eof()) {
        logger->info("isValidPythonNumber {} -> true (entier)", s);
        return true;
    }

    // Utilise un flux de chaînes pour vérifier si "noSpaces" est un flottant
    std::stringstream ss2(noSpaces);
    float floatVal;
    // Si la conversion en flottant réussit et qu'il n'y a rien après le flottant, retourne vrai
    if (ss2 >> floatVal && ss2.eof()) {
        logger->info("isValidPythonNumber {} -> true (flottant)", s);
        return true;
    }

    // Cherche le caractère '+' après la première position (pour gérer les nombres complexes)
    size_t pos = noSpaces.find('+', 1);
    // Si '+' n'est pas trouvé, cherche le caractère '-'
    if (pos == std::string::npos) {
        pos = noSpaces.find('-', 1);
    }

    // Si '+' ou '-' est trouvé et que le dernier caractère est 'j' (pour représenter un nombre complexe en Python)
    if (pos != std::string::npos && noSpaces.back() == 'j') {
        // Extrait la partie réelle et imaginaire du nombre complexe
        std::string realPart = noSpaces.substr(0, pos);
        std::string imagPart = noSpaces.substr(pos + 1, noSpaces.length() - pos - 2);

        // Utilise des flux de chaînes pour vérifier si les parties réelle et imaginaire sont des flottants
        std::stringstream ssReal(realPart), ssImag(imagPart);
        float real, imag;
        // Si les conversions en flottants réussissent et qu'il n'y a rien après les flottants, retourne vrai
        if ((ssReal >> real && ssReal.eof()) && (ssImag >> imag && ssImag.eof())) {
            logger->info("isValidPythonNumber {} -> true (complexe)", s);
            return true;
        }
    }
    // Si aucune des conditions ci-dessus n'est remplie, retourne faux
    logger->info("isValidPythonNumber {} -> false", s);
    return false;
}
