#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <cctype>
#include <sstream>

using namespace std;

///enum tip koji oznacava vrstu linije
enum LineType {
    DEKLARACIJA,
    DODJELA,
    PETLJA,
    ZAGRADA,
    OUTPUT,
    OSTALO
};
///struktura koja za svaku liniju daje odgovarajuce podatke
struct CodeLine {
    string podatak; //kakva je linija u originalnom fajlu
    int blokID; //kojem bloku pripada linija
    LineType tip; //vrsta linije
    string lijevaStrana; //lijeva strana ukoliko je dodjela
    vector<string> desnaStrana; //desna strana cuva varijable i operacije
};

///pomocna funkcija
static string trim(const string &s) {
    size_t pocetak = s.find_first_not_of(" \t\r\n");
    if (pocetak == string::npos) return "";
    size_t kraj = s.find_last_not_of(" \t\r\n");
    return s.substr(pocetak, kraj - pocetak + 1);
}
///pomocna funkcija koja provjerava da li je linija - zagrada
bool jeLiZagrada(const string &linija) {
    string t = trim(linija);
    return (t == "{" || t == "}");
}
///pomocna funkcija koja provjerava da li je linija for petlja
bool jeLiPetlja(const string &linija) {
    string t = trim(linija);
    if (t.find("for(") != string::npos)
        return true;
    return false;
}
///pomocna funkcija koja provjerava da li je deklaracija
///omoguceni su svi tipovi sa kojima je moguce izvrsiti operaciju sabiranja
bool jeLiDeklaracija(const string &linija) {
    static const vector<string> tipovi = {"int", "float", "double", "long", "short"};
    string trimmedLinija = trim(linija);
    if (trimmedLinija.find(';') == string::npos) return false;
    for (const auto &kw : tipovi) {
        if (trimmedLinija.rfind(kw, 0) == 0) {
            return true;
        }
    }
    return false;
}
///pomocna funkcija koja provjerava da li je linija dodjele
bool jeLiDodjela(const string &linija) {
    string t = trim(linija);
    if (t.find('=') == string::npos || t.back() != ';') return false;
    if (jeLiZagrada(t) || jeLiPetlja(t) || jeLiDeklaracija(t)) return false;
    return true;
}
///pomocna funkcija koja provjerava da li je cout odnosno ispis
bool jeLiOutput(const string &linija) {
    return trim(linija).find("cout") != string::npos;
}
///funkcija koja vrsi parsiranje dodjele odnosno x=y+z
///daje nam podatak sta je desna strana izraza,a sta lijeva strana izraza
void parsiranjeDodjele(const string &linija, string &LS, vector<string> &DS) {
    LS.clear();
    DS.clear();

    string t = trim(linija);
    //uklanja ; na kraju izraza
    if (!t.empty() && t.back() == ';')
        t.pop_back();

    //pronalazi znak jednakosti kako bi se moglo odrediti ko je LS,a ko DS
    size_t pozicijaJednako = t.find('=');
    if (pozicijaJednako == string::npos) return;

    //na lijevoj strani obicno imamo samo jednu varijablu odnosno x iz izraza x=y+z
    LS = trim(t.substr(0, pozicijaJednako));
    //desna strana se nalazi od znaka jednakosti pa do kraja linije
    string desniDio = trim(t.substr(pozicijaJednako + 1));
    string var;
    //pronalazi broj i spasava ga kao varijablu
    for (char c : desniDio) {
        if (isalnum(c) || c == '_') {
            var.push_back(c);
        } else {
            if (!var.empty()) {
                DS.push_back(var);
                var.clear();
            }
        }
    }
    if (!var.empty())
        DS.push_back(var);
}
///struktura koja cuva id bloka i linije koje mu pripadaju
struct Blok {
    int id;
    vector<int> linijeIndices;
};
///funkcija koja cita fajl i klasificira linije untar tog fajla
///odredjuje blokove untar jednog fajla
vector<CodeLine> citanjeKlasifikacijaFajla(const string &inputFajl) {
    ifstream fin(inputFajl);
    if (!fin.is_open()) {
        cerr << "Error: Ne moze se otvoriti " << inputFajl << endl;
        exit(1);
    }

    vector<CodeLine> linije;
    int trenutniBlokID = 0; // Blok ID za kod izvan petlji
    int maxIdBlok = 0;      // Blok ID za petlje
    vector<int> blokStek;
    blokStek.push_back(trenutniBlokID);

    string podatak;
    while (getline(fin, podatak)) {
        string trimmed = trim(podatak);
        LineType lt = OSTALO;

        if (trimmed.empty()) {
            linije.push_back({podatak, blokStek.back(), OSTALO, "", {}});
            continue;
        }
        //provjerava koji je tip linije koristeci pomocne funkcije
        if (jeLiZagrada(trimmed)) lt = ZAGRADA;
        else if (jeLiPetlja(trimmed)) lt = PETLJA;
        else if (jeLiDeklaracija(trimmed)) lt = DEKLARACIJA;
        else if (jeLiDodjela(trimmed)) lt = DODJELA;
        else if (jeLiOutput(trimmed)) lt = OUTPUT;

        CodeLine cl = {podatak, blokStek.back(), lt, "", {}};
        //ako je linija dodjele,izvrsi parsiranje lijeve i desne strane
        if (lt == DODJELA){
            parsiranjeDodjele(trimmed, cl.lijevaStrana, cl.desnaStrana);
        }
        linije.push_back(cl);

        //ako je linija petlja, povecavamo maxIdBlok i koristimo ga za blokId za sve linije te petlje
        if (lt== PETLJA) {
            maxIdBlok++;
            blokStek.push_back(maxIdBlok);
        }

        //ako linija sadrzi otvorenu zagradu (osim unutar petlje), kreiramo novi blok ID.
        if (trimmed.find('{') != string::npos && lt!=PETLJA) {
            maxIdBlok++;
            blokStek.push_back(maxIdBlok);
        }

        //kada pronadjemo zatvorenu zagradu,uklanjamo blokId i vracamo se na prethodni blok
        if (trimmed.find('}') != string::npos) {
            if (!blokStek.empty()) {
                blokStek.pop_back();
            } else {
                cerr << "Error: Neispravna struktura zagrada." << endl;
                exit(1);
            }
        }

        //ako je stek prazan ili smo na osnovnom nivou, postavljamo trenutniBlokID na 1
        trenutniBlokID = blokStek.empty() ? 1 : blokStek.back();
    }

    fin.close();
    return linije;
}

