#include <iostream>
#include <string>
// Inclure l'en-tête et non le fichier .cpp

// 🔹 Programme principal
int main() {
    std::string usbPath = DetecterUSB();
    if (!usbPath.empty()) {
        AfficherContenuUSB(usbPath);
        ScannerUSB(usbPath);
    }

    return 0;
}