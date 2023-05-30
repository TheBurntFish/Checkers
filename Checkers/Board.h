#pragma once
#include <vector>

#include "SDL.h"
#include "SDL_image.h"

#include <iostream>
#include <cmath>


//y then x

//char board[8][8]{
//	{' ','o',' ','o',' ','o',' ','o'},
//	{'o',' ','o',' ','o',' ','o',' '},
//	{' ','o',' ','o',' ','o',' ','o'},
//	{' ',' ',' ',' ',' ',' ',' ',' '},
//	{' ',' ',' ',' ',' ',' ',' ',' '},
//	{'O',' ','O',' ','O',' ','O',' '},
//	{' ','O',' ','O',' ','O',' ','O'},
//	{'O',' ','O',' ','O',' ','O',' '}
//};

char board[8][8]{
	{' ',' ',' ','o',' ','o',' ',' '},
	{' ',' ','O',' ',' ',' ','O',' '},
	{' ',' ',' ','O',' ',' ',' ',' '},
	{' ',' ','O',' ','O',' ',' ',' '},
	{' ',' ',' ',' ',' ',' ',' ',' '},
	{' ',' ',' ',' ',' ',' ',' ',' '},
	{' ',' ',' ',' ',' ',' ',' ',' '},
	{'O',' ','O',' ','O',' ','O',' '}
};
SDL_Rect rect_board[8][8];
SDL_Rect rect;
SDL_Rect background;
void draw_board(SDL_Renderer* render, int width, int height) {
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	background = { 0, 0, width, height };
	SDL_RenderFillRect(render, &background);
	int rect_width = width/8;
	int rect_height = height/8;

	SDL_SetRenderDrawColor(render, 0, 255, 255, 255);





	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if ((row + col) % 2 == 1) {

				rect = { rect_width * row, rect_height * col, rect_width, rect_height };

				rect_board[row][col] = rect;
				SDL_RenderFillRect(render, &rect_board[row][col]);
			}
		}
	}
}


auto textures = std::vector<SDL_Texture*>{};

void draw_chips(SDL_Renderer* render) {
	SDL_Surface* black_chips = IMG_Load("CheckersB.png"); // Load the image
	SDL_Surface* white_chips = IMG_Load("CheckersW.png"); // Load the image

	SDL_Surface* black_kings = IMG_Load("CheckersBKing.png"); // Load the image
	SDL_Surface* white_kings = IMG_Load("CheckersWKing.png"); // Load the image
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (board[row][col] == 'o') {


				SDL_Texture* texture = SDL_CreateTextureFromSurface(render, black_chips); // Create a texture from the image
				SDL_RenderCopy(render, texture, NULL, &rect_board[col][row]);
				textures.push_back(texture);
			}
			if (board[row][col] == 'O') {

				SDL_Texture* texture = SDL_CreateTextureFromSurface(render, white_chips); // Create a texture from the image
				SDL_RenderCopy(render, texture, NULL, &rect_board[col][row]);
				textures.push_back(texture);

			}
			if (board[row][col] == 'k') {

				SDL_Texture* texture = SDL_CreateTextureFromSurface(render, black_kings); // Create a texture from the image
				SDL_RenderCopy(render, texture, NULL, &rect_board[col][row]);
				textures.push_back(texture);

			}
			if (board[row][col] == 'K') {

				SDL_Texture* texture = SDL_CreateTextureFromSurface(render, white_kings); // Create a texture from the image
				SDL_RenderCopy(render, texture, NULL, &rect_board[col][row]);
				textures.push_back(texture);

			}
		}
	}
	SDL_FreeSurface(black_chips); // Free the image surface
	SDL_FreeSurface(white_chips); // Free the image surface
	SDL_FreeSurface(black_kings); // Free the image surface
	SDL_FreeSurface(white_kings); // Free the image surface
}




