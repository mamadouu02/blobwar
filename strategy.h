#ifndef __STRATEGY_H
#define __STRATEGY_H

#define MAX_DEPTH 4

#include "common.h"
#include "bidiarray.h"
#include "move.h"



class Strategy {

protected:
    //! array containing all blobs on the board
    bidiarray<Sint16> _blobs;
    //! an array of booleans indicating for each cell whether it is a hole or not.
    const bidiarray<bool>& _holes;
    //! Current player
    Uint16 _current_player;
    
    //! Call this function to save your best move.
    //! Multiple call can be done each turn,
    //! Only the last move saved will be used.
    void (*_saveBestMove)(movement&);

public:
        // Constructor from a current situation
    Strategy (bidiarray<Sint16>& blobs, 
              const bidiarray<bool>& holes,
              const Uint16 current_player,
              void (*saveBestMove)(movement&))
            : _blobs(blobs),_holes(holes), _current_player(current_player), _saveBestMove(saveBestMove)
        {
        }
    
              
    
        // Copy constructor
    Strategy (const Strategy& St)
            : _blobs(St._blobs), _holes(St._holes),_current_player(St._current_player) 
        {}
    
        // Destructor
    ~Strategy() {}
    
        /** 
         * Apply a move to the current state of blobs
         * Assumes that the move is valid
         */
    void applyMove (bidiarray<Sint16>& blobs, const movement& mv, Uint16 player);

        /**
         * Compute the vector containing every possible moves
         */
    vector<movement>& computeValidMoves (const bidiarray<Sint16>& blobs, vector<movement>& valid_moves, Uint16 player) const;

        /**
         * Estimate the score of the current state of the game
         */
    Sint32 estimateCurrentScore (const bidiarray<Sint16>& blobs) const;

        /**
         * Find the best move.
         */
    void computeBestMove ();


};

class GreedyStrategy : public Strategy {

public:
        // Constructor from a current situation
    GreedyStrategy (bidiarray<Sint16>& blobs, 
              const bidiarray<bool>& holes,
              const Uint16 current_player,
              void (*saveBestMove)(movement&))
            : Strategy(blobs, holes, current_player, saveBestMove)
        {
        }


        /**
         * Find the best move.
         */
    void computeBestMove ();
    
        /**
         * Count ennemies.
         */
    Uint32 countEnnemies (const movement& mv);


};

class MinMaxStrategy : public Strategy {

public:
        // Constructor from a current situation
    MinMaxStrategy (bidiarray<Sint16>& blobs, 
              const bidiarray<bool>& holes,
              const Uint16 current_player,
              void (*saveBestMove)(movement&))
            : Strategy(blobs, holes, current_player, saveBestMove)
        {
        }


        /**
         * Find the best move.
         */
    void computeBestMove ();

        /**
         * Compute the best score.
         */
    Sint32 minmax (const bidiarray<Sint16>& blobs, Uint8 depth, bool max);

};

#endif
