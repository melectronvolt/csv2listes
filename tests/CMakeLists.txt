# Ajoute un chemin supplémentaire pour la recherche de packages CMake, spécifiquement pour Catch2
list(APPEND CMAKE_PREFIX_PATH "O:/Frameworks/Catch2/3.5.3")

# Recherche le package Catch2 (version 3)
find_package(Catch2 3 REQUIRED)

# Ajoute un exécutable pour les tests nommé "test_csv2list" avec la liste des fichiers sources associés
add_executable(test_csv2listes tests_csv2list.cpp
        ../src/src_functions.cpp
        ../src/src_stringutils.cpp
        ../src/src_clipboardutils.cpp
        ../src/src_pythonutils.cpp
        ../src/src_windowsutils.cpp
        ../src/src_filesutils.cpp)

# Lie l'exécutable des tests aux bibliothèques spécifiées : Catch2 et spdlog
target_link_libraries(test_csv2listes PRIVATE Catch2::Catch2WithMain spdlog::spdlog)
