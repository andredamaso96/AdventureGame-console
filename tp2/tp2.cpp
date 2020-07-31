#include "stdafx.h"
#include <stdio.h> //printf e scanf
#include <stdlib.h> //system("pause")
#include <locale.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>

#pragma comment (lib , "winmm.lib")



#define MAX_PLAYER_NAME 100
#define MONSTER_NAME 100
#define PLAYER_INITIAL_ENERGY 100
#define MAX_SU_PLAYER_ENERGY 1000
#define PLAYER_INITIAL_LOCATION 0
#define MAX_DESCRIPTION 200
#define MAX_CELLS 15
#define MOVEMENT 10
#define CELL_0_DESCRIPTION "Está na entrada da Mansão Assombrada, derrote o bruxo e procure a outra saída\n(n) (s) (e) (mapa)\n"
#define NO_MACHADO 0
#define CELL_TREASURE 15
#define MAX_LINE 100 
#define MONSTER_INITIAL_LOCATION 8
#define nplays 100


void PrintToConsole(char text[]) {

	printf("%s", text);
	fflush(stdout);
}


struct Player {
	char name[MAX_PLAYER_NAME];
	int energy;
	int location;
	int chave;
	int machado;

};

struct Monster
{

	char name[MONSTER_NAME];
	int healthpoints;
	int location;
	bool dead;
};

struct Cell {
	int north;
	int south;
	int west;
	int east;
	int up;
	int down;
	char description[MAX_DESCRIPTION];
	int treasure;

};

void mapImg();

void showMap(struct Cell map[]);
void CarregarJogo(Player *pPlayer, Monster *pMonster, Cell map[]);
void init();
void guardarJogo(Player *pPlayer, Monster *pMonster, Cell cell[]);
void history();
//inicialização do player e do monstro
void InitPlayer(struct Player *pPlayer);
void ShowPlayer(struct Player *pPlayer, Monster *pMonster, bool super);
void InitMonster(struct Monster *pMonster);
void MovementCommand(Player *pPlayer, Monster *pMonster, Cell map[], bool super);
void Movement(Cell map[], Player *pPlayer);
void MovementMonster(Cell map[], Monster *pMonster, bool super);
void Fight(struct Player  *pPlayer, Monster *pMonster);
void monsterImg();
void gameOver();

int BackgroundSounds(int sounds);
void axeImg();
void keyImg();
//inicialização do mapa
int InitMap(struct Cell map[]);
void readMap(struct Cell map[]);

struct Player player;
struct Cell map[MAX_CELLS];
struct Monster monster;

int nCells;
HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
char resposta[MOVEMENT];
char apanhar[] = "get";
char machado[20];
char chave[20];
char acao[10];


bool superU;
bool death = false;
bool gameover = false;

HANDLE playMutex, monsterMutex;

HANDLE hThreadPlayer[nplays];
HANDLE hThreadMonster[nplays];

DWORD WINAPI ThreadPlayer(LPVOID lpParam)
{

	while(player.energy > 0)
	{
		WaitForSingleObject(playMutex, INFINITE);
		Movement(map, &player);
		MovementCommand(&player, &monster, map, superU);
		ReleaseMutex(playMutex);
	}
	
	
	return 0;
}


DWORD WINAPI ThreadMonster(LPVOID lpParam)
{

	while (monster.healthpoints > 0)
	{
		WaitForSingleObject(monsterMutex, INFINITE);
		MovementMonster(map, &monster, superU);
		ReleaseMutex(monsterMutex);
		//InitMonster(&monster);
		//Sleep(MovementMonster * 500);
		printf("\n O monstro está na sala: %d\n ", monster.location);
		Sleep(8000);

		
	}
	return 0;
}


