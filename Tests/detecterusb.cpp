#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include "detecterusb.h"


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
        bool usbDetected = false;

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            usbDetected = true;
            mountPoint = buffer;
            if (!mountPoint.empty()) {
                mountPoint.erase(mountPoint.find_last_not_of(" \n\r\t") + 1); // Nettoyage des espaces
            }
        }

        pclose(pipe);

        if (usbDetected && !mountPoint.empty()) {
            std::cout << "✅ Clé USB détectée !\n📂 Point de montage : " << mountPoint << std::endl;
            return mountPoint;
        }

        std::cout << "🔄 Aucune clé USB détectée. Nouvelle tentative dans 2 secondes...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
