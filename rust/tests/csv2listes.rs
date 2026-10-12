//! Portage des tests Catch2 de `tests/tests_csv2list.cpp`, complétés par des
//! tests de l'extraction complète. Les fichiers de test sont partagés avec la
//! version C++ (`../tests/assets`).

use std::path::PathBuf;

use csv2listes::extraction::{Answer, Extraction, Settings, Step, Target};
use csv2listes::files::*;
use csv2listes::memstream::MemStream;
use csv2listes::python::*;
use csv2listes::strings::{decode_text, split};

fn asset(name: &str) -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("../tests/assets").join(name)
}

fn strings(values: &[&str]) -> Vec<String> {
    values.iter().map(|value| value.to_string()).collect()
}

// ----- Python -----

#[test]
fn pep8_short() {
    let input = "random_numbers = [647, 237, 93]";
    assert_eq!(to_pep8(input), input);
}

#[test]
fn pep8_long() {
    let values: Vec<String> = (0..500).map(|i| (100 + i * 7 % 900).to_string()).collect();
    let input = format!("random_numbers = [{}]", values.join(", "));
    let output = to_pep8(&input);
    assert_ne!(output, input);
    assert!(output.lines().all(|line| line.len() <= 79));
    assert_eq!(output.replace("\n    ", ""), input);
}

#[test]
fn to_python_var() {
    let words = strings(&["bonjour", "0hello", "bonjour", "welc0me", "b@nzai", "twOtwo", "hello(r)"]);
    let variables = strings(&["bonjour", "_0hello", "bonjour_1", "welc0me", "b_nzai", "twOtwo", "hello_r_"]);
    assert_eq!(to_python_vars(words), variables);
}

#[test]
fn to_python_var_empty_and_accents() {
    let words = strings(&["", "FIZéZIQ", "T(s)"]);
    assert_eq!(to_python_vars(words), strings(&["Liste_1", "FIZ_ZIQ", "T_s_"]));
}

#[test]
fn valid_python_number() {
    assert!(is_valid_python_number("1.23e5"));
    assert!(is_valid_python_number("123000.0"));
    assert!(is_valid_python_number("-10"));
    assert!(is_valid_python_number("2 + 3j"));
    assert!(is_valid_python_number("3.14"));
    assert!(!is_valid_python_number("1.23f5"));
    assert!(!is_valid_python_number("123000,0"));
    assert!(!is_valid_python_number("-+10"));
    assert!(!is_valid_python_number("2 + 3h"));
    assert!(!is_valid_python_number("3,14"));
    assert!(!is_valid_python_number("inf"));
    assert!(!is_valid_python_number("NaN"));
    assert!(!is_valid_python_number(""));
}

#[test]
fn transform_the_python_list() {
    let words = strings(&["5", "0hello", "4e6", "welc0me", "b@nzai", "twOtwo", "4,5", "a\"b"]);
    let values = strings(&["5", "\"0hello\"", "4e6", "\"welc0me\"", "\"b@nzai\"", "\"twOtwo\"", "4.5", "\"a\\\"b\""]);
    assert_eq!(to_python_values(words), values);
}

// ----- Découpage -----

#[test]
fn split_simple() {
    assert_eq!(split("9;0,297;0,703;0,146", ";").len(), 4);
}

#[test]
fn split_like_cpp() {
    assert_eq!(split("a;b;", ";"), strings(&["a", "b"]));
    assert_eq!(split("a;;", ";"), strings(&["a", ""]));
    assert_eq!(split(";a", ";"), strings(&["", "a"]));
    assert_eq!(split("", ";"), strings(&[""]));
    assert_eq!(split("a", ";"), strings(&["a"]));
    assert_eq!(split("a<>b", "<>"), strings(&["a", "b"]));
    assert_eq!(split("a;b", ""), strings(&["a;b"]));
}

#[test]
fn decode_utf8_latin1_bom() {
    assert_eq!(decode_text("FIZéZIQ".as_bytes()), "FIZéZIQ");
    assert_eq!(decode_text(b"\xEF\xBB\xBFt;x"), "t;x");
    assert_eq!(decode_text(b"FIZ\xE9ZIQ"), "FIZéZIQ");
}

// ----- Chargement en mémoire -----

#[test]
fn memstream_empty() {
    let stream = MemStream::default();
    assert!(stream.is_empty());
    assert_eq!(stream.line_number(), 0);
}

