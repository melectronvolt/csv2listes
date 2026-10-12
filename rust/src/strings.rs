//! Fonctions utilitaires sur les chaînes de caractères.

/// Convertit le contenu brut d'un fichier en texte.
///
/// Le contenu est lu en UTF-8 (sans l'éventuel BOM). S'il n'est pas valide,
/// il est lu en Latin-1, encodage encore utilisé par d'anciens logiciels Windows.
pub fn decode_text(bytes: &[u8]) -> String {
    let bytes = bytes.strip_prefix(b"\xEF\xBB\xBF").unwrap_or(bytes);
    match std::str::from_utf8(bytes) {
        Ok(text) => text.to_owned(),
        Err(_) => {
            log::debug!("Fichier non UTF-8 : lecture en Latin-1");
            bytes.iter().map(|&byte| char::from(byte)).collect()
        }
    }
}

/// Découpe une chaîne selon un délimiteur.
///
/// Comme dans la version C++ d'origine, un délimiteur placé en toute fin de
/// ligne ne crée pas de colonne vide : `"a;b;"` donne `["a", "b"]`.
pub fn split(s: &str, delimiter: &str) -> Vec<String> {
    if delimiter.is_empty() {
        return vec![s.to_owned()];
    }
    let mut tokens: Vec<String> = s.split(delimiter).map(str::to_owned).collect();
    if tokens.len() > 1 && tokens.last().is_some_and(String::is_empty) {
        tokens.pop();
    }
    log::debug!("split {s:?} avec {delimiter:?} : {} morceaux", tokens.len());
    tokens
}
