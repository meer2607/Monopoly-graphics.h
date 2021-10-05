#include<stdio.h>
#include<conio.h>
#include<graphics.h>
#include<dos.h> //for delay()
#include<math.h>
#include<string.h>
#include<fstream.h> //Loading and saving data
#include<iostream.h>
#include<stdlib.h> //for atoi() - Converting char* to int
				   //and itoa() - Converting int to char*
				   //and rand()
#include<time.h> // for intialising random number generator

void drawLine(int, int, int, int, int = WHITE, int=0); //Draws a line using DDA
int absoluteValue(int); //Return positive value regardless of sign
void drawRectangle(int, int, int, int, int = WHITE, int=0);
void drawBoard(void); // draws the complete board(Properties, players, etc)
int round(float); //Rounds a decimal to nearest integer
void fetchData(void); //Fetches Property data from the file Places.txt
void initialMenu(void); //Displays the starting menu before starting the game.
void fillRectangle(int, int, int, int, int = BLACK);
int searchInString(char[], char); //Searches for a character in a String
void substring(char[], char[], int, int);
void playGame(void); //The entire gameplay
void print(char[], int=0);  //Prints logs, questions, etc inside the board.
													//When the second parameter is 1, it erases all items.
void displayPlayOptions(); //Displays options to the player.
void addBalance(int, int);
//Because another player cannot be reffered from a player's object
void displayPlayerDetails(void);
int generateRandomNumber(int, int);

int printedItems = 0; //Stores the number of currently displayed text on screen,
											//so that next one will be printed below it.
int inGamePlayers;
char message[35]; //To display logs, questions, etc on screen.

class Place{
	public:
		int position, price, rent[6], color, coordinates[4], owner;
		int upgradeCost, upgrades;
		//Coordinate format-> topLeft, bottomRight.
		//These are to display the details:
		//eg. name will be printed on coordinates[x1]+nameOffset[0], coordinates[y1]+nameOffset[1]
		int colorOffset[4], nameOffset[4], priceOffset[2], playerOffset[2];
		char name[25];

		Place(){
			for(int i=0; i<4; i++)
				colorOffset[i] = 0;
			owner = -1;
			upgrades = 0;
		}

		void displayDetails(){
			sprintf(message, "Name: %s", name);
			print(message);
			sprintf(message, "Price: %d", price);
			print(message);
			print("Rents:");
			sprintf(message, "\tBasic: %d", rent[0]);
			print(message);
			sprintf(message, "\tWith a house: %d", rent[1]);
			print(message);
			sprintf(message, "\tWith 2 houses: %d", rent[2]);
			print(message);
			sprintf(message, "\tWith 3 houses: %d", rent[3]);
			print(message);
			sprintf(message, "\tWith 4 houses: %d", rent[4]);
			print(message);
			sprintf(message, "\tWith a hotel: %d", rent[5]);
			print(message);
			sprintf(message, "Upgrade Cost: %d", upgradeCost);
			print(message);
			sprintf(message, "Mortgage value: %d", price/2);
			print(message);
		}
}place[40];

class Player{
	public:
		static int numberOfPlayers;
		int id, balance, position, offset[4], color, coordinates[2], skipTurns;
		int lastRoll[2], isInGame;
		char name[20];

		/* Offset Format:
		0, 1 -> Offset for vertical places(Top and bottom)
		2, 3 -> Offset for Horizontal places(Left and right) */

	Player(){
		balance = 1500;
		offset[0] = 0;
		offset[1] = 0;
		skipTurns = 0;
		position = 0;
		isInGame = 1;
	}

	void rollDice(int numberOfRolls=0){
		lastRoll[0] = generateRandomNumber(1, 6);
		lastRoll[1] = generateRandomNumber(1, 6);
		sprintf(message, "You got %d(%d+%d)", lastRoll[0]+lastRoll[1],
																					lastRoll[0], lastRoll[1]);
		print(message);
		movePlayer((position + lastRoll[0] + lastRoll[1])%40);
		if(lastRoll[0]==lastRoll[1]){
			if(numberOfRolls == 3){
				print("Woah! You got 3 continuous doubles.");
				print("You know what that means?");
				delay(3);
				print("Straight to JAIL!");
				getch();
				sendToJail();
			}
			else{
				print("You got doubles.");
				print("Press any key to roll again.");
				rollDice(++numberOfRolls);
			}
		}
	}

