#pragma once

#include <vector>
#include <algorithm>
#include "GameState.hpp"

class PlayingState : public GameState
{
private:
  std::vector<float> firing_cooldown;
  std::vector<bool> is_respawning;
  std::vector<float> respawn_cooldown;
  std::vector<float> thrust_sound_cooldown;

public:
  void enter(Game *game);
  GAME_STATE handle(Game *game, float elapsed_time);
  void exit(Game *game);
  const char *get_text()
  {
    if(std::find(is_respawning.begin(), is_respawning.end(), true) != is_respawning.end())
    {
      return "PRESS FIRE!";
    }
    return nullptr;
  }
};