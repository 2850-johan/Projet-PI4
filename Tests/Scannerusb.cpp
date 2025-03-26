#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <cstdlib>
#include <filesystem>
#include <atomic>

#include "Scannerusb.h"


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

extern std::atomic<bool> stopLoading;

void ScannerUSB(const std::string& path) {
    if (!fs::exists(path)) {
        std::cerr << "❌ Erreur : Le chemin " << path << " n'existe pas.\n";
        return;
    }

    std::cout << "🛡️ Scan de la clé USB en cours...\n";
    stopLoading = false; // Réinitialise le flag d'arrêt
    std::thread loader(AfficherChargement);
    std::string scanResultpATH = "Data/scan_result.txt";

    // 🔹 Exécuter ClamAV et capturer le résultat dans un fichier
    std::string scanCommand = "clamscan -r --stdout \"" + path + "\" > scan_result.txt 2>&1";
    int scanStatus = system(scanCommand.c_str());

    stopLoading = true;  // Stoppe l'animation du chargement
    loader.join();       // Attend la fin du thread proprement
    std::cout << "\r✅ Scan terminé !                                      \n";

    if (scanStatus != 0) {
        std::cerr << "❌ Erreur lors de l'exécution du scan ClamAV.\n";
        return;
    }

    // 🔹 Lire et afficher le contenu de scan_result.txt
    std::ifstream resultFile("scan_result.txt");
    if (!resultFile) {
        std::cerr << "❌ Erreur : Impossible d'ouvrir scan_result.txt.\n";
        return;
    }

    std::string line;
    bool virusFound = false;

    while (getline(resultFile, line)) {
        std::cout << line << std::endl; // Affichage en temps réel
        if (line.find("FOUND") != std::string::npos) {
            std::cout << "🚨 Virus détecté : " << line << std::endl;
            virusFound = true;
        }
    }
    resultFile.close();

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