	void movePlayer(int position){
		fillRectangle(coordinates[0], coordinates[1],
									coordinates[0]+10, coordinates[1]+10);
		for(int i=0; i<2; i++)
			if((position>0&&position<10) || (position>20&&position<30))
				coordinates[i] = place[position].coordinates[i] +
												 place[position].playerOffset[i] + offset[i];
			else
				coordinates[i] = place[position].coordinates[i] +
												 place[position].playerOffset[i] + offset[i+2];
		fillRectangle(coordinates[0], coordinates[1], coordinates[0]+10,
									coordinates[1]+10, color);
		if(this->position>position) //1 round complete.
			landOnGO();
		this->position = position;
		action();
	}

	void action(){
		if(place[position].price!=0)
			if(place[position].owner==-1){
				int status = buyPlace(position);
			}
			else{
				int status = payRent(place[position].owner);
			}
		else{
			if(position==4)  //Income Tax
				pay(200);
			else if(position==38)  //Super Tax
				pay(100);
			else if(position==2 || position==17 || position==33) //Community Chest
				communityChest();
			else if(position==7 || position==22 || position==36) //Chance
				chance();
			else if(position==30) //Jail
				sendToJail();
		}
		displayPlayerDetails();
	}

	int buyPlace(int position){
		print("This property is unsold.");
		print("Would you like to buy it?");
		place[position].displayDetails();
		sprintf(message, "Current balance is %dM.", balance);
		print(message);
		sprintf(message, "New balance if purchased = %dM", balance-place[position].price);
		print(message);
		int decision;
		print("1. Yup, go ahead.");
		print("2. Nope, put this one on auction.");
		do{
			decision = getch() - '0';
			if(decision!=1 && decision!=2)
				print("Try again.");
		}while(decision!=1 && decision!=2);
		if(decision == 1){
			int status = pay(place[position].price);
			if(status == 0){
				print("Insufficient funds.", 1);
				print("Do you want to trade or mortgage?");
				print("1. Yes, 0. No");
				int sellStuff = getch() - '0';
				if(sellStuff==1)
					//sell();
				return 0;
			}
			else{
				print("Purchase Successful.");
				drawRectangle(place[position].coordinates[0], place[position].coordinates[1],
											place[position].coordinates[2], place[position].coordinates[3],
											color, 2);
				place[position].owner = this->id;
				getch();
				print(" ", 1);
				return 1;
			}
		}
		else{
			//place[position].auction();
			return 2;
		}
	}

	int payRent(int ownerID){
		int currentRent = place[position].rent[place[position].upgrades];
		int status = transfer(ownerID, currentRent);
		if(status==1){
			getch();
			return 0;
		}
		else{
			print("Not enough balance to pay.");
			return 1;
		}
	}

	int transfer(int playerID, int amount){
		int status = pay(amount);
		if(status == 1)
			addBalance(playerID, amount);
		return status;
	}

	int pay(int amount, int mandatory=0){
		int newBalance = balance - amount;
		if(newBalance<0){
			sprintf(message, "Insufficient funds. Need %d more",
							absoluteValue(newBalance));
			print(message);
			if(mandatory==1){
				int successful = 0; //sell();
				if(successful)
					return 1;
				else{
					print("You are bankrupt.");
					retire();
				}
			}
			return 0;
		}
		else{
			balance = newBalance;
			sprintf(message, "%dM deducted. ", amount);
			print(message);
			displayPlayerDetails();
			return 1;
		}
	}

	void sendToJail(){
		skipTurns = 3;
		movePlayer(10);
	}

	void landOnGO(){
		print("Round completed. You got 200M salary!");
		addBalance(id, 200);
		displayPlayerDetails();
		getch();
	}

	void communityChest(){
		int luck = generateRandomNumber(0, 1);
		if(luck==0){
			print("You lost 100M :(");
			pay(100, 1);
		}
		else{
			print("Congratulations!");
			print("You won 100M! :D");
			addBalance(id, 100);
		}
		getch();
	}

	void chance(){
		int luck = generateRandomNumber(0, 1);
		if(luck==0){
			print("You lost 50M :(");
			pay(50, 1);
		}
		else{
			print("Congratulations!");
			print("You won 50M! :D");
			addBalance(id, 50);
		}
		getch();
	}

