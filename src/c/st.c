/************************************************************************************

ISSUE1

			STAR TRECK Version 1.00


Universe cords (X,Y)

      (0,0)-----------Y
        |
	|
	|
	|
	|
	X

Y increases from left to right
X increases from top to bottom


************************************************************************************/

// Get the following compile error with MS Visual Studio 2022
// "Error	C4996	'scanf': This function or variable may be unsafe.Consider using scanf_s instead.
// To disable deprecation, use _CRT_SECURE_NO_WARNINGS.""
// Add the following statement befire inclusion of the libraries to supress above error
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/******************************************* Game parameters **************************************/

#define UNIVERSE_SIZE 60			// 60   Universe sise, type int 
#define SRS_RANGE 3					// 3   Short Range Scan range, type int 
#define SCREEN_ROWS 25
#define K_PROB 45.0					// 45.0 Klingon probability, type float 
#define S_PROB 2					// 2   Space Station probability, type int 
#define W_PROB 1					// 1   Worm Hole probability, type int
#define MAX_RAN 800					// 800 Max my_random numbers for K and S per MAX_RAN, type int
#define MAX_NO_DOCK 15				// 15   Max. elapse time after dock before re-dock, type int
// Also see 						// Other fundamental game parameters approx. line 139

/******************************************* Game parameters **************************************/

char universe[UNIVERSE_SIZE][UNIVERSE_SIZE];
int star_date ;						// Star date 
int max_star_date;					// Game end date 

int ux, uy; 						// Universe cords. 
int ex, ey;							// xy cords. of Enterprise 
int dex = 0, dey = 0;				// xy cords. of docked Enterprise 
int sex[5], sey[5];					// Sector saves 
int sector_saved_already[4];		// Flag to indicate sector already saved 
int ssK[4], ssS[4], ssW[4];			// K, S & W count for sector save 
int can_dock_date;					// Docked clock - no docking after dock for a period of time 
int K_in_universe = 0;				// Number of Klingons in universe 
int SS_in_universe = 0;				// Space Stations in universe 
int K_in_sector;					// Klingons in current sector 
int S_in_sector;					// Space Stations in current sector 
int W_in_sector;					// Worm Hole in current sector 
int K_count[8];						// Klingon count for LRS 
int S_count[8];						// Space Station count for LRS 
int W_count[8];						// Worm hole count for LRS 

int shield = 0;						// Shields fully charged at 99 
int phaser = 0;						// Phasers fully charged at 50 
float  max_energy, energy;			// Consumed by Phasers, Photon Torpedoes, 
									// Impulse, Warp, SRS, LRS  
int phaser_power, phaser_dir;		// Phaser direction & power 
int pt, max_pt;						// Photon torpedoes 
int pt_power, pt_dir;				// Photon Torpedoes power & direction 
int impulse_power, impulse_dir;		// Impulse power & direction 
int warp_power, warp_dir;			// Warp power & direction 

int srs_flag = 1;					// If <= 0 srs not available 
int lrs_flag = 1;					// If <= 0 lrs not available 
int map_flag = 0;					// If <= 0 lrs not available 
int es_flag = 1;					// if <= 0 cannot energise shields 
int phaser_flag = 1;				// if <= 0 phasers not available 
int ep_flag = 1;					// if <= 0 cannot energise phasers 
int pt_flag = 1;					// if <= 0 photon torpedoes not available 
int warp_flag = 1;					// if <= 0 Warp power not available 
int impulse_flag = 1;				// if <= 0 Impulse power not available 
int squad_flag = 1;					// if <= 0 save quadrant not availablr 
int warp_active = 0;				// Need flag for SS collision 
int worm_active = 0;				// Flag fro worm hole active during moveea for recursave call 


/******************************************* Function prototypes **************************************/

void us(void);						// Universe Scan, DEBUG function
void map(void);						// Map
void map_debug(void);				// Debug map
void lrs(int display_flag);			// Long Range Scan 
void srs(int no_lines);				// Short Range Scan 
void srs_noprint(void);				// Short range scan to get sector stats simular to srs but no display 
void clears(void);					// Clear Screen
// void moveer(int iex, int iey);  	// Move Eeterprise relative
void moveea(int aex, int aey);		// Move Enterprise absolute 
void fphaser(int power, int dir); 	// Fire Phaser
void fpt(int power, int dir);		// Fire Photon Torpedoes
void impulse(int power, int dir);	// Impulse power
void warp(int power, int dir);		// Warp power 
void warps(int save_no);			// Warp to saved sector 
void str(void);						// Status Report
void damage(int dlevel);			// Allocate damage 
void intro(void);					// Intro Screen 
// void instructions(void); 		// Game instructions 
int ins(void);
void squad(int);					// Save Quadrant 
void lines(int lines);				// print lines 
void save_game(void);				// Save game 
int load_game(void);				// Load game 
int my_random(int); 				// Emulates assumed behaviour of old Borland Tubro C function

/**************************************** End Function prototypes *************************************/


