//! Journal d'activité, activé par l'argument `log` : le fichier
//! `csv2listes_log.txt` est créé dans le dossier Documents de l'utilisateur.

use std::fs::{self, File, OpenOptions};
use std::io::Write;
use std::path::{Path, PathBuf};
use std::sync::Mutex;

use csv2listes::{MAX_LINE_COUNT, MAX_SIZE_FILE, MAX_SIZE_LINE, MIN_LINE_COUNT};

const FILE_NAME: &str = "csv2listes_log.txt";
/// Au-delà de cette taille, le journal est archivé au démarrage.
const MAX_SIZE: u64 = 5 * 1024 * 1024;
/// Nombre de journaux archivés conservés (`csv2listes_log.1.txt`, …).
const MAX_FILES: usize = 3;

struct FileLogger {
    file: Mutex<File>,
}

impl log::Log for FileLogger {
    fn enabled(&self, metadata: &log::Metadata) -> bool {
        // Seuls les messages du logiciel sont conservés, pas ceux des bibliothèques
        metadata.target().starts_with("csv2listes")
    }

    fn log(&self, record: &log::Record) {
        if !self.enabled(record.metadata()) {
            return;
        }
        let now = chrono::Local::now().format("%d/%m/%Y %H:%M:%S%.3f %z");
        if let Ok(mut file) = self.file.lock() {
            let _ = writeln!(file, "{now}\t{}\t{}\t{}", record.level(), std::process::id(), record.args());
        }
    }

    fn flush(&self) {
        if let Ok(mut file) = self.file.lock() {
            let _ = file.flush();
        }
    }
}

pub fn init() {
    let directory = dirs::document_dir()
        .or_else(dirs::home_dir)
        .unwrap_or_else(std::env::temp_dir);
    let path = directory.join(FILE_NAME);
    rotate(&path);
    let file = match OpenOptions::new().create(true).append(true).open(&path) {
        Ok(file) => file,
        Err(error) => {
            eprintln!("Impossible de créer le journal {} : {error}", path.display());
            return;
        }
    };
    if log::set_boxed_logger(Box::new(FileLogger { file: Mutex::new(file) })).is_ok() {
        log::set_max_level(log::LevelFilter::Debug);
    }
    log::info!("Journal : {}", path.display());
    log::info!("MAX_SIZE_LINE : {MAX_SIZE_LINE}");
    log::info!("MAX_SIZE_FILE : {MAX_SIZE_FILE}");
    log::info!("MIN_LINE_COUNT : {MIN_LINE_COUNT}");
    log::info!("MAX_LINE_COUNT : {MAX_LINE_COUNT}");
}

/// Archive le journal s'il est trop gros : `.txt` → `.1.txt` → `.2.txt`…
fn rotate(path: &Path) {
    if fs::metadata(path).map_or(true, |metadata| metadata.len() < MAX_SIZE) {
        return;
    }
    for index in (1..MAX_FILES).rev() {
        let _ = fs::rename(archive(path, index), archive(path, index + 1));
    }
    let _ = fs::rename(path, archive(path, 1));
}

fn archive(path: &Path, index: usize) -> PathBuf {
    path.with_extension(format!("{index}.txt"))
}
