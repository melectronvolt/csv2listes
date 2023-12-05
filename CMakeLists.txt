# Indique la version minimale de CMake requise pour ce fichier
cmake_minimum_required(VERSION 3.26)

# Nomme le projet "csv2list_app"
project(csv2listes_app)

# Définit la norme C++ utilisée pour ce projet comme étant C++23
set(CMAKE_CXX_STANDARD 17)

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


set(spdlog_BASE_PATH "F:/Frameworks/spdlog/1.12.0")

# Ajoute un chemin pour la recherche de packages CMake
list(APPEND CMAKE_PREFIX_PATH "F:/Frameworks/spdlog/1.12.0")

# Indique si spdlog est utilisé en tant que DLL (bibliothèque dynamique) ou non.
SET(USE_DYNAMIC_spdlog OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (USE_DYNAMIC_spdlog)
        set(spdlog_BASE_PATH_find "${spdlog_BASE_PATH}/build_debug_dll")
    else ()
        set(spdlog_BASE_PATH_find "${spdlog_BASE_PATH}/build_debug")
    endif ()
else ()
    if (USE_DYNAMIC_spdlog)
        set(spdlog_BASE_PATH_find "${spdlog_BASE_PATH}/build_release_dll")
    else ()
        set(spdlog_BASE_PATH_find "${spdlog_BASE_PATH}/build_release")
    endif ()
endif ()

# Affiche le type de build (Debug, Release, etc.).
MESSAGE("Type: ${CMAKE_BUILD_TYPE}")

# Si spdlog est utilisé comme une DLL.
if (USE_DYNAMIC_spdlog)
    MESSAGE("Link: Dynamic (DLL)")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(spdlog_DLL_PATH "${spdlog_BASE_PATH}/build_debug/Debug")
    else ()
        set(spdlog_DLL_PATH "${spdlog_BASE_PATH}/build_release/Release")
    endif ()

    # Définit le répertoire de destination pour les DLLs.
    set(DLL_DEST_DIR ${CMAKE_BINARY_DIR})
    MESSAGE("Copy the DLLs to the executable directory : ${DLL_DEST_DIR}")

    # Récupère tous les fichiers DLL du chemin spécifié.
    file(GLOB spdlog_DLLS "${spdlog_DLL_PATH}/*.dll")
    # Boucle pour copier chaque DLL dans le répertoire d'exécution.
    foreach (DLL ${spdlog_DLLS})
        get_filename_component(DLL_NAME ${DLL} NAME)
        MESSAGE("DLL : ${DLL_NAME}")
        configure_file("${spdlog_DLL_PATH}/${DLL_NAME}" "${DLL_DEST_DIR}/${DLL_NAME}" COPYONLY)
    endforeach ()
else ()
    MESSAGE("Link: Static")
endif ()

# Ajoute le chemin de base de spdlog à CMAKE_PREFIX_PATH pour faciliter la recherche de paquets.
list(APPEND CMAKE_PREFIX_PATH "${spdlog_BASE_PATH_find}")

# Ajoute le répertoire d'en-tête de spdlog aux répertoires d'inclusion du projet.
include_directories("${spdlog_BASE_PATH}/include")

# Recherche le paquet spdlog version 3 et indique qu'il est obligatoire.
find_package(spdlog REQUIRED)

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
target_link_libraries(csv2listes_app PRIVATE comctl32.lib Version.lib)


if (USE_DYNAMIC_spdlog)
    target_link_libraries(csv2listes_app PRIVATE spdlog::spdlog)
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(csv2listes_app PRIVATE "${spdlog_BASE_PATH}/build_debug/Debug/spdlogd.lib")
    else ()
        target_link_libraries(csv2listes_app PRIVATE "${spdlog_BASE_PATH}/build_release/Release/spdlog.lib")
    endif ()
endif ()

target_link_options(csv2listes_app PRIVATE "/MAP")
# Ajoute un sous-répertoire contenant un autre fichier CMakeLists.txt pour les tests
add_subdirectory(tests)
