#include "chess.hpp"

chess::chess() { setBoard(); }

void chess::setBoard() {
    std::vector<piece_type> pieces{piece_type::rook, piece_type::knight, piece_type::bishop, piece_type::queen,
                                   piece_type::king, piece_type::bishop, piece_type::knight, piece_type::rook};
    for (int i = 0; i < 8; ++i) {
        _square[0][i] = occupant{false, player::white, pieces[i]};
        _square[1][i] = occupant{false, player::white, piece_type::pawn};
        _square[6][i] = occupant{false, player::black, piece_type::pawn};
        _square[7][i] = occupant{false, player::black, pieces[i]};
    }

    for (int i = 2; i < 6; ++i) {
        for (int j = 0; j < 8; ++j) {
            _square[i][j].is_empty = true;
        }
    }
}

occupant chess::at(position p) const {
    return _square[p.rank - 1][p.file - 1];
}

occupant &chess::at(position p) {
    return _square[p.rank - 1][p.file - 1];
}

// Checks weather the path is clear
// bool castling = true, when checking empty path along with the possibility of being in check on the way when castling
result chess::empty_path(position from, int file_distance, int rank_distance, bool castling) {
    bool empty_square = true;

    while (empty_square) {
        if (file_distance < 0) file_distance++;
        if (file_distance > 0) file_distance--;
        if (rank_distance < 0) rank_distance++;
        if (rank_distance > 0) rank_distance--;

        position square = {from.file + file_distance, from.rank + rank_distance};
        empty_square = at(square).is_empty;

        if (castling && in_check(square)) {
            return result::would_check;
        }
    }
    // Found a piece on the way
    if (file_distance != 0 || rank_distance != 0) {
        return result::blocked;
    }
    return result::ok;
}

result chess::pawn_move(position from, position to, int file_distance, int rank_distance) {
    // Check correct direction
    if ((rank_distance > 0 && at(from).owner == player::black)
        || (rank_distance < 0 && at(from).owner == player::white)) {
        return result::bad_move;
    }

    // First move of a pawn, 2 steps.
    if (std::abs(rank_distance) == 2 && file_distance == 0) {
        if ((rank_distance == 2 && from.rank == 2)
            || (rank_distance == -2 && from.rank == 7)) {
            if (at(to).is_empty && at({to.file, to.rank - rank_distance / 2}).is_empty) {
                return result::ok;
            }
            return result::blocked;
        }
    }
    // 1 step.
    if (std::abs(rank_distance) == 1) {
        // Passing
        if (file_distance == 0) {
            if (at(to).is_empty) {
                return result::ok;
            }
            return result::blocked;
        }
        // Capture
        if (std::abs(file_distance) == 1) {
            // En passant capture
            position capture_ep{from.file + file_distance, from.rank};
            if (!at(capture_ep).is_empty && at(capture_ep).owner != at(from).owner) {
                if (to.rank == 3 || to.rank == 6) {
                    return result::ok;
                }
            }
            // Diagonal capture
            if (!at(to).is_empty && at(from).owner != at(to).owner) {
                return result::ok;
            }
        }
    }
    return result::bad_move;
}

result chess::rook_move(position from, int file_distance, int rank_distance) {
    if (file_distance != 0 && rank_distance != 0) {
        return result::bad_move;
    }
    return empty_path(from, file_distance, rank_distance, false);
}

result chess::knight_move(int file_distance, int rank_distance) {
    if (!((std::abs(file_distance) == 2 && std::abs(rank_distance) == 1)
          || (std::abs(file_distance) == 1 && std::abs(rank_distance) == 2))) {
        return result::bad_move;
    }
    return result::ok;
}

result chess::bishop_move(position from, int file_distance, int rank_distance) {
    if (std::abs(file_distance) != std::abs(rank_distance)) {
        return result::bad_move;
    }
    return empty_path(from, file_distance, rank_distance, false);
}

result chess::queen_move(position from, int file_distance, int rank_distance) {
    if ((file_distance != 0 && rank_distance != 0)
        && std::abs(file_distance) != std::abs(rank_distance)) {
        return result::bad_move;
    }
    return empty_path(from, file_distance, rank_distance, false);
}

result chess::king_move(position from, position to, int file_distance, int rank_distance) {
    if (std::abs(file_distance) <= 1 && std::abs(rank_distance) <= 1) {
        return result::ok;
    }
    // Check correct castling layout
    if (std::abs(file_distance) == 2 && from.file == 5 && rank_distance == 0) {
        if (at(get_rook(to)).piece != piece_type::rook) {
            return result::bad_move;
        }
        return empty_path(from, file_distance, rank_distance, false);
    }
    return result::bad_move;
}

bool chess::position_out_of_bounds(position position) {
    return ((position.file < 1 || position.file > 8)
            || (position.rank < 1 || position.rank > 8));
}

result chess::validate_move(position from, position to, piece_type piece) {
    assert(!position_out_of_bounds(from));
    assert(!position_out_of_bounds(to));

    int distance_file = to.file - from.file;
    int distance_rank = to.rank - from.rank;

    if (distance_rank == 0 && distance_file == 0) {
        return result::bad_move;
    }

    switch (piece) {
        case piece_type::pawn: {
            return pawn_move(from, to, distance_file, distance_rank);
        }
        case piece_type::rook: {
            return rook_move(from, distance_file, distance_rank);
        }
        case piece_type::knight: {
            return knight_move(distance_file, distance_rank);
        }
        case piece_type::bishop: {
            return bishop_move(from, distance_file, distance_rank);
        }
        case piece_type::queen: {
            return queen_move(from, distance_file, distance_rank);
        }
        case piece_type::king: {
            return king_move(from, to, distance_file, distance_rank);
        }
    }
    return result::ok;
}