#[test]
fn memstream_load_and_read() {
    let mut stream = MemStream::load(&asset("fizziq_prob.csv")).unwrap();
    assert!(!stream.is_empty());
    assert_eq!(stream.line_number(), 0);
    assert_eq!(stream.line_count(), 28);
    assert_eq!(stream.line(1), Some("Cahier-13:45"));
    assert_eq!(stream.line_number(), 0);

    assert_eq!(stream.read_next_line(false).as_deref(), Some("Cahier-13:45"));
    assert_eq!(stream.line_number(), 1);
    assert_eq!(stream.read_next_line(false).as_deref(), Some(""));
    assert_eq!(stream.line_number(), 2);
    assert_eq!(stream.read_next_line(false).as_deref(), Some("FIZéZIQ;"));
    assert_eq!(stream.line_number(), 3);

    for _ in 0..100 {
        stream.read_next_line(false);
    }
    assert_eq!(stream.read_next_line(false), None);
    assert_eq!(stream.line_number(), 28);
    assert!(stream.check_max_size_line());
}

#[test]
fn memstream_stop_on_blank_line() {
    let mut stream = MemStream::from_text("a\n\nb\n");
    assert_eq!(stream.read_next_line(true).as_deref(), Some("a"));
    assert_eq!(stream.read_next_line(true), None);
    assert_eq!(stream.line_number(), 2);
}

#[test]
fn memstream_windows_line_endings() {
    let stream = MemStream::from_text("t;x\r\n1;2\r\n");
    assert_eq!(stream.line_count(), 2);
    assert_eq!(stream.line(1), Some("t;x"));
    assert_eq!(stream.line(2), Some("1;2"));
}

#[test]
fn memstream_line_too_long() {
    let stream = MemStream::load(&asset("fizziq_prob_toolong.csv")).unwrap();
    assert!(!stream.check_max_size_line());
}

// ----- Taille et nombre de lignes -----

#[test]
fn file_exists() {
    assert!(file_exists_and_readable(&asset("fizziq_prob.csv")));
    assert!(file_exists_and_readable(&asset("bin.bin")));
    assert!(!file_exists_and_readable(&asset("fizziq_pffrob.csv")));
    assert!(!file_exists_and_readable(&asset("bidn.bin")));
    assert!(!file_exists_and_readable(&asset("")));
}

#[test]
fn file_sizes() {
    // 542 octets sous Windows, où le fichier est extrait avec des fins de ligne CRLF
    assert_eq!(file_size(&asset("fizziq_prob.csv")), Some(514));
    assert_eq!(file_size(&asset("bin.bin")), Some(87040));
    assert_eq!(file_size(&asset("absent.csv")), None);
    assert!(is_not_too_big(&asset("fizziq_prob.csv")));
    assert!(!is_not_too_big(&asset("fizziq_prob_too_big.csv")));
}

#[test]
fn line_counts() {
    assert!(has_not_too_many_lines(&asset("fizziq_prob.csv")));
    assert!(!has_not_too_many_lines(&asset("fizziq_prob_too_big.csv")));
    assert!(has_enough_lines(&asset("fizziq_prob.csv")));
    assert!(!has_enough_lines(&asset("fizziq_prob_too_less.csv")));
}

// ----- Binaire / texte -----

#[test]
fn binary_or_text() {
    assert!(is_binary(&asset("bin.bin")));
    assert!(!is_binary(&asset("nonread.txt")));
    assert!(is_file_human_readable(&asset("avimeca.txt")));
    assert!(!is_file_human_readable(&asset("nonread.txt")));
}

#[test]
fn sample_files_are_text() {
    for name in ["fizziq_prob.csv", "fizziq_dot.csv", "fizziq_comma.csv", "avimeca.txt", "avimeca_pv.txt"] {
        let path = asset(name);
        assert!(is_file_text(&path), "{name} n'est pas un fichier texte");
        let first_line = MemStream::load(&path).unwrap().line(1).map(str::to_owned);
        assert!(first_line.is_some_and(|line| !line.is_empty()), "{name} : première ligne vide");
    }
}

// ----- Extraction complète -----

fn settings(has_header: bool, nb_columns: usize, separator: &str) -> Settings {
    Settings {
        has_header,
        nb_columns,
        separator: separator.to_owned(),
    }
}

/// Répond « Oui » aux lignes `header` puis `data`, « Non » aux autres.
fn run(extraction: &mut Extraction, header: usize, data: usize) -> Step {
    let mut step = extraction.next_step();
    while let Step::Ask(question) = &step {
        let wanted = match question.target {
            Target::Header => header,
            Target::FirstData => data,
        };
        let answer = if question.number == wanted { Answer::Yes } else { Answer::No };
        step = extraction.answer(answer);
    }
    step
}

