#include "esp_log.h"
#include "PlayingState.hpp"
#include "../Game.hpp"
#include "../../Controls/Controls.hpp"
#include "../../Audio/SoundFX.h"
#include "../GameObjects/ShipObject.hpp"

#define FIRE_COOLDOWN 0.2f
#define RESPAWN_COOLDOWN 1.0f

static const char *TAG = "GAME";

void PlayingState::enter(Game *game)
{
  game->start_new_game();
  // clear our own state down
  firing_cooldown.clear();
  respawn_cooldown.clear();
  thrust_sound_cooldown.clear();
  is_respawning.clear();
  for(int player=0; player<game->players; player++) {
    firing_cooldown.push_back(0);
    respawn_cooldown.push_back(0);
    thrust_sound_cooldown.push_back(0);
    is_respawning.push_back(false);
  }
}

GAME_STATE PlayingState::handle(Game *game, float elapsed_time)
{
for(int player=0; player<game->players; player++) {
  // handle respawn
  if (is_respawning[player])
  {
    respawn_cooldown.at(player) = respawn_cooldown[player]-elapsed_time;
    if (respawn_cooldown[player] < 0 && game->get_controls(player)->is_firing())
    {
      ESP_LOGI(TAG, "Respawn");
      is_respawning.at(player) = false;
      game->add_player_ship(player);
    }
  }
  // check to see if the ship has been hit
  if (game->is_ship_hit(player))
  {
    game->destroy_player_ship(player);
    game->set_lives(game->get_lives() - 1);
    game->get_sound_fx()->bang_large();
    game->get_controls(player)->shake();
    if (game->get_lives() == 0)
    {
      ESP_LOGI(TAG, "Switching to GAME over state");
      return GAME_STATE_GAME_OVER;
    }
    else
    {
      ESP_LOGI(TAG, "Allow respawn ship in 2 seconds");
      respawn_cooldown.at(player) = 2;
      is_respawning.at(player) = true;
    }
  }
  auto ship = game->get_ship(player);
  if (ship)
  {
    // upate the angle of the ship
    ship->setAngle(game->get_controls(player)->get_direction());
    // is the user pushing the thrust button?
    float thrust = game->get_controls(player)->get_thrust();
    if (thrust>0.0f)
    {
      ship->thrust(250 * elapsed_time * thrust);
      if (thrust_sound_cooldown[player] <= 0)
      {
        game->get_sound_fx()->thrust(thrust);
        thrust_sound_cooldown.at(player) = 0.25;
      }
      else
      {
        thrust_sound_cooldown.at(player) = thrust_sound_cooldown[player]-elapsed_time;
      }
      ship->set_is_thrusting(true);
    }
    else
    {
      ship->set_is_thrusting(false);
    }
    // handle the fire button
    if (firing_cooldown.at(player) > 0)
    {
      firing_cooldown.at(player) = firing_cooldown[player]-elapsed_time;
    }
    if (game->get_controls(player)->is_firing() && firing_cooldown[player] <= 0 && game->can_add_bullet(player))
    {
      game->add_bullet(player);
      // prevent firing for some time period
      firing_cooldown.at(player) = FIRE_COOLDOWN;
      game->get_sound_fx()->fire();
    }
  }
  if (!game->has_asteroids())
  {
    game->increase_difficulty();
    // need to create new asteroids
    game->reset_player_ship(player);//!
    game->add_asteroids();
  }
}
  return GAME_STATE_PLAYING;
}

void PlayingState::exit(Game *game)
{
  // nothing to do
}