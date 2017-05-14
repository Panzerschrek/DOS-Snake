#include "../include/stdio.h"
#include "../include/stdlib.h"
#define NTVDM

#define COLOR_BLACK   0
#define COLOR_BLUE    1
#define COLOR_GREEN   2
#define COLOR_CYAN    3
#define COLOR_RED     4
#define COLOR_MAGENTA 5
#define COLOR_BROWN   6
#define COLOR_LGRAY   7
#define COLOR_DGRAY   8
#define COLOR_LBLUE   9
#define COLOR_LGREEN  10
#define COLOR_LCYAN   11
#define COLOR_LRED    12
#define COLOR_LMAGENTA 13
#define COLOR_YELLOW  14
#define COLOR_WHITE   15


#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

void SetCursor( int x, int y )
{
	_asm
	 {
		mov ah, 2 ;cursor set
		mov bh, 0
		mov dh, y
		mov dl, x
		int 10h
	 }
}

void HexNop( unsigned int hex_nop_n )
{
	_asm
	{
		mov cx, 4096
		m2:
		push cx

		mov cx, hex_nop_n
		m1:
		nop
		loop m1

		pop cx
		loop m2
	}
}

void ShowSymbol( unsigned char x, unsigned char y,
char symbol, char b_color, char f_color )
{
	_asm
	{

	       ; pusha

		mov ah, 2 ;cursor set
		mov bh, 0
		mov dh, y
		mov dl, x
		int 10h



		mov ah, 9;symbol print
		mov bh, 0

		mov bl, f_color
		push ax
		mov al, 0
		mov al, b_color
		shl al, 1
		shl al, 1
		shl al, 1
		shl al, 1
		or bl, al
		pop ax

		mov al, symbol
		mov cx, 1
		int 10h

	       ; popa

	}
}


void ClearScreen(void)
{
	_asm
	{

	       ; pusha
		mov cx, 25*80

		clear:
		push cx
		mov ah, 14
		mov bh, 0
		mov al, ' '
		mov bh, 01110000b
		int 10h
		pop cx
		loop clear
	       ; popa
	}
}


void SetVideoMode(void)
{
	_asm
	{

	       ; pusha
		mov ah, 0
		mov al, 3
		int 10h
	       ; popa
	}
}


void Intro()
{
	int i, j;
	char* game_name=   "DOS-Snake by Panzerschrek";
	char* copyright=   "(c) 2013 Artjom Kunz";

	ClearScreen();
	j= strlen( game_name );
	for( i=0; i< j; i++ )
	ShowSymbol( 30+i,10, game_name[i], COLOR_BLACK, COLOR_LRED );
	j= strlen( copyright );
	for( i=0; i< j; i++ )
	ShowSymbol( 30+i, 11, copyright[i], COLOR_BLACK, COLOR_DGRAY );


	for( i=0; i<4; i++ )
		HexNop(65535);
}

void PrintField()
{
	int x, y;
	for( x=0; x< 80; x++ )
	{
		ShowSymbol( x, 0, '#', COLOR_BLACK, COLOR_LGRAY );
		ShowSymbol( x, 23, '#', COLOR_BLACK, COLOR_LGRAY );
	}
	for( y=1; y< 24; y++ )
	{
		ShowSymbol( 0, y, '#', COLOR_BLACK, COLOR_LGRAY );
		ShowSymbol( 79,y, '#', COLOR_BLACK, COLOR_LGRAY );
	}
}


int snake_segments[64][3];//3-d coordinate - rotation
int num_segments;
int head_rotation= LEFT;

int apple[2]= { 34, 12 };
int lifes=2, level= 1, score= 0;


void ShowScoreAndLevel()
{
       SetCursor( 1, 24 );
	printf( "Score: %d", score );
	printf( "  Level: %d", level );
	printf( "  Lifes: %d", lifes );
	SetCursor( 54, 24 );
	printf( "controls - WASD, Q - quit" );
	return;
}


void InitSnake()
{
	snake_segments[0][0]= 38;
	snake_segments[0][1]= 12;
	snake_segments[1][0]= 39;
	snake_segments[1][1]= 12;
	snake_segments[2][0]= 40;
	snake_segments[2][1]= 12;
	snake_segments[0][2]= snake_segments[1][2]= snake_segments[2][2]='<';

	num_segments= 3;
	head_rotation= LEFT;
}


void DrawApple()
{
	ShowSymbol( apple[0], apple[1], '*', COLOR_BLACK, COLOR_LRED );
}

void SpawnApple()
{

	int new_apple[2];
	int i;
	int in_snake= 1;

	while( in_snake )
	{
		new_apple[0]= ( rand() % 78 ) + 1;
		new_apple[1]= ( rand() % 22 ) + 1;
		in_snake= 0;
		for( i=0; i< num_segments; i++ )
		{
			if ( new_apple[0] == snake_segments[i][0] && new_apple[1] == snake_segments[i][1]  )
				in_snake= 1;
		}

	}
	apple[0]= new_apple[0];
	apple[1]= new_apple[1];
}


void Death()
{
	int i, game_over= 0;
	InitSnake();
	SpawnApple();
	score= 0;
	if( lifes > 0 )
		lifes--;
	else
		level= 1, score= 0, lifes=2, game_over= 1;

	ClearScreen();
	SetCursor( 35, 11 );
	printf( "You die" );
	if( game_over )
	{
		 SetCursor( 34, 12 );
		 printf( "Game over" );
	}

	for( i=0; i< 3; i++ )
	HexNop( 65535 );
	ClearScreen();

	PrintField();
	ShowScoreAndLevel();

}

void NextLevel()
{
	int i;
	InitSnake();


	level++;
	SpawnApple();
	lifes++;
	ClearScreen();

	SetCursor( 35, 11 );
	printf( "Level %d", level );
	for( i=0; i< 4; i++ )
	HexNop( 65535 );
	ClearScreen();
	PrintField();
	ShowScoreAndLevel();

}

//snake_segments[0] - head
//snake_segments[ num_segments - 1 ] - tail
void MoveSnake()
{
	int move_vector[2];
	int new_head[2];
	int i;
	char rotation_char;
	if( head_rotation == UP )
	{
		move_vector[0]= 0;
		move_vector[1]= -1;
		rotation_char= '^';
	}
	else if( head_rotation == DOWN )
	{
		move_vector[0]= 0;
		move_vector[1]= 1;
		rotation_char= 'V';
	}
	else if( head_rotation == LEFT )
	{
		move_vector[0]= -1;
		move_vector[1]= 0;
		rotation_char= '<';
	}
	else if( head_rotation == RIGHT )
	{
		move_vector[0]= 1;
		move_vector[1]= 0;
		rotation_char= '>';
	}


	new_head[0]= move_vector[0] + snake_segments[0][0];
	new_head[1]= move_vector[1] + snake_segments[0][1];
	if( new_head[0] < 1 || new_head[0] > 78 || new_head[1] < 1 || new_head[1] > 22 )
	{
		Death();
		return;
	}
	else
	{
		ShowSymbol( snake_segments[ num_segments -1][0],
			    snake_segments[ num_segments -1][1],
			    ' ', COLOR_BLACK, COLOR_LGRAY );//clear tail

		  //picked up apple
		if( new_head[0] == apple[0] && new_head[1] == apple[1] )
		{
			snake_segments[ num_segments ][0]= snake_segments[ num_segments - 1 ][0];
			snake_segments[ num_segments ][1]= snake_segments[ num_segments - 1 ][1];
			snake_segments[ num_segments ][2]= snake_segments[ num_segments - 1 ][2];
			num_segments++;
			SpawnApple();
			score+= level;
			ShowScoreAndLevel();
			if( num_segments > level * 2 + 3 || num_segments > 63 )
			 { NextLevel(); return;}

		}

		for( i= num_segments-1; i>=0; i-- )
		{
			snake_segments[i][0]= snake_segments[i-1][0];
			snake_segments[i][1]= snake_segments[i-1][1];
			snake_segments[i][2]= snake_segments[i-1][2];
		}//move snake

		snake_segments[0][0]= new_head[0];
		snake_segments[0][1]= new_head[1];
		snake_segments[0][2]= rotation_char;

		for( i= 1; i< num_segments; i++ )
		{
			if( snake_segments[i][0] == new_head[0] && snake_segments[i][1] == new_head[1] )
			{
				Death();//snake eat himself
				return;
			}
		}
	}
}

char GetKey()
{
	char result;
	_asm
	{
		mov ah, 6
		mov dl, 0FFh
		int 21h
		jz no_result
		mov result, al
		jmp end_func

		no_result:
		mov result, 0
		end_func:
	}
	return result;
}

void DrawSnake()
{
	int i;

	ShowSymbol( snake_segments[0][0], snake_segments[0][1], snake_segments[0][2], COLOR_BLACK, COLOR_LGREEN );
	for( i= 1; i< num_segments; i++ )
	{
		ShowSymbol( snake_segments[i][0], snake_segments[i][1], snake_segments[i][2], COLOR_BLACK, COLOR_GREEN );
	}
}


void MainLoop()
{
       unsigned  int i, j;
       char c;


       while(1)
       {

      while( c= GetKey() )
      {
       if( c =='w' )
       {
	     if( head_rotation != DOWN ) head_rotation= UP;
       }
       else if( c == 's' )
       {
		if( head_rotation != UP ) head_rotation= DOWN;
       }
       else if ( c == 'a' )
       {
	     if(head_rotation !=RIGHT)   head_rotation= LEFT;
       }
	else if( c == 'd' )
       {
	     if( head_rotation != LEFT )   head_rotation= RIGHT;
       }
	else if ( c == 'q' || c == 'Q' )
		return;
       }
	MoveSnake();
	DrawSnake();
	DrawApple();
	j= 65535 -( level  - 1 ) * 5461;
       // for( i=0; i< 1024; i++ )
	HexNop( j /2 );
       }
}

int main()
{

	unsigned int i;
	SetVideoMode();
	Intro();
	ClearScreen();
	PrintField();
	ShowScoreAndLevel();
	InitSnake();

       MainLoop();
       ClearScreen();
       SetVideoMode();
       return 0;
}
