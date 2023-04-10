#ifndef SAN_PARSING_H
#define SAN_PARSING_H

/////////////////////////////////////////////////////////////////////
// san_type: san = standard algebraic notation for chess.
//           These are used as return values, when parsing input.
//           Should be mostly self-explanatory.
//           The "_FILE", "_RANK" and "_BOTH" suffixes denote if
//           a move has been further specified to avoid ambiguity.
//           e.g.: "Qc6"      no suffix
//                 "Qaxd3"    _FILE suffix
//                 "Q4h2"     _RANK suffix
//                 "Na2c3"    _BOTH suffix
/////////////////////////////////////////////////////////////////////
typedef enum {
    INVALID_SAN = 0,
    CASTLE_LEFT,
    CASTLE_RIGHT,
    PAWN_MOVE,
    PAWN_MOVE_PROMOTE,
    PAWN_CAPTURE,
    PAWN_CAPTURE_PROMOTE,
    PIECE_MOVE,
    PIECE_MOVE_FILE,
    PIECE_MOVE_RANK,
    PIECE_MOVE_BOTH,
    PIECE_CAPTURE,
    PIECE_CAPTURE_FILE,
    PIECE_CAPTURE_RANK,
    PIECE_CAPTURE_BOTH,
} san_type;

/////////////////////////////////////////////////////////////////////
// trim_whitespace(): 
/////////////////////////////////////////////////////////////////////
void trim_whitespace(char *string);

/////////////////////////////////////////////////////////////////////
// get_san_type(): 
/////////////////////////////////////////////////////////////////////
san_type get_san_type(const char *input);

/////////////////////////////////////////////////////////////////////
// san_type_pawn(): checks if string is in correct (export)
//                  format for a san-pawn-move.
//                  Input needs to be '\0' terminated and can not
//                  have leading or trailing whitespace.
//                  If these conditions don't hold, behavior is
//                  undefined.
/////////////////////////////////////////////////////////////////////
//san_type san_type_pawn(const char *input);

/////////////////////////////////////////////////////////////////////
// san_type_piece(): checks if string is in syntactically correct
//                   (export) format for a san-piece-move.
//                   Input needs to be '\0' terminated and can not
//                   have leading or trailing whitespace.
//                   If these conditions don't hold, behavior is
//                   undefined.
//                   Syntactically correct san expressions,
//                   which are impossible moves in chess,
//                   e.g. "Nah3" will not be returned as 
//                   INVALID_SAN but as the san_type corresponding
//                   to the syntax. In the case of the example:
//                   PIECE_MOVE_FILE
/////////////////////////////////////////////////////////////////////
//san_type san_type_piece(const char *input);

/////////////////////////////////////////////////////////////////////
// san_type_castle(): Checks if string is in syntactically correct
//                    format for a san casteling move.
//                    Returns CASTLE_LEFT, CASTLE_RIGHT or
//                    INVALID_SAN.
/////////////////////////////////////////////////////////////////////
//san_type san_type_castle(const char *input);


#endif // SAN_PARSING_H
