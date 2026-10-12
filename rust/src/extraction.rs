//! Déroulement d'une extraction : vérification du fichier, questions posées à
//! l'utilisateur puis construction des listes Python.
//!
//! L'extraction ne dépend pas de l'interface graphique : elle renvoie des
//! [`Step`] (question, erreur, résultat) et reçoit les réponses par
//! [`Extraction::answer`].

use std::path::Path;

use crate::memstream::MemStream;
use crate::strings::split;
use crate::{MAX_SIZE_FILE, files, python};

/// Réglages choisis dans la fenêtre principale.
#[derive(Debug, Clone)]
pub struct Settings {
    pub has_header: bool,
    pub nb_columns: usize,
    pub separator: String,
}

/// Réponse de l'utilisateur à une [`Question`].
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Answer {
    Yes,
    No,
    Cancel,
}

/// Ce que l'on demande à l'utilisateur à propos d'une ligne.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Target {
    Header,
    FirstData,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Question {
    pub target: Target,
    /// Numéro de la ligne (à partir de 1).
    pub number: usize,
    pub line: String,
}

impl Question {
    pub fn prompt(&self) -> String {
        match self.target {
            Target::Header => format!("Est-ce que la ligne n°{} correspond aux entêtes :", self.number),
            Target::FirstData => format!(
                "Est-ce que la ligne n°{} correspond à la première ligne de données :",
                self.number
            ),
        }
    }
}

