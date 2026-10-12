# csv2listes pour Linux (Rust) 🐧

Portage en Rust de csv2listes pour Linux. La version Windows d'origine (C++ / Win32) se trouve à la racine du dépôt.

L'utilisation est la même : régler les étapes 1 et 2, ouvrir le fichier CSV, répondre aux questions, puis coller les listes avec Ctrl + V.

## Prérequis ⚙️
- Rust 1.88 ou plus récent (édition 2024) :
  - Fedora : `sudo dnf install cargo`
  - Debian / Ubuntu : `sudo apt install build-essential`, puis Rust via [rustup](https://rustup.rs) (la version des dépôts est souvent trop ancienne)
- Un bureau avec le portail XDG (`xdg-desktop-portal`, installé d'office avec GNOME et KDE) pour la fenêtre « Ouvrir un fichier ». Sinon, `zenity` est utilisé.

## Lancer le programme 🦾
```bash
cd rust
cargo run --release
```
La première compilation prend une ou deux minutes. Le programme compilé se trouve ensuite dans `target/release/csv2listes` et peut être lancé directement.

Pour obtenir un journal d'activité, ajoutez l'argument `log` :
```bash
cargo run --release -- log
```
Le fichier `csv2listes_log.txt` est créé dans votre dossier Documents.

Dans les questions, Entrée répond « Oui » et Échap « Annuler ».

## Installation (facultative) 📦
Ces commandes ajoutent csv2listes au menu des applications :
```bash
cargo install --path . --root ~/.local
sed "s|^Exec=.*|Exec=$HOME/.local/bin/csv2listes|" assets/csv2listes.desktop > ~/.local/share/applications/csv2listes.desktop
install -Dm644 assets/csv2listes.png ~/.local/share/icons/hicolor/256x256/apps/csv2listes.png
```

## Tests 🕸️
```bash
cargo test
```
Les tests reprennent ceux de `tests/tests_csv2list.cpp` et utilisent les mêmes fichiers (`../tests/assets`). Les tests du presse-papiers ne sont pas repris : la copie passe par la fenêtre.

## Organisation du code
| Fichier | Rôle | Équivalent C++ |
|---|---|---|
| `src/main.rs` | Fenêtre, boîtes de dialogue, presse-papiers | `main.cpp`, `src_clipboardutils.cpp` |
| `src/extraction.rs` | Déroulement de l'extraction (vérifications, questions, listes) | `ExtractData()` de `main.cpp` |
| `src/files.rs` | Vérifications sur le fichier | `src_filesutils.cpp`, `src_functions.cpp` |
| `src/memstream.rs` | Fichier chargé en mémoire et lu ligne par ligne | `src_functions.cpp` |
| `src/python.rs` | Noms de variables, valeurs, découpage PEP 8 | `src_pythonutils.cpp` |
| `src/strings.rs` | Découpage des lignes, décodage du texte | `src_stringutils.cpp` |
| `src/logger.rs` | Journal d'activité | spdlog |

Les limites sont définies dans `src/lib.rs` :
```rust
pub const MAX_SIZE_LINE: usize = 1024;
pub const MIN_LINE_COUNT: usize = 2;
pub const MAX_LINE_COUNT: usize = 67000;
pub const MAX_SIZE_FILE: u64 = 1_000_000;
```

## Différences avec la version Windows
- Les fichiers en UTF-8 (avec ou sans BOM) et en Latin-1 sont acceptés, avec des fins de ligne Windows ou Linux.
- Un délimiteur personnalisé vide affiche une erreur (la version C++ bouclait sans fin).
- Si aucune ligne de données n'est choisie, une erreur s'affiche au lieu de listes vides.
- Les guillemets et antislashs des chaînes de caractères sont échappés pour que le code Python reste valide.
- Un entête vide donne le nom `Liste_N`, et un caractère accentué est remplacé par un seul `_`.
- Le journal s'active avec l'argument `log` ou `debug` (la version C++ l'activait pour n'importe quel argument sauf `debug`).
- Quelques fautes d'orthographe sont corrigées dans les messages.