#[test]
fn extract_fizziq() {
    let mut extraction = Extraction::open(&asset("fizziq_dot.csv"), settings(true, 4, ";")).unwrap();
    let Step::Done(output) = run(&mut extraction, 5, 6) else {
        panic!("extraction non terminée");
    };
    assert_eq!(output.nb_columns, 4);
    assert_eq!(output.nb_values, 22);
    let text = output.text;
    assert!(text.starts_with("Comment = [0, 1, 2, 3, 4, 5,"), "{text}");
    assert!(text.contains("\nT_s_ = [0.0, 0.033, 0.066,"), "{text}");
    assert!(text.contains("\nx_m_ = [0.135,"), "{text}");
    assert!(text.contains("\ny_m_ = [0.338,"), "{text}");
    assert!(text.ends_with("0.348]\n"), "{text}");
    assert!(text.lines().all(|line| line.len() <= 79));
}

#[test]
fn extract_avimeca_tab_and_comma() {
    let mut extraction = Extraction::open(&asset("avimeca.txt"), settings(true, 3, "\t")).unwrap();
    let Step::Done(output) = run(&mut extraction, 2, 4) else {
        panic!("extraction non terminée");
    };
    assert_eq!(output.nb_values, 53);
    assert!(output.text.starts_with("t = [0.000, 0.040,"), "{}", output.text);
    assert!(output.text.contains("\nx = [4.00E-2, 4.60E-1,"), "{}", output.text);
    assert!(output.text.contains("\ny = [4.00E-2, 4.79E-1,"), "{}", output.text);
}

#[test]
fn extract_without_header() {
    let mut extraction = Extraction::open(&asset("avimeca_pv.txt"), settings(false, 3, ";")).unwrap();
    let Step::Done(output) = run(&mut extraction, 0, 4) else {
        panic!("extraction non terminée");
    };
    assert!(output.text.starts_with("Liste_1 = [0.000,"), "{}", output.text);
    assert!(output.text.contains("\nListe_3 = ["), "{}", output.text);
}

#[test]
fn extract_wrong_column_count_in_data() {
    // La ligne 9 de fizziq_prob.csv contient une colonne de trop
    let mut extraction = Extraction::open(&asset("fizziq_prob.csv"), settings(true, 4, ";")).unwrap();
    let Step::Error(message) = run(&mut extraction, 5, 6) else {
        panic!("erreur attendue");
    };
    assert!(message.starts_with("Ligne 9 : 3;0.099;0;267;0.256\n"), "{message}");
    assert!(message.contains("Colonnes comptées : 5"), "{message}");
}

#[test]
fn extract_wrong_column_count_in_header() {
    let mut extraction = Extraction::open(&asset("fizziq_dot.csv"), settings(true, 3, ";")).unwrap();
    let Step::Error(message) = run(&mut extraction, 5, 6) else {
        panic!("erreur attendue");
    };
    assert!(message.contains("Colonnes comptées : 4\nColonnes indiquées : 3"), "{message}");
}

#[test]
fn extract_no_header_chosen() {
    let mut extraction = Extraction::open(&asset("fizziq_dot.csv"), settings(true, 4, ";")).unwrap();
    assert_eq!(run(&mut extraction, 0, 0), Step::Error("Fin du fichier, aucun entête choisi.".into()));
}

#[test]
fn extract_no_data_chosen() {
    let mut extraction = Extraction::open(&asset("fizziq_dot.csv"), settings(true, 4, ";")).unwrap();
    assert_eq!(
        run(&mut extraction, 5, 0),
        Step::Error("Fin du fichier, aucune ligne de données choisie.".into())
    );
}

#[test]
fn extract_cancel() {
    let mut extraction = Extraction::open(&asset("fizziq_dot.csv"), settings(true, 4, ";")).unwrap();
    let Step::Ask(question) = extraction.next_step() else {
        panic!("question attendue");
    };
    assert_eq!((question.target, question.number), (Target::Header, 1));
    assert_eq!(question.prompt(), "Est-ce que la ligne n°1 correspond aux entêtes :");
    assert_eq!(extraction.answer(Answer::Cancel), Step::Cancelled);
}

#[test]
fn open_rejects_bad_files() {
    let open = |name: &str, separator: &str| Extraction::open(&asset(name), settings(true, 4, separator)).unwrap_err();
    assert_eq!(open("absent.csv", ";"), "Le fichier n'est pas accessible ou n'est pas lisible.");
    assert_eq!(open("bin.bin", ";"), "Le fichier n'est pas un fichier texte.");
    assert_eq!(open("nonread.txt", ";"), "Le fichier n'est pas un fichier texte.");
    assert_eq!(open("fizziq_prob_too_less.csv", ";"), "Le fichier texte ne contient pas assez de lignes.");
    assert_eq!(
        open("fizziq_prob_too_big.csv", ";"),
        "La taille du fichier est trop importante pour ce programme pédagogique."
    );
    assert_eq!(
        open("fizziq_prob_toolong.csv", ";"),
        "Une ligne est plus longue que prévu par ce programme pédagogique."
    );
    assert_eq!(open("fizziq_dot.csv", ""), "Le délimiteur personnalisé est vide.");
}
