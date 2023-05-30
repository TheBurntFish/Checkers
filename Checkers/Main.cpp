#include "Board.h"

#include "SDL.h"
#include "SDL_image.h"

#include <cmath>

//rows then columns y then x

int GameState = 0;

char turn = 'W';
bool chainAttack = false;
bool flagOnce = true;
bool is_white, is_king, has_moveable_pieces;
SDL_Rect piece;
//bool selected = false;
int x, y, selected_x, selected_y, movement, attack, forward;

std::vector<int> possibleAttacks;
std::vector<int> possibleMoves;

std::vector<int> availableX;
std::vector<int> availableY;

SDL_Window* window = SDL_CreateWindow("ReallyBadCheckers", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 720, 720, (SDL_WINDOW_SHOWN));
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
SDL_Event event;


void checkForced(int row, int col, bool chainAttack) {
	if (chainAttack) {
		availableX.clear();
		availableY.clear();
	}
	if (turn == 'B') {
		if (board[row][col] == 'o') {
			if (canAttack(col, row, false, false).size() > 0) {
				availableX.push_back(col);
				availableY.push_back(row);
				has_moveable_pieces = true;
			}
			if (row == 7) {
				board[row][col] = 'k';
			}
			if (!has_moveable_pieces && canMove(col, row, false, false).size() > 0) {
				has_moveable_pieces = true;
			}
		}
		if (board[row][col] == 'k') {
			if (canAttack(col, row, false, true).size() > 0) {
				availableX.push_back(col);
				availableY.push_back(row);
				has_moveable_pieces = true;
			}
			if (!has_moveable_pieces && canMove(col, row, false, true).size() > 0) {
				has_moveable_pieces = true;
			}
		}
	}
	else if (turn == 'W') {
		if (board[row][col] == 'O') {
			if (canAttack(col, row, true, false).size() > 0) {
				availableX.push_back(col);
				availableY.push_back(row);
				has_moveable_pieces = true;
			}
			if (row == 0) {
				board[row][col] = 'K';
			}
			if (!has_moveable_pieces && canMove(col, row, true, false).size() > 0) {
				has_moveable_pieces = true;
			}
		}
		if (board[row][col] == 'K') {
			if (canAttack(col, row, true, true).size() > 0) {
				availableX.push_back(col);
				availableY.push_back(row);
				has_moveable_pieces = true;
			}
			if (!has_moveable_pieces && canMove(col, row, true, true).size() > 0) {
				has_moveable_pieces = true;
			}
		}
	}
}


void nextTurn() {
	//selected = false;
	GameState = 0;
	has_moveable_pieces = false;
	turn = (turn == 'B' ? 'W' : 'B');
	for (auto texture : textures) {
		SDL_DestroyTexture(texture);
	}

	chainAttack = false;
	//for forced attacks
	availableX.clear();
	availableY.clear();


	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {

			checkForced(row, col, chainAttack);
			//need to fix being able to not always committing to a chain attack

			std::cout << board[row][col];

		}
		std::cout << std::endl;
	}

	if (!has_moveable_pieces && turn == 'W') {
		std::cout << "Black Wins" << std::endl;
	}
	if (!has_moveable_pieces && turn == 'B') {
		std::cout << "White Wins" << std::endl;
	}
}

void pieceSelect() {
	possibleMoves.clear();
	possibleAttacks.clear();

	selected_x = x;
	selected_y = y;
	if (board[y][x] == 'o' || board[y][x] == 'k') {
		is_white = false;
	}
	else {
		is_white = true;
	}
	if (board[y][x] == 'k' || board[y][x] == 'K') {
		is_king = true;
	}
	else {
		is_king = false;
	}

	forward = is_white ? -1 : 1;
	possibleMoves = canMove(selected_x, selected_y, is_white, is_king);
	std::cout << "Movement: ";
	for (int index = 0; index < possibleMoves.size(); index++) {
		std::cout << possibleMoves.at(index) << ", ";
	}
	std::cout << std::endl;

	possibleAttacks = canAttack(selected_x, selected_y, is_white, is_king);
	std::cout << "Attack: ";

	for (int index = 0; index < possibleAttacks.size(); index++) {
		std::cout << possibleAttacks.at(index) << ", ";
	}
	std::cout << std::endl;

	//std::cout << "Selected piece: " << selected_x << " , " << selected_y << std::endl;
}
void highlight(int width, int height) {

	SDL_SetRenderDrawColor(renderer, 200, 0, 0, 100);
	for (int index = 0; index < availableX.size(); index++) {

		//rect_board[availableX.at(index)][availableY.at(index)]
		SDL_RenderFillRect(renderer, &rect_board[availableX.at(index)][availableY.at(index)]);
	}

}

