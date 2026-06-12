#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <string>

class ScoreManager
{
public:
    static void save(std::string name, int score, std::string time);

    static std::string loadAll();
};

#endif