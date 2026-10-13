//! Interface graphique de csv2listes.

// Pas de console au lancement sous Windows
#![cfg_attr(windows, windows_subsystem = "windows")]

mod logger;

use std::path::{Path, PathBuf};
use std::sync::mpsc::{self, Receiver, TryRecvError};

use csv2listes::extraction::{Answer, Extraction, Question, Settings, Step};
use eframe::egui;

const APP_ID: &str = "csv2listes";
const TITLE: &str = "Convertisseur : fichier CSV -> listes en Python";
const COPYRIGHT: &str = "Rémi MEVAERE© 2024";
/// Numérotation à 4 chiffres de la version Windows d'origine (0.0.9 dans Cargo → 0.0.0.9).
const VERSION: &str = concat!("0.", env!("CARGO_PKG_VERSION"));
const WEBSITE: &str = "https://sciences-physiques.net/softwares/csv2listes";
const DESCRIPTION: &str = "\
- Permet de convertir les colonnes d'un fichier CSV en listes pour Python ;
- Pour l'export : choisir un séparateur décimal différent de celui du CSV ;
- Suivre les étapes puis ouvrir le fichier désiré ;
- Les listes en Python seront stockées dans le presse-papiers.";

fn main() -> eframe::Result {
    if std::env::args().skip(1).any(|arg| matches!(arg.as_str(), "log" | "--log" | "debug")) {
        logger::init();
    }
    log::info!("Démarrage du logiciel, version {VERSION}");
    for (index, arg) in std::env::args().enumerate() {
        log::info!("Argument {index} : {arg}");
    }

    let mut viewport = egui::ViewportBuilder::default()
        .with_title(TITLE)
        .with_app_id(APP_ID)
        .with_inner_size([540.0, 400.0])
        .with_resizable(false)
        .with_maximize_button(false);
    match eframe::icon_data::from_png_bytes(include_bytes!("../assets/csv2listes.png")) {
        Ok(icon) => viewport = viewport.with_icon(icon),
        Err(error) => log::error!("Icône illisible : {error}"),
    }
    let options = eframe::NativeOptions {
        viewport,
        ..Default::default()
    };
    eframe::run_native(APP_ID, options, Box::new(|_| Ok(Box::new(App::default()))))
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Delimiter {
    Semicolon,
    Comma,
    Tab,
    Space,
    Pipe,
    Custom,
}

enum Dialog {
    Question(Question),
    Message {
        title: &'static str,
        text: String,
        error: bool,
    },
    About,
}

impl Dialog {
    fn error(text: String) -> Self {
        Self::Message {
            title: "Erreur",
            text,
            error: true,
        }
    }
}

enum DialogAction {
    Answer(Answer),
    Close,
}

struct App {
    has_header: bool,
    nb_columns: usize,
    delimiter: Delimiter,
    custom_delimiter: String,
    /// Résultat du sélecteur de fichier, qui s'exécute dans un autre thread.
    file_picker: Option<Receiver<Option<PathBuf>>>,
    extraction: Option<Extraction>,
    dialog: Option<Dialog>,
}

impl Default for App {
    fn default() -> Self {
        Self {
            has_header: true,
            nb_columns: 3,
            delimiter: Delimiter::Semicolon,
            custom_delimiter: ";".to_owned(),
            file_picker: None,
            extraction: None,
            dialog: None,
        }
    }
}

impl eframe::App for App {
    fn logic(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        self.poll_file_picker(ctx);
    }

    fn ui(&mut self, ui: &mut egui::Ui, _frame: &mut eframe::Frame) {
        let ctx = ui.ctx().clone();

        egui::Panel::bottom("status_bar").show(ui, |ui| {
            ui.horizontal(|ui| {
                let status = ui
                    .add(egui::Label::new(format!("{COPYRIGHT} - {VERSION}")).sense(egui::Sense::click()))
                    .on_hover_cursor(egui::CursorIcon::PointingHand)
                    .on_hover_text("Plus d'informations");
                let about = ui.with_layout(egui::Layout::right_to_left(egui::Align::Center), |ui| {
                    ui.link("À propos")
                });
                if status.clicked() || about.inner.clicked() {
                    log::info!("GUI : ouverture de « À propos ».");
                    self.dialog = Some(Dialog::About);
                }
            });
        });

        egui::CentralPanel::default().show(ui, |ui| {
            section(ui, "Description :", |ui| {
                ui.label(DESCRIPTION);
            });
            section(ui, "Etape 1 : fichier CSV", |ui| self.csv_config_ui(ui));
            section(ui, "Etape 2 : Choix du Délimiteur CSV", |ui| self.delimiter_ui(ui));
            section(ui, "Etape 3 : Choisir un fichier", |ui| {
                let busy = self.file_picker.is_some() || self.extraction.is_some();
                let button = egui::Button::new("Ouvrir le fichier CSV et convertir en listes pour python.")
                    .min_size(egui::vec2(ui.available_width(), 40.0));
                if ui.add_enabled(!busy, button).clicked() {
                    self.pick_file(&ctx);
                }
            });
        });

        self.dialog_ui(&ctx);
    }
}

impl App {
    fn csv_config_ui(&mut self, ui: &mut egui::Ui) {
        let header = ui.checkbox(
            &mut self.has_header,
            "Il existe une ligne donnant le titre des colonnes dans le fichier CSV.",
        );
        if header.changed() {
            log::debug!("has_header = {}", self.has_header);
        }
        ui.horizontal(|ui| {
            ui.label("Nombre de colonnes (descripteurs), dans le fichier CSV :");
            let columns = ui.add(egui::DragValue::new(&mut self.nb_columns).range(1..=999));
            if columns.changed() {
                log::debug!("nb_columns = {}", self.nb_columns);
            }
        });
    }

    fn delimiter_ui(&mut self, ui: &mut egui::Ui) {
        let previous = self.delimiter;
        egui::Grid::new("delimiters")
            .num_columns(3)
            .min_col_width(150.0)
            .show(ui, |ui| {
                ui.radio_value(&mut self.delimiter, Delimiter::Semicolon, "Point virgule");
                ui.radio_value(&mut self.delimiter, Delimiter::Comma, "Virgule");
                ui.radio_value(&mut self.delimiter, Delimiter::Tab, "Tabulation");
                ui.end_row();
                ui.radio_value(&mut self.delimiter, Delimiter::Space, "Espace");
                ui.radio_value(&mut self.delimiter, Delimiter::Pipe, "Barre verticale |");
                ui.horizontal(|ui| {
                    ui.radio_value(&mut self.delimiter, Delimiter::Custom, "Autre :");
                    let custom = egui::TextEdit::singleline(&mut self.custom_delimiter)
                        .char_limit(5)
                        .desired_width(50.0);
                    ui.add_enabled(self.delimiter == Delimiter::Custom, custom);
                });
                ui.end_row();
            });
        if self.delimiter != previous {
            log::debug!("Séparateur : {:?}", self.separator());
        }
    }

    fn separator(&self) -> String {
        match self.delimiter {
            Delimiter::Semicolon => ";",
            Delimiter::Comma => ",",
            Delimiter::Tab => "\t",
            Delimiter::Space => " ",
            Delimiter::Pipe => "|",
            Delimiter::Custom => &self.custom_delimiter,
        }
        .to_owned()
    }

    fn pick_file(&mut self, ctx: &egui::Context) {
        log::info!("GUI : le bouton d'ouverture a été cliqué.");
        let (sender, receiver) = mpsc::channel();
        let ctx = ctx.clone();
        // Le sélecteur bloque jusqu'au choix : il ne doit pas figer la fenêtre
        std::thread::spawn(move || {
            let path = rfd::FileDialog::new()
                .set_title("Ouvrir un fichier CSV")
                .add_filter("Fichiers CSV et Texte (*.csv, *.txt)", &["csv", "CSV", "txt", "TXT"])
                .add_filter("Fichiers CSV (*.csv)", &["csv", "CSV"])
                .add_filter("Fichiers Texte (*.txt)", &["txt", "TXT"])
                .add_filter("Tous les fichiers", &["*"])
                .pick_file();
            let _ = sender.send(path);
            ctx.request_repaint();
        });
        self.file_picker = Some(receiver);
    }

    fn poll_file_picker(&mut self, ctx: &egui::Context) {
        let Some(receiver) = &self.file_picker else {
            return;
        };
        match receiver.try_recv() {
            Ok(Some(path)) => {
                self.file_picker = None;
                self.start_extraction(&path, ctx);
            }
            Ok(None) | Err(TryRecvError::Disconnected) => self.file_picker = None,
            Err(TryRecvError::Empty) => {}
        }
    }

    fn start_extraction(&mut self, path: &Path, ctx: &egui::Context) {
        log::info!("Fichier sélectionné : {}", path.display());
        let settings = Settings {
            has_header: self.has_header,
            nb_columns: self.nb_columns,
            separator: self.separator(),
        };
        match Extraction::open(path, settings) {
            Ok(mut extraction) => {
                let step = extraction.next_step();
                self.extraction = Some(extraction);
                self.handle_step(step, ctx);
            }
            Err(message) => self.dialog = Some(Dialog::error(message)),
        }
    }

    /// Affiche la suite de l'extraction : question, erreur ou résultat.
    fn handle_step(&mut self, step: Step, ctx: &egui::Context) {
        match step {
            Step::Ask(question) => self.dialog = Some(Dialog::Question(question)),
            Step::Error(message) => {
                self.extraction = None;
                self.dialog = Some(Dialog::error(message));
            }
            Step::Cancelled => {
                log::info!("Extraction annulée.");
                self.extraction = None;
            }
            Step::Done(output) => {
                self.extraction = None;
                ctx.copy_text(output.text);
                self.dialog = Some(Dialog::Message {
                    title: "Succès",
                    text: format!(
                        "Le programme a copié dans le presse-papiers :\n{} colonnes de {} valeurs.",
                        output.nb_columns, output.nb_values
                    ),
                    error: false,
                });
            }
        }
    }

    fn dialog_ui(&mut self, ctx: &egui::Context) {
        let Some(dialog) = &self.dialog else {
            return;
        };
        let mut action = None;
        egui::Modal::new(egui::Id::new("dialog")).show(ctx, |ui| {
            ui.set_width(440.0);
            match dialog {
                Dialog::Question(question) => {
                    ui.heading("Question");
                    ui.label(question.prompt());
                    egui::Frame::group(ui.style()).show(ui, |ui| {
                        ui.set_width(ui.available_width());
                        if question.line.trim().is_empty() {
                            ui.label(egui::RichText::new("(ligne vide)").italics().weak());
                        } else {
                            ui.label(egui::RichText::new(question.line.replace('\t', "    ")).monospace());
                        }
                    });
                    ui.add_space(4.0);
                    ui.horizontal(|ui| {
                        if dialog_button(ui, "Oui") {
                            action = Some(DialogAction::Answer(Answer::Yes));
                        }
                        if dialog_button(ui, "Non") {
                            action = Some(DialogAction::Answer(Answer::No));
                        }
                        if dialog_button(ui, "Annuler") {
                            action = Some(DialogAction::Answer(Answer::Cancel));
                        }
                    });
                }
                Dialog::Message { title, text, error } => {
                    let mut heading = egui::RichText::new(*title).heading();
                    if *error {
                        heading = heading.color(ui.visuals().error_fg_color);
                    }
                    ui.label(heading);
                    ui.label(text);
                    ui.add_space(4.0);
                    if dialog_button(ui, "OK") {
                        action = Some(DialogAction::Close);
                    }
                }
                Dialog::About => {
                    ui.heading("À propos");
                    ui.label(egui::RichText::new(format!("csv2listes {VERSION}")).strong());
                    ui.label(TITLE);
                    ui.add_space(4.0);
                    ui.label(format!("Auteur : {}", env!("CARGO_PKG_AUTHORS")));
                    ui.label(format!("{COPYRIGHT} - Licence {}", env!("CARGO_PKG_LICENSE")));
                    ui.hyperlink_to("sciences-physiques.net", WEBSITE);
                    ui.add_space(4.0);
                    if dialog_button(ui, "Fermer") {
                        action = Some(DialogAction::Close);
                    }
                }
            }
        });

        // Clavier : Entrée = Oui / OK, Échap = Annuler / fermer
        if action.is_none() {
            let (enter, escape) = ctx.input_mut(|input| {
                (
                    input.consume_key(egui::Modifiers::NONE, egui::Key::Enter),
                    input.consume_key(egui::Modifiers::NONE, egui::Key::Escape),
                )
            });
            action = match dialog {
                Dialog::Question(_) if enter => Some(DialogAction::Answer(Answer::Yes)),
                Dialog::Question(_) if escape => Some(DialogAction::Answer(Answer::Cancel)),
                Dialog::About | Dialog::Message { .. } if enter || escape => Some(DialogAction::Close),
                _ => None,
            };
        }

        match action {
            Some(DialogAction::Answer(answer)) => {
                self.dialog = None;
                if let Some(extraction) = &mut self.extraction {
                    let step = extraction.answer(answer);
                    self.handle_step(step, ctx);
                }
            }
            Some(DialogAction::Close) => self.dialog = None,
            None => {}
        }
    }
}

/// Cadre titré, équivalent des « group box » de la version Windows.
fn section(ui: &mut egui::Ui, title: &str, add_contents: impl FnOnce(&mut egui::Ui)) {
    egui::Frame::group(ui.style()).show(ui, |ui| {
        ui.set_width(ui.available_width());
        ui.label(egui::RichText::new(title).strong());
        ui.add_space(2.0);
        add_contents(ui);
    });
    ui.add_space(6.0);
}

fn dialog_button(ui: &mut egui::Ui, text: &str) -> bool {
    ui.add(egui::Button::new(text).min_size(egui::vec2(80.0, 0.0))).clicked()
}
