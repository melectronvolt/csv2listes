//! Vérifications effectuées sur le fichier avant son traitement.

use std::fs;
use std::path::Path;

use crate::strings::decode_text;
use crate::{MAX_LINE_COUNT, MAX_SIZE_FILE, MIN_LINE_COUNT};

/// Vérifie que le chemin désigne un fichier existant et lisible.
pub fn file_exists_and_readable(path: &Path) -> bool {
    let readable = path.is_file() && fs::File::open(path).is_ok();
    log::debug!("file_exists_and_readable({}) : {readable}", path.display());
    readable
}

/// Taille du fichier en octets, `None` si elle ne peut pas être obtenue.
pub fn file_size(path: &Path) -> Option<u64> {
    let size = fs::metadata(path).ok().map(|metadata| metadata.len());
    log::debug!("file_size({}) : {size:?}", path.display());
    size
}

/// Un fichier contenant un octet nul ou `0x1A` (fin de fichier DOS) est
/// considéré comme binaire, de même qu'un fichier impossible à ouvrir.
pub fn is_binary(path: &Path) -> bool {
    let binary = match fs::read(path) {
        Ok(bytes) => bytes.iter().any(|&byte| byte == 0 || byte == 0x1A),
        Err(_) => true,
    };
    log::debug!("is_binary({}) : {binary}", path.display());
    binary
}

/// Un fichier est lisible par l'homme si moins de 10 % de ses octets ne sont
/// pas affichables.
///
/// Comme dans la version C++ (où `char` est signé), les octets ≥ 0x80 sont
/// comptés comme non affichables : un texte majoritairement non latin est refusé.
pub fn is_file_human_readable(path: &Path) -> bool {
    let Ok(bytes) = fs::read(path) else {
        log::error!("Le fichier ne peut pas être ouvert : {}", path.display());
        return false;
    };
    if bytes.is_empty() {
        return false;
    }
    let non_printable = bytes
        .iter()
        .filter(|&&byte| (byte < 32 || byte >= 0x80) && !matches!(byte, b'\n' | b'\r' | b'\t'))
        .count();
    let ratio = non_printable as f64 / bytes.len() as f64;
    log::debug!("Ratio des caractères non affichables : {:.1} %", ratio * 100.0);
    ratio < 0.1
}

/// Le fichier n'est pas binaire et il est lisible par l'homme.
pub fn is_file_text(path: &Path) -> bool {
    let text = !is_binary(path) && is_file_human_readable(path);
    log::info!("is_file_text({}) : {text}", path.display());
    text
}

/// La taille du fichier est non nulle et inférieure à [`MAX_SIZE_FILE`].
pub fn is_not_too_big(path: &Path) -> bool {
    file_size(path).is_some_and(|size| size > 0 && size < MAX_SIZE_FILE)
}

/// Le fichier contient au moins [`MIN_LINE_COUNT`] lignes non vides.
pub fn has_enough_lines(path: &Path) -> bool {
    let Ok(bytes) = fs::read(path) else {
        return false;
    };
    let count = decode_text(&bytes).lines().filter(|line| !line.is_empty()).count();
    log::debug!("has_enough_lines : {count} / {MIN_LINE_COUNT}");
    count >= MIN_LINE_COUNT
}

/// Le fichier contient moins de [`MAX_LINE_COUNT`] lignes.
pub fn has_not_too_many_lines(path: &Path) -> bool {
    let Ok(bytes) = fs::read(path) else {
        return false;
    };
    let count = decode_text(&bytes).lines().count();
    log::debug!("has_not_too_many_lines : {count} / {MAX_LINE_COUNT}");
    count < MAX_LINE_COUNT
}
