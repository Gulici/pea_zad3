#ifndef SA
#define SA

#include "graph.h"
#include <vector>
#include <assert.h>
#include <unordered_map>
#include <list>
#include <chrono>
#include "utils.h"
#include <math.h>

class Tsp_sa 
{
    int gSize;
    std::vector<vector<int>> neightmatrix;
    bool DEBUG;
    bool lconst;

    double Tmax, Tmin;
    double tmulti;              // wspolczynnik Tmax
    double alfa;                // wspolczynnik chlodzenia geometrycznego
    int L = 100;                // liczba transformacji dla Tk
    std::vector<int> solution;  // znalezione rozwiazanie

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> diff;
    std::chrono::minutes maxtime = std::chrono::minutes(1);
    bool overrun = false;

    public:
    Tsp_sa(Graph& g, bool debug, int max_time, double alfa, double tmulti, int lmulti, bool lconst);
    int solve();
    std::vector<int> startSolution();
    std::vector<int> neighSolution(std::vector<int> path);
    int calDistance(std::vector<int> path);
};

Tsp_sa::Tsp_sa(Graph& g, bool debug, int max_time, double alfa, double tmulti, int lmulti, bool lconst) 
{
    gSize = g.getSize();
    neightmatrix = g.neighMatrix();
    DEBUG = debug;
    this->lconst = lconst;
    maxtime = std::chrono::minutes(max_time);

    //wyznaczenie temperatury maksymalnej
    Tmax = gSize * tmulti;
    // wyznaczenie temperatury minimalnej
    Tmin = 1;
    // wyznaczenie początkowej ilości powtórzeń
    L = gSize * lmulti;

    assert (alfa > 0 && alfa < 1);
    this->alfa = alfa;
}

int Tsp_sa::solve() 
{   
    // wyznaczenie poczatkowego rozwiazania
    solution = startSolution();
    int solDist = calDistance(solution);
    int Lk = L;
    int Lmin = L * 0.1;
    double T = Tmax;

    start = std::chrono::system_clock::now();

    while(T >= Tmin && !overrun) {
        for(int i = 0; i < Lk; i++) {
            std::vector<int> n = neighSolution(solution);
            int nDist = calDistance(n);
            // przyjmujemy mniejsze rozwiazanie jesli jego koszt jest mniejszy
            if(nDist <= solDist) {
                solution = n;
                solDist = nDist;
            }
            else {
                // obliczenie prawdopodobienstwa wybrania rozwiazania o wiekszym koszcie
                // p tym mniejsze im mniejsza temperatura i im wieksza roznica kosztow
                double p = std::exp((double)(solDist - nDist)/T);
                std::srand((unsigned)time(NULL));
                double r = (double) rand() / RAND_MAX;

                if(p > r) {
                    solution = n;
                    solDist = nDist;
                }
            }

            //sprawdzenie czy nie przekroczono maksymalnego czasu
            end = std::chrono::system_clock::now();
            diff = end - start;
            auto tm = std::chrono::duration_cast<std::chrono::minutes>(diff);
            if(tm >= maxtime) {
                overrun = true;
                break;
            }
        }
        // obnizenie temperatury
        T *= alfa;
        // obnizenie liczby iteracji
        if(!lconst){
            if(Lk > Lmin) Lk *= alfa;
            if(Lk < Lmin) Lk = Lmin;
        } 
    }

    end = std::chrono::system_clock::now();
    diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    cout << "SA\nNajlepszy znaleziony cykl:";
    for(int v : solution) cout << v << " ";
    cout << " Koszt: " << solDist << endl;

    return tms.count();
}

// rozwiazanie dla grafow pelnych
inline std::vector<int> Tsp_sa::startSolution()
{   
    vector<int> path;
    vector<bool> visited(gSize, false);
    int currentCity = 0;
    path.push_back(currentCity);
    visited[currentCity] = true;

    for (int i = 1; i < gSize; ++i) {
        int nearest = -1;
        int minDistance = INT_MAX;

        for (int j = 0; j < gSize; ++j) {
            int dist = neightmatrix[currentCity][j];
            if (!visited[j] && dist > 0 && dist < minDistance) {
                minDistance = dist;
                nearest = j;
            }
        }

        if (nearest != -1) {
            path.push_back(nearest);
            visited[nearest] = true;
            currentCity = nearest;
        }
    }
    return path;
}

inline std::vector<int> Tsp_sa::neighSolution(std::vector<int> current)
{   
    std::vector<int> path = current;
    int v1 = random(0,gSize-1);
    int v2 = random(0,gSize-1);
    while(v1 == v2) v2 = random(0,gSize-1);
    
    std::swap(path[v1], path[v2]);

    return path;
}

inline int Tsp_sa::calDistance(std::vector<int> path)
{   
    int cost = 0;
    for (int i = 0; i < gSize-1; i++) {
        cost += neightmatrix[path[i]][path[i+1]];
    }
    cost += neightmatrix[path[gSize-1]][path[0]];
    return cost;
}

#endif