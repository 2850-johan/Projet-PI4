#include <iostream>
#include <filesystem>
#include <string>

#include "affichercontenu.h"

// Fonction de détection de la clé USB (déjà définie précédemment)
//std::string DetecterUSB();

namespace fs = std::filesystem;

void AfficherContenuUSB() {
    std::string path = DetecterUSB(); // On appelle la fonction qui détecte la clé avant d’afficher son contenu

    if (fs::exists(path) && fs::is_directory(path)) {
        std::cout << "📂 Contenu de la clé USB (" << path << ") :\n";
        try {
            for (const auto& entry : fs::directory_iterator(path)) {
                std::cout << "  - " << entry.path().filename().string() << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Erreur lors de l'accès au contenu : " << e.what() << "\n";
        }
    } else {
        std::cerr << "❌ Erreur : Le chemin spécifié n'existe pas ou n'est pas un dossier valide.\n";
    }
}