int main(int argc, char* argv[])
{

	SetConsoleTitle(TEXT("Jogo de Aventura"));


	setlocale(LC_ALL, "Portuguese");


	if (argc > 1 && strcmp(argv[1], "su") == 0)
	{
		superU = true;
	}
	else
	{
		superU = false;
	}

	history();
	system("pause");
	system("cls");
	init();

	if (strcmp("1", resposta) == 0)
	{

		system("cls");
		showMap(map);
		InitPlayer(&player);
		ShowPlayer(&player, &monster, superU);
		printf("\nComandos (n) (s) (w) (e) (up) (down) (mapa)\n");
		printf("\nAjuda - Digite (mapa) para ver em que sitio da casa está situado\n");
		//readMap(map);
		nCells = InitMap(map);
		InitMap(map);
		InitMonster(&monster);
	}

	if (strcmp("2", resposta) == 0)
	{

		system("cls");
		showMap(map);
		CarregarJogo(&player, &monster, map);

		readMap(map);
		nCells = InitMap(map);
		InitMap(map);
		InitMonster(&monster);

	}

	HANDLE hPlayer = CreateThread(
		NULL,              // default security attributes
		0,                 // use default stack size  
		ThreadPlayer,        // thread function 
		NULL,             // argument to thread function 
		0,                 // use default creation flags 
		NULL);   // returns the thread identifier 

	HANDLE hThreadMonster = CreateThread(
		NULL,              // default security attributes
		0,                 // use default stack size  
		ThreadMonster,        // thread function 
		NULL,             // argument to thread function 
		0,                 // use default creation flags 
		NULL);   // returns the thread identifier 


	

	playMutex = CreateMutex(
		NULL,                       // default security attributes
		FALSE,                      // initially not owned
		NULL);                      // unnamed mutex


	monsterMutex = CreateMutex(
		NULL,                       // default security attributes
		FALSE,                      // initially not owned
		NULL);                      // unnamed mutex
	


	while (gameover == false)
	{

		//MovementCommand(&player, &monster, map, superU);
		//Movement(map, &player);


		if (player.machado == 1 || player.machado == 0)
		{
			if (player.location == monster.location && monster.healthpoints == 100) {
				WaitForSingleObject(playMutex, INFINITE);
				WaitForSingleObject(monsterMutex, INFINITE);
				printf("OH MEU DEUS O QUE É ISTO!?\nMas que grande monstro, mas não te perdo-o pelo que fizes-te as pessoas!!!!\nCoragem para derrotares os monstro, defende pela paz na cidade!\n\n");
				Fight(&player, &monster);


				if (death == false && monster.dead == true) {
					printf("Finalmente a aldeia pode viver sem o medo da maldição. %s foste um grande corajoso \n", player.name);


				}
				if (death != false && monster.dead != true)
				{
					//system("cls");
					gameOver();

					system("pause");
					gameover = true;
				}







			}

		}


	}

	//WaitForSingleObject(hThreadPlayer[play], INFINITE);
	//WaitForSingleObject(hThreadMonster[play], INFINITE);
	CloseHandle(hThreadPlayer);
	CloseHandle(hThreadMonster);
	CloseHandle(monsterMutex);
	CloseHandle(playMutex);


	system("pause");
	return 0;


}

void InitPlayer(struct Player *pPlayer)
{

	//Init player
	printf("Olá aventureiro! Qual o seu nome?\n");
	scanf("%s", pPlayer->name);
	pPlayer->energy = PLAYER_INITIAL_ENERGY;
	pPlayer->location = 0;
	//pPlayer->chave = 0;
	pPlayer->machado = NO_MACHADO;


	if (superU == false)
	{
		pPlayer->energy = PLAYER_INITIAL_ENERGY;
	}
	else
	{
		pPlayer->energy = MAX_SU_PLAYER_ENERGY;
	}
}

void ShowPlayer(struct Player *pPlayer, Monster *pMonster, bool super)
{
	//Show player
	printf("Nome: %s\n", pPlayer->name);
	printf("Energia: %d\n", pPlayer->energy);
	printf("Local: %d\n", pPlayer->location);
	//printf("chave: %d\n", pPlayer->chave);

	if (super == true)
	{
		if (pPlayer->machado == 0)
		{
			printf("Monstro:%s - Celula: %d \n", pMonster->name, MONSTER_INITIAL_LOCATION);
			printf("");
		}
		else
		{
			if (pPlayer->machado == 1)
				printf("Monstro:%s Celula: %d \n", pMonster->name, MONSTER_INITIAL_LOCATION);
			printf("");
		}
	}
}
void InitMonster(struct Monster *pMonster)
{

	//Init Monster

	srand(time(NULL));

	int locationPos;

	locationPos = rand() & (4) + 8; //random para as posições do monstro nas salas do primeiro andar 

	strcpy(pMonster->name, "Lord Babau");
	pMonster->healthpoints = PLAYER_INITIAL_ENERGY;
	pMonster->location = MONSTER_INITIAL_LOCATION;
}


void MovementCommand(Player *pPlayer, Monster *pMonster, Cell map[], bool super)
{

	if (pPlayer->energy > 0)
	{
		printf(map[pPlayer->location].description);
	}

	if (map[pPlayer->location].treasure == 1 && pPlayer->machado == 0)
	{

		scanf("%s", resposta);
		if (strcmp(apanhar, resposta) == 0)
		{
			pPlayer->machado = 1;
			printf("Apanhas-te o Machado, usa-o para derrotar o bruxo\n");
			axeImg();
		}
	}

	if (map[pPlayer->location].treasure == 2 && pPlayer->chave == 1)
	{
		system("cls");
		pPlayer->location = 13;

		printf(map[player.location].description);
		BackgroundSounds(2);



	}
	if (map[pPlayer->location].treasure == 4)
	{
		BackgroundSounds(4);
	}
	if (map[pPlayer->location].treasure == 5)
	{
		BackgroundSounds(5);

	}
	if (map[pPlayer->location].treasure == 6)
	{
		BackgroundSounds(6);

	}



}