void chess::update_movement_records(position from, position to) {
    // Update first move for castling
    if (at(to).piece == piece_type::king) {
        king_moved() = true;
    }
    if (at(to).piece == piece_type::rook) {
        rook_moved(from.file) = true;
    }
    // Update pawn double move
    if (at(from).piece == piece_type::pawn && std::abs(from.rank - to.rank) == 2) {
        pawn_double_moved = {to.file, to.rank};
    } else {
        pawn_double_moved = {0, 0};
    }
}

void chess::switch_players() {
    if (_current_player == player::white) {
        _current_player = player::black;
    } else {
        _current_player = player::white;
    }
}

position chess::find_king() {
    position king_position{};
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (!_square[i][j].is_empty && _square[i][j].owner == _current_player
                && _square[i][j].piece == piece_type::king) {
                king_position = {j + 1, i + 1};
                break;
            }
        }
    }
    return king_position;
}

position chess::get_rook(position to) {
    if (to.file == 3) {
        return {(1), to.rank};
    }
    return {(8), to.rank};
}

bool &chess::king_moved() {
    if (_current_player == player::white) {
        return _w_king_moved;
    }
    return _b_king_moved;
}

bool &chess::rook_moved(int file) {
    if (_current_player == player::white) {
        if (file > 5) {
            return _w_rook_2_moved;
        }
        return _w_rook_1_moved;
    }
    if (file > 5) {
        return _b_rook_2_moved;
    }
    return _b_rook_1_moved;
}

bool chess::in_check(position king_position) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (!_square[i][j].is_empty && _square[i][j].owner != _current_player) {
                if (validate_move({j + 1, i + 1}, king_position, at({j + 1, i + 1}).piece) == result::ok) {
                    return true;
                }
            }
        }
    }
    return false;
}

occupant chess::empty_square() {
    return {true, player::white, piece_type::pawn};
}

result chess::en_passant(position from, position to, occupant &captured, bool started_in_check) {
    position capture_ep = {from.file - (from.file - to.file), from.rank};

    if (capture_ep.file != pawn_double_moved.file || capture_ep.rank != pawn_double_moved.rank) {
        return result::lapsed;
    }
    // Remove the opponent's pawn and apply the move
    captured = at(capture_ep);
    at(capture_ep) = empty_square();
    apply_move(from, to, empty_square());

    // Revert the move if in check
    if (in_check(find_king())) {
        at(capture_ep) = captured;
        apply_move(to, from, empty_square());
        if (started_in_check) {
            return result::in_check;
        }
        return result::would_check;
    }
    update_movement_records(from, to);
    switch_players();
    return result::capture;
}

result chess::castling(position from, position to, bool started_in_check) {
    if (rook_moved(to.file) || king_moved()) {
        return result::has_moved;
    }
    if (started_in_check) {
        return result::in_check;
    }
    position rook_position = get_rook(to);
    int new_file = rook_position.file == 1 ? 4 : 6; // King to Rook distance
    position new_rook_position = {new_file, rook_position.rank};

    result path = empty_path(from, rook_position.file - from.file, 0, true);
    if (path != result::ok) {
        return path;
    }
    // King move
    apply_move(from, to, empty_square());
    // Rook move
    apply_move(rook_position, new_rook_position, empty_square());

    update_movement_records(from, to);
    switch_players();
    return result::ok;
}

void chess::apply_move(position from, position to, occupant replace) {
    at(to) = at(from);
    at(from) = replace;
}

result chess::play(position from, position to, piece_type promote) {
    if (at(from).is_empty) {
        return result::no_piece;
    }
    if (at(from).owner != _current_player) {
        return result::bad_piece;
    }
    result move_validity = validate_move(from, to, at(from).piece);
    if (move_validity != result::ok) {
        return move_validity;
    }
    if (!at(to).is_empty && at(to).owner == at(from).owner) {
        return result::blocked;
    }
    occupant captured = at(to);
    bool started_in_check = in_check(find_king());

    // En passant
    if (at(from).piece == piece_type::pawn) {
        if (from.file - to.file != 0 && at(to).is_empty) {
            return en_passant(from, to, captured, started_in_check);
        }
    }
    // Castling
    if (at(from).piece == piece_type::king) {
        if (from.file == 5 && (to.file == 7 || to.file == 3)) {
            return castling(from, to, started_in_check);
        }
    }
    apply_move(from, to, empty_square());

    // Revert the move if in check
    if (in_check(find_king())) {
        apply_move(to, from, captured);
        if (started_in_check) {
            return result::in_check;
        }
        return result::would_check;
    }
    // Promotion of the pawn
    if ((at(to).piece == piece_type::pawn) && (to.rank == 1 || to.rank == 8)) {
        if (promote == piece_type::king || promote == piece_type::pawn) {
            apply_move(to, from, captured);
            return result::bad_promote;
        }
        at(to).piece = promote;
    }
    update_movement_records(from, to);
    switch_players();

    if (captured.is_empty) {
        return result::ok;
    }
    return result::capture;
}

int main() {}