// 0 -> no // 1 -> left // 2 -> right // 3 -> left or right
std::vector<int> canMove(int x_, int y_, bool is_white_, bool is_king_) {
	std::vector<int> moves;

	int forward_ = is_white_ ? -1 : 1;
	//check relative left
	if (x_ > 0) {
		//check 1 ahead
		if (board[y_ + forward_][x_ - 1] == ' ') {
			moves.push_back(1);
		}
		//check 1 behind left for king
		if (is_king_ && board[y_ - forward_][x_ - 1] == ' ') {
			moves.push_back(3);
		}
	}


	//check relative right
	if (x_ < 7) {
		//check 1 ahead
		if (board[y_ + forward_][x_ + 1] == ' ') {
			moves.push_back(2);
		}
		if (is_king_ && board[y_ - forward_][x_ + 1] == ' ') {
			moves.push_back(4);
		}
	}



	return moves;
}



std::vector<int> canAttack(int x_, int y_, bool is_white_, bool is_king_) {
	std::vector<int> attacks;
	int forward_ = is_white_ ? -1 : 1;

	char enemy = (is_white_) ? 'o' : 'O';
	char enemyKing = (is_white_) ? 'k' : 'K';


	//check relative left
	if (x_ > 1) {

		//if left is enemy and then empty
		if (board[y_ + forward_][x_ - 1] == enemy || board[y_ + forward_][x_ - 1] == enemyKing) {
			if (board[y_ + (forward_ * 2)][x_ - 2] == ' ') {
				attacks.push_back(1);
			}
		}
		//if left and behind is enemy then empty
		if (is_king_ && (board[y_ - forward_][x_ - 1] == enemy || board[y_ - forward_][x_ - 1] == enemyKing)) {
			if (board[y_ - (forward_ * 2)][x_ - 2] == ' ') {
				attacks.push_back(3);
			}
		}

	}

	//check relative right
	if (x_ < 6) {
		//if right and forward is enemy then empty
		if (board[y_ + forward_][x_ + 1] == enemy || board[y_ + forward_][x_ + 1] == enemyKing) {
			if (board[y_ + (forward_ * 2)][x_ + 2] == ' ') {
				attacks.push_back(2);
			}
		}
		//if right and behind is enemy then empty
		if (is_king_ && (board[y_ - forward_][x_ + 1] == enemy || board[y_ - forward_][x_ + 1] == enemyKing)) {
			if (board[y_ - (forward_ * 2)][x_ + 2] == ' ') {
				attacks.push_back(4);
			}
		}
	}
	return attacks;
}
//gets char piece base on is white and is king
char checkPiece(bool is_white_, bool is_king_) {
	char piece;
	if (is_white_) {
		piece = 'O';
		if (is_king_) {
			piece = 'K';
		}
	}
	else {
		piece = 'o';
		if (is_king_) {
			piece = 'k';
		}
	}
	return piece;
}
//implement attacking as a function/recursion

//get piece location, check attack, get mouse input, get next location, check attack, get mouse input


//////Movement
void moveForwardLeft(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ + forward_][x_ - 1] = piece;
}

void moveForwardRight(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ + forward_][x_ + 1] = piece;

}

void moveBackwardLeft(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ - forward_][x_ - 1] = piece;
}

void moveBackwardRight(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ - forward_][x_ + 1] = piece;
}

/////////Attacks
void attackForwardLeft(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ + forward_][x_ - 1] = ' ';
	board[y_ + (forward_ * 2)][x_ - 2] = piece;
}

void attackForwardRight(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ + forward_][x_ + 1] = ' ';
	board[y_ + (forward_ * 2)][x_ + 2] = piece;
}

void attackBackwardLeft(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ - forward_][x_ - 1] = ' ';
	board[y_ - (forward_ * 2)][x_ - 2] = piece;
}

void attackBackwardRight(int x_, int y_, bool is_white_, bool is_king_) {
	int forward_ = is_white_ ? -1 : 1;
	char piece = checkPiece(is_white_, is_king_);
	board[y_][x_] = ' ';
	board[y_ - forward_][x_ + 1] = ' ';
	board[y_ - (forward_ * 2)][x_ + 2] = piece;
}