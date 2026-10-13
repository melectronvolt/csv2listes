//! Sous Windows, intègre l'icône et les informations de version dans l'exécutable
//! (équivalent de `resources.rc` de la version C++).

use std::env;

use winresource::{VersionInfo, WindowsResource};

fn main() {
    println!("cargo:rerun-if-changed=../app_icon.ico");
    if env::var("CARGO_CFG_TARGET_OS").as_deref() != Ok("windows") {
        return;
    }

    // Numérotation à 4 chiffres de la version Windows d'origine : 0.0.9 dans Cargo → 0.0.0.9
    let part = |name: &str| env::var(name).unwrap().parse::<u64>().unwrap();
    let version = format!("0.{}", env::var("CARGO_PKG_VERSION").unwrap());
    let numeric = part("CARGO_PKG_VERSION_MAJOR") << 32
        | part("CARGO_PKG_VERSION_MINOR") << 16
        | part("CARGO_PKG_VERSION_PATCH");

    let mut resource = WindowsResource::new();
    resource
        .set_icon("../app_icon.ico")
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
