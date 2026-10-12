//! Conversion des données vers la syntaxe Python.

use std::collections::HashSet;

/// Longueur maximale d'une ligne selon la PEP 8.
const PEP8_MAX_LINE: usize = 79;

/// Coupe une liste Python trop longue sur plusieurs lignes (PEP 8).
pub fn to_pep8(input: &str) -> String {
    if input.len() <= PEP8_MAX_LINE {
        return input.to_owned();
    }
    let (Some(start), Some(end)) = (input.find('['), input.find(']')) else {
        return input.to_owned();
    };
    if end < start {
        return input.to_owned();
    }

    let mut result = input[..=start].to_owned();
    let mut items: Vec<&str> = input[start + 1..end].split(',').collect();
    if items.last() == Some(&"") {
        items.pop();
    }
    let mut cuts = 0;
    for item in items {
        // Longueur de la ligne en cours, comptée comme dans la version C++
        let line_len = match result.rfind('\n') {
            Some(newline) => result.len() - newline,
            None => result.len() + 1,
        };
        if line_len + item.len() + 1 > PEP8_MAX_LINE {
            result.push_str("\n    ");
            cuts += 1;
        }
        result.push_str(item);
        result.push(',');
    }
    if result.ends_with(',') {
        result.pop();
    }
    result.push_str(&input[end..]);
    log::debug!("to_pep8 : {cuts} découpages");
    result
}

/// Transforme les entêtes en noms de variables Python valides et uniques.
///
/// Les caractères non alphanumériques sont remplacés par `_`, un nom commençant
/// par un chiffre est préfixé par `_` et les doublons sont numérotés.
pub fn to_python_vars(names: Vec<String>) -> Vec<String> {
    let mut seen = HashSet::new();
    names
        .into_iter()
        .enumerate()
        .map(|(index, name)| {
            let mut var: String = name
                .chars()
                .map(|c| if c.is_ascii_alphanumeric() { c } else { '_' })
                .collect();
            if var.is_empty() {
                var = format!("Liste_{}", index + 1);
            } else if var.starts_with(|c: char| c.is_ascii_digit()) {
                var.insert(0, '_');
            }
            let original = var.clone();
            let mut counter = 1;
            while seen.contains(&var) {
                var = format!("{original}_{counter}");
                counter += 1;
            }
            seen.insert(var.clone());
            var
        })
        .collect()
}

/// Transforme les valeurs lues en éléments de liste Python.
///
/// Les virgules deviennent des points ; ce qui n'est pas un nombre est placé
/// entre guillemets.
pub fn to_python_values(values: Vec<String>) -> Vec<String> {
    values
        .into_iter()
        .map(|value| {
            let value = value.replace(',', ".");
            if is_valid_python_number(&value) {
                value
            } else {
                format!("\"{}\"", value.replace('\\', "\\\\").replace('"', "\\\""))
            }
        })
        .collect()
}

/// Vérifie si une chaîne est un nombre Python valide (entier, flottant ou complexe).
pub fn is_valid_python_number(s: &str) -> bool {
    let compact: String = s.chars().filter(|c| !c.is_whitespace()).collect();
    if is_decimal(&compact) {
        log::debug!("is_valid_python_number({s:?}) : true (réel)");
        return true;
    }

    // Nombre complexe : partie réelle, signe, partie imaginaire suivie de « j »
    let sign = compact
        .get(1..)
        .and_then(|rest| rest.find('+').or_else(|| rest.find('-')))
        .map(|position| position + 1);
    if let Some(sign) = sign
        && compact.ends_with('j')
    {
        let real = &compact[..sign];
        let imaginary = &compact[sign + 1..compact.len() - 1];
        if is_decimal(real) && is_decimal(imaginary) {
            log::debug!("is_valid_python_number({s:?}) : true (complexe)");
            return true;
        }
    }
    log::debug!("is_valid_python_number({s:?}) : false");
    false
}

/// Nombre écrit en notation décimale : `12`, `-3.5`, `.5`, `1.23e5`…
fn is_decimal(s: &str) -> bool {
    s.bytes().any(|byte| byte.is_ascii_digit())
        && s.bytes().all(|byte| byte.is_ascii_digit() || matches!(byte, b'+' | b'-' | b'.' | b'e' | b'E'))
        && s.parse::<f64>().is_ok()
}