///Glavna funkcija u kojoj se izvrsava random shuffling
void randomReordering(vector<CodeLine> &linije) {
        unordered_map<int, vector<int>> blokDodjela; //dodjele,rasporedjene po blokovima

        // Parsiranje linija koda i identifikacija dodjele sa blokovima
        for (int i = 0; i < linije.size(); i++) {
            if (linije[i].tip == DODJELA) {
                blokDodjela[linije[i].blokID].push_back(i);
            }
        }

        //generisanje random
        random_device rd;
        mt19937 rng(rd());

        for (auto &unos : blokDodjela) {
            int bId = unos.first;
            auto &dodjelaIndices = unos.second;

            if (dodjelaIndices.size() < 2) {
                continue;
            }

            //kreiranje grafa zavisnosti za topolosko sortiranje
            int n = dodjelaIndices.size();
            vector<vector<int>> susjedi(n);
            vector<int> inDegree(n, 0);
            unordered_map<string, int> lastDef; //pracenje posljednje def var u bloku


            for (int i = 0; i < n; i++) {
                int stvarniIndeks = dodjelaIndices[i];
                CodeLine &cl = linije[stvarniIndeks];

                //obrada desne strane sa zavisnostima
                for (const string &desnaStranaVar : cl.desnaStrana) {
                    if (lastDef.count(desnaStranaVar)) {
                        int prethodna = lastDef[desnaStranaVar];
                            if (prethodna >= 0) {
                                susjedi[prethodna].push_back(i);
                                inDegree[i]++;
                            }
                    }
                    //obrada lijeve strane sa zavisnostima
                    for (int j = i + 1; j < n; j++) {
                        int sljedeci = dodjelaIndices[j];
                            if (linije[sljedeci].lijevaStrana == desnaStranaVar) {
                                susjedi[i].push_back(j);
                                inDegree[j]++;
                            }
                    }
                }

                if (!cl.lijevaStrana.empty()) {
                    lastDef[cl.lijevaStrana] = i;
                }
            }

            //pronalazenje cvorova sa ulaznim stepenom 0
            //nad njima izvrsavamo topolosko sortiranje u skladu sa randomizacijom i zavisnostima
            vector<int> nulaStepeni;
            for (int i = 0; i < n; i++) {
                if (inDegree[i] == 0) {
                    nulaStepeni.push_back(i);
                }
            }

            vector<int> topolosko;
            while (!nulaStepeni.empty()) {
                //random mijesanje
                shuffle(nulaStepeni.begin(), nulaStepeni.end(), rng);
                int izabraniEl = nulaStepeni.back();
                nulaStepeni.pop_back();
                topolosko.push_back(izabraniEl);
                for (int sljedeci : susjedi[izabraniEl]) {
                    if (--inDegree[sljedeci] == 0) {
                        nulaStepeni.push_back(sljedeci);
                    }
                }
            }

            if (topolosko.size() < n) {
                cerr << "Pronađen ciklus u bloku " << bId << ". Preskačemo reordering." << endl;
                continue;
            }

            vector<CodeLine> pomocna(n);
            for (int i = 0; i < n; i++) {
                pomocna[i] = linije[dodjelaIndices[topolosko[i]]];
            }
            //konacna raspodjela linija po blokovima
            for (int i = 0; i < n; i++) {
                linije[dodjelaIndices[i]] = pomocna[i];
            }
        }
}
///Funkcija koja nam sluzi za upisivanje podataka u output fajl
void ispisOutput(const vector<CodeLine> &linije, const string &imeFajla) {
    ofstream fout(imeFajla);
    if (!fout.is_open()) {
        cerr << "Error: Ne moze se otvoriti " << imeFajla <<endl;
        exit(1);
    }
    for (const auto &cl : linije) {
        fout << cl.podatak << "\n";
    }
    fout.close();
}

///Main program
///Da bi program radio, trebaju biti otvoreni i input i output fajl
int main() {
    string inputFajl = "input.cpp";
    vector<CodeLine> linije = citanjeKlasifikacijaFajla(inputFajl);
    randomReordering(linije);
    string outputFajl = "output.cpp";
    ispisOutput(linije, outputFajl);
    cout << "Izmjenjeni kod je napisan u fajlu " << outputFajl << endl;
    return 0;
}

