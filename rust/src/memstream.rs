//! Fichier chargé en mémoire puis lu ligne par ligne.

use std::fs;
use std::io;
use std::path::Path;

use crate::MAX_SIZE_LINE;
use crate::strings::decode_text;

/// Lignes d'un fichier et position de lecture.
///
/// Les fins de ligne Windows (`\r\n`) sont acceptées.
#[derive(Debug, Default)]
pub struct MemStream {
    lines: Vec<String>,
    /// Nombre de lignes déjà lues, c'est-à-dire le numéro de la dernière ligne lue.
    line_number: usize,
}

impl MemStream {
    pub fn from_text(text: &str) -> Self {
        Self {
            lines: text.lines().map(str::to_owned).collect(),
            line_number: 0,
        }
    }

    /// Charge le contenu d'un fichier en mémoire.
    pub fn load(path: &Path) -> io::Result<Self> {
        let bytes = fs::read(path)?;
        log::debug!("Le fichier {} a été copié en mémoire.", path.display());
        Ok(Self::from_text(&decode_text(&bytes)))
    }

    pub fn is_empty(&self) -> bool {
        self.lines.is_empty()
    }

    pub fn line_count(&self) -> usize {
        self.lines.len()
    }

    /// Ligne numéro `number` (la première ligne porte le numéro 1).
    pub fn line(&self, number: usize) -> Option<&str> {
        self.lines.get(number.checked_sub(1)?).map(String::as_str)
    }

    /// Numéro de la dernière ligne lue (0 si aucune ligne n'a été lue).
    pub fn line_number(&self) -> usize {
        self.line_number
    }

    /// Vérifie qu'aucune ligne ne dépasse [`MAX_SIZE_LINE`] octets.
    pub fn check_max_size_line(&self) -> bool {
        match self.lines.iter().find(|line| line.len() > MAX_SIZE_LINE) {
            Some(line) => {
                log::error!("Ligne trop longue : {} / {MAX_SIZE_LINE}", line.len());
                false
            }
            None => true,
        }
    }

    /// Lit la ligne suivante, `None` à la fin du fichier.
    ///
    /// Si `stop_on_blank` est vrai, une ligne vide termine aussi la lecture.
    pub fn read_next_line(&mut self, stop_on_blank: bool) -> Option<String> {
        let line = self.lines.get(self.line_number)?.clone();
        self.line_number += 1;
        log::debug!("Ligne {} lue", self.line_number);
        if stop_on_blank && line.trim().is_empty() {
            log::info!("Ligne vide -> arrêt de la lecture à la ligne {}", self.line_number);
            return None;
        }
        Some(line)
    }
}
