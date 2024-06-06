#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>

// Definirea codurilor de culoare
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

// Definirea clasei pentru Masa
class Masa {
private:
    int capacitate;    // Capacitatea mesei
    bool disponibila;  // Indicatorul de disponibilitate al mesei


public:

    // Constructorul clasei Masa
    Masa(int capacitate) : capacitate(capacitate), disponibila(true) {}

    // Metoda pentru a obține capacitatea mesei
    int getCapacitate() const {
        return capacitate;
    }

     // Metoda pentru a verifica dacă masa este disponibilă
    bool isDisponibila() const {
        return disponibila;
    }

    // Metoda pentru a seta disponibilitatea mesei
    void setDisponibila(bool disponibila) {
        this->disponibila = disponibila;
    }

    // Metoda pentru serializarea mesei în fișier
    void serialize(std::ofstream& fout) const {
        fout << capacitate << '\n';
    }
};

// Definirea clasei pentru Rezervare
class Rezervare {
private:
 std::string nume;   // Numele persoanei care a făcut rezervarea
    int capacitate;     // Capacitatea mesei rezervate
    std::string data;   // Data rezervării
    std::string ora;    // Ora rezervării

public:
 // Constructorul clasei Rezervare
    Rezervare(const std::string& nume, int capacitate, const std::string& data, const std::string& ora)
        : nume(nume), capacitate(capacitate), data(data), ora(ora) {}
 
 // Metoda pentru a obține numele persoanei care a făcut rezervarea
    const std::string& getNume() const {
        return nume;
    }

// Metoda pentru a obține capacitatea mesei rezervate
    int getCapacitate() const {
        return capacitate;
    }

 // Metoda pentru a obține data rezervarii
    const std::string& getData() const {
        return data;
    }

    
    // Metoda pentru a obține ora rezervării
    const std::string& getOra() const {
        return ora;
    }


    // Metoda pentru serializarea rezervării în fișier
    void serialize(std::ofstream& fout) const {
        fout << nume << '\n' << capacitate << '\n' << data << '\n' << ora << '\n';
    }
};

// Definirea clasei pentru Restaurant
class Restaurant {
private:
    std::vector<Masa> mese;            // Vectorul de mese
    std::vector<Rezervare> rezervari;  // Vectorul de rezervări

public:
// Metoda pentru citirea mesele din fișierul de intrare
    void citeste_mese(const std::string& managment) {
        std::ifstream fisier(managment);
        if (!fisier) {
            std::cout << RED << "Eroare la deschiderea fisierului." << RESET << '\n';
            return;
        }

        mese.clear();

        int capacitate;
        while (fisier >> capacitate) {
            Masa masa(capacitate);
            mese.push_back(masa);
        }

        fisier.close();
    }

    // Metoda pentru introducerea unei mese noi
        void scrie_mese(const std::string& managment) {
        std::ofstream fisier(managment);
        if (!fisier) {
            std::cout << RED << "Eroare la deschiderea fisierului." << RESET << '\n';
            return;
        }

        for (const Masa& masa : mese) {
            fisier << masa.getCapacitate() << '\n';
        }

        fisier.close();
    }

    // Metoda pentru afișarea mesele disponibile
    void afiseaza_mese() const {
        std::cout << "--- Mese disponibile ---" << '\n';
        for (const Masa& masa : mese) {
            if (masa.isDisponibila()) {
                std::cout << "Capacitate: " << masa.getCapacitate() << '\n';
            }
        }
        std::cout << '\n';
    }

//Metoda de adaugare a unei mese noi
    void adauga_masa(int capacitate) {
        Masa masa(capacitate);
        mese.push_back(masa);

        std::cout << GREEN << "Masa cu capacitatea " << capacitate << " a fost adaugata." << RESET << '\n';
    }

// Metoda de stergere a unei meae din fisier
    void sterge_masa(int capacitate) {
        auto it = std::find_if(mese.begin(), mese.end(), [capacitate](const Masa& masa) {
            return masa.getCapacitate() == capacitate;
        });

        if (it != mese.end()) {
            mese.erase(it);
            std::cout << GREEN << "Masa cu capacitatea " << capacitate << " a fost stearsa." << RESET << '\n';
        }
        else {
            std::cout << RED << "Nu exista o masa cu capacitatea " << capacitate << "." << RESET << '\n';
        }
    }

//Metoda de citire a rezervarilor din fisier
    void citeste_rezervari(const std::string& rezervari_file) {
        std::ifstream fisier(rezervari_file);
        if (!fisier) {
            std::cout << RED << "Eroare la deschiderea fisierului." << RESET << '\n';
            return;
        }

        rezervari.clear();

        std::string nume;
        int capacitate;
        std::string data;
        std::string ora;
        while (fisier >> nume >> capacitate >> data >> ora) {
            Rezervare rezervare(nume, capacitate, data, ora);
            rezervari.push_back(rezervare);
        }

        fisier.close();
    }
    