void Movement(Cell map[], Player *pPlayer)
{
	scanf("%s", resposta);
	if (strcmp(resposta, "mapa") == 0)
	{
		mapImg();
	}
	if (strcmp(resposta, "exit") == 0)
	{
		exit(1);
	}


	if (strcmp(resposta, "guardar") == 0)
	{
		guardarJogo(&player, &monster, map);
	}

	if (strcmp(resposta, "carregar") == 0)
	{
		CarregarJogo(&player, &monster, map);
	}
	SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	if (strcmp(resposta, "n") == 0 && map[player.location].north != -1)
	{
		player.location = map[player.location].north;

	}
	else if ((strcmp(resposta, "n") == 0) && (map[player.location].north == -1))
	{
		printf("Tente por outro lado, nessa direção não existe caminho!!\n\n");

	}



	if (strcmp(resposta, "s") == 0 && map[player.location].south != -1)
	{
		player.location = map[player.location].south;

	}
	else if (strcmp(resposta, "s") == 0 && map[player.location].south == -1)
	{
		printf("Tente por outro lado, nessa direção não existe caminho!!\n\n");

	}



	if (strcmp(resposta, "e") == 0 && map[player.location].east != -1)
	{
		player.location = map[player.location].east;

	}
	else if (strcmp(resposta, "e") == 0 && map[player.location].east == -1)
	{
		printf("Tente por outro lado, nessa direção não existe caminho!!\n\n");

	}




	if (strcmp(resposta, "w") == 0 && map[player.location].west != -1)
	{
		player.location = map[player.location].west;

	}
	else if (strcmp(resposta, "w") == 0 && map[player.location].west == -1)
	{
		printf("Tente por outro lado, nessa direção não existe caminho!!\n\n");

	}


	if (strcmp(resposta, "up") == 0 && map[player.location].up != -1)
	{
		player.location = map[player.location].up;

	}
	else if (strcmp(resposta, "up") == 0 && map[player.location].up == -1)
	{
		printf("Tente por outro lado, nessa direção não existe caminho!!\n\n");

	}



	if (strcmp(resposta, "down") == 0 && map[player.location].down != -1)
	{
		player.location = map[player.location].down;

	}
	else if (strcmp(resposta, "down") == 0 && map[player.location].down == -1)
	{
		printf("Tente por outro lado, nessa direção não existe caminho!!\n\n");

	}

	SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

}

void MovementMonster(Cell map[], Monster *pMonster, bool super) {
	int random = rand() % 6 + 1;

	if (random == 1) {
		if (map[pMonster->location].north != -1) {
			pMonster->location = map[pMonster->location].north;
		}
		else {
			pMonster->location = pMonster->location;
		}
	}
	if (random == 2) {
		if (map[pMonster->location].south != -1) {
			pMonster->location = map[pMonster->location].south;
		}
		else {
			pMonster->location = pMonster->location;
		}
	}
	if (random == 3) {
		if (map[pMonster->location].west != -1) {
			pMonster->location = map[pMonster->location].west;
		}
		else {
			pMonster->location = pMonster->location;
		}
	}
	if (random == 4) {
		if (map[pMonster->location].east != -1) {
			pMonster->location = map[pMonster->location].east;
		}
		else {
			pMonster->location = pMonster->location;
		}
	}

	if (random == 5) {
		if (map[pMonster->location].up != -1) {
			pMonster->location = map[pMonster->location].up;
		}
		else {
			pMonster->location = pMonster->location;
		}
	}

	if (random == 6) {
		if (map[pMonster->location].down != -1) {
			pMonster->location = map[pMonster->location].down;
		}
		else {
			pMonster->location = pMonster->location;
		}
	}
}


