#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <cstdlib>
#include <filesystem>
#include <atomic>

namespace fs = std::filesystem;
std::atomic<bool> stopLoading(false); // Flag pour arrêter l'animation du chargement

void AfficherChargement() {
    const std::string animation = "|/-\\";
    int i = 0;
    while (!stopLoading) {
        std::cout << "\r🛡️ Scan en cours " << animation[i % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        i++;
    }
}

void ScannerUSB(const std::string& path) {
    if (!fs::exists(path)) {
        std::cerr << "❌ Erreur : Le chemin " << path << " n'existe pas.\n";
        return;
    }

    std::cout << "🛡️ Scan de la clé USB en cours...\n";
    stopLoading = false; // Réinitialise le flag d'arrêt
    std::thread loader(AfficherChargement);

    // 🔹 Exécuter ClamAV et capturer le résultat
    std::string scanCommand = "clamscan -r --stdout \"" + path + "\"";
    FILE* pipe = popen(scanCommand.c_str(), "r");
    if (!pipe) {
        std::cerr << "❌ Erreur lors de l'exécution de ClamAV.\n";
        return;
    }

    // 🔹 Vérifier les résultats du scan
    std::string line;
    bool virusFound = false;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        line = buffer;
        line.erase(line.find_last_not_of(" \n\r\t") + 1); // Supprime les espaces et retours à la ligne

        if (line.find("FOUND") != std::string::npos) {
            std::cout << "🚨 Virus détecté : " << line << std::endl;
            virusFound = true;
        }
    }
    pclose(pipe);

    stopLoading = true;  // Stoppe l'animation du chargement
    loader.join();       // Attend la fin du thread proprement
    std::cout << "\r✅ Scan terminé !                                      \n";

    if (virusFound) {
        std::cout << "❓ Voulez-vous supprimer les fichiers infectés ? (y/n) : ";
        char choice;
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            std::string deleteCommand = "clamscan --remove -r \"" + path + "\"";
            system(deleteCommand.c_str());
            std::cout << "🗑️ Fichiers infectés supprimés !\n";
        }
    } else {
        std::cout << "✅ Aucun virus détecté sur la clé USB.\n";
    }
}
