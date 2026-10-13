//! Sous Windows, intègre dans l'exécutable l'icône, les informations de version
//! et le manifeste (équivalent de `resources.rc` de la version C++).

use std::env;

use winresource::{VersionInfo, WindowsResource};

const ICON: &str = "../app_icon.ico";
const MANIFEST: &str = "assets/csv2listes.exe.manifest";

fn main() {
    println!("cargo:rerun-if-changed={ICON}");
    println!("cargo:rerun-if-changed={MANIFEST}");
    if env::var("CARGO_CFG_TARGET_OS").as_deref() != Ok("windows") {
        return;
    }
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    // Numérotation à 4 chiffres de la version Windows d'origine : 0.0.9 dans Cargo → 0.0.0.9
    let part = |name: &str| env::var(name).unwrap().parse::<u64>().unwrap();
    let version = format!("0.{}", env::var("CARGO_PKG_VERSION").unwrap());
    let numeric = part("CARGO_PKG_VERSION_MAJOR") << 32
        | part("CARGO_PKG_VERSION_MINOR") << 16
        | part("CARGO_PKG_VERSION_PATCH");

    let mut resource = WindowsResource::new();
    resource
        .set_icon(&format!("{dir}/{ICON}"))
        .set_manifest_file(&format!("{dir}/{MANIFEST}"))
        .set_language(0x040C) // français (France)
        .set_version_info(VersionInfo::FILEVERSION, numeric)
        .set_version_info(VersionInfo::PRODUCTVERSION, numeric)
        .set("FileVersion", &version)
        .set("ProductVersion", &version)
        .set("ProductName", "csv2listes")
        .set("FileDescription", "Convertisseur : Fichier CSV -> Listes en Python")
        .set("CompanyName", "MElectronSoft")
        .set("LegalCopyright", "Rémi MEVAERE© 2024");
    resource.compile().expect("compilation des ressources Windows");
}