void Fight(struct Player  *pPlayer, Monster *pMonster)
{
	printf("%s: %d Vida\n", pPlayer->name, pPlayer->energy);
	printf("%s: %d Vida\n", pMonster->name, pMonster->healthpoints);

	monsterImg();

	while (death == false && monster.dead == false)
	{
		int rand_dmg = rand() % 50 % 15;
		int player_dmg = rand_dmg;
		int machado_dmg = rand() % 100 % 70;
		int monster_dmg = rand_dmg;
		int rand_num = rand() % 2;

		if (pPlayer->machado == 0) {

			if (rand_num == 0) {
				pMonster->healthpoints = pMonster->healthpoints - player_dmg;
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
				printf("Atingiste o monstro com sucesso e tiraste-lhe %d dano! O %s tem %d de vida!\n", player_dmg, pMonster->name, pMonster->healthpoints);
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				rand_num = rand() % 2;
				system("pause");
			}
			else {
				pPlayer->energy = pPlayer->energy - monster_dmg;
				SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
				printf("A maldição é superior! Tirou-te %d de dano. Tens %d de vida!\n", monster_dmg, pPlayer->energy);
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				system("pause");
				rand_num = rand() % 2;
			}



			if (pPlayer->energy < 0) {
				SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
				printf("O %s conseguiu aniquilar-te ... A cidade continuará a viver tempos de agonia e medo!\n\n", pMonster->name);
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				death = true;
				gameover = true;
			}

			if (pMonster->healthpoints < 0) {
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
				printf("Muito bem %s! Finalmente conseguiste pôr um fim ao bruxo %s e manda-lo para o inferno! \n Agora que tens a chave finalmente podes sair dai!! \n", pPlayer->name, pMonster->name);
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				/*if (map[pPlayer->location].treasure == 1 && pPlayer->chave == 0)
				{
					pPlayer->chave = 1;
					keyImg();
					printf("Apanhas-te a chave, talvez consiga usa-la para sair daqui\n");
					map[player.location].east = 13;
				}*/
				monster.dead = true;
				break;
			}
		}
		else {
			if (rand_num == 0) {
				pMonster->healthpoints = pMonster->healthpoints - machado_dmg;
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
				printf("Atingiste Babau com o machado %d dano! O %s tem %d de vida!\n", machado_dmg, pMonster->name, pMonster->healthpoints);
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				rand_num = rand() % 2;
				system("pause");
			}
			else {
				pPlayer->energy = pPlayer->energy - monster_dmg;
				SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
				printf("A força do bruxo foi superior!Tirou - te %d de dano. Tens %d de vida!\n", monster_dmg, pPlayer->energy);
				rand_num = rand() % 2;
				system("pause");
			}

			if (pPlayer->energy < 0) {
				SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
				printf("O %s conseguiu aniquilar - te ... A cidade continuará a viver tempos de agonia e medo!\n\n", pMonster->name);
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				death = true;
				gameover = true;
			}

			if (pMonster->healthpoints < 0) {
				SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
				printf("Muito bem %s! Finalmente conseguiste pôr um fim ao bruxo %s e manda-lo para o inferno! \n Agora que tens a chave finalmente podes sair dai!! \n", pPlayer->name, pMonster->name);
				if (map[pPlayer->location].treasure == 1 && pPlayer->chave == 0)
				{
					pPlayer->chave = 1;
					keyImg();
					printf("Apanhas-te a chave, talvez consiga usa-la para sair daqui\n");
					SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					map[player.location].east = 13;
				}
				monster.dead = true;
				break;

			}


		}


	}


}