/// Listes Python prêtes à être copiées.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Output {
    pub text: String,
    pub nb_columns: usize,
    pub nb_values: usize,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Step {
    Ask(Question),
    Error(String),
    Cancelled,
    Done(Output),
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Phase {
    Header,
    FirstData,
    Data,
    Finished,
}

#[derive(Debug)]
pub struct Extraction {
    settings: Settings,
    stream: MemStream,
    phase: Phase,
    /// Dernière ligne présentée à l'utilisateur.
    pending_line: String,
    names: Vec<String>,
    rows: Vec<Vec<String>>,
}

impl Extraction {
    /// Vérifie le fichier puis le charge en mémoire.
    pub fn open(path: &Path, settings: Settings) -> Result<Self, String> {
        log::info!("Extraction de {} avec {settings:?}", path.display());
        if settings.separator.is_empty() {
            return Err(fail("Le délimiteur personnalisé est vide."));
        }
        if !files::file_exists_and_readable(path) {
            return Err(fail("Le fichier n'est pas accessible ou n'est pas lisible."));
        }
        match files::file_size(path) {
            None => return Err(fail("Impossible de détecter la taille du fichier.")),
            Some(size) if size > MAX_SIZE_FILE => {
                return Err(fail(
                    "La taille du fichier est trop importante pour ce programme pédagogique.",
                ));
            }
            Some(_) => {}
        }
        if !files::is_file_text(path) {
            return Err(fail("Le fichier n'est pas un fichier texte."));
        }
        if !files::has_enough_lines(path) {
            return Err(fail("Le fichier texte ne contient pas assez de lignes."));
        }
        if !files::has_not_too_many_lines(path) {
            return Err(fail(
                "Le fichier texte contient trop de lignes pour ce programme pédagogique.",
            ));
        }
        let stream =
            MemStream::load(path).map_err(|_| fail("Impossible de charger en mémoire le fichier."))?;
        if !stream.check_max_size_line() {
            return Err(fail(
                "Une ligne est plus longue que prévu par ce programme pédagogique.",
            ));
        }

        // Sans ligne d'entêtes, les colonnes reçoivent un nom par défaut
        let (phase, names) = if settings.has_header {
            (Phase::Header, Vec::new())
        } else {
            let names = (1..=settings.nb_columns).map(|i| format!("Liste_{i}")).collect();
            (Phase::FirstData, names)
        };
        Ok(Self {
            settings,
            stream,
            phase,
            pending_line: String::new(),
            names,
            rows: Vec::new(),
        })
    }

    /// Avance jusqu'à la prochaine question ou jusqu'à la fin de l'extraction.
    pub fn next_step(&mut self) -> Step {
        match self.phase {
            Phase::Header | Phase::FirstData => {
                let target = if self.phase == Phase::Header {
                    Target::Header
                } else {
                    Target::FirstData
                };
                let Some(line) = self.stream.read_next_line(false) else {
                    self.phase = Phase::Finished;
                    return Step::Error(fail(match target {
                        Target::Header => "Fin du fichier, aucun entête choisi.",
                        Target::FirstData => "Fin du fichier, aucune ligne de données choisie.",
                    }));
                };
                self.pending_line = line.clone();
                let question = Question {
                    target,
                    number: self.stream.line_number(),
                    line,
                };
                log::info!("Question : {} {:?}", question.prompt(), question.line);
                Step::Ask(question)
            }
            Phase::Data => {
                while let Some(line) = self.stream.read_next_line(true) {
                    if let Err(message) = self.add_row(&line) {
                        self.phase = Phase::Finished;
                        return Step::Error(message);
                    }
                }
                self.phase = Phase::Finished;
                Step::Done(self.build_output())
            }
            Phase::Finished => Step::Cancelled,
        }
    }

    /// Traite la réponse à la dernière question posée.
    pub fn answer(&mut self, answer: Answer) -> Step {
        log::debug!("Réponse : {answer:?}");
        match (answer, self.phase) {
            (_, Phase::Data | Phase::Finished) => Step::Cancelled,
            (Answer::Cancel, _) => {
                self.phase = Phase::Finished;
                Step::Cancelled
            }
            (Answer::No, _) => self.next_step(),
            (Answer::Yes, Phase::Header) => {
                log::debug!("Entêtes sélectionnés à la ligne {}", self.stream.line_number());
                let names = split(&self.pending_line, &self.settings.separator);
                if names.len() != self.settings.nb_columns {
                    self.phase = Phase::Finished;
                    return Step::Error(fail(column_mismatch(names.len(), self.settings.nb_columns)));
                }
                self.names = python::to_python_vars(names);
                self.phase = Phase::FirstData;
                self.next_step()
            }
            (Answer::Yes, Phase::FirstData) => {
                log::debug!("Première ligne de données : {}", self.stream.line_number());
                let line = std::mem::take(&mut self.pending_line);
                if let Err(message) = self.add_row(&line) {
                    self.phase = Phase::Finished;
                    return Step::Error(message);
                }
                self.phase = Phase::Data;
                self.next_step()
            }
        }
    }

    fn add_row(&mut self, line: &str) -> Result<(), String> {
        let values = split(line, &self.settings.separator);
        if values.len() != self.settings.nb_columns {
            return Err(fail(format!(
                "Ligne {} : {line}\n{}",
                self.stream.line_number(),
                column_mismatch(values.len(), self.settings.nb_columns)
            )));
        }
        self.rows.push(python::to_python_values(values));
        Ok(())
    }

    fn build_output(&self) -> Output {
        log::info!(
            "Préparation des listes : {} colonnes, {} lignes",
            self.names.len(),
            self.rows.len()
        );
        let mut text = String::new();
        for (column, name) in self.names.iter().enumerate() {
            let values: Vec<&str> = self.rows.iter().map(|row| row[column].as_str()).collect();
            text.push_str(&python::to_pep8(&format!("{name} = [{}]", values.join(", "))));
            text.push('\n');
        }
        Output {
            text,
            nb_columns: self.names.len(),
            nb_values: self.rows.len(),
        }
    }
}

fn column_mismatch(counted: usize, expected: usize) -> String {
    format!(
        "Le nombre de colonnes ne correspond pas !\nColonnes comptées : {counted}\nColonnes indiquées : {expected}"
    )
}

/// Journalise un message d'erreur et le renvoie.
fn fail(message: impl Into<String>) -> String {
    let message = message.into();
    log::error!("Erreur : {message}");
    message
}
