#include "strategy.h"

#define MAX_DEPTH 4
#define MAX_DEPTH_ALPHA_BETA 5

const std::vector<pair<int, int>> neighbors = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

void Strategy::applyMove (bidiarray<Sint16>& blobs, const movement& mv, Uint16 player) {
    blobs.set(mv.nx, mv.ny, player);

    if ((abs(mv.ox - mv.nx) > 1) || (abs(mv.oy - mv.ny) > 1)) {
        blobs.set(mv.ox, mv.oy, -1);
    }

    for (const pair<int, int>& neighbor : neighbors) {
        int x = mv.nx + neighbor.first;
        int y = mv.ny + neighbor.second;

        if (x < 0 || x > 7 || y < 0 || y > 7) {
            continue;
        }

        if (_holes.get(x, y)) {
            continue;
        }

        if (blobs.get(x, y) == (int) 1 - player) {
            blobs.set(x, y, player);
        }
    }
}

Sint32 Strategy::estimateCurrentScore (const bidiarray<Sint16>& blobs) const {
    Sint32 score = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (blobs.get(i, j) == (int) _current_player) {
                score++;
            } else if (blobs.get(i, j) == (int) (1 - _current_player)) {
                score--;
            }
        }
    }

    return score;
}

vector<movement>& Strategy::computeValidMoves (const bidiarray<Sint16>& blobs, vector<movement>& valid_moves, Uint16 player) const {
    // The following code finds a valid move.
    movement mv(0, 0, 0, 0);
    // iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (blobs.get(mv.ox, mv.oy) == (int) player) {
                // iterate on possible destinations
                for(mv.nx = std::max(0, mv.ox-2) ; mv.nx <= std::min(7, mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0, mv.oy-2) ; mv.ny <= std::min(7, mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (blobs.get(mv.nx, mv.ny) == -1) {
                            valid_moves.push_back(mv);
                        }
                    }
                }
            }
        }
    }

    return valid_moves;
}

/* Greedy algorithm */

Uint32 GreedyStrategy::countEnnemies(const movement& mv) {
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

void GreedyStrategy::computeBestMove () {
    vector<movement> valid_moves;
    computeValidMoves(_blobs, valid_moves, _current_player);

    if (valid_moves.empty()) {
        return;
    }
    
    Uint32 best_score = 0;
    movement best_move = valid_moves.front();
    
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
    
    _saveBestMove(best_move);
}

/* Min-max algorithm */

Sint32 MinMaxStrategy::minmax(const bidiarray<Sint16>& blobs, Uint8 depth, bool max) {
    Sint32 best_score;
    
    if (depth == 0) {
        return estimateCurrentScore(blobs);
    }

    if (max) {
        best_score = INT8_MIN;
        vector<movement> valid_moves;
        computeValidMoves(blobs, valid_moves, _current_player);

        if (valid_moves.empty()) {
            return estimateCurrentScore(blobs);
        }

        for (movement& mv : valid_moves) {
            bidiarray<Sint16> new_blobs(blobs);
            applyMove(new_blobs, mv, _current_player);
            Sint32 score = minmax(new_blobs, depth - 1, !max);

            if (score > best_score) {
                best_score = score;
                if (depth == MAX_DEPTH) {
                    _saveBestMove(mv);
                }
            }
        }
    } else {
        best_score = INT8_MAX;
        vector<movement> valid_moves;
        computeValidMoves(blobs, valid_moves, 1 - _current_player);

        if (valid_moves.empty()) {
            return estimateCurrentScore(blobs);
        }

        for (movement& mv : valid_moves) {
            bidiarray<Sint16> new_blobs(blobs);
            applyMove(new_blobs, mv, 1 - _current_player);
            best_score = std::min(best_score, minmax(new_blobs, depth - 1, !max));
        }
    }

    return best_score;
}

void MinMaxStrategy::computeBestMove () {
    minmax(_blobs, MAX_DEPTH, true);
}

/* Alpha-beta algorithm */

Sint32 AlphaBetaStrategy::alphabeta(const bidiarray<Sint16>& blobs, Uint8 depth, Sint32 alpha, Sint32 beta, bool max) {    
    if (depth == 0) {
        return estimateCurrentScore(blobs);
    }

    if (max) {
        Sint32 m = INT8_MIN;
        vector<movement> valid_moves;
        computeValidMoves(blobs, valid_moves, _current_player);

        if (valid_moves.empty()) {
            return estimateCurrentScore(blobs);
        }

        for (movement& mv : valid_moves) {
            bidiarray<Sint16> new_blobs(blobs);
            applyMove(new_blobs, mv, _current_player);
            Sint32 score = alphabeta(new_blobs, depth - 1, alpha, beta, !max);

            if (score > m) {
                if (depth == MAX_DEPTH_ALPHA_BETA) {
                    _saveBestMove(mv);
                }
                m = score;
            }

            if (m >= beta) {
                return m;
            }

            alpha = std::max(alpha, m);
        }

        return m;
    } else {
        Sint32 M = INT8_MAX;
        vector<movement> valid_moves;
        computeValidMoves(blobs, valid_moves, 1 - _current_player);

        if (valid_moves.empty()) {
            return estimateCurrentScore(blobs);
        }

        for (movement& mv : valid_moves) {
            bidiarray<Sint16> new_blobs(blobs);
            applyMove(new_blobs, mv, 1 - _current_player);
            Sint32 score = alphabeta(new_blobs, depth - 1, alpha, beta, !max);

            if (score < M) {
                M = score;
            }

            if (M <= alpha) {
                return M;
            }

            beta = std::min(beta, M);
        }

        return M;
    }
}

void AlphaBetaStrategy::computeBestMove () {
    alphabeta(_blobs, MAX_DEPTH_ALPHA_BETA, INT8_MIN, INT8_MAX, true);
}