	void retire(){
		isInGame = 0;
		fillRectangle(coordinates[0], coordinates[1],
									coordinates[0]+10, coordinates[1]+10);
	}
}player[6];
int Player::numberOfPlayers = 0;

void addBalance(int playerID, int amount){
	player[playerID].balance+=amount;
	sprintf(message, "%s got %d", player[playerID].name, amount);
	print(message);
	displayPlayerDetails();
}

void displayPlayerDetails(){
	int numberOfLines = 0;
	settextstyle(0, 0, 1);
	fillRectangle(462, 8, getmaxx(), getmaxy());
	outtextxy(465, 10+10*numberOfLines, "Balance:");
	numberOfLines++;
	for(int i=0; i<Player::numberOfPlayers; i++){
		if(player[i].isInGame!=0){
			sprintf(message, "%s -> %d", player[i].name, player[i].balance);
			outtextxy(465, 10+10*numberOfLines, message);
			numberOfLines++;
		}
	}
}

void main(){
	int graphicsDriver = DETECT, graphicsMode;
	clrscr();
	initialMenu();
	initgraph(&graphicsDriver, &graphicsMode, "C:\\TURBOC3\\BGI");
	time_t t;
	srand((unsigned) time(&t));  //initialising random number generator
	fetchData();
	drawBoard();
	playGame();
	getch();
	closegraph();
}

void initialMenu(){
	printf("Welcome to shitty version of Monopoly.\n");
	printf("Enter number of players: ");
	scanf("%d", &Player::numberOfPlayers);
	for(int i=0; i<Player::numberOfPlayers; i++){
		player[i].id = i;
		printf("\nPlayer %d:\n", i+1);
		printf("\tName: ");
		scanf("%s", player[i].name);
		printf("\tColor: ");
		scanf("%d", &player[i].color);
		player[i].position = 0;
	}

	//Offset Coordinates for top and bottom:
	player[1].offset[1] = 12;
	player[2].offset[0] = 12;
	player[3].offset[0] = 12;	player[3].offset[1] = 12;
	player[4].offset[1] = 24;
	player[5].offset[0] = 24; player[5].offset[1] = 12;

	//Offset Coordinates for left and right:
	player[1].offset[2] = 12;
	player[2].offset[3] = 12;
	player[3].offset[2] = 12; player[3].offset[3] = 12;
	player[4].offset[3] = 24;
	player[5].offset[2] = 12; player[5].offset[3] = 24;
}