int InitMap(struct Cell map[]) {

	//Cell 0
	map[0].north = 1;
	map[0].south = 2;
	map[0].west = -1;
	map[0].east = 3;
	map[0].up = -1;
	map[0].down = -1;
	strcpy(map[0].description, CELL_0_DESCRIPTION);
	map[0].treasure = 0;

	//Cell 1
	map[1].north = -1;
	map[1].south = 0;
	map[1].west = -1;
	map[1].east = 4;
	map[1].up = -1;
	map[1].down = -1;
	strcpy(map[1].description, "Celula 1 - Entrou no primeiro quarto da casa, pela aparência parece que viviam aqui crianças que foram assassinadas\n(s) (e) (mapa)\n");
	map[1].treasure = 0;

	//Cell 2
	map[2].north = 0;
	map[2].south = -1;
	map[2].west = -1;
	map[2].east = 6;
	map[2].up = -1;
	map[2].down = -1;
	strcpy(map[2].description, " Celula 2 - Agora entrou no escritório, este é o sitío com livros sobre o monstro e o tesouro, seja rápido a descobrir\n(n) (e) (mapa)\n");
	map[2].treasure = 0;

	//Cell 3
	map[3].north = 4;
	map[3].south = 6;
	map[3].west = 0;
	map[3].east = -1;
	map[3].up = 8;
	map[3].down = -1;
	strcpy(map[3].description, "Celula 3 - Aqui é o hall, houve se uns barulhos de fundos, portas a bater....tem hipoteses de subir para o primeiro andar, o Monstro sente a sua presença...\n(n) (s) (w) (up) (mapa)\n");
	map[3].treasure = 0;

	//Cell 4
	map[4].north = -1;
	map[4].south = 3;
	map[4].west = 1;
	map[4].east = 5;
	map[4].up = -1;
	map[4].down = -1;
	strcpy(map[4].description, "Celula 4 - Estou numa divisão que me parece como uma cozinha, vejo pratos com restos de comida, parece que não estou sozinho......\n UHHHHHHHHHHH que barulho foi este!?\n(s) (w) (e) (mapa)\n");
	map[4].treasure = 6;

	//Cell 5
	map[5].north = -1;
	map[5].south = -1;
	map[5].west = 4;
	map[5].east = -1;
	map[5].up = -1;
	map[5].down = -1;
	strcpy(map[5].description, "Celula 5 - Isto é uma dispensa cheio de teias de aranha, esta escuro e houve-se barulhos...\nMas espera aí... Vejo um machado, se o levar comigo talvez consiga derrotar facilmente esse monstro.\n (w) (get) (mapa)\n");
	map[5].treasure = 1;

	//Cell 6
	map[6].north = 3;
	map[6].south = 7;
	map[6].west = 2;
	map[6].east = -1;
	map[6].up = -1;
	map[6].down = -1;
	strcpy(map[6].description, "Celula 6 - Estou numa sala sinistra, vejo um sofá e ao meu lado liga-se de repente uma televisão...\n Isto não me agrada, nesta casa passam-se coisas muito estranhas\n(n) (s) (w) (mapa)\n");
	map[6].treasure = 5;

	//Cell 7
	map[7].north = 6;
	map[7].south = -1;
	map[7].west = -1;
	map[7].east = -1;
	map[7].up = -1;
	map[7].down = -1;
	strcpy(map[7].description, "Celula 7 - Entrei na garagem, sinto que não estou seguro aqui, vejo também um portão de saída mas parece-me estar trancado\n(n) (e) (mapa)\n");
	map[7].treasure = 2;

	//Cell 8
	map[8].north = 9;
	map[8].south = 11;
	map[8].west = -1;
	map[8].east = -1;
	map[8].up = -1;
	map[8].down = 3;
	strcpy(map[8].description, "Celula 8 - Subi até ao 1º andar, vejo várias portas...\n(n) (s) (down) (mapa)\n");
	map[8].treasure = 0;

	//Cell 9
	map[9].north = -1;
	map[9].south = 8;
	map[9].west = 12;
	map[9].east = -1;
	map[9].up = -1;
	map[9].down = -1;
	strcpy(map[9].description, "Celula 9 - Estou num quarto que muito estranho com vista para uma varanda/n De repente cai uma gota de sangue....\n(s) (w) (mapa)\n");
	map[9].treasure = 0;

	//Cell 10
	map[10].north = 12;
	map[10].south = -1;
	map[10].west = -1;
	map[10].east = 11;
	map[10].up = -1;
	map[10].down = -1;
	strcpy(map[10].description, "Celula 10 - Estou na casa de banho e deparo-me que está tudo destruído e está iluminada por uma vela junto ao espelho, onde esta escrito com sangue....\n Bem vindo ao Mundo dos mortos\n(n) (e) (mapa)\n");
	map[10].treasure = 6;

	//Cell 11
	map[11].north = 8;
	map[11].south = -1;
	map[11].west = 10;
	map[11].east = -1;
	map[11].up = -1;
	map[11].down = -1;
	strcpy(map[11].description, "Celula 11 - Acho que não devia ter entrado aqui, parece-me que dormia aqui um casal, e vejo um quadro com os seus retratos\n Oiço alguém a pedir ajuda....n\ Parece-me ser uma voz de uma mulher\n(n) (w) (mapa)\n");
	map[11].treasure = 1;

	//Cell 12
	map[12].north = -1;
	map[12].south = 10;
	map[12].west = -1;
	map[12].east = 9;
	map[12].up = -1;
	map[12].down = -1;
	strcpy(map[12].description, "Celula 12 - Estou na varanda e a única luz que tenho é a do luar, dentro da casa oiço passos e grunhidos\n(s) (e) (mapa)\n");
	map[12].treasure = 0;

	//Cell 13
	/*map[13].north = -1;
	map[13].south = -1;
	map[13].west = 7;
	map[13].east = -1;
	map[13].up = -1;
	map[13].down = -1;
	strcpy(map[13].description, "Finalmente estou cá fora!!!\n");
	map[13].treasure = 3;*/

	return 13;
}

void history() {

	SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
	printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n\n");
	printf("   |-|     Era uma uma noite escura e chuvosa naquela cidade, na casa número 23 conhecida como uma casa desabitada, rumores diziam             |-|\n"
		"   |-|     que estava assombrada e quem la entrava não saí-a com vida. A propriedade pertencia a um suposto Buxo, Lord Babau, que              |-|\n"
		"   |-|     sacrificou seu filho com uma semana de idade e sua família ao diabo e se matou em 1873, depois de almadiçoar todos os que           |-|\n"
		"   |-|     tentassem tomar suas terras.                                                                                                        |-|\n"
		"   |-|                                                                                                                                         |-|\n"
		"   |-|       Nessa noite, um bravo aventureiro quis por fim a maldição dessa casa, para que as pessoas da pequena cidade podessem              |-|\n"
		"   |-|     viver sem o medo de serem amaldiçoadas por um suposto bruxo que levava desde assassinatos a suicídios.                              |-|\n"
		"   |-|                                                                                                                                         |-|\n"
		"   |-|       A casa tem 11 divisões, uma garagem e um acesso ao primeiro andar, ao todo são 13 Células.Lá dentro encontra - se o suposto       |-|\n"
		"   |-|     bruxo que lhe vai tentar fazer lhe a cabeça e amaldiçoa - lo para sempre. Vai ter de se defender e ter muito cuidado,               |-|\n"
		"   |-|     rumores dizem que dentro da casa caminha um monstro que poderá ser o bruxo a evocar o Diabo.                                        |-|\n"
		"   |-|                                                                                                                                         |-|\n"
		"   |-|       Enquanto andar pela casa, procure objetos que poderão servir e conduz o aventureiro a por um final à maldição que ronda           |-|\n"
		"   |-|     pela cidade.                                                                                                                        |-|\n"
		"   |-|     O aventureiro entra em casa e a porta fecha - se sozinha e fica trancada, impossibilitando de sair..                                |-|\n"
		"   |-|                                                                                                                                         |-|\n");
	printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
	printf("\n");
	BackgroundSounds(3);

	SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);


}

