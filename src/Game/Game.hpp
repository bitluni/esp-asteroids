//
//  Game.hpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 04/01/2021.
//

#pragma once

#include <list>
#include <set>
#include <vector>
#include "GameStateMachine/GameState.hpp"
#include "box2d/box2d.h"

class b2World;
class GameObject;
class ShipObject;
class RenderBuffer;
class Controls;
class SoundFX;

class Game : public b2ContactListener
{
private:
    int score;
    int lives;
    std::vector<bool> _is_ship_hit;
    bool _did_asteroids_collide;
    float asteroid_speed;
    b2World *world;
    std::vector<ShipObject *> ships;

    std::list<GameObject *> objects;
    std::list<GameObject *> bullets;
    std::list<GameObject *> lives_indicators;
    std::set<GameObject *> hitAsteroids;
    std::set<GameObject *> deadBullets;

    float size;
    SoundFX *sound_fx;
    std::vector<Controls *> controls;

    GAME_STATE current_game_state;
    GameState *current_game_state_handler;

    GameState *game_state_start_handler;
    GameState *game_over_state_handler;
    GameState *game_playing_state_handler;

    // world processing functions
    void process_bullets(float elapsed_time, int player=0);
    void process_asteroids();
    void wrap_objects();

public:
    Game(float size, std::vector<Controls *> controls, SoundFX *sound_fx);
    void start_new_game();
    std::list<GameObject *> &getObjects()
    {
        return objects;
    }
    Controls *get_controls(int player=0)
    {
        return this->controls[player];
    }
    SoundFX *get_sound_fx()
    {
        return this->sound_fx;
    }
    ShipObject *get_ship(int player)
    {
        return this->ships[player];
    }
    void step_world(float elapsedTime);
    void reset();

    void add_bullet(int player);
    bool has_asteroids();
    bool can_add_bullet(int player=0);
    void increase_difficulty()
    {
        asteroid_speed += 2;
    }
    void add_asteroids();
    void add_player_ship(int player);
    void destroy_player_ship(int player);
    void add_lives(int player=0);
    void reset_player_ship(int player);

    int get_lives(int player=0)
    {
        return this->lives;
    }
    void set_lives(int new_value, int player=0);
    void set_score(int new_score, int player=0);
    int get_score(int player=0)
    {
        return score;
    }
    bool is_ship_hit(int player)
    {
        return _is_ship_hit[player];
    }
    void clear_is_ship_hit(int player)
    {
        _is_ship_hit[player] = false;
    }
    bool show_scores()
    {
        return current_game_state != GAME_STATE_START;
//        return false;
    }
    const char *get_main_text()
    {
        return current_game_state_handler->get_text();
    }

    void BeginContact(b2Contact *contact);
    void EndContact(b2Contact *contact);

    int players;
};