void fetchData(){
	ifstream readFile;
	readFile.open("C:\\TURBOC3\\BIN\\Monopoly\\Final\\Places.txt");

	//Names
	for(int i=0; i<40; i++)
		readFile.getline(place[i].name, 25, ',');
	//cout<<place[3].name;

	//Prices and positions
	char temp[25];
	for(i=0; i<40; i++){
		readFile.getline(temp, 10, ',');
		place[i].price = atoi(temp); //atoi converts String to int
		place[i].position = i;
	}
	//cout<<place[39].price;

	//Color
	for(i=0; i<40; i++){
		readFile.getline(temp, 10, ',');
		place[i].color = atoi(temp);
	}
	//cout<<place[39].color;

	//Rents
	for(int j=0; j<6; j++) //6 types of rents(basic, 4 houses, Hotel)
		for(i=0; i<40; i++){
			if(place[i].color!=-1){
				readFile.getline(temp, 10, ',');
				place[i].rent[j] = atoi(temp);
			}
			else if(i%5==0 && i%10!=0){ //Stations
				if(j==0)
					place[i].rent[j] = 25; //To display
				else if(j==1)
					place[i].rent[j] = 25; //1 station owned
				else if(j==2)
					place[i].rent[j] = 50; //2 stations owned
				else if(j==3)
					place[i].rent[j] = 100;//3 stations owned
				else if(j==4)
					place[i].rent[j] = 200;//4 stations owned
			}
			else if(i==12 || i==28){ //Utilities
				if(j==0)
					place[i].rent[j] = 4;
				if(j==1)
					place[i].rent[j] = 4;
				if(j==2)
					place[i].rent[j] = 10;
			}
		}

	//Upgrade Cost
	for(i=0; i<40; i++){
		if(i>0 && i<10)
			place[i].upgradeCost = 50;
		else if(i>10 && i<20)
			place[i].upgradeCost = 100;
		else if(i>20 && i<30)
			place[i].upgradeCost = 150;
		else
			place[i].upgradeCost = 200;
	}

	//Coordinate data:
		float variance=0;
		int x1, y1, x2, y2;

		//Bottom:
		y1 = 400; y2 = 460;
		for(i=1; i<10; i++){  //Old Kent to Pentonville
			x2 = 400 - round(variance);
			variance+=36.66;
			x1 = 400 - round(variance);
			place[i].coordinates[0]=x1; place[i].coordinates[1]=y1;
			place[i].coordinates[2]=x2; place[i].coordinates[3]=y2;
			place[i].colorOffset[3] = -50;
			place[i].nameOffset[0] = 2;	place[i].nameOffset[1] = 12;
			place[i].nameOffset[2] = 2;	place[i].nameOffset[3] = 19;
			place[i].priceOffset[0] = 12;	place[i].priceOffset[1] = 51;
			place[i].playerOffset[0] = 1;	place[i].playerOffset[1] = 27;
		}

		//Left:
		x1 = 10; x2 = 70; variance = 0;
		for(i=11; i<20; i++){
			y2 = 400 - round(variance);
			variance+=36.66;
			y1 = 400 - round(variance);
			place[i].coordinates[0]=x1; place[i].coordinates[1]=y1;
			place[i].coordinates[2]=x2; place[i].coordinates[3]=y2;
			place[i].colorOffset[0] = 50;
			place[i].nameOffset[0] = 36;	place[i].nameOffset[1] = 2;
			place[i].nameOffset[2] = 43;	place[i].nameOffset[3] = 2;
			place[i].priceOffset[0] = 2;	place[i].priceOffset[1] = 12;
			place[i].playerOffset[0] = 12;	place[i].playerOffset[1] = 1;
		}

		//Top:
		y1 = 10; y2 = 70; variance = 0;
		for(i=21; i<30; i++){
			x1 = 70 + round(variance);
			variance+=36.66;
			x2 = 70 + round(variance);
			place[i].coordinates[0]=x1; place[i].coordinates[1]=y1;
			place[i].coordinates[2]=x2; place[i].coordinates[3]=y2;
			place[i].colorOffset[1] = 50;
			place[i].nameOffset[0] = 2;	place[i].nameOffset[1] = 36;
			place[i].nameOffset[2] = 2;	place[i].nameOffset[3] = 43;
			place[i].priceOffset[0] = 12;	place[i].priceOffset[1] = 2;
			place[i].playerOffset[0] = 1;	place[i].playerOffset[1] = 12;
		}

		//Right:
		x1 = 400; x2 = 460; variance = 0;
		for(i=31; i<40; i++){
			y1 = 70 + round(variance);
			variance+=36.66;
			y2 = 70 + round(variance);
			place[i].coordinates[0]=x1; place[i].coordinates[1]=y1;
			place[i].coordinates[2]=x2; place[i].coordinates[3]=y2;
			place[i].colorOffset[2] = -50;
			place[i].nameOffset[0] = 12;	place[i].nameOffset[1] = 2;
			place[i].nameOffset[2] = 19;	place[i].nameOffset[3] = 2;
			place[i].priceOffset[0] = 51;	place[i].priceOffset[1] = 12;
			place[i].playerOffset[0] = 27;	place[i].playerOffset[1] = 1;
		}

		//GO:
		place[0].coordinates[0] = 400; place[0].coordinates[1] = 400;
		place[0].coordinates[2] = 460; place[0].coordinates[3] = 460;

		//Jail:
		place[10].coordinates[0] = 10; place[10].coordinates[1] = 400;
		place[10].coordinates[2] = 70; place[10].coordinates[3] = 460;

		//Muft Mein Parking:
		place[20].coordinates[0] = 10; place[20].coordinates[1] = 10;
		place[20].coordinates[2] = 70; place[20].coordinates[3] = 70;

		//Go to Jail:
		place[30].coordinates[0] = 400; place[30].coordinates[1] = 10;
		place[30].coordinates[2] = 460; place[30].coordinates[3] = 70;

		for(i=0; i<4; i++){ //Setting offsets for corners
			place[i*10].playerOffset[0] = 2;
			place[i*10].playerOffset[1] = 2;
		}
}