void monsterImg()
{
	printf("  ");
	printf("                 (    )\n");
	printf("                  ((((()))\n");
	printf("                  |o\ /o)|\n");
	printf("                 ( (  _')\n");
	printf("                  (._.  /\__\n");
	printf("                  ,\___,/ '  ')\n");
	printf("    '.,_,,       (  .- .   .    )\n");
	printf("     \   \\     ( '        )(    )\n");
	printf("      \   \\    \.  _.__ ____( .  |\n");
	printf("       \  /\\   .(   .'  /\  '.  )\n");
	printf("        \(  \\.-' ( /    \/    \)\n");
	printf("         '  ()) _'.-|/\/\/\/\/\|\n");
	printf("             '\\ .( |\/\/\/\/\/|\n");
	printf("               '((  \    /\    /\n");
	printf("               ((((  '.__\/__.')\n");
	printf("                ((,) /   ((()   )\n");
	printf("                 \"/.. - , (()(\"/   /\n");
	printf("                  _//.   ((() .\"/\n");
	printf("         _____ //,/\"/ ___(((', ___\n");
	printf("                           ((  )\n");
	printf("                            / /\n");
	printf("                          _/,/'\n");
	printf("                          /,/,\"/\n");


}

void guardarJogo(Player *pPlayer, Monster *pMonster, Cell cell[]) {
	FILE *jogoGuardado;

	jogoGuardado = fopen("save.txt", "w");


	fprintf(jogoGuardado, "%s\n", pPlayer->name);
	fprintf(jogoGuardado, "%d\n", pPlayer->energy);
	fprintf(jogoGuardado, "%d\n", pPlayer->location);
	fprintf(jogoGuardado, "%d\n", pPlayer->chave);
	fprintf(jogoGuardado, "%d\n", pPlayer->machado);
	fprintf(jogoGuardado, "\n");
	fprintf(jogoGuardado, "%s\n", pMonster->name);
	fprintf(jogoGuardado, "%d\n", pMonster->healthpoints);

	if (pPlayer->machado == 0)
	{
		for (int i = 0; i < MAX_CELLS; i++)
		{
			if (map[i].treasure == 1)
			{
				fprintf(jogoGuardado, "%d", i);
			}
		}
	}
	fclose(jogoGuardado);
	printf("Obrigado por teres jogado, ficamos a tua espera!\n");
}
void axeImg()
{
	printf("  ,  /\  .\n");
	printf(" //`-||-'\\\n");
	printf("(| -=||=- |)\n");
	printf(" \\,-||-.//\n");
	printf("  `  ||  '\n");
	printf("     ||\n");
	printf("     ||\n");
	printf("     ||\n");
	printf("     ||\n");
	printf("     ||\n");
	printf("     ()\n");
}

void CarregarJogo(Player *pPlayer, Monster *pMonster, Cell map[])
{
	FILE *jogoCarregado;
	char data[MAX_LINE];

	jogoCarregado = fopen("save.txt", "r");

	fgets(data, MAX_LINE, jogoCarregado);
	data[strcspn(data, "\n")] = '\0';
	strcpy(pPlayer->name, data);

	fgets(data, MAX_LINE, jogoCarregado);
	pPlayer->energy = atoi(data);

	fgets(data, MAX_LINE, jogoCarregado);
	pPlayer->location = atoi(data);

	fgets(data, MAX_LINE, jogoCarregado);
	pPlayer->machado = atoi(data);

	fgets(data, MAX_LINE, jogoCarregado);
	pMonster->location = atoi(data);

	fgets(data, MAX_LINE, jogoCarregado);
	for (int i = 0; i < MAX_CELLS; i++)
	{
		map[i].treasure = 0;
	}

	if (pPlayer->machado == 0)
	{
		map[atoi(data)].treasure = 1;
	}

	printf("Bem vindo de volta %s! continuas na sala %d.\n", pPlayer->name, pPlayer->location);
	system("pause");
}
void keyImg()
{
	printf("\n");
	printf("\n");
	printf("     8 8 8 8                     ,ooo.\n");
	printf("     8a8 8a8                    oP   ?b\n");
	printf("    d888a888zzzzzzzzzzzzzzzzzzzz8     8b\n");
	printf("         `""^""'                    ?o___oP'\n");
	printf("                     \n");
	printf("\n");
}
void init()
{
	printf("Novo jogo - 1\n");
	printf("Carregar jogo - 2\n");
	scanf("%s", resposta);
}
int BackgroundSounds(int sounds)
{
	if (sounds == 1)
	{
		PlaySound(TEXT("gameOver.wav"), NULL, SND_SYNC | SND_FILENAME);
		return 0;
	}
	if (sounds == 2)
	{
		PlaySound(TEXT("vitoria.wav"), NULL, SND_SYNC | SND_FILENAME);
		return 0;
	}
	if (sounds == 3)
	{
		PlaySound(TEXT("orgao.wav"), NULL, SND_FILENAME);
		return 0;
	}
	if (sounds == 4)
	{
		PlaySound(TEXT("grito.wav"), NULL, SND_FILENAME);
		return 0;

	}
	if (sounds == 5)
	{
		PlaySound(TEXT("TV.wav"), NULL, SND_FILENAME);
		return 0;

	}
	if (sounds == 6)
	{
		PlaySound(TEXT("passos.wav"), NULL, SND_FILENAME);
		return 0;

	}

}
void gameOver()