int main()
{

	int no_lines;					// Number of lines for SRS 
	char c[] = "xxxx";				// Input command 

	int e;							// Energy value for shields and phasers 
	int p; 							// Power value for shields and phasers 
	int sn;							// Save number for Save Quadrant, squad 
	int sqn;						// Sectro save number 
	int i;


	for (i = 0; i <= 4; i++)		// Initilise sector_saved_already array 
		sector_saved_already[i] = 0;

	for (i = 0; i <= 3; i++)		// Initilise ssK array 
		ssK[i] = 0;

	for (i = 0; i <= 3; i++)		// Initilise ssS array 
		ssS[i] = 0;

	for (i = 0; i <= 3; i++)		// Initilise ssW array 
		ssW[i] = 0;	

/**********************************************************************

Create a universe of size (UNIVERSE_SIZE x UNIVERSE_SIZE).

**********************************************************************/ 

	//my_randomize(); - Borland Turbo C function to randomize random number generator seed.
    // Use this instead:
    // Seed the random number generator with the current time 
    srand(time(NULL)); 

	for (ux = 0; ux <= (UNIVERSE_SIZE - 1); ++ux) {
		for ( uy = 0; uy <= (UNIVERSE_SIZE - 1); ++uy ) {
	
			if (my_random(MAX_RAN) < K_PROB) {
				universe[ux][uy] = 'K';
				++K_in_universe;
			}
			else if (my_random(MAX_RAN) < S_PROB) {
				universe[ux][uy] = 'S';
				++SS_in_universe;
			}
			else if (my_random(MAX_RAN) < W_PROB) {
				universe[ux][uy] = 'W';
				
			}
			else
				universe[ux][uy] = '-';
		}
	}



	if ( universe[0][0] == 'K') {					// Make sure no Klingon at 0x0 to avoid
		universe[0][0] = '-';						// small bug in lrs which always counts
		--K_in_universe;							//Kingons at 0x0 even if within 1 srs  
	}				        						// of universe top left hand corner.  
	if ( universe[0][UNIVERSE_SIZE - 1] == 'K') {	// ISSUE1                            
		universe[0][UNIVERSE_SIZE - 1] = '-';		// Need to do for SS as well?
		--K_in_universe;							// Maybe better to fix bug !!	
	}   
	if ( universe[UNIVERSE_SIZE - 1][0] == 'K') {
		universe[UNIVERSE_SIZE - 1][0] = '-';
		--K_in_universe;
	}
	if ( universe[UNIVERSE_SIZE - 1][UNIVERSE_SIZE - 1] == 'K') {
		universe[0][0] = '-';
		--K_in_universe;
	}	


// Other game parameters 

	max_energy = (4 * UNIVERSE_SIZE) + ( (K_PROB / MAX_RAN) * ( (float) UNIVERSE_SIZE * (float) UNIVERSE_SIZE ));

	energy = max_energy;

	max_star_date = K_in_universe * 18;	// 18 Set max star date & Photon 
	star_date = max_star_date/2;
	max_pt = K_in_universe/15;			//    Torpedoes based on number  
	pt = max_pt;						//    of Klingons in universe   

	can_dock_date = star_date;

/**********************************************************************
 
Place the Star Ship Enterprise somewhere in the universe. It must never
be closer to the edge by less than one short range scan.

**********************************************************************/

	ex = my_random(UNIVERSE_SIZE - 1 - (2 * SRS_RANGE)) + SRS_RANGE;
	ey = my_random(UNIVERSE_SIZE - 1 - (2 * SRS_RANGE)) + SRS_RANGE;

/*##DEBUG1 main#######################################################
printf("DEBUG1 main\n");

	ex = 96; ey = 96;

#############################################################DEBUG##*/
	
	universe[ex][ey] = 'E';


/**********************************************************************

Main program

**********************************************************************/

	intro();
	clears(); 
/*	srs(SCREEN_ROWS/4);
	clears(); */
	srs_noprint();
	str();
	
	while ((c[0] != 'E') && (c[1] != 'X')) {
		printf("Commands: srs, lrs, map, imp, wrp, wrq, sq, phr, pht, es, ep, str, save, load, help, ins, EX.\n\n");
		printf("Input command: ");
		scanf("%s", c);

		if (star_date > max_star_date) {
			clears();
			printf("You have run out of time. Current star date is: %4d\n", star_date);
			printf("You should have been finished by: %4d\n", max_star_date);
			printf("Gane Over\n");
			exit(0);
	
		}

		if ( energy <= 0 ) {
			clears();
			printf(" No energy available - Game over\n");
			exit(0);
		}

// Short Range Scan 
/* srs */	if ((c[0] == 's') && (c[1] == 'r') && (c[2] == 's')) { 
			clears();
			srs(SCREEN_ROWS/4); 
		}

// Phaser 
/* phr */	else if ((c[0] == 'p') && (c[1] == 'h') && (c[2] == 'r')) {
			printf("Input [Power][Direction]: "); 
			scanf("%1d%1d", &phaser_power, &phaser_dir);
			fphaser(phaser_power, phaser_dir);
			star_date = ++star_date;
		}


// Photon Torpedoes
/* pht */	else if ((c[0] == 'p') && (c[1] == 'h') && (c[2] == 't')) {
			printf("Input [Power][Direction]: "); 
			scanf("%1d%1d", &pt_power, &pt_dir);
			fpt(pt_power, pt_dir);
			star_date = star_date + 2;
		}

// Impulse power 
/* imp */	else if ((c[0] == 'i') && (c[1] == 'm') && (c[2] == 'p')) {
			printf("Input [Power][Direction]: "); 
			scanf("%1d%1d", &impulse_power, &impulse_dir);
			impulse(impulse_power, impulse_dir);
			star_date = star_date + impulse_power;
		}

// Warp power
/* wrp */	else if ((c[0] == 'w') && (c[1] == 'r') && (c[2] == 'p')) {
			printf("Input [Factor][Direction]: "); 
			scanf("%1d%1d", &warp_power, &warp_dir);
			warp(warp_power, warp_dir);
			star_date = star_date + (warp_power * SRS_RANGE);
		}

// Warp to saved quadrant 
/* wrq */	else if ((c[0] == 'w') && (c[1] == 'r') && (c[2] == 'q')) {
			printf("Input [Quadrant save number]: "); 
			scanf("%1d", &sqn);
			warps(sqn);
			star_date = star_date + SRS_RANGE;
		}

// Energise Shields
/* es */	else if ((c[0] == 'e') && (c[1] == 's')) {
			printf("Input Energy: "); 
			scanf("%d", &e);
			energy = energy - e;
			shield = shield + e;
			if ( shield > 99 ) {
				energy = energy + ( shield - 99 );
				shield = 99;
			}
			clears();
			str();				
		}

// Energise Phasers 
/* ep */	else if ((c[0] == 'e') && (c[1] == 'p')) {
			printf("Input Energy: "); 
			scanf("%d", &p);
			energy = energy - p;
			phaser = phaser + p;
			if ( phaser > 99 ) {
				energy = energy + ( phaser - 99 );
				phaser = 99;
			}
			clears();
			str();				
		}

// Long Range Scan 
/* lrs */		else if ((c[0] == 'l') && (c[1] == 'r') && (c[2] == 's')) { 
			clears();
			lrs(1);
		}

// Clear Screen 
/* clr */
		else if ((c[0] == 'c') && (c[1] == 'l') && (c[2] == 'r'))  
			clears();

// Enterprise Cordinates (hidden command)
/* xy! */	else if ((c[0] == 'x') && (c[1] == 'y') && (c[2] == '!')) {
			clears();
			printf("ex,ey: %d,%d\n\n", ex, ey);
		}
// Docked Enterprise cordinates (hidden command)
/*dxy! */		else if ((c[0] == 'd') && (c[1] == 'x') && (c[2] == 'y') && (c[3] == '!')) {
			clears();
			printf("dex,dey: %d,%d\n\n", dex, dey);
		}		
// Exit 
/* EX */	else if ((c[0] == 'E') && (c[1] == 'X'))
 			printf("\nGoing so soon !!\n");

// Status Report 
/* str */	else if ((c[0] == 's') && (c[1] == 't') && (c[2] == 'r')) {
			clears();
			str();
		}

// Save Quadrant 
/* sq */	else if ((c[0] == 's') && (c[1] == 'q')) {
			printf("Input Save number: "); 
			scanf("%d", &sn);
			clears();
			srs_noprint();
			lrs(0);
			squad(sn);
		}

// Universe Scan ( hidden command output to "US.TXT" )
/* us! */	else if ((c[0] == 'u') && (c[1] == 's') && (c[2] == '!')) {
			clears();
			us();
			printf("Number of Kliongs left: %2d\n", K_in_universe);
				
		}

// Debug map (hidden command)
/* map! */	else if ((c[0] == 'm') && (c[1] == 'a') && (c[2] == 'p') && (c[3] == '!')) {
			clears();
			map_debug();
		}
// Map 
/* map */	else if ((c[0] == 'm') && (c[1] == 'a') && (c[2] == 'p')) {
			clears();
			map();
		}

// Save Game 
/* save */	else if ((c[0] == 's') && (c[1] == 'a') && (c[2] == 'v') && (c[3] == 'e')) {
			clears();
			save_game();
		}
// Load Game
/* load */	else if ((c[0] == 'l') && (c[1] == 'o') && (c[2] == 'a') && (c[3] == 'd')) {
			clears();
			load_game();
		}
// Instructions
/* ins */	else if ((c[0] == 'i') && (c[1] == 'n') && (c[2] == 's')) {
			clears();
			ins();
			clears();
		}
// Default 
		else {
			clears();
			printf("srs    	- Short Range Scan.\n");
			printf("lrs    	- Long Range Scan.\n");
			printf("map    	- Universe map.\n");
			printf("imp 	- Impulse Power.\n");
			printf("wrp	- Warp Power ( 2 for 1 sector move, 0 = Factor 10 ).\n");
			printf("wrq	- Warp to saved quadrant.\n");
			printf("sq	- Save Quadrant.\n");
			printf("phr	- Phasers.\n");
			printf("pht	- Photon Torpedoes.\n");
			printf("es	- Energise Shields ( Max 100 ).\n");
			printf("ep	- Energise Phasers ( Max 100 ).\n");
			printf("str    	- Status Report.\n");
			printf("save	- Save game (note: only one game can be saved).\n");
			printf("load	- Load game.\n");
			printf("help   	- Help.\n");
			printf("ins	- Print full instructions.\n");
			printf("EX     	- End Game.\n\n");
			printf("A 0 used in any command parameter (except Warp power 0 which\n");
			printf("represents Warp Factor 10) results in the command being aborted\n");
			lines(1);
		}
	}
	return 0;
}

void fphaser(int power, int dir)
{

	int x, y;

	if (phaser_flag <= 0) {
		clears();
		printf("Phasers unavailable.\n");
		return;
	}

	if ( power == 0 || dir == 0 ) {
		clears();
		printf("\nInvalid power or direction\n\n");
		return;
	}
	else if ( power > (2 * SRS_RANGE) )
		power = 2 * SRS_RANGE;

	if (phaser <= power) {
		phaser = 0;
		printf("Not enough phasers energy, please re-energise.\n");
		shield = shield - K_in_sector - K_in_sector;
		clears();
		printf("\nKlingons attacks.\n\n");
		damage(2);
	}
	else {
		phaser = phaser - (3 * power);

		switch (dir) {
		case 1:	x = ex - power;
 			y = ey;
			break;
		case 2:	x = ex - power;
			y = ey + power;
			break;
		case 3:	x = ex;
			y = ey + power;
			break;
		case 4:	x = ex + power;
			y = ey + power;
			break;
		case 5: x = ex + power;
			y = ey;
			break;
		case 6: x = ex + power;
			y = ey - power;
			break;
		case 7: x = ex;
			y = ey - power;
			break;
		case 8:	x = ex - power;
			y = ey - power;
			break;
		}
	
		if (universe[x][y] == 'K') {
			universe[x][y] = '-';
			--K_in_universe;
			--K_in_sector;
			shield = shield - K_in_sector;
			clears();
			srs(SCREEN_ROWS/4); 
			printf("\nKlingon destroyed\n\n");
			damage(1);
		}
		else if (universe[x][y] == 'S') {
			universe[x][y] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}		
		else {
			clears();
			printf("\nYou missed!!\n\n");
			shield = shield -  K_in_sector - K_in_sector;
			clears();
			printf("\nKlingons attack.\n\n");
			damage(2);
		}
	}
}			