void drawBoard(){
	drawRectangle(10, 10, 460, 460); //Outer Rectangle
	drawRectangle(70, 70, 400, 400); //Inner Rectangle

	for(float f=70; f<=400; f+=36.66){ // Lines for Properties
		drawLine(round(f), 10, round(f), 70); //Top
		drawLine(400, round(f), 460, round(f)); //Right
		drawLine(round(f), 400, round(f), 460); //Bottom
		drawLine(10, round(f), 70, round(f)); //Left
	}

	//Players:
	for(int i=0; i<Player::numberOfPlayers; i++){
		if(i==0){
			player[i].coordinates[0] = 410; player[i].coordinates[1] = 410;
			player[i].coordinates[2] = 420; player[i].coordinates[3] = 420;
		}
		else if(i<3){
			player[i].coordinates[0] = 410;
			player[i].coordinates[1] = player[i-1].coordinates[1]+12;
		}
		else if(i==3){
			player[i].coordinates[0] = 422; player[i].coordinates[1] = 410;
		}
		else{
			player[i].coordinates[0] = 422;
			player[i].coordinates[1] = player[i-1].coordinates[1]+12;
		}
		fillRectangle(player[i].coordinates[0], player[i].coordinates[1],
									player[i].coordinates[0]+10, player[i].coordinates[1]+10,
									player[i].color);
	}

	//Jail:
	fillRectangle(37, 400, 70, 440, WHITE);
	for(i=37; i<70; i+=10)
		fillRectangle(i, 400, i+2, 440, BLACK); //bars

	//Filling Details:
	for(i=1; i<40; i++)
		if(i%10!=0){ //All places except corners

			//Color:
			if(place[i].color!=-1){
				fillRectangle(place[i].coordinates[0]+place[i].colorOffset[0],
											place[i].coordinates[1]+place[i].colorOffset[1],
											place[i].coordinates[2]+place[i].colorOffset[2],
											place[i].coordinates[3]+place[i].colorOffset[3],
											place[i].color);
			}

			//Names:
			if(i<10 || (i>20 && i<30)) //Top and bottom places
				settextstyle(2, 0, 2); // 2-Font:Small_Font, 0-Direction:Horizontal, 2-charSize
			else
				settextstyle(2, 1, 2); //Vertical Text
			int lastSpace = searchInString(place[i].name, ' ');
			if(lastSpace == -1)
				outtextxy(place[i].coordinates[0]+place[i].nameOffset[0],
									place[i].coordinates[1]+place[i].nameOffset[1],
									place[i].name);
			else{
				char namePart1[9], namePart2[9];
				if(lastSpace<9){
					substring(place[i].name, namePart1, 0, lastSpace);
					substring(place[i].name, namePart2, lastSpace+1, 18);
				}
				else{
					substring(place[i].name, namePart1, 0, 8);
					substring(place[i].name, namePart2, 8, 16);
				}
				outtextxy(place[i].coordinates[0]+place[i].nameOffset[0],
									place[i].coordinates[1]+place[i].nameOffset[1],
									namePart1);
				outtextxy(place[i].coordinates[0]+place[i].nameOffset[2],
									place[i].coordinates[1]+place[i].nameOffset[3],
									namePart2);
			}

			//Prices:
			char price[3];
			itoa(place[i].price, price, 10);
			if(place[i].price!=0)
				outtextxy(place[i].coordinates[0] + place[i].priceOffset[0],
									place[i].coordinates[1] + place[i].priceOffset[1], price);
	}
	settextstyle(0, 0, 1);
	outtextxy(420, 450, "<-GO");
	outtextxy(30, 450, "Jail");
	settextstyle(2, 0, 2);
	outtextxy(15, 60, "Muft Parking");
	outtextxy(410, 60, "Go to Jail");

	//"Monopoly" Text
	fillRectangle(105, 72, 366, 107, RED);
	settextstyle(0, 0, 4); //0- Default Font, 0-Horizontal, 4-Size
	outtextxy(108, 75, "MONOPOLY");

	displayPlayerDetails();
}