{

	BackgroundSounds(1);
	printf("´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢¢¢¢¢¢¢¢¢¢¢¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´´´´´´¶¢¢¢¢¢¢¢¶´´´´´´´´¶¢¢¢¢¢¢¢¶´´´´´´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´´´¶¢¢¢¢´´´´´´´´´´´´´´´´´´´´´´¶¢¢¢¶´´´´´´´´´´´´´´´´´´´\n");
	printf(" ´´´´´´´´´´´´´´´´´¶¢¢¶´´´´´´´´´´´´´´´´´´´´´´´´´´´´¶¢¢¶´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´¢¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¶¢¢´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´¶¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´¢¢´´¶´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢´´¢¢´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´¢¶´´¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¶´´¢´´´´´´´´´´´´´\n");
	printf(" ´´´´´´´´´´´´´¢¶´´¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢´´¢´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´¢¢´´¢¶´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢´´´¢´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´¢¢´´¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢´´¶¢´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´¶¢´´´¢¶´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¶´´¢¢´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´¢¢´´¢¢´´´´´´´¶¶¢¶´´´´´´´´¶¢¶¶´´´´´´´´¢´´¢¢´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´¢¢´¢¢´´¢¢¢¢¢¢¢¢¢¢´´´´´´¢¢¢¢¢¢¢¢¢¢¶´´¢´¢¢´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´¢¢¢¶´¢¢¢¢¢¢¢¢¢¢¢´´´´´´¢¢¢¢¢¢¢¢¢¢¢´´¢¢¢´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´¢¢´´¢¢¢¢¢¢¢¢¢¢¶´´´´´´´¢¢¢¢¢¢¢¢¢¢´´¢¢´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´¢¢¢¢¶´´´´´´´¢¢´´´¢¢¢¢¢¢¢¢¶´´´´´´´´´¢¢¢¢¢¢¢¢´´´¶¢´´´´´´´¶¢¢¢¢´´´´´\n");
	printf("´´´´´¢´´¶¢¢´´´´´´¢¢´´´´¢¢¢¢¢¢´´´´´¶´´´´´´¢¢¢¢¢¢´´´´´¢´´´´´´¢¢´´´¢´´´´´\n");
	printf("´´´´´¢´´´´¢¢´´´´´¢¢´´´´´´¶¶´´´´´¶¢¢´¢¢´´´´´¶¶´´´´´´¶¢´´´´´¢¢´´´´¢¢´´´´\n");
	printf("´´´¶¢¢´´´´´¢¢¢´´´´¢´´´´´´´´´´´´´¢¢¶´¢¢¢´´´´´´´´´´´´¢¢´´´¢¢¢¶´´´´¢¢¢´´´\n");
	printf("´´¢¢´´´´´´´´´¢¢¢¢´¢¢¶´´´´´´´´´´¢¢¢¶´¢¢¢´´´´´´´´´´¶¢¢´¢¢¢¢´´´´´´´´´¢¢´´\n");
	printf("´¢¢´´´´´´´´´´´´´¶¢¢¢¢¢¢¢¢´´´´´´¢¢¢¢´¢¢¢´´´´´´¢¢¢¢¢¢¢¢¶´´´´´´´´´´´´´¢¢´\n");
	printf("´´¢¢¢¢¢¢¢¢¢¢¢´´´´´´´¶¢¢´¢¢¢´´´´¶¢¢´´¶¢¢´´´´¢¢¢´¢¢¶´´´´´´´¶¢¢¢¢¢¢¢¢¢¢´´\n");
	printf("´´´´´´´´´´´¶¢¢¢¢´´´´´´¢´¢´¢¢´´´´´´´´´´´´´´¢¢´¢´¢´´´´´´¢¢¢¢¢´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´¢¢¢¢¢´´¢¢´¢´¢¢´´´´´´´´´´´¶´¢´¶´¢¢´´¶¢¢¢¢´´´´´´´´´´´´´´´\n");
	printf("                 ´¶¢¢¢¢¢´¢´¢´¢´¢¢¢¢¢¢¢¢¶¢´¢´¢´¢¢¢¢¢¶´´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´´´´´´¢¢´¢´¢´¢´¢´¶´¶´´¶´¢¶¢´¢´¢¢´´´´´´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´´´´¶¢¢¢´´¢¢´¢´¢¶¢¶¢¢¢¢´¶´¢¢¶´¢¢¢¶´´´´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´¢¢¢¢¶´¢´´´¢¢¢´¢´¶´¢´¶¢¶¢¢¢´´´¢¢´¢¢¢¢´´´´´´´´´´´´´´´´´\n");
	printf("´´´´´´´´¢¢¢´´¶¢¢¢¢¶´´´´¢¢´´´´´´¶¢¢¢¢¢¢¶´´´´´´¢¢´´´´´¢¢¢¢¢´´¢¢¢´´´´´´´´\n");
	printf("´´´´´´¢¢¢¢¢¢¢¢¶´´´´´´´¶¢¢¢´´´´´´´´´´´´´´´´´´¢¢¢¢´´´´´´´¶¢¢¢¢¢¢¢¢¶´´´´´\n");
	printf("´´´´´´¢¢´´´´´´´´´´´¢¢¢¢´´¢¢¢´´´´´´´´´´´´´´¢¢¢´´¢¢¢¢´´´´´´´´´´´´¢¢´´´´´\n");
	printf("´´´´´´´¢¢¢´´´´´´¢¢¢¢´´´´´´´¢¢¢¢¢¢¢¢¢¢¢¢¢¢¢¢´´´´´´´¢¢¢¢¶´´´´´¶¢¢´´´´´´´\n");
	printf("´´´´´´´´´¢¶´´´¢¢¢´´´´´´´´´´´´´´¶¶¢¢¢¢¢¶´´´´´´´´´´´´´´¶¢¢´´´´¢¢´´´´´´´´\n");
	printf("´´´´´´´´´¢¢´´¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢´´´¢´´´´´´´´´\n");
	printf("´´´´´´´´´¢¢¢¢¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢¢¢¢´´´´´´´´´\n");
	printf("´´´´´´´´´´¶¢´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´¢¢´´´´´´´´´´\n");
	printf("´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´´\n");
	printf(" Parece que o Bruxo conseguiu continuar com o seu reinado de terror, melhor sorte para a proxima vez\n");

}