void fpt(int power, int dir)
{

	int x, y;
	int K_killed = 0;

	if (pt_flag <= 0) {
		clears();
		printf("Photon Torpedoes unavailable.\n");
		return;
	}

	if ( power == 0 || dir == 0 )
		return;
	else if ( power > (2 * SRS_RANGE) )
		power = 2 * SRS_RANGE;

	if (energy <= power) {
		shield = shield - K_in_sector - K_in_sector;
		clears();
		printf("\nNot enough energy available\n");
		printf("Klingons attacks\n\n");
	}
		
	if (pt <= 0) {
		pt = 0;
		printf("No Photon Torpedoes left\n");
		shield = shield - K_in_sector - -K_in_sector;
		clears();
		printf("\nKlingons attacks\n\n");
		damage(2);
	}
	else {
		energy = energy - power;
		--pt;

		switch (dir) {
		case 1:	x = ex - power;
 			y = ey;
			break;
		case 2:	x = ex - power;
			y = ey + power;
			break;
		case 3:	x = ex;
			y = ey + power;
			break;
		case 4:	x = ex + power;
			y = ey + power;
			break;
		case 5: x = ex + power;
			y = ey;
			break;
		case 6: x = ex + power;
			y = ey - power;
			break;
		case 7: x = ex;
			y = ey - power;
			break;
		case 8:	x = ex - power;
			y = ey - power;
			break;
		}
	
		
		if (universe[x][y] == 'K') {
			universe[x][y] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;
			damage(1);
		}
		else if (universe[x][y] == 'S') {
			universe[x][y] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		if (universe[x+1][y] == 'K') {
			universe[x+1][y] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;
			damage(1);
		}
		else if (universe[x+1][y] == 'S') {
			universe[x+1][y] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		if (universe[x-1][y] == 'K') {
			universe[x-1][y] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;
			damage(1);
		}
		else if (universe[x-1][y] == 'S') {
			universe[x-1][y] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		if (universe[x][y+1] == 'K') {
			universe[x][y+1] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;
			damage(1);
		}
		else if (universe[x][y+1] == 'S') {
			universe[x][y+1] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		if (universe[x][y-1] == 'K') {
			universe[x][y-1] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;
			damage(1);
		}
		else if (universe[x][y-1] == 'S') {
			universe[x][y-1] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		if (universe[x+1][y+1] == 'K') {
			universe[x+1][y+1] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;			
			damage(1);
		}
		else if (universe[x+1][y+1] == 'S') {
			universe[x+1][y+1] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		if (universe[x-1][y-1] == 'K') {
			universe[x-1][y-1] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;
			damage(1);
		}
		else if (universe[x-1][y-1] == 'S') {
			universe[x-1][y-1] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		if (universe[x+1][y-1] == 'K') {
			universe[x+1][y-1] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;			
			damage(1);
		}
		else if (universe[x+1][y-1] == 'S') {
			universe[x+1][y-1] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}


		if (universe[x-1][y+1] == 'K') {
			universe[x-1][y+1] = '-';
			--K_in_universe;
			--K_in_sector;
			++K_killed;
			shield = shield - 1;			
			damage(1);
		}
		else if (universe[x-1][y+1] == 'S') {
			universe[x-1][y+1] = 'X';
			clears();
			printf("You have attacked and severely damages a Space Station.\n");
			printf("The Space Station is badly damaged but manages to return fire.\n");
			printf("You better run away before the Space Police catch you.\n");
			damage(5);
		}

		shield = shield - K_in_sector;

		if (K_killed == 0 ) {
			clears();
			printf("\nYou missed!!\n\n");
			shield = shield - K_in_sector - K_in_sector;
			clears();
			printf("\nKlingons attacks\n\n");
			damage(2);
		}
		else {
			clears();
			srs(SCREEN_ROWS/4); 
			printf("\n%2d Klingons destroyed\n\n", K_killed);
		}		
	}
}

void impulse(int power, int dir)
{

	int x, y;

	map_flag = 0;

	if ( power == 0 || dir == 0 ) {
		clears();
		printf("\nInvalid power or direction\n\n");
		return;
	}

	if (impulse_flag <= 0) {
		clears();
		printf("Impulse power unavailable.\n");
		return;
	}

	if ( power > SRS_RANGE )
		power = SRS_RANGE;


	srs_flag = ( srs_flag <= 0 ) ? srs_flag + 1 : 1;
	lrs_flag = ( srs_flag <= 0 ) ? lrs_flag + 1 : 1;
	es_flag = ( es_flag <= 0 ) ? es_flag + 1 : 1;
	phaser_flag = ( phaser_flag <= 0 ) ? phaser_flag + 1 : 1;
	ep_flag = ( ep_flag <= 0 ) ? ep_flag + 1 : 1;
	pt_flag = ( pt_flag <= 0 ) ? pt_flag + 1 : 1;
	warp_flag = ( warp_flag <= 0 ) ? warp_flag + 1 : 1;
	squad_flag = ( squad_flag <= 0 ) ? squad_flag + 1 : 1;


	if ( K_in_sector > 0 ) {
		clears();
		printf("\nKlingons Attack\n\n");
		shield = shield - K_in_sector + 1;
		damage(1);
	}

	energy = energy - (3 * power);
	switch (dir) {
	case 1:	x = ex - power;
 		y = ey;
		break;
	case 2:	x = ex - power;
		y = ey + power;
		break;
	case 3:	x = ex;
		y = ey + power;
		break;
	case 4:	x = ex + power;
		y = ey + power;
		break;
	case 5: x = ex + power;
		y = ey;
		break;
	case 6: x = ex + power;
		y = ey - power;
		break;
	case 7: x = ex;
		y = ey - power;
		break;
	case 8:	x = ex - power;
		y = ey - power;
		break;
	}
	
	moveea(x,y);
}			

void warp(int power, int dir)
{

	int x, y;

	map_flag = 0;

	if ( power != 0 && dir == 0 ) {
		clears();
		printf("\nInvalid direction\n\n");
		return;
	}

	if (warp_flag <= 0) {
		clears();
		printf("Warp power unavailable.\n");
		return;
	}

	warp_active = 1;

	srs_flag = ( srs_flag <= 0 ) ? srs_flag + 1 : 1;
	lrs_flag = ( srs_flag <= 0 ) ? lrs_flag + 1 : 1;
	es_flag = ( es_flag <= 0 ) ? es_flag + 1 : 1;
	phaser_flag = ( phaser_flag <= 0 ) ? phaser_flag + 1 : 1;
	ep_flag = ( ep_flag <= 0 ) ? ep_flag + 1 : 1;
	pt_flag = ( pt_flag <= 0 ) ? pt_flag + 1 : 1;
	impulse_flag = ( impulse_flag <= 0 ) ? impulse_flag + 1 : 1;

	if ( (power > 0) && (power <= 9) ) {
		energy = energy - (power * SRS_RANGE) + 1;
		switch (dir) {
		case 1:	x = ex - ((power * SRS_RANGE) + 1); 			
 			y = ey;
			break;
		case 2:	x = ex - ((power * SRS_RANGE) + 1);
			y = ey + ((power * SRS_RANGE) + 1);
			break;
		case 3:	x = ex;
			y = ey + ((power * SRS_RANGE) + 1);
			break;
		case 4:	x = ex + ((power * SRS_RANGE) + 1);
			y = ey + ((power * SRS_RANGE) + 1);
			break;
		case 5: x = ex + ((power * SRS_RANGE) + 1);
			y = ey;
			break;
		case 6: x = ex + ((power * SRS_RANGE) + 1);
			y = ey - ((power * SRS_RANGE) + 1);
			break;
		case 7: x = ex;
			y = ey - ((power * SRS_RANGE) + 1);
			break;
		case 8:	x = ex - ((power * SRS_RANGE) + 1);
			y = ey - ((power * SRS_RANGE) + 1);
			break;
		}
	
		clears();
		moveea(x,y);
		srs_noprint();


	}
	else if ( power == 0 ) {
		energy = energy - (11 * SRS_RANGE);
		x = my_random(UNIVERSE_SIZE);
		y = my_random(UNIVERSE_SIZE);
		clears();
		printf("\nWarp Factor 10 - Hyperspace\n\n");
		moveea(x,y);
	}
}			

void warps(int save_no) 	// Warp to a saved sector
{ 
		
	if (save_no < 0 || save_no > 3) {
		clears();
		printf("\nsave value must be between 0 and 3\n\n");
	}
	else {
		moveea(sex[save_no],sey[save_no]);
		energy = energy - (15 * SRS_RANGE);
		clears();
		printf("\nWarp to saved sector: %d\n\n", save_no);
	}
}		



void clears(void)	// Clear screen 
{

	int i;

	for (i = 0; i <= SCREEN_ROWS; ++i)
		printf("\n");
}

void us(void)		// Universe Scan 
{
	FILE *US_File;
	int i = 0;

	US_File = fopen("US.TXT", "wt");
	for (ux = 0; (ux <= UNIVERSE_SIZE - 1); ++ux) {
		fprintf(US_File, "%3d ", i);
		++i;
		for (uy = 0; (uy <= UNIVERSE_SIZE - 1); ++uy)
			fprintf(US_File, "%c ", universe[ux][uy]);
		fputc('\n', US_File);	
	}
	fflush(US_File);
	fclose(US_File);
}

void map(void)		// Map 
{

	int i = 0;
	int first_time_in = 1;

	if (map_flag == 0) {
		clears();
		printf("Map only unavailable when docked.\n");
		return;
	}

	energy = energy - 4;
	for (ux = 0; (ux <= UNIVERSE_SIZE - 1); ++ux) {
		if ( i < SCREEN_ROWS - 2 )
			printf("%6d ", ux);
		++i; 
		for (uy = 0; (uy <= UNIVERSE_SIZE - 1); ++uy) {
			if (i == (SCREEN_ROWS - 1) && uy == 0) {
				i = 0;
				if (first_time_in == 1) {
					first_time_in = 0;
					printf("<RETURN> to continue ");
					getchar();
					getchar();
					printf("%6d ", ux);
					
				}
				else {
					printf("<RETURN> to continue ");
					getchar();
					printf("%6d ", ux);
				}
			}
			// Print full map
			//putchar(universe[ux][uy]);

			// Print map showing SS E, SS & WH
			if (universe[ux][uy] == '-' || universe[ux][uy] == 'K')
			//  universe[ux][uy] == 'S' || universe[ux][uy] == 'W')
				putchar('-');
			else
				putchar(universe[ux][uy]);
		}
		printf(" %d", ux);
		putchar('\n');	
	}
}

void map_debug(void)		// Debug Map
{

	int i = 0;
	int first_time_in = 1;

	energy = energy - 4;
	for (ux = 0; (ux <= UNIVERSE_SIZE - 1); ++ux) {
		if ( i < SCREEN_ROWS - 2 )
			printf("%6d ", ux);
		++i; 
		for (uy = 0; (uy <= UNIVERSE_SIZE - 1); ++uy) {
			if (i == (SCREEN_ROWS - 1) && uy == 0) {
				i = 0;
				if (first_time_in == 1) {
					first_time_in = 0;
					printf("<RETURN> to continue ");
					getchar();
					getchar();
					printf("%6d ", ux);
					
				}
				else {
					printf("<RETURN> to continue ");
					getchar();
					printf("%6d ", ux);
				}
			}
			// Print full map
			putchar(universe[ux][uy]);
		}
		printf(" %d", ux);
		putchar('\n');	
	}
}

void lrs(int display_flag)	// Long Range Scan 
{

/**********************************************************************

			SECTOR8 SECTOR1 SECTOR2		
			SECTOR7         SECTOR3
			SECTOR6 SECTOR5 SECTOR4	


**********************************************************************/

	int lsx, lsy;					// Long Range Scan cords 

	int lsx_s1_min, lsx_s1_max;		// SECTOR8 min/max cords 
	int lsy_s1_min, lsy_s1_max;
	int lsx_s2_min, lsx_s2_max;		// SECTOR1 min/max cords 
	int lsy_s2_min, lsy_s2_max;
	int lsx_s3_min, lsx_s3_max;		// SECTOR2 min/max cords 
	int lsy_s3_min, lsy_s3_max;
	int lsx_s4_min, lsx_s4_max;		// SECTOR7 min/max cords 
	int lsy_s4_min, lsy_s4_max;
	int lsx_s5_min, lsx_s5_max;		// SECTOR3 min/max cords 
	int lsy_s5_min, lsy_s5_max;
	int lsx_s6_min, lsx_s6_max;		// SECTOR6 min/max cords 
	int lsy_s6_min, lsy_s6_max;
	int lsx_s7_min, lsx_s7_max;		// SECTOR5 min/max cords 
	int lsy_s7_min, lsy_s7_max;
	int lsx_s8_min, lsx_s8_max;		// SECTOR4 min/max cords 
	int lsy_s8_min, lsy_s8_max;

	if (lrs_flag <= 0) {
		clears();
		printf("Long Range Scan unavailable.\n");
		return;
	}

	energy = energy - 3;

	lsx_s1_min = (ex - (3 * SRS_RANGE + 1)) < 0 ? 0 : ex - (3 * SRS_RANGE + 1);
	lsy_s1_min = (ey - (3 * SRS_RANGE + 1)) < 0 ? 0 : ey - (3 * SRS_RANGE + 1);
	lsx_s1_max = (ex - (SRS_RANGE + 1)) < 0 ? 0 : ex - (SRS_RANGE + 1);
	lsy_s1_max = (ey - (SRS_RANGE + 1)) < 0 ? 0 : ey - (SRS_RANGE + 1);

	lsx_s2_min = lsx_s1_min;
	lsy_s2_min = (ey - SRS_RANGE < 0) ? 0 : ey - SRS_RANGE;
	lsx_s2_max = lsx_s1_max;
	lsy_s2_max = (ey + SRS_RANGE > (UNIVERSE_SIZE - 1)) ? UNIVERSE_SIZE - 1 
							    : ey + SRS_RANGE;
	lsx_s3_min = lsx_s1_min;
	lsy_s3_min = (ey + (SRS_RANGE + 1)) > (UNIVERSE_SIZE - 1) ? 
							UNIVERSE_SIZE - 1 
						      : ey + (SRS_RANGE + 1);
	lsx_s3_max = lsx_s1_max;
	lsy_s3_max = (ey + (3 * SRS_RANGE + 1)) > (UNIVERSE_SIZE - 1) ?
							UNIVERSE_SIZE - 1
						      : ey + (3 * SRS_RANGE + 1);

	lsx_s4_min = (ex - SRS_RANGE < 0) ? 0 : ex - SRS_RANGE; 
	lsy_s4_min = lsy_s1_min;
	lsx_s4_max = (ex + SRS_RANGE) > (UNIVERSE_SIZE - 1) ?
							UNIVERSE_SIZE - 1
						      :	ex + SRS_RANGE;
	lsy_s4_max = lsy_s1_max;

	lsx_s5_min = lsx_s4_min;
	lsy_s5_min = lsy_s3_min;
	lsx_s5_max = lsx_s4_max;
	lsy_s5_max = lsy_s3_max;
 
	lsx_s6_min = ((ex + SRS_RANGE) + 1 < 0) ? 0 : ex + SRS_RANGE + 1;
	lsy_s6_min = lsy_s1_min;
	lsx_s6_max = ((ex + (3 * SRS_RANGE) + 1) > (UNIVERSE_SIZE - 1)) ? UNIVERSE_SIZE - 1
							    : ex + (3 * SRS_RANGE) + 1; 
	lsy_s6_max = lsy_s1_max;

	lsx_s7_min = lsx_s6_min;
	lsy_s7_min = lsy_s2_min;
	lsx_s7_max = lsx_s6_max;
	lsy_s7_max = lsy_s2_max;

	lsx_s8_min = lsx_s6_min; 
	lsy_s8_min = lsy_s3_min;
	lsx_s8_max = lsx_s6_max;
	lsy_s8_max = lsy_s3_max;

/**********************************************************************

Scan for SECTOR8

**********************************************************************/

	K_count[0] = 0;
	S_count[0] = 0;
	W_count[0] = 0;

	for (lsx = lsx_s1_min; lsx <= lsx_s1_max; ++lsx) {
		for (lsy = lsy_s1_min; lsy <= lsy_s1_max; ++lsy) {
/*#DEBUG1 lrs########################################################
printf("DEBUG1 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

###########################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[0];
			if (universe[lsx][lsy] == 'S')
				++S_count[0];
			if (universe[lsx][lsy] == 'W')
				++W_count[0];
		}
	}

/*##DEBUG2 lrs#####################################################
printf("DEBUG2 lrs\n");
	printf("Total Klingons in SECTOR8 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s1_min: %d  lsx_s1_max: %d\n", lsx_s1_min, lsx_s1_max);
	printf("lsy_s1_min: %d  lsy_s1_max: %d\n", lsy_s1_min, lsy_s1_max);

##########################################################DEBUG##*/	

/**********************************************************************

Scan for SECTOR1

**********************************************************************/

	K_count[1] = 0;
	S_count[1] = 0;
	W_count[1] = 0;

	for (lsx = lsx_s2_min; lsx <= lsx_s2_max; ++lsx) {
		for (lsy = lsy_s2_min; lsy <= lsy_s2_max; ++lsy) {
/*#DEBUG3 lrs########################################################
printf("DEBUG3 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

############################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[1];
			if (universe[lsx][lsy] == 'S')
				++S_count[1];
			if (universe[lsx][lsy] == 'W')
				++W_count[1];
		}
	}

/*##DEBUG4 lrs#########################################################
printf("DEBUG4 lrs\n");
	printf("Total Klingons in SECTOR1 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s2_min: %d  lsx_s2_max: %d\n", lsx_s2_min, lsx_s2_max);
	printf("lsy_s2_min: %d  lsy_s2_max: %d\n", lsy_s2_min, lsy_s2_max);

#############################################################DEBUG##*/

/**********************************************************************

Scan for SECTOR2

**********************************************************************/

	K_count[2] = 0;
	S_count[2] = 0;
	W_count[2] = 0;

	for (lsx = lsx_s3_min; lsx <= lsx_s3_max; ++lsx) {
		for (lsy = lsy_s3_min; lsy <= lsy_s3_max; ++lsy) {
/*#DEBUG5 lrs########################################################
printf("DEBUG5 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

############################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[2];
			if (universe[lsx][lsy] == 'S')
				++S_count[2];
			if (universe[lsx][lsy] == 'W')
				++W_count[2];
		}
	}


/*##DEBUG6 lrs#########################################################
printf("DEBUG6 lrs\n");
	printf("Total Klingons in SECTOR2 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s3_min: %d  lsx_s3_max: %d\n", lsx_s3_min, lsx_s3_max);
	printf("lsy_s3_min: %d  lsy_s3_max: %d\n", lsy_s3_min, lsy_s3_max);

#############################################################DEBUG##*/

/**********************************************************************

Scan for SECTOR7

**********************************************************************/

	K_count[3] = 0;
	S_count[3] = 0;
	W_count[3] = 0;

	for (lsx = lsx_s4_min; lsx <= lsx_s4_max; ++lsx) {
		for (lsy = lsy_s4_min; lsy <= lsy_s4_max; ++lsy) {
/*#DEBUG7 lrs########################################################
printf("DEBUG7 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

############################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[3];
			if (universe[lsx][lsy] == 'S')
				++S_count[3];
			if (universe[lsx][lsy] == 'W')
				++W_count[3];
		}
	}

/*##DEBUG8 lrs#########################################################
printf("DEBUG8 lrs\n");
	printf("Total Klingons in SECTOR7 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s4_min: %d  lsx_s4_max: %d\n", lsx_s4_min, lsx_s4_max);
	printf("lsy_s4_min: %d  lsy_s4_max: %d\n", lsy_s4_min, lsy_s4_max);

#############################################################DEBUG##*/

/**********************************************************************

Scan for SECTOR3

**********************************************************************/

	K_count[4] = 0;
	S_count[4] = 0;
	W_count[4] = 0;

	for (lsx = lsx_s5_min; lsx <= lsx_s5_max; ++lsx) {
		for (lsy = lsy_s5_min; lsy <= lsy_s5_max; ++lsy) {
/*#DEBUG9 lrs########################################################
printf("DEBUG9 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

############################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[4];
			if (universe[lsx][lsy] == 'S')
				++S_count[4];
			if (universe[lsx][lsy] == 'W')
				++W_count[4];
		}
	}

/*##DEBUG10 lrs########################################################
printf("DEBUG10 lrs\n");
	printf("Total Klingons in SECTOR3 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s5_min: %d  lsx_s5_max: %d\n", lsx_s5_min, lsx_s5_max);
	printf("lsy_s5_min: %d  lsy_s5_max: %d\n", lsy_s5_min, lsy_s5_max);

#############################################################DEBUG##*/

/**********************************************************************

Scan for SECTOR6

**********************************************************************/

	K_count[5] = 0;
	S_count[5] = 0;
	W_count[5] = 0;

	for (lsx = lsx_s6_min; lsx <= lsx_s6_max; ++lsx) {
		for (lsy = lsy_s6_min; lsy <= lsy_s6_max; ++lsy) {
/*#DEBUG11 lrs#######################################################
printf("DEBUG11 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

############################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[5];
			if (universe[lsx][lsy] == 'S')
				++S_count[5];
			if (universe[lsx][lsy] == 'W')
				++W_count[5];
		}
	}

/*##DEBUG12 lrs########################################################
printf("DEBUG12 lrs\n");
	printf("Total Klingons in SECTOR6 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s6_min: %d  lsx_s6_max: %d\n", lsx_s6_min, lsx_s6_max);
	printf("lsy_s6_min: %d  lsy_s6_max: %d\n", lsy_s6_min, lsy_s6_max);

#############################################################DEBUG##*/

/**********************************************************************

Scan for SECTOR5

**********************************************************************/

	K_count[6] = 0;
	S_count[6] = 0;
	W_count[6] = 0;

	for (lsx = lsx_s7_min; lsx <= lsx_s7_max; ++lsx) {
		for (lsy = lsy_s7_min; lsy <= lsy_s7_max; ++lsy) {
/*#DEBUG13 lrs#######################################################
printf("DEBUG13 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

############################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[6];
			if (universe[lsx][lsy] == 'S')
				++S_count[6];
			if (universe[lsx][lsy] == 'W')
				++W_count[6];
		}
	}

/*##DEBUG14 lrs########################################################
printf("DEBUG14 lrs\n");
	printf("Total Klingons in SECTOR5 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s7_min: %d  lsx_s7_max: %d\n", lsx_s7_min, lsx_s7_max);
	printf("lsy_s7_min: %d  lsy_s7_max: %d\n", lsy_s7_min, lsy_s7_max);

#############################################################DEBUG##*/

/**********************************************************************

Scan for SECTOR4

**********************************************************************/

	K_count[7] = 0;
	S_count[7] = 0;
	W_count[7] = 0;

	for (lsx = lsx_s8_min; lsx <= lsx_s8_max; ++lsx) {
		for (lsy = lsy_s8_min; lsy <= lsy_s8_max; ++lsy) {
/*#DEBUG15 lrs#######################################################
printf("DEBUG13 lrs\n");
		printf("%c\t%d %d\n", universe[lsx][lsy], lsx, lsy);

############################################################DEBUG##*/ 
			if (universe[lsx][lsy] == 'K')
				++K_count[7];
			if (universe[lsx][lsy] == 'S')
				++S_count[7];
			if (universe[lsx][lsy] == 'W')
				++W_count[7];
		}
	}

/*##DEBUG16 lrs########################################################
printf("DEBUG14 lrs\n");
	printf("Total Klingons in SECTOR4 is: %d \n", K_count);

	printf("ex: %d ey: %d\n", ex, ey);
	
	printf("lsx_s8_min: %d  lsx_s8_max: %d\n", lsx_s8_min, lsx_s8_max);
	printf("lsy_s8_min: %d  lsy_s8_max: %d\n", lsy_s8_min, lsy_s8_max);

#############################################################DEBUG##*/

	if (display_flag != 0 ) {

		printf("\t      ___________________________________\n"); 
		printf("\t     |  SECTOR8  |  SECTOR1  |  SECTOR2  |\n");
		printf("\t     |           |           |           |\n");
		printf("\t     |  K =%3d   |  K =%3d   |  K =%3d   |\n", K_count[0], K_count[1], K_count[2]);  
		printf("\t     |  S =%3d   |  S =%3d   |  S =%3d   |\n", S_count[0], S_count[1], S_count[2]);
		printf("\t     |  W =%3d   |  W =%3d   |  W =%3d   |\n", W_count[0], W_count[1], W_count[2]);
		printf("\t     |___________|___________|___________|\n");
		printf("\t     |  SECTOR7  |           |  SECTOR3  |\n");
		printf("\t     |           |           |           |\n");
		printf("\t     |  K =%3d   |   SS E    |  K =%3d   |\n", K_count[3], K_count[4]);
		printf("\t     |  S =%3d   |           |  S =%3d   |\n", S_count[3], S_count[4]);                      
		printf("\t     |  W =%3d   |           |  W =%3d   |\n", W_count[3], W_count[4]);
		printf("\t     |___________|___________|___________|\n");
		printf("\t     |  SECTOR6  |  SECTOR5  |  SECTOR4  |\n");
		printf("\t     |           |           |           |\n");
		printf("\t     |  K =%3d   |  K =%3d   |  K =%3d   |\n", K_count[5], K_count[6], K_count[7]);  
		printf("\t     |  S =%3d   |  S =%3d   |  S =%3d   |\n", S_count[5], S_count[6], S_count[7]);
		printf("\t     |  W =%3d   |  W =%3d   |  W =%3d   |\n", W_count[5], W_count[6], W_count[7]);
		printf("\t     |___________|___________|___________|\n");
		lines(1);

		// Test for scan beyond the bounds of the universe

		if (lsx_s1_min == 0 || lsy_s1_min == 0 || lsx_s1_max == 0 || lsy_s1_max == 0 || lsy_s2_min == 0 || lsx_s4_min == 0 || lsx_s6_min == 0 ||
			lsy_s2_max == UNIVERSE_SIZE - 1 || lsy_s3_min == UNIVERSE_SIZE - 1 || lsy_s3_max == UNIVERSE_SIZE - 1 || lsx_s4_max == UNIVERSE_SIZE - 1 ||
			lsx_s6_max == UNIVERSE_SIZE - 1 ){
				printf("\t     Long range scan beyond universe bounds, data unreliable ");
				lines(1);
		}
	}
}

void srs_noprint(void)		// Short range scan to get sector stats simular to srs but no display
{
	int ssx, ssy;			// Short Range Scan cords 

	K_in_sector = 0;
	S_in_sector = 0;
	W_in_sector = 0;

	for (ssx = (ex - SRS_RANGE); ssx <= (ex + SRS_RANGE); ++ssx) {
		for ( ssy = (ey - SRS_RANGE); ssy <= (ey + SRS_RANGE); ++ssy ) {
			if ( ssx > 0 && ssx < (UNIVERSE_SIZE - 1) && ssy > 0 && ssy < (UNIVERSE_SIZE - 1)) {
				if (universe[ssx][ssy] == 'K')
					++K_in_sector;
				else if (universe[ssx][ssy] == 'S')
					++S_in_sector;
				else if (universe[ssx][ssy] == 'W')
					++W_in_sector;
			}
		}
	}
}

void srs(int no_lines)		// Short Range Scan 
{

	int ssx, ssy;	// Short Range Scan cords 
	int i;

	if (srs_flag <= 0) {
		clears();
		printf("Short Range Scan unavailable.\n");
		return;
	}

	energy = --energy;
	K_in_sector = 0;
	S_in_sector = 0;
	W_in_sector = 0;
	if (srs_flag <= 0)
		printf("Short Range Scan not available\n");
	else {
		printf("            ");
		printf("8");
		for (i = 0; (i <= SRS_RANGE + 3); ++i) 
			printf(" ");
		printf("1");
		for (i = 0; (i <= SRS_RANGE + 3); ++i)
			printf(" ");
		printf("2\n");		
		for (ssx = (ex - SRS_RANGE); ssx <= (ex + SRS_RANGE); ++ssx) {
			if ( ssx == ex )
				printf("            7 ");
			else
				printf("              "); 
			for ( ssy = (ey - SRS_RANGE); ssy <= (ey + SRS_RANGE); ++ssy ) {
				if ( ssx < 0 || ssx > (UNIVERSE_SIZE - 1) )
					printf("  ");
				else if ( ssy < 0 || ssy > (UNIVERSE_SIZE - 1) )
					printf("  ");
				else {
					if ( universe[ssx][ssy] == 'O' )
						printf("%c ", '-');
					else
						printf("%c ", universe[ssx][ssy]);

					if (universe[ssx][ssy] == 'K')
						++K_in_sector;
					else if (universe[ssx][ssy] == 'S')
						++S_in_sector;
					else if (universe[ssx][ssy] == 'W')
						++W_in_sector;
				}
				if ( ssy == (ey + SRS_RANGE) && ssx == ex )
				printf("3");
			}
			printf("\n");
		}
		printf("            ");
		printf("6");
		for (i = 0; (i <= SRS_RANGE + 3); ++i) 
			printf(" ");
		printf("5");
		for (i = 0; (i <= SRS_RANGE + 3); ++i)
			printf(" ");
		printf("4\n");		
		for (i = 0; (i <= no_lines); ++i)
			printf("\n");
	}
}


/*********************************************************************************/

/*********************** Currently not used **************************************/

/*

void moveer(int iex, int iey)	/* Move Enterprise relative */

				/* Note needs DAMAGE added  */
/*
{

	if ( srs_flag < 0 )
		++srs_flag;
	if ( lrs_flag < 0 )
		++lrs_flag;
	universe[ex][ey] = '-';
	ex = ex + iex;
	ey = ey + iey;
	if ( ex < 0 || ex > ( UNIVERSE_SIZE - 1 )) {
		ex = ex - iex;
		ey = ey - iey;
		printf("You cannot escape the bounds of the universe.\n");
		damage(3);
	}
	else if ( ey < 0 || ey > ( UNIVERSE_SIZE - 1 )) {
		ex = ex - iex;
		ey = ey - iey;		
		printf("You cannot escape the bounds of the universe.\n");
		damage(3);
	}
	else if ( universe[ex][ey] == 'K') {
		printf("You collieded with a Klingon.\n");
		printf("Klingon destroyed.");
		--K_in_universe;
		--K_in_sector;
		damage(4)
	}
	else if ( universe[ex][ey] == 'S' ) {
		printf("You collieded with a Space Station.");
		damage(4);
		--SS_in_universe;
	} 
	universe[ex][ey] = 'E';
}

*/

void moveea(int aex, int aey)	// Move Enterprise absolute 
{

	int sex, sey;	// Start position of Enterprise 
	int w_unstable; // Unstable Worm Hole 

	sex = ex;	// Need to be able to move back if collision with K etc 
	sey = ey; 


	if (dex >= 1 && dey >= 1 ) {
		universe[dex][dey] = 'S';
		dex = 0;
		dey = 0;
	}
	else if (worm_active == 1) {
		universe[ex][ey] = 'W';
		worm_active = 0;
	}
	else
		universe[ex][ey] = 'O'; // Replace current Enterprise position with 'O' 

	ex = aex;
	ey = aey;

	if ( ex < 0 || ey < 0 || ex > ( UNIVERSE_SIZE - 1 ) || ey > ( UNIVERSE_SIZE - 1 )) {

		ex = sex; // Return Enterprise to start position 
		ey = sey;
		
		printf("You cannot escape the bounds of the universe.\n");
		damage(3);

		if ( shield >= 20 ) {
			shield = shield - 20;
			energy = energy - 20;
		}
		else {
			shield = 0;
			energy = energy - 60;
			lrs_flag = -3;
			srs_flag = -1;
		}
		
		if ( energy < 0 ) {
			clears();
			printf("You cannot escape the bounds of the universe.\n");
			printf("Enterprise Destroyed.\nGame over\n\n");
			exit(0);
		}
	}

	
	else if ( universe[ex][ey] == 'K') {
		printf("You collieded with a Klingon and suffered damage.\n");
		printf("Klingon destroyed.\n");
		--K_in_universe;
		--K_in_sector;
		if ( shield >= 10 ) {
			shield = shield - 10;
			energy = energy - 10;
			damage(4);
		}
		else {
			shield = 0;
			energy = energy - 30;
			damage(4);
		}

		if ( energy < 0 ) {
			clears();
			printf("Enterprise Destroyed.\nGame over\n\n");
			exit(0);
		}
	}

	else if ( universe[ex][ey] == 'W' ) {
		worm_active = 1;
		clears();
		printf("\nYou have entered a Worm Hole! Time runs backwards\n");
		star_date = star_date - (my_random(50) + 50);
		w_unstable = my_random(10);
		if (w_unstable <= 5) {
			star_date = (star_date - (5 * w_unstable)) <= 1000 ? 1000 : star_date - (5 * w_unstable);
			printf("Worm Hole is unstable; time is badly distorted in your favour\n");
			shield = (shield > 30) ? 5 : shield - 30;
			damage(my_random(5));
		}
		printf("\n");
		moveea(my_random(UNIVERSE_SIZE - 1), my_random(UNIVERSE_SIZE - 1));
	}
	
	else if ( universe[ex][ey] == 'S' ) {

		if (warp_active == 1) {
			--SS_in_universe;
			universe[ex][ey] = 'E'; 
			printf("\nYou have collided with and destroyed a Space Station\n\n");
			damage(4);
		}
		else {

			if ( star_date > can_dock_date) {
				can_dock_date = star_date + MAX_NO_DOCK;
				clears();
				printf("\nYou have docked sucesssfully.\n");
				printf("Your energy and photon torpedoes have been replenished\n");
				printf("The Federation has granted you permission to view the universe map\n");
				star_date = star_date + (max_energy - energy) / 4;
				star_date = star_date - ((srs_flag + lrs_flag + es_flag + phaser_flag +
							  ep_flag + pt_flag + warp_flag + impulse_flag - 8 ) * 4);
							 
				dex = ex;
				dey = ey;
				energy = max_energy;
				pt = max_pt;
				srs_flag = 1;
				lrs_flag = 1;
				es_flag = 1;
				phaser_flag = 1;
				ep_flag = 1;
				pt_flag = 1;
				warp_flag = 1;
				impulse_flag = 1;
				map_flag = 1;
				
			}
			else {
				ex = sex; // Return Enterprise to start position 
				ey = sey;
				printf("You cannot re-dock so quickly\n");
			}
		}		
	}

	if (universe[ex][ey] == 'S')

		universe[ex][ey] = 'D';
	else
		universe[ex][ey] = 'E';

	warp_active = 0;
	srs_noprint(); 
}

void squad(int save_no) // Save Quadrant between 0 and 3 

{

	if (squad_flag <= 0 ) {
		printf("\nSave Quadrant unavailable\n");
		return;
	}

	if (save_no < 0 || save_no > 3) {
		clears();
		printf("\nSave value must be between 0 and 3\n\n");
	}
	else {
		if (sector_saved_already[save_no] == 1) {
			clears();
			printf("\nSector number %d already saved. Overwrite? (y/n) ", save_no);
			getchar();
			if (getchar() != 'y') {
				printf("Save Sector aborted\n");
				return;
			}
			else {
				sex[save_no] = ex;
				sey[save_no] = ey;
				sector_saved_already[save_no] = 1;
				ssK[save_no] = K_in_sector + K_count[0] + K_count[1] + K_count[2] + K_count[3]
							   + K_count[4] + K_count[5] + K_count[6] + K_count[7];
				ssS[save_no] = S_in_sector + S_count[0] + S_count[1] + S_count[2] + S_count[3]
							   + S_count[4] + S_count[5] + S_count[6] + S_count[7];
				ssW[save_no] = W_in_sector + W_count[0] + W_count[1] + W_count[2] + W_count[3]
							   + W_count[4] + W_count[5] + W_count[6] + W_count[7];
				clears(); 
				sector_saved_already[save_no] = 1;
				printf("\nCurrent sector saved as save number: %d\n", save_no); 
			}
		}
		sex[save_no] = ex;
		sey[save_no] = ey;
		ssK[save_no] = K_in_sector + K_count[0] + K_count[1] + K_count[2] + K_count[3]
					   + K_count[4] + K_count[5] + K_count[6] + K_count[7];
		ssS[save_no] = S_in_sector + S_count[0] + S_count[1] + S_count[2] + S_count[3]
					   + S_count[4] + S_count[5] + S_count[6] + S_count[7];
		ssW[save_no] = W_in_sector + W_count[0] + W_count[1] + W_count[2] + W_count[3]
					   + W_count[4] + W_count[5] + W_count[6] + W_count[7];
		clears(); 
		sector_saved_already[save_no] = 1;
		printf("\nCurrent sector saved as save number: %d\n", save_no); 
	}
	printf("\n");		
}
		
void str(void)	// Status report 
{

	printf("Status Report: STAR DATE: %4d. Life support systems available until: %4d\n", star_date, max_star_date);
	printf("--------------------------------------------------------------------------\n");
//	printf("End date: %4d\n\n", max_star_date); 

	printf("Energy:            %4.1f%%", (energy * 100)/max_energy);
	printf(" %7.0f Energy Units\n", energy);

	printf("Shields:           %4d\n", shield);
	printf("Phasers:           %4d\n", phaser);
	printf("Photon torpedoes:   %4d\n\n", pt);

	if (srs_flag > 0)
		printf("SRS operational\n");
	else
		printf("SRS unavailable\n");
	if (lrs_flag > 0)
		printf("LRS operational\n");
	else
		printf("LRS unavailable\n");
	if (squad_flag > 0)
		printf("Save Quadrant operational\n");
	else
		printf("Save Quadrant unavailable\n");
	if (phaser_flag > 0)
		printf("Phasers operational\n");
	else
		printf("Phasers unavailable\n");
	if (pt_flag > 0)
		printf("Photon Torpedoes operational\n");
	else
		printf("Photon Torpedoes unavailable\n");
	if (warp_flag > 0)
		printf("Warp power available\n");
	else
		printf("Warp power unavailable\n");
	if (impulse_flag > 0)
		printf("Impulse power available\n\n");
	else
		printf("Impulse power unavailable\n\n");

	printf("Klingons in sector: %2d\n", K_in_sector);
	printf("Klingons in universe:  %3d\n", K_in_universe);
	printf("Space Stations in universe: %3d\n\n", SS_in_universe);
	printf("Saved Quadrants: Q0: K=%2d S=%2d W=%2d   Q1: K=%2d S=%2d W=%2d\n", ssK[0], ssS[0], ssW[0], ssK[1], ssS[1], ssW[1]);
	printf("                 Q2: K=%2d S=%2d W=%2d   Q3: K=%2d S=%2d W=%2d\n\n", ssK[2], ssS[2], ssW[2], ssK[3], ssS[3], ssW[3]);
}


void damage(int dlevel)
{

/*

DAMAGE1	- You sucessfully kill Klingons.
	    - You move on impulse power.

DAMAGE2	- You attempt an attack with an unavailable weapon.

DAMAGE3	- You try to escape the universe.

DAMAGE4	- You collide with a K or S.

DAMAGE5	- You attack a Space Station.

*/

	switch (dlevel) {
	case 0 : break;
	case 1 : if (shield > 20 && shield <= 30) {					
			--ep_flag;
            printf("Shield energy is less than 30%%, you have suffered damage\n");
		}
		else if (shield > 10 && shield <= 20) {	
			--ep_flag;
			--warp_flag;
			printf("Shield energy is less than 20%%, you have suffered damage\n");
		}
		else if ( shield > 0 && shield <= 10) {
			--ep_flag;
			--warp_flag;
			--lrs_flag;
			printf("Sheild energy is less than 10%%, you have suffered damage\n");
		}
		else if ( shield <= 0 ) {
			energy = energy + shield - K_in_sector - K_in_sector - K_in_sector;
			shield = 0;
			--ep_flag;
			--warp_flag;
			--lrs_flag;
			--srs_flag;
			--pt_flag;
			printf("Sheild energy is 0, you have suffered substancial damage\n\n");		
		}
		break;
	case 2 : if (shield > 20 && shield <= 30) {						
			--es_flag;
			--squad_flag;
			printf("Sheild energy is less than 30%%, you have suffered damage\n\n");
		}
		else if (shield > 10 && shield <= 20) {	
			--es_flag;
			--phaser_flag;
			--srs_flag;
			--squad_flag;
			printf("Sheild energy is less than 20%%, you have suffered damage\n\n");
		}
		else if ( shield > 0 && shield <= 10) {
			--es_flag;
			--phaser_flag;
			--impulse_flag;
			--srs_flag;
			--squad_flag;
			printf("Sheild energy is less than 10%%, you have suffered damage\n\n");
		}
		else if ( shield <= 0 ) {
			energy = energy + shield - K_in_sector - K_in_sector - K_in_sector;
			shield = 0;
			--ep_flag;
			warp_flag = warp_flag - 2;
			lrs_flag = lrs_flag - 2;
			--srs_flag;
			pt_flag = pt_flag - 3;
			--squad_flag;
			printf("Sheild energy is 0, you have suffered substancial damage\n\n");		
		}
		break;
	case 3 : if (shield > 20 && shield <= 30) {					
			energy = energy - 50;
			--es_flag;
			printf("Sheild energy is less than 30%%, you have suffered damage\n\n");
		}
		else if (shield > 10 && shield <= 20) {	
			energy = energy - 50;
			--es_flag;
			--pt_flag;
			printf("Sheild energy is less than 20%%, you have suffered damage\n\n");
		}
		else if ( shield > 0 && shield <= 10) {
			energy = energy - 50;
			--es_flag;
			--pt_flag;
			--srs_flag;
			printf("Sheild energy is less than 10%%, you have suffered damage\n\n");
		}
		else if ( shield <= 0 ) {
			energy = energy + shield - 50;
			shield = 0;
			--es_flag;
			warp_flag = warp_flag -2;
			lrs_flag = lrs_flag - 2;
			srs_flag = srs_flag - 2;
			pt_flag = pt_flag - 3;
			--srs_flag;
			squad_flag = squad_flag - 2;
			printf("Sheild energy is 0, you have suffered substancial damage\n\n");		
		}
		break;
	case 4 : if (shield > 20 && shield <= 30) {					
			energy = energy - K_in_sector - K_in_sector - 20;
			--es_flag;
			--squad_flag;
			printf("Sheild energy is less than 30%%, you have suffered damage\n\n");
		}
		else if (shield > 10 && shield <= 20) {	
			energy = energy - K_in_sector - K_in_sector - 20;
			--es_flag;
			--squad_flag;
			pt_flag = pt_flag - 2;
			printf("Sheild energy is less than 20%%, you have suffered damage\n\n");
		}
		else if ( shield > 0 && shield <= 10) {
			energy = energy - K_in_sector - K_in_sector - 20;
			--es_flag;
			squad_flag = squad_flag - 2;
			pt_flag = pt_flag - 2;
			lrs_flag = lrs_flag -2;
			printf("Sheild energy is less than 10%%, you have suffered damage\n\n");
		}
		else if ( shield <= 0 ) {
			energy = energy - K_in_sector - K_in_sector - 20;
			shield = 0;
			--es_flag;
			warp_flag = warp_flag -2;
			lrs_flag = lrs_flag - 2;
			srs_flag = srs_flag - 2;
			pt_flag = pt_flag - 3;
			squad_flag = squad_flag - 3;
			printf("Sheild energy is 0, you have suffered substancial damage\n\n");		
		}
		break;
	case 5 : if (shield > 20 && shield <= 30) {					
			energy = energy - K_in_sector - K_in_sector - 20;
			--es_flag;
			--pt_flag;
			squad_flag = squad_flag - 2;
			printf("Sheild energy is less than 30%%, you have suffered damage\n\n");
		}
		else if (shield > 10 && shield <= 20) {	
			energy = energy - K_in_sector - K_in_sector - 20;
			--es_flag;
			pt_flag = pt_flag - 2;
			--lrs_flag;
			squad_flag = squad_flag - 2;
			printf("Sheild energy is less than 20%%, you have suffered damage\n\n");
		}
		else if ( shield > 0 && shield <= 10) {
			energy = energy - K_in_sector - K_in_sector - 20;
			--es_flag;
			pt_flag = pt_flag - 2;
			lrs_flag = lrs_flag - 2;
			squad_flag = squad_flag - 3;
			printf("Sheild energy is less than 10%%, you have suffered damage\n\n");
		}
		else if ( shield <= 0 ) {
			energy = energy - K_in_sector - K_in_sector - 20;
			shield = 0;
			--es_flag;
			warp_flag = warp_flag -2;
			lrs_flag = lrs_flag - 3;
			srs_flag = srs_flag - 2;
			pt_flag = pt_flag - 3;
			squad_flag = squad_flag - 3;
			printf("Sheild energy is 0, you have suffered substancial damage\n\n");		
		}
		break;
	}
}

void intro(void)
{
	clears();
	lines(7);
	printf("\t\t\t * * * * * * * * * * *  *\n");
	printf("\t\t\t *    **************    *\n");
	printf("\t\t\t *    * STAR TRECK *    *\n");
	printf("\t\t\t *    **************    *\n");
	printf("\t\t\t *                      *\n");
	printf("\t\t\t * Version 1.0          *\n");
	printf("\t\t\t * By William White     *\n");
	printf("\t\t\t * All rights reserved  *\n");
	printf("\t\t\t * * * * * * * * * * *  *\n");
	lines(8);
	printf("To list instrictions at any time type 'ins' at the command prompt.\n");



	printf("Type <RETURN> to continue");
	getchar();
	return;

}

void save_game(void)

{
	FILE *game_status;
	int i;
	
	game_status = fopen("GAME1.SAV", "wt");
	
	// Save cords. of Enterprise 
	fputc(':', game_status);
	fprintf(game_status, "%2d", ex);
	fputc(':', game_status);
	fprintf(game_status, "%2d", ey);
	fputc(':', game_status);

	/* Save docked Enterprise cords. */
	fprintf(game_status, "%2d", dex);
	fputc(':', game_status); 
	fprintf(game_status, "%2d", dey);
	fputc(':', game_status); 

	fprintf(game_status, "%4d", can_dock_date);
	fputc(':', game_status);

	for (i = 0; i <= 5; i++) {
		fprintf(game_status, "%1d", sex[i]);
		fputc(':', game_status);
		fprintf(game_status, "%1d", sey[i]);
		fputc(':', game_status);
	}

	fprintf(game_status, "%3d", K_in_universe);
	fputc(':', game_status);
	fprintf(game_status, "%3d", SS_in_universe);
	fputc(':', game_status);
	fprintf(game_status, "%2d", K_in_sector);
	fputc(':', game_status);
	fprintf(game_status, "%2d", S_in_sector);
	fputc(':', game_status);
	fprintf(game_status, "%2d", W_in_sector);

	for (i = 0; i <= 8; i++) {
		fputc(':', game_status);
		fprintf(game_status, "%2d", K_count[i]);
		fputc(':', game_status);
		fprintf(game_status, "%2d", S_count[i]);
		fputc(':', game_status);
		fprintf(game_status, "%2d", W_count[i]);
	}

	for (i = 0; i <= 3; i++) {
		fputc(':', game_status);
		fprintf(game_status, "%1d", sector_saved_already[i]);
		fputc(':', game_status);
		fprintf(game_status, "%2d", ssK[i]);
		fputc(':', game_status);
		fprintf(game_status, "%2d", ssS[i]);
		fputc(':', game_status);	
		fprintf(game_status, "%2d", ssW[i]);
	}


	fputc(':', game_status);
	fprintf(game_status, "%1d",srs_flag);
	fputc(':', game_status);		
	fprintf(game_status, "%1d",lrs_flag);
	fputc(':', game_status);	
	fprintf(game_status, "%1d",es_flag);
	fputc(':', game_status);		
	fprintf(game_status, "%1d",phaser_flag);
	fputc(':', game_status);	
	fprintf(game_status, "%1d",ep_flag);
	fputc(':', game_status);		
	fprintf(game_status, "%1d",pt_flag);
	fputc(':', game_status);	
	fprintf(game_status, "%1d",warp_flag);
	fputc(':', game_status);		
	fprintf(game_status, "%1d",impulse_flag);
	fputc(':', game_status);	
	fprintf(game_status, "%1d",squad_flag);
	fputc(':', game_status);
	fprintf(game_status, "%1d",warp_active);
	fputc(':', game_status);		
	fprintf(game_status, "%1d",worm_active);

	fputc(':', game_status);
	fprintf(game_status, "%3d", shield);
	fputc(':', game_status);		
	fprintf(game_status, "%3d", phaser);
	fputc(':', game_status);			
	fprintf(game_status, "%6.2f", max_energy);
	fputc(':', game_status); 
	fprintf(game_status, "%6.2f", energy);
	fputc(':', game_status);
	fprintf(game_status, "%3d", pt);
	fputc(':', game_status);
	fprintf(game_status, "%3d", max_pt);
	
	
	fputc(':', game_status);
	for (ux = 0; ux <= (UNIVERSE_SIZE - 1); ++ux) {
		for ( uy = 0; uy <= (UNIVERSE_SIZE - 1); ++uy ) {
			fputc(universe[ux][uy], game_status);
		}
	}


	fclose(game_status);
	printf("\nGame saved as GAME1.SAV\n");
}

int load_game(void)
{
	FILE *game_status;
	int i;
	char c;

	game_status = fopen("GAME1.SAV", "rt");
	if (game_status == NULL) {
		printf("Cannot open GAME1.SAV file\n");
		printf("Type <RETURN> to continue");
		getchar();
		return(1);
	}

	
	// Save cords. of Enterprise 
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%2d", &ex);
	
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%2d", &ey);

	// Save docked Enterprise cords. 
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%2d", &dex); 

	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%2d", &dey); 

	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%4d", &can_dock_date);

	for (i = 0; i <= 5; i++) {
		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%1d", &sex[i]);

		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%1d", &sey[i]);
	}

	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%3d", &K_in_universe);

	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%3d", &SS_in_universe);

	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%2d", &K_in_sector);

	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%2d", &S_in_sector);

	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%2d", &W_in_sector);


	for (i = 0; i <= 8; i++) {
		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%2d", &K_count[i]);

		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%2d", &S_count[i]);

		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%2d", &W_count[i]);
	}

	for (i = 0; i <= 3; i++) {
		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%1d", &sector_saved_already[i]);

		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%2d", &ssK[i]);

		do  c = fgetc(game_status); while (c != ':');
		fscanf(game_status, "%2d", &ssS[i]);

		do  c = fgetc(game_status); while (c != ':');	
		fscanf(game_status, "%2d", &ssW[i]);
	}


	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%1d", &srs_flag);	
	do  c = fgetc(game_status); while (c != ':');	
	fscanf(game_status, "%1d", &lrs_flag);	
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%1d", &es_flag);
	do  c = fgetc(game_status); while (c != ':');		
	fscanf(game_status, "%1d", &phaser_flag);
	do  c = fgetc(game_status); while (c != ':');	
	fscanf(game_status, "%1d", &ep_flag);	
	do  c = fgetc(game_status); while (c != ':');	
	fscanf(game_status, "%1d", &pt_flag);	
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%1d", &warp_flag);	
	do  c = fgetc(game_status); while (c != ':');	
	fscanf(game_status, "%1d", &impulse_flag);
	do  c = fgetc(game_status); while (c != ':');	
	fscanf(game_status, "%1d", &squad_flag);
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%1d", &warp_active);
	do  c = fgetc(game_status); while (c != ':');		
	fscanf(game_status, "%1d", &worm_active);


	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%3d", &shield);	
	do  c = fgetc(game_status); while (c != ':');	
	fscanf(game_status, "%3d", &phaser);	
	do  c = fgetc(game_status); while (c != ':');		
	fscanf(game_status, "%6f", &max_energy); 
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%6f", &energy);
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%3d", &pt);
	do  c = fgetc(game_status); while (c != ':');
	fscanf(game_status, "%3d", &max_pt);	
	

	do  c = fgetc(game_status); while (c != ':');
	// Save Universe 
	for (ux = 0; ux <= (UNIVERSE_SIZE - 1); ++ux) {
		for ( uy = 0; uy <= (UNIVERSE_SIZE - 1); ++uy ) {
			universe[ux][uy] = fgetc(game_status);
		}
	}



	fclose(game_status);
	printf("\nGame GAME1.SAV loaded\n");
}

int ins(void)		// startup instructions 

{

	FILE *instructions;

	#define spaces  "    "		// Spaces to start a line 

	int c, k;
	int nline_flag = 0;
	int first_time_in_flag = 1;
	int cr = 0;

	instructions = fopen("INSTRUCTIONS.TXT", "rt");
	if (instructions == NULL) {
		printf("Cannot open NSTRUCTIONS.TXT file\n");
		printf("Type <RETURN> to continue");
		getchar();
		return(1);
	}

	printf(spaces);
	while ((c = fgetc(instructions)) != EOF) {
		if (c == '\n') {
			cr++;
			nline_flag = 1;
		}
		if (cr == (SCREEN_ROWS - 4)) {
			cr = cr;
			cr = 0;
			lines(2);
			printf("Type <RETURN> to continue or 'e' to exit ");
			k = getchar();
			if (k == 'e')
				return(0); 
			if ( first_time_in_flag == 1) {
				k = getchar();
				if (k == 'e')
					return(0); 
				first_time_in_flag = 0;
			}
			while ( k != '\n') {
				k = getchar();
			}
			clears();
		}
		else 
			putchar(c);

		if (nline_flag == 1) {
			nline_flag = 0;
			printf(spaces);
		} 
	}

	lines(SCREEN_ROWS - cr - 3);
	printf("Type <RETURN> to continue");
	getchar();
	clears();
	
	fflush(instructions);
	fclose(instructions);
	return(0);
}

void lines(int lines)
{

	int i;

	for (i = 0; i <= lines; ++i)
		printf("\n");
}

int my_random(int max){
	return rand() % (max + 1);
}