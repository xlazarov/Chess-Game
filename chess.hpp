#include <cassert>
#include <vector>
#include <string>

/* The goal of this task is to implement the standard rules of
 * chess. */

struct position {
    int file; /* column 'letter', a = 1, b = 2, ... */
    int rank; /* row number, starting at 1 */
};

enum class piece_type {
    pawn, rook, knight, bishop, queen, king
};

enum class player {
    white, black
};

/* The following are the possible outcomes of ‹playy›. The outcomes
 * are shown in the order of precedence, i.e. the first applicable
 * is returned.
 *
 * ├┄┄┄┄┄┄┄┄┄┄┄┄┄┄▻┼◅┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┤
 * │ ‹capture›     │ the move was legal and resulted in a capture │
 * │ ‹ok›          │ the move was legal and was performed         │
 * │ ‹no_piece›    │ there is no piece on the ‹from› square       │
 * │ ‹bad_piece›   │ the piece on ‹from› is not ours              │
 * │ ‹bad_move›    │ this move is not available for this piece    │
 * │ ‹blocked›     │ another piece is in the way                  │
 * │ ‹lapsed›      │ «en passant» capture is no longer allowed    │
 * │ ‹has_moved›   │ one of the castling pieces has already moved │
 * │ ‹in_check›    │ the player is currently in check and the     │
 * │               │ move does not get them out of it             │
 * │ ‹would_check› │ the move would place the player in check     │
 * │ ‹bad_promote› │ promotion to a pawn or king was attempted    │
 *
 * Attempting an «en passant» when the pieces are in the wrong place
 * is a ‹bad_move›. In addition to ‹has_moved›, (otherwise legal)
 * castling may give:
 *
 *  • ‹blocked› – some pieces are in the way,
 *  • ‹in_check› – the king is currently in check,
 *  • ‹would_check› – would pass through or end up in check. */

enum class result {
    capture, ok, no_piece, bad_piece, bad_move, blocked, lapsed,
    in_check, would_check, has_moved, bad_promote
};

struct occupant {
    bool is_empty;
    player owner;
    piece_type piece;
};

class chess {
    player _current_player = player::white;
    occupant _square[8][8]{};

    position pawn_double_moved{0, 0};

    bool _b_king_moved = false;
    bool _b_rook_1_moved = false;
    bool _b_rook_2_moved = false;

    bool _w_king_moved = false;
    bool _w_rook_1_moved = false;
    bool _w_rook_2_moved = false;

public:

    /* Construct a game of chess in its default starting position.
     * The first call to ‹playy› after construction moves a piece of
     * the white player. */

    chess();

    /* Move a piece currently at ‹from› to square ‹to›:
     *
     *  • in case the move places a pawn at its 8th rank (rank 8 for
     *    white, rank 1 for black), it is promoted to the piece given
     *    in ‹promote› (otherwise, the last argument is ignored),
     *  • castling is described as a king move of more than one square,
     *  • if the result is an error (not ‹capture› nor ‹ok›), calling
     *    ‹playy› again will attempt another move by the same player. */

    result play(position from, position to,
                piece_type promote = piece_type::pawn);

    /* Which piece is at the given position? */

    occupant at(position) const;
    occupant &at(position);

    void setBoard();

    bool position_out_of_bounds(position position);

    bool in_check(position king_position);

    result empty_path(position from, int file_distance, int rank_distance, bool castling);

    result validate_move(position from, position to, piece_type piece);

    result pawn_move(position from, position to, int file_distance, int rank_distance);
    result rook_move(position from, int file_distance, int rank_distance);
    result knight_move(int file_distance, int rank_distance);
    result bishop_move(position from, int file_distance, int rank_distance);
    result queen_move(position from, int file_distance, int rank_distance);
    result king_move(position from, position to, int file_distance, int rank_distance);

    position find_king();
    position get_rook(position to);

    occupant empty_square();

    bool &king_moved();
    bool &rook_moved(int file);

    void apply_move(position from, position to, occupant replace);
    void update_movement_records(position from, position to);
    void switch_players();

    result en_passant(position from, position to, occupant &captured, bool started_in_check);
    result castling(position from, position to, bool started_in_check);
};