void mapImg()
{
	printf("	        ____________________________________________________________          _______________________________________________ \n");
	printf("		|                    |                     |		    |         |          |                                   | \n");
	printf("		|        1           =         4           =		5   |         |          =                                   |\n");
	printf("		|		     =	                   =		    |         |          =               9                   |\n");
	printf("		|________[_]_________|_________[ ]_________|________________|         |          |                                   |\n");
	printf("		|                    |                     |                |         |    12    |____________[ ]____________________|\n");
	printf("		|        0           =         3                   8  ||||||          |          |                                   | \n");
	printf("		|                    =                                ||||||          |          |        8            |||||||||||||||\n");
	printf("		|________[ ]_________|_________[ ]_________|________________|         |          |                     |||||||||||||||\n");
	printf("		|                    |                     |                |         |____[ ]___|_____________[ ]___________________|\n");
	printf("		|        2           =         6           |                |         |          |                                   |\n");
	printf("		|                    =                     |       7        |         |          =                                   |\n");
	printf("		|____________________|_________[ ]_________|                =         |    10    =               11	                 |\n");
	printf("		                     |                                      =         |          |                                   |\n");
	printf("    		                     |______________________________________|         |__________|___________________________________|\n");



}


void showMap(struct Cell map[])
{
	FILE *mapTxt;
	char l[MAX_LINE];

	mapTxt = fopen("map.txt", "w");

	for (int i = 0; i < MAX_CELLS; i++)
	{
		fprintf(mapTxt, "%d %d %d %d %d %d %d", map[i].north, map[i].south, map[i].east, map[i].west, map[i].up, map[i].down, map[i].treasure);
		fprintf(mapTxt, "%s", map[i].description);

	}

	fclose(mapTxt);
}

void readMap(struct Cell map[])
{

	FILE *mapTxt;
	char l[MAX_LINE];

	mapTxt = fopen("map.txt", "r");

	for (int i = 0; i < MAX_CELLS; i++)
	{
		fgets(l, MAX_LINE, mapTxt);
		map[i].north = atoi(l);
		fgets(l, MAX_LINE, mapTxt);
		map[i].south = atoi(l);
		fgets(l, MAX_LINE, mapTxt);
		map[i].east = atoi(l);
		fgets(l, MAX_LINE, mapTxt);
		map[i].west = atoi(l);
		fgets(l, MAX_LINE, mapTxt);
		map[i].up = atoi(l);
		fgets(l, MAX_LINE, mapTxt);
		map[i].down = atoi(l);
		fgets(l, MAX_LINE, mapTxt);
		map[i].treasure = atoi(l);
		fgets(l, MAX_LINE, mapTxt);
		strcpy(map[i].description, l);
	}



	fclose(mapTxt);

}