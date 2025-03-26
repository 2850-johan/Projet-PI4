#include <iostream>
#include "Scannerusb.h"
#include "detecterusb.h"
#include "affichercontenu.h"

int main() {
    std::string usbPath = DetecterUSB();
    if (!usbPath.empty()) {
        AfficherContenuUSB(usbPath);
        ScannerUSB(usbPath);
    }
    return 0;
}