    // Metoda pentru scriere a rezervarii in fisier
    void scrie_rezervari(const std::string& rezervari_file) {
        std::ofstream fisier(rezervari_file);
        if (!fisier) {
            std::cout << RED << "Eroare la deschiderea fisierului." << RESET << '\n';
            return;
        }

        for (const Rezervare& rezervare : rezervari) {
            rezervare.serialize(fisier);
        }

        fisier.close();
    }

// Metoda pentru rezervarea unei mese
void rezerva_masa(const std::string& nume, int capacitate, const std::string& data, const std::string& ora) {
    auto it = std::find_if(mese.begin(), mese.end(), [capacitate](const Masa& masa) {
        return masa.getCapacitate() == capacitate && masa.isDisponibila();
    });

    if (it != mese.end()) {
        
        std::tm reservationTime = {};
        std::istringstream ss(data);
        ss >> std::get_time(&reservationTime, "%Y-%m-%d");

        if (ss.fail()) {
            std::cout << RED << "Formatul datei este invalid. Utilizati formatul 'YYYY-MM-DD'." << RESET << '\n';
            return;
        }

        int reservationWeekday = reservationTime.tm_wday;
        if (reservationWeekday == 0 || reservationWeekday == 6) {
            std::cout << RED << "Restaurantul este deschis doar de luni pana sambata. Nu se pot face rezervari in zilele de duminica." << RESET << '\n';
            return;
        }


        int reservationHour = std::stoi(ora.substr(0, 2));
        if (reservationHour < 8 || reservationHour >= 22) {
            std::cout << RED << "Restaurantul este deschis intre 8:00 si 22:00. Nu se pot face rezervari in afara acestor ore." <<RESET << '\n';
            return;
        }


        std::time_t currentTime = std::time(nullptr);
        std::time_t reservationDateTime = std::mktime(&reservationTime);
        if (reservationDateTime < currentTime) {
            std::cout << RED << "Data rezervarii este anterioara datei curente. Va rugam sa selectati o data ulterioara." << RESET << '\n';
            return;
        }

        auto conflictIt = std::find_if(rezervari.begin(), rezervari.end(), [&](const Rezervare& rezervare) {
            return rezervare.getCapacitate() == capacitate &&
                   rezervare.getData() == data &&
                   rezervare.getOra() == ora;
        });

        if (conflictIt != rezervari.end()) {
            std::cout << RED << "Masa cu capacitatea " << capacitate << " este deja rezervata pentru " << conflictIt->getNume() << "." << RESET << '\n';
        } else {
            it->setDisponibila(false);
            Rezervare rezervare(nume, capacitate, data, ora);
            rezervari.push_back(rezervare);
            std::cout <<RED << "Masa cu capacitatea " << capacitate << " a fost rezervata pentru " << nume << "." << RESET << '\n';
        }
    } else {
        std::cout << RED << "Nu exista o masa disponibila cu capacitatea " << capacitate << "." << RESET << '\n';
    }
}


// Metoda pentru afișarea rezervărilor
    void afiseaza_rezervari() const {
        std::cout << "--- Rezervari ---" << '\n';
        for (const Rezervare& rezervare : rezervari) {
            std::cout << "Nume: " << rezervare.getNume() << '\n';
            std::cout << "Capacitate masa: " << rezervare.getCapacitate() << '\n';
            std::cout << "Data: " << rezervare.getData() << '\n';
            std::cout << "Ora: " << rezervare.getOra() << '\n';
            std::cout << '\n';
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout <<RED << "Argumente insuficiente."<<RESET<<'\n';
        return 0;
    }

    std::string comanda(argv[1]);
    std::string managment = "managment.txt";
    std::string rezervariFile = "rezervariFile.txt";

    Restaurant restaurant;

    if (comanda == "vizualizare") {
        restaurant.citeste_mese(managment);
        restaurant.afiseaza_mese();
    } else if (comanda == "adauga") {
        int capacitate;
        std::cout << "Introduceti capacitatea mesei: ";
        std::cin >> capacitate;
        restaurant.citeste_mese(managment);
        restaurant.adauga_masa(capacitate);
        restaurant.scrie_mese(managment);
    } else if (comanda == "sterge") {
        int capacitate;
        std::cout << "Introduceti capacitatea mesei: ";
        std::cin >> capacitate;
        restaurant.citeste_mese(managment);
        restaurant.sterge_masa(capacitate);
        restaurant.scrie_mese(managment);
    } else if (comanda == "rezerva") {
        std::string nume, data, ora;
        int capacitate;
        std::cout << "Introduceti numele clientului: ";
        std::cin >> nume;
        std::cout << "Introduceti capacitatea dorita: ";
        std::cin >> capacitate;
        std::cout << "Introduceti data rezervarii: ";
        std::cin >> data;
        std::cout << "Introduceti ora rezervarii: ";
        std::cin >> ora;
        restaurant.citeste_mese(managment);
        restaurant.citeste_rezervari(rezervariFile);
        restaurant.rezerva_masa(nume, capacitate, data, ora);
        restaurant.scrie_mese(managment);
        restaurant.scrie_rezervari(rezervariFile);
    } else if(comanda =="afiseaza")
    {restaurant.citeste_rezervari(rezervariFile);
    restaurant.afiseaza_rezervari();}
    else {
        std::cout << "Comanda invalida." << '\n';
    }

    return 0;
}