void playGame(){
	int currentPlayer = 0;
	inGamePlayers=Player::numberOfPlayers;
	player[0].skipTurns = 0;
	while(inGamePlayers>1){
		if(player[currentPlayer].isInGame!=0){
			if(player[currentPlayer].skipTurns!=0){
				sprintf(message, "You are in jail, %s", player[currentPlayer].name);
				print(message);
				print("This turn was skipped.");
				player[currentPlayer].skipTurns--;
				sprintf(message, "Skips remaining: %d",player[currentPlayer].skipTurns);
				print(message);
			}
			else{
				sprintf(message, "Press any key to roll dice, player %d", currentPlayer+1);
				print(message);
				getch();
				print("", 1);
				sprintf(message, "%s:", player[currentPlayer].name);
				print(message, 1);
				player[currentPlayer].rollDice();
				displayPlayOptions();
				int choice = getch() - '0';
				switch(choice){
					case 1:
						break;
					case 0:
						inGamePlayers--;
						player[currentPlayer].retire();
						break;
				}
			}
		}
		currentPlayer = (currentPlayer + 1) % Player::numberOfPlayers;
	}
	print(" ", 1);
	settextstyle(0, 0, 4);
	for(int i=0; i<Player::numberOfPlayers; i++)
		if(player[i].isInGame){
			outtextxy(105, 120, "Congrats");
			sprintf(message, "%s!", player[i].name);
			outtextxy(105, 160, message);
			outtextxy(105, 200, "You Win!");
		}
}

void displayPlayOptions(){
	print("1. End Turn");
	//print("2. Trade");
	//print("3. Mortgage");
	//print("4. Upgrade Properties");
	print("0. Retire");
}

void print(char text[], int reset){
	settextstyle(0, 0, 1);
	if(reset==1){
		printedItems = 0;
		fillRectangle(71, 110, 399, 399);
	}
	outtextxy(105, 112 + 10*printedItems, text);
	printedItems++;
	if(printedItems>25){
		print(" ", 1);
		printedItems--;
	}
}

void drawLine(int x1, int y1, int x2, int y2, int color, int delayed){
	int dx, dy, d, s1, s2, x, y, isSwapped, i, temp;

	dx = absoluteValue(x2-x1);
	dy = absoluteValue(y2-y1);
	s1 = ((x2-x1)>0) ? 1 : -1;
	s2 = ((y2-y1)>0) ? 1 : -1;
	isSwapped = 0;

	if(dy>dx){
		temp = dx;
		dx = dy;
		dy = temp;
		isSwapped = 1;
	}

	d = 2*dy - dx;
	x = x1;
	y = y1;

	for(i=0; i<dx; i++){
		putpixel(x, y, color);
		//printf("%d %d\n", x, y);
		while(d>=0){
			if(isSwapped==1)
				x+=s1;
			else{
				y+=s2;
				d = d - 2*dx;
			}
		}
		if(isSwapped==1)
			y+=s2;
		else{
			x+=s1;
			d = d + 2*dy;
		}
		if(delayed!=0)
			delay(delayed);
	}
}

int absoluteValue(int value){
	if(value<0)
		return -value;
	else
		return value;
}

void drawRectangle(int x1, int y1, int x2, int y2, int color, int delayed){
	drawLine(x1, y1, x2, y1, color, delayed);
	drawLine(x2, y1, x2, y2, color, delayed);
	drawLine(x2, y2, x1, y2, color, delayed);
	drawLine(x1, y2, x1, y1, color, delayed);
}

void fillRectangle(int x1, int y1, int x2, int y2, int color){
	for(int i=x1+1; i<x2; i++)
		for(int j=y1+1; j<y2; j++)
			putpixel(i, j, color);
}

int round(float number){
	if(number - (int)number < 0.5)
		return (int) number;
	else
		return (int) number + 1;
}

int searchInString(char string[], char character){
	int length = strlen(string), found=-1;
	for(int i=0; i<length; i++)
		if(string[i]==character)
			found = i;
	return found;
}

void substring(char string[], char newString[], int start, int end){
	for(int i=0; i<9; i++){
		if(i<end-start)
			newString[i] = string[i+start];
		else
			newString[i] = '\0';
	}
}

int generateRandomNumber(int start, int end){
	return (rand() % (end-start+1)) + start;
}
