/*#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>


void DetecterUSB() {
    while (true) {
        std::cout << "Recherche de périphériques USB...\n";
        int result = system("lsblk -o NAME,MOUNTPOINT,SIZE,MODEL | grep sd");
        if (result == 0) {
            std::cout << "Clé USB détectée !\n";
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Vérification toutes les 2 secondes
        std::cout << "Détection des périphériques USB..." << std::endl;
    }
}

int main ()
{
	DetecterUSB() ;
return 0;
}

*/
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// 🔹 Chemin FIXE pour le point de montage de la clé USB
const std::string USB_MOUNT_PATH = "/media/projet/EMTEC C450";

// Fonction pour vérifier si la clé USB est accessible
bool VerifierAccessibiliteUSB(const std::string& path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        std::cout << "✅ Clé USB détectée avec succès à : " << path << std::endl;
        return true;
    } else {
        std::cerr << "❌ Erreur : Le chemin '" << path << "' n'existe pas ou la clé USB n'est pas montée.\n";
        return false;
    }
}

// Fonction pour afficher le contenu de la clé USB
void AfficherContenuUSB(const std::string& path) {
    if (!VerifierAccessibiliteUSB(path)) return;

    std::cout << "📂 Contenu de la clé USB (" << path << ") :\n";
    for (const auto& entry : fs::directory_iterator(path)) {
        std::cout << "  - " << entry.path().filename().string() << "\n";
    }
}

// Fonction pour scanner la clé USB avec ClamAV
void ScannerUSB(const std::string& path) {
    if (!VerifierAccessibiliteUSB(path)) return;

    std::cout << "🛡️ Scan de la clé USB en cours...\n";

    // 🔹 Ajout de guillemets pour éviter les erreurs dues aux espaces
    std::string scanCommand = "clamscan -r --stdout \"" + path + "\" > scan_result.txt 2>&1";
    int scanStatus = system(scanCommand.c_str());

    if (scanStatus != 0) {
        std::cerr << "❌ Erreur lors de l'exécution du scan ClamAV. Vérifiez que ClamAV est installé et que la clé est accessible.\n";
        return;
    }

    // Vérifier si le fichier scan_result.txt existe avant de l'ouvrir
    std::ifstream resultFile("scan_result.txt");
    if (!resultFile) {
        std::cerr << "❌ Erreur : Impossible d'ouvrir le fichier scan_result.txt.\n";
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

// Programme principal
int main() {
    std::cout << "🔍 Vérification de la clé USB...\n";
    if (!VerifierAccessibiliteUSB(USB_MOUNT_PATH)) {
        return 1; // Quitte le programme si la clé n'est pas trouvée
    }

    AfficherContenuUSB(USB_MOUNT_PATH);
    ScannerUSB(USB_MOUNT_PATH);

    return 0;
}
