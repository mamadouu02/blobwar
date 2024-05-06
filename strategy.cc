#include "strategy.h"

const std::vector<pair<int, int>> neighbors = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

void Strategy::applyMove (const movement& mv) {
    _blobs.set(mv.nx, mv.ny, _current_player);

    if ((abs(mv.ox - mv.nx) <= 1) && (abs(mv.oy - mv.ny) <= 1)) {
        _blobs.set(mv.ox, mv.oy, -1);
    }
}

Sint32 Strategy::estimateCurrentScore () const {
    Sint32 score = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (_blobs.get(i, j) == (int) _current_player) {
                score++;
            } else if (_blobs.get(i, j) == (int) 1 - _current_player) {
                score--;
            }
        }
    }

    return score;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves) const {
    // The following code finds a valid move.
    movement mv(0, 0, 0, 0);
    // iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                // iterate on possible destinations
                for(mv.nx = std::max(0, mv.ox-2) ; mv.nx <= std::min(7, mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0, mv.oy-2) ; mv.ny <= std::min(7, mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) {
                            valid_moves.push_back(mv);
                        }
                    }
                }
            }
        }
    }

    return valid_moves;
}

void Strategy::computeBestMove () {
    vector<movement> valid_moves;
    computeValidMoves(valid_moves);

    if (valid_moves.empty()) {
        return;
    }
    
    movement best_move = greedy(valid_moves);
    
    _saveBestMove(best_move);
    return;
}

Uint32 Strategy::countEnnemies(const movement& mv) {
    Uint32 score = 0;

    for (const pair<int, int>& neighbor : neighbors) {
        int x = mv.nx + neighbor.first;
        int y = mv.ny + neighbor.second;

        if (x < 0 || x > 7 || y < 0 || y > 7) {
            continue;
        }

        if (_holes.get(x, y)) {
            continue;
        }
        
        if (_blobs.get(x, y) == (int) 1 - _current_player) {
            score++;
        }
    }

    return score;
}

movement Strategy::greedy(vector<movement>& valid_moves) {
    Uint32 best_score = 0;
    movement best_move = valid_moves[0];
    
    Uint32 score = 0;

    for (const movement& mv : valid_moves) {
        score = countEnnemies(mv);
        if (score > best_score) {
            best_score = score;
            best_move = mv;
        } else if ((score == best_score) && (abs(mv.ox - mv.nx) <= 1) && (abs(mv.oy - mv.ny) <= 1)) {
            best_move = mv;
        }
    }

    return best_move;
}
