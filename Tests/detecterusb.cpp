#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

// 🔹 Fonction pour détecter la clé USB et récupérer son point de montage
std::string DetecterUSB() {
    while (true) {
        std::cout << "🔍 Recherche de périphériques USB...\n";
        FILE* pipe = popen("lsblk -o MOUNTPOINT | grep /media", "r");
        if (!pipe) {
            std::cerr << "❌ Erreur lors de l'exécution de la commande lsblk." << std::endl;
            return "";
        }

        char buffer[256];
        std::string mountPoint;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            mountPoint = buffer;
            mountPoint.erase(mountPoint.find_last_not_of(" \n\r\t") + 1); // Nettoyage des espaces
        }
        pclose(pipe);

        if (!mountPoint.empty()) {
            std::cout << "✅ Clé USB détectée !\n📂 Point de montage : " << mountPoint << std::endl;
            return mountPoint;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "🔄 Détection des périphériques USB en cours...\n";
    }
}

// 🔹 Fonction pour afficher le contenu de la clé USB
void AfficherContenuUSB(const std::string& path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        std::cout << "📂 Contenu de la clé USB (" << path << ") :\n";
        for (const auto& entry : fs::directory_iterator(path)) {
            std::cout << "  - " << entry.path().filename().string() << "\n";
        }
    } else {
        std::cerr << "❌ Erreur : Le chemin spécifié n'existe pas ou n'est pas un dossier valide.\n";
    }
}

// 🔹 Fonction pour afficher une barre de chargement pendant le scan
void AfficherChargement() {
    const std::string animation = "|/-\\";
    int i = 0;
    while (true) {
        std::cout << "\r🛡️ Scan en cours " << animation[i % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        i++;
    }
}

// 🔹 Fonction pour scanner la clé USB avec ClamAV ou chkrootkit
void ScannerUSB(const std::string& path) {
    if (!fs::exists(path)) {
        std::cerr << "❌ Erreur : Le chemin " << path << " n'existe pas.\n";
        return;
    }

    std::cout << "🛡️ Scan de la clé USB en cours...\n";

    // 🔹 Lancement du chargement en arrière-plan
    std::thread loader(AfficherChargement);

    // 🔹 Exécuter ClamAV et stocker le résultat dans un fichier
    std::string scanCommand = "clamscan -r --stdout \"" + path + "\" > scan_result.txt 2>&1";
    int scanStatus = system(scanCommand.c_str());

    // 🔹 Arrêter l'animation du chargement
    loader.detach();
    std::cout << "\r✅ Scan terminé !                                      \n";

    if (scanStatus != 0) {
        std::cerr << "❌ Erreur lors de l'exécution du scan ClamAV. Vérification avec chkrootkit...\n";

        // 🔹 Alternative : Tester avec chkrootkit (si ClamAV échoue)
        std::string alternativeScan = "sudo chkrootkit > chkrootkit_result.txt 2>&1";
        int altScanStatus = system(alternativeScan.c_str());

        if (altScanStatus != 0) {
            std::cerr << "❌ Échec de chkrootkit également. Vérifiez votre système.\n";
            return;
        }

        std::ifstream altResult("chkrootkit_result.txt");
        std::string line;
        bool rootkitFound = false;

        while (getline(altResult, line)) {
            if (line.find("INFECTED") != std::string::npos) {
                std::cout << "🚨 Menace détectée par chkrootkit : " << line << std::endl;
                rootkitFound = true;
            }
        }
        altResult.close();

        if (!rootkitFound) {
            std::cout << "✅ Aucun virus ou rootkit détecté par chkrootkit.\n";
        }
        return;
    }

    // 🔹 Vérifier si le fichier scan_result.txt existe
    std::ifstream resultFile("scan_result.txt");
    if (!resultFile) {
        std::cerr << "❌ Erreur : Impossible d'ouvrir scan_result.txt.\n";
        return;
    }

    std::string line;
    bool virusFound = false;

    while (getline(resultFile, line)) {
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

// 🔹 Programme principal
int main() {
    std::string usbPath = DetecterUSB();
    if (!usbPath.empty()) {
        AfficherContenuUSB(usbPath);
        ScannerUSB(usbPath);
    }

    return 0;
}
