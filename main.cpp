
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "utils.h"
#include "graph.h"
#include "sa.h"

using namespace std;

bool DEBUG = true;
int MAX_TIME = 15;
double alfa = 0.9;
double tmulti = 100;
double lmulti = 100;
bool lconst = true;
std::map<std::string, int> graphs;

inline void readConfig();
inline int parseInt(std::string line);
inline double parseDouble(std::string line);

int main() {

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    readConfig();

    for(auto& gfile : graphs) {
        string filename = "data/";
        filename.append(gfile.first);
        Graph g = Graph(filename, gfile.second);

        vector<vector<int>> matrix = g.getMatrix();
        int size = g.getSize();

        cout << gfile.first << " Najlepsza znana wartosc:" << gfile.second << "\n";
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                cout << matrix[i][j] << "\t";
            }
            cout << endl;
        }

        int time;

        Tsp_sa sa = Tsp_sa(g, DEBUG, MAX_TIME,alfa,tmulti,lmulti,lconst);
        time = sa.solve();
        std::cout << "Czas: " << time << std::endl;


        // bfs_wyniki.push_back(size);\        // bfs_wyniki.push_back(time);
        // bfs_wyniki.push_back(bfs.getBestRes());
        
    }


    // saveVecToTxt(bfs_wyniki, "bfs_wyniki.txt");

    return 0;
}

inline void readConfig() {
    ifstream file("config.txt");

    if(file.is_open()) {
        std::string line;

        std::getline(file, line);
        if(!parseInt(line)) DEBUG = false;

        std::getline(file, line);
        MAX_TIME = parseInt(line);

        std::getline(file, line);
        alfa = parseDouble(line);

        std::getline(file, line);
        tmulti = parseDouble(line);

        std::getline(file, line);
        lmulti = parseDouble(line);

        std::getline(file, line);
        if(!parseInt(line)) lconst = false;

        std::getline(file, line);
        int fileNum = parseInt(line);

        for(int i = 0; i < fileNum; i++) {
            std::getline(file, line);
            std::string filename;
            int cost;
            std::stringstream ss(line);
            ss >> filename;
            ss >> cost;
            graphs[filename] = cost;
        }

        file.close();
    }
    else {
        std::cerr << "Nie udało się otworzyć pliku konfiguracyjnego\n";
    } 
}

inline int parseInt(std::string line) {
    std::stringstream ss(line);
    std::string word;
    int number = 0;

    while (ss >> word) {
        try {
            number = std::stoi(word);
            break; 
        } catch (const std::invalid_argument&) {
        }
    }
    return number;
}

inline double parseDouble(std::string line) {
    std::stringstream ss(line);
    std::string word;
    double number = 0;

    while (ss >> word) {
        try {
            number = std::stod(word);
            break; 
        } catch (const std::invalid_argument&) {
        }
    }
    return number;
}