void render() {
	SDL_RenderClear(renderer);
	draw_board(renderer, 720, 720);
	highlight(720, 720);
	draw_chips(renderer);
	SDL_RenderPresent(renderer);
}
////////////////////////uses ram when clicking on nothing repeatedly prob creating ****

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	bool quit = false;

	int width, height;

	SDL_GetWindowSize(window, &width, &height);




	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
			//detect left click
			/*
			States:
			0 -> Piece select
			1 -> move / attack
			2 -> Chain attack
			3 ->
			4 -> Promote?
			5 ->
			*/
			switch (GameState) {
			case 0: //Piece not selected yet
				if(flagOnce){
					std::cout << "Unselected" << std::endl;
					flagOnce = false;
					render();
				}
				//detect left click
				if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)) {




					GameState = 0;
					//Get Coordinates for tile clicked on
					x = (int)ceil(event.button.x / 90);
					y = (int)ceil(event.button.y / 90);
					std::cout << "Selected Position:" << x << ", " << y << std::endl;

					//check forced attacks are empty
					if (availableX.empty()) {
						//check if tile has a piece
						if (board[y][x] != ' ') {
							//if black piece
							if (board[y][x] == 'o' && turn == 'B' && (canMove(x, y, false, false).size() > 0 || canAttack(x, y, false, false).size() > 0)) {
								pieceSelect();
								GameState = 1; // piece selected
							}
							//if white piece \/
							else if (board[y][x] == 'O' && turn == 'W' && (canMove(x, y, true, false).size() > 0 || canAttack(x, y, true, false).size() > 0)) {
								pieceSelect();
								GameState = 1; // piece selected
							}
							//if black king
							else if (board[y][x] == 'k' && turn == 'B' && (canMove(x, y, false, true).size() > 0 || canAttack(x, y, false, true).size() > 0)) {
								pieceSelect();
								GameState = 1; // piece selected
							}
							//if white king
							else if (board[y][x] == 'K' && turn == 'W' && (canMove(x, y, true, true).size() > 0 || canAttack(x, y, true, true).size() > 0)) {
								pieceSelect();
								GameState = 1; // piece selected
							}
							else {
								std::cout << "Piece not Selected" << std::endl;
								GameState = 0; // piece not selected
							}
						}
						else {
							std::cout << "Piece not Selected" << std::endl;
						}
					}
					else {
						for (int index = 0; index < availableX.size(); index++) {
							std::cout << "Forced: " << availableX.at(index) << ", " << availableY.at(index) << std::endl;
							if (x == availableX.at(index) && y == availableY.at(index)) {
								pieceSelect();
								GameState = 1;
							}
						}
					}
					render();
				}

				break;
			case 1: //Piece has been selected
				if (flagOnce) {
					flagOnce = false;
					render();
				}
				if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)) {
					flagOnce = true;
					bool aForwardLeft = false;
					bool aForwardRight = false;
					bool aBackwardLeft = false;
					bool aBackwardRight = false;
					bool mForwardLeft = false;
					bool mForwardRight = false;
					bool mBackwardLeft = false;
					bool mBackwardRight = false;

					x = (int)ceil(event.button.x / 90);
					y = (int)ceil(event.button.y / 90);

					for (int index = 0; index < possibleAttacks.size(); index++) {
						if (possibleAttacks.at(index) == 1) {
							aForwardLeft = true;
						}
						if (possibleAttacks.at(index) == 2) {
							aForwardRight = true;
						}
						if (possibleAttacks.at(index) == 3) {
							aBackwardLeft = true;
						}
						if (possibleAttacks.at(index) == 4) {
							aBackwardRight = true;
						}
					}

					for (int index = 0; index < possibleMoves.size(); index++) {
						if (possibleMoves.at(index) == 1) {
							mForwardLeft = true;
						}
						if (possibleMoves.at(index) == 2) {
							mForwardRight = true;
						}
						if (possibleMoves.at(index) == 3) {
							mBackwardLeft = true;
						}
						if (possibleMoves.at(index) == 4) {
							mBackwardRight = true;
						}
					}

					if (possibleAttacks.empty()) {
						//move left
						if (mForwardLeft) {
							if (x == selected_x - 1 && y == selected_y + forward) {
								moveForwardLeft(selected_x, selected_y, is_white, is_king);
								nextTurn();
							}
							else {
								GameState = 0;
							}
						}
						//move right
						if (mForwardRight) {
							if (x == selected_x + 1 && y == selected_y + forward) {
								moveForwardRight(selected_x, selected_y, is_white, is_king);

								nextTurn();
							}
							else {
								GameState = 0;
							}
						}
						if (mBackwardLeft) {
							if (is_king && x == selected_x - 1 && y == selected_y - forward) {
								moveBackwardLeft(selected_x, selected_y, is_white, is_king);

								nextTurn();
							}
							else {
								GameState = 0;
							}
						}
						if (mBackwardRight) {
							if (is_king && x == selected_x + 1 && y == selected_y - forward) {
								moveBackwardRight(selected_x, selected_y, is_white, is_king);

								nextTurn();
							}
							else {
								GameState = 0;
							}
						}
					}

					//attack left
					if (aForwardLeft) {
						if (x == selected_x - 2 && y == selected_y + (forward * 2)) {
							attackForwardLeft(selected_x, selected_y, is_white, is_king);


							if (canAttack(selected_x - 2, selected_y + (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;
								pieceSelect();
								GameState = 2; //Chain attack

								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}
					//attack right
					if (aForwardRight) {
						if (x == selected_x + 2 && y == selected_y + (forward * 2)) {
							attackForwardRight(selected_x, selected_y, is_white, is_king);


							if (canAttack(selected_x + 2, selected_y + (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;

								pieceSelect();
								GameState = 2; //Chain attack

								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}

					//attack back left
					if (aBackwardLeft) {
						if (is_king && x == selected_x - 2 && y == selected_y - (forward * 2)) {
							attackBackwardLeft(selected_x, selected_y, is_white, is_king);


							if (canAttack(selected_x - 2, selected_y - (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;

								pieceSelect();
								GameState = 2; //Chain attack

								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}
					//attack right
					if (aBackwardRight) {
						if (is_king && x == selected_x + 2 && y == selected_y - (forward * 2)) {
							attackBackwardRight(selected_x, selected_y, is_white, is_king);
							draw_board(renderer, width, height); highlight(width, height);

							if (canAttack(selected_x + 2, selected_y - (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;

								pieceSelect();
								GameState = 2; //Chain attack
								draw_board(renderer, width, height); highlight(width, height);
								draw_chips(renderer);
								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}
				}
				break;
			case 2: //chain attack

				checkForced(selected_y, selected_x, true);
				if (flagOnce) {
					flagOnce = false;
					render();
				}
				if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT)) {
					flagOnce = true;
					bool aForwardLeft = false;
					bool aForwardRight = false;
					bool aBackwardLeft = false;
					bool aBackwardRight = false;

					x = (int)ceil(event.button.x / 90);
					y = (int)ceil(event.button.y / 90);

					for (int index = 0; index < possibleAttacks.size(); index++) {
						if (possibleAttacks.at(index) == 1) {
							aForwardLeft = true;
						}
						if (possibleAttacks.at(index) == 2) {
							aForwardRight = true;
						}
						if (possibleAttacks.at(index) == 3) {
							aBackwardLeft = true;
						}
						if (possibleAttacks.at(index) == 4) {
							aBackwardRight = true;
						}
					}

					//attack left
					if (aForwardLeft) {
						if (x == selected_x - 2 && y == selected_y + (forward * 2)) {
							attackForwardLeft(selected_x, selected_y, is_white, is_king);
							draw_board(renderer, width, height); highlight(width, height);

							if (canAttack(selected_x - 2, selected_y + (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;
								pieceSelect();
								GameState = 2; //Chain attack
								draw_board(renderer, width, height); highlight(width, height);
								draw_chips(renderer);
								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}
					//attack right
					if (aForwardRight) {
						if (x == selected_x + 2 && y == selected_y + (forward * 2)) {
							attackForwardRight(selected_x, selected_y, is_white, is_king);
							draw_board(renderer, width, height); highlight(width, height);

							if (canAttack(selected_x + 2, selected_y + (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;
								pieceSelect();
								GameState = 2; //Chain attack
								draw_board(renderer, width, height); highlight(width, height);
								draw_chips(renderer);
								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}

					//attack back left
					if (aBackwardLeft) {
						if (is_king && x == selected_x - 2 && y == selected_y - (forward * 2)) {
							attackBackwardLeft(selected_x, selected_y, is_white, is_king);
							draw_board(renderer, width, height); highlight(width, height);

							if (canAttack(selected_x - 2, selected_y - (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;
								pieceSelect();
								GameState = 2; //Chain attack
								draw_board(renderer, width, height); highlight(width, height);
								draw_chips(renderer);
								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}
					//attack back right
					if (aBackwardRight) {
						if (is_king && x == selected_x + 2 && y == selected_y - (forward * 2)) {
							attackForwardRight(selected_x, selected_y, is_white, is_king);
							draw_board(renderer, width, height); highlight(width, height);

							if (canAttack(selected_x + 2, selected_y - (forward * 2), is_white, is_king).size() > 0) {
								std::cout << "chain" << std::endl;
								pieceSelect();
								GameState = 2; //Chain attack
								draw_board(renderer, width, height); highlight(width, height);
								draw_chips(renderer);
								//nextTurn();
							}
							else {
								nextTurn();
							}
						}
						else {
							//GameState = 0;
						}
					}
				}


			}

			/////////implement kings dumb ass
		}
	}

	for (auto texture : textures) {
		SDL_DestroyTexture(texture);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}