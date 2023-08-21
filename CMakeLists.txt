# Indique la version minimale de CMake requise pour ce fichier
cmake_minimum_required(VERSION 3.26)

# Nomme le projet "csv2list_app"
project(csv2listes_app)

# Définit la norme C++ utilisée pour ce projet comme étant C++23
set(CMAKE_CXX_STANDARD 23)

# Ajoute des définitions pour utiliser Unicode
add_definitions(-DUNICODE -D_UNICODE)

# Inclut le répertoire "include" pour la recherche de fichiers d'en-tête
include_directories(${PROJECT_SOURCE_DIR}/include)

# Définit les fichiers de ressources du projet
set(RESOURCE_FILES resources.rc)

# Définit le chemin du répertoire source pour les tests
set(source_dir ${CMAKE_CURRENT_SOURCE_DIR}/tests/assets)

# Définit le chemin de destination dans le répertoire de construction pour les tests
set(destination_dir ${CMAKE_BINARY_DIR}/tests)

# Ajoute un chemin pour la recherche de packages CMake
list(APPEND CMAKE_PREFIX_PATH "P:/Frameworks/Install/spdlog")

# Recherche le package SPDLOG
find_package(SPDLOG REQUIRED)

# Ajoute un exécutable nommé "csv2list_app" et indique les fichiers sources associés
add_executable(csv2listes_app WIN32 main.cpp
        src/src_functions.cpp
        src/src_stringutils.cpp
        src/src_clipboardutils.cpp
        src/src_pythonutils.cpp
        src/src_windowsutils.cpp
        src/src_filesutils.cpp
        ${RESOURCE_FILES})

# Copie les fichiers depuis le répertoire source vers le répertoire de destination
file(COPY ${source_dir} DESTINATION ${destination_dir})

# Lie l'exécutable aux bibliothèques spécifiées
target_link_libraries(csv2listes_app comctl32.lib Version.lib spdlog::spdlog)

# Ajoute un sous-répertoire contenant un autre fichier CMakeLists.txt pour les tests
add_subdirectory(tests)
