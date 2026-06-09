#include "ScoreManager.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

//struktura pomocnicza do sortowania
struct ScoreEntry {
    std::string name;
    int score;
    std::string time;
};

//funkcja porównuj¹ca do sortowania
bool compareScores(const ScoreEntry& a, const ScoreEntry& b) {
    return a.score > b.score;
}

void ScoreManager::save(std::string name, int score, std::string time) {
    std::ofstream file("highscores.txt", std::ios::app);
    if (file.is_open()) {
        file << name << " " << score << " " << time << "\n";
        file.close();
    }
}

std::string ScoreManager::loadAll() {
    std::vector<ScoreEntry> scores;
    std::ifstream file("highscores.txt");
    std::string name;
    int score;
    std::string time;

    //wczytywanie
    while (file >> name >> score >> time) {
        scores.push_back({ name, score, time });
    }
    file.close();

    if (scores.empty()) return "BRAK WYNIKOW";

    //sortowanie
    std::sort(scores.begin(), scores.end(), compareScores);

    //TOP 10
    std::stringstream ss;
    int limit = (scores.size() < 10) ? scores.size() : 10;

    for (int i = 0; i < limit; i++) {
        ss << i + 1 << ". " << scores[i].name << " : " << scores[i].score << " : " << scores[i].time << "\n";
    }

    return ss.str();
}