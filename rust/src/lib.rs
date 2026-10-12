//! csv2listes : extraction de listes Python à partir de fichiers CSV.
//!
//! Ce module regroupe le cœur du logiciel, indépendant de l'interface
//! graphique, afin de pouvoir le tester.

pub mod extraction;
pub mod files;
pub mod memstream;
pub mod python;
pub mod strings;

/// Longueur maximale (en octets) d'une ligne du fichier CSV.
pub const MAX_SIZE_LINE: usize = 1024;
/// Nombre minimal de lignes non vides dans le fichier.
pub const MIN_LINE_COUNT: usize = 2;
/// Le fichier doit contenir strictement moins de lignes que cette valeur.
pub const MAX_LINE_COUNT: usize = 67000;
/// Taille maximale (en octets) du fichier.
pub const MAX_SIZE_FILE: u64 = 1_000_000;
