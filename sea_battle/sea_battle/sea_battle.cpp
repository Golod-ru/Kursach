
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

#pragma warning(disable: 4996)

bool is_fire = true;

bool kostil = true;

bool is_hit = false;

int how_many_ships;

const int N = 10;

int sea[N][N] = { 0 };

int enemy_sea[N][N] = { 0 };


SOCKET Connection;

HANDLE hConsole;

enum Packet
{
	P_ChatMessage,
	P_fire,
	P_hit,
	P_victory
};

enum Color
{
	blue = 1,
	green = 2,
	sky_blue = 3,
	red = 4,
	gold = 6,
	white = 7
};


void show()
{
	cout << endl;
	cout << "Справа поле, куда вы будете стрелять, оно же - вражеское поле. А поле слево - это ваше поле. На нем вы будете видеть куда враг стреляет" << endl;
	cout << "давай немного подскажу, что обозначают значки снизу" << endl;
	SetConsoleTextAttribute(hConsole, green);
	cout << 'X';
	SetConsoleTextAttribute(hConsole, white);
	cout<<" - это означает ваш корабль" << endl;
	SetConsoleTextAttribute(hConsole, red);
	cout << 'X';
	SetConsoleTextAttribute(hConsole, white);
	cout << " - это означает, что вы попали по кораблю" << endl;
	SetConsoleTextAttribute(hConsole, blue);
	cout << 'X';
	SetConsoleTextAttribute(hConsole, white);
	cout << " - это означает, что вы или ваш противник промахнулся" << endl;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N * 2; j++)
		{
			if (j < 10)
			{
				if (j == 0)
				{
					cout << "\t\t| ";
				}
				if (sea[i][j] == 1)
				{
					SetConsoleTextAttribute(hConsole, green);
					cout << 'X';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
				else if (sea[i][j] == 2)
				{
					SetConsoleTextAttribute(hConsole, red);
					cout << 'X';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
				else if (sea[i][j]==3)
				{
					SetConsoleTextAttribute(hConsole, blue);
					cout << 'X';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
				else
				{
					SetConsoleTextAttribute(hConsole, sky_blue);
					cout << 'O';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
			}
			else if (j == N)
			{
				if (enemy_sea[i][j - N] == 3)
				{
					cout << "\t|\t| ";
					SetConsoleTextAttribute(hConsole, blue);
					cout << 'X';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
				else if (enemy_sea[i][j - N] == 2)
				{
					cout << "\t|\t| ";
					SetConsoleTextAttribute(hConsole, red);
					cout << 'X';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
				else
				{
					cout << "\t|\t| ";
					SetConsoleTextAttribute(hConsole, sky_blue);
					cout << 'O';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
			}
			else
			{
				if (enemy_sea[i][j-N] == 3)
				{
					SetConsoleTextAttribute(hConsole, blue);
					cout << 'X';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
				else if (enemy_sea[i][j - N] == 2)
				{
					SetConsoleTextAttribute(hConsole, red);
					cout << 'X';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
				else
				{
					SetConsoleTextAttribute(hConsole, sky_blue);
					cout << 'O';
					SetConsoleTextAttribute(hConsole, white);
					cout << " | ";
				}
			}
		}

		cout << endl;
		for (int j = 0; j < N * 4; j++)
		{
			if (j == 0)
			{
				cout << "\t\t-";
			}
			cout << "-";
		}
		cout << "\t|\t";
		for (int j = 0; j < N * 4 + 1; j++)
		{
			cout << "-";
		}
		cout << endl;
	}
}

bool ProcessPacket(Packet packettype) {
	switch (packettype)
	{
	case P_ChatMessage:
	{
		setlocale(LC_ALL, "");
		int msg_size;
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);
		if (kostil)
		{
			cout << msg << endl;
			kostil = false;
		}
		else
		{
			cout << "Ваш опонент:" << msg << endl;
		}
		delete[] msg;
		break;
	}
	case P_fire:
	{
		int x;
		int y;
		recv(Connection, (char*)&x, sizeof(int), NULL);
		recv(Connection, (char*)&y, sizeof(int), NULL);
		is_fire = true;
		system("cls");
		cout << "Враг выстрелил по координатам: x-" << x << " y-" << y << endl;
		if (sea[y - 1][x - 1] == 1)
		{
			Packet msgtype = P_hit;
			send(Connection, (char*)&msgtype, sizeof(Packet), NULL);
			send(Connection, (char*)&x, sizeof(int), NULL);
			send(Connection, (char*)&y, sizeof(int), NULL);
			cout << "Противник попал, -1 корабль на вашей стороне" << endl;
			sea[y - 1][x - 1] = 2;
			--how_many_ships;

		}
		else
		{
			cout << "Противник промахнулся. Время дать ответный удар!" << endl;
			sea[y - 1][x - 1] = 3;
		}
		if (how_many_ships == 0)
		{
			system("cls");
			cout << "вы проиграли" << endl;
			Packet msgtype = P_victory;
			send(Connection, (char*)&msgtype, sizeof(Packet), NULL);
			system("pause");
			exit(0);
		}
		Sleep(200);
		show();
		cout << "У вас осталось " << how_many_ships << " кораблей\n";
		cout << "Что вы хотите сделать?\n";
		cout << "вы можете отправить координаты только 1 раз. Сообщения - сколько угодно(вдруг вы захотите победить морально)\n";
		cout << "1)отправить сообщение в чат\n";
		cout << "2)отправить координаты\n";
		break;
	}
	case P_hit:
	{
		int x;
		int y;
		recv(Connection, (char*)&x, sizeof(int), NULL);
		recv(Connection, (char*)&y, sizeof(int), NULL);
		enemy_sea[y - 1][x - 1] = 2;
		break;
	}
	case P_victory:
	{
		system("cls");
		SetConsoleTextAttribute(hConsole, gold);
		cout << "\t\t\tВы победили, спасибо что победили!" << endl;
		SetConsoleTextAttribute(hConsole, white);
		system("pause");
		exit(0);
		break;
	}
	default:
		cout << "Unrecognized packet: " << packettype << endl;
		break;
	}
	return true;
}

void ClientHandler()
{
	Packet packettype;
	while (true)
	{
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(packettype))
		{
			break;
		}
	}
	closesocket(Connection);
}


int dif()//норм. но можно и крсивно сделать
{
	int diff;
	SetConsoleTextAttribute(hConsole, green);
	cout << "Морской бой по сети" << endl;
	SetConsoleTextAttribute(hConsole, white);
	cout << "давай обсудим сложность игры:" << endl;
	cout << "давай скажем, что длинна каждого корабля будет такая: каждая лодка занимает всего одну клетку. ставить корабли рядом друг с другом можно" << endl;
	cout << "Также, есть три варината растоновки кораблей, они будут ниже расписаны. Игроки могут выбирать любой из этих режимов." << endl;
	cout << "1) 10 - норм" << endl;
	cout << "2) 7 - немного сложнее" << endl;
	cout << "3) 3 - кому повезет" << endl;
	cout << "что ты выберешь?" << endl;
	do
	{
		cin >> diff;
		if (diff < 1 || diff > 3)
		{
			cout << "ты выбрал не то" << endl;
		}
	} while (!(diff > 0 && diff < 4));
	return diff;
}

void logic_game(int ship)
{
	cout << "Теперь расставим твой флот на море" << endl;
	int x, y;
	bool a = true;
	for (int i = 0; i < ship; i++)
	{
		cout << "поставим твой " << i + 1 << " корабль" << endl;
		cout << "впиши два числа: 1ое-х, 2ое-у. А числа должны быть от 1 до 10" << endl;
		a = true;
		while (a)
		{
			cin >> x >> y;
			if ((x < 11 && x>0) && (y < 11 && y>0))
			{
				if ((sea[y - 1][x - 1] == 1))
				{
					cout << "клетка занята, возьми другую" << endl;
				}
				else
				{
					sea[y - 1][x - 1] = 1;
					a = false;
				}
			}
			else
			{
				cout << "Ты ввел координаты не правильно" << endl;
			}
		}
	}
}

void game()
{
	setlocale(LC_ALL, "");
	int difficult = dif();

	switch (difficult)
	{
	case 1:
		how_many_ships = 10;
		logic_game(10);
		break;
	case 2:
		how_many_ships = 7;
		logic_game(7);
		break;
	case 3:
		how_many_ships = 3;
		logic_game(3);
		break;
	default:
		cout << "Ты как смог сломать мою игры? error1" << endl;
	}
	
}

int main()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	setlocale(LC_ALL, "");
	WSAData wsaData;
	WORD DLLVers = MAKEWORD(2, 1);
	if (WSAStartup(DLLVers, &wsaData) != 0)
	{
		cout << "Error1" << endl;
		exit(1);
	}
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	string ip_adress;
	cout << "Введи IP сервера, если IP выбран по умолчанию, то напиши !" << endl;
	cin >> ip_adress;
	if (ip_adress == "!")
	{
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(ip_adress.c_str());
	}
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		cout << "Error: failed connect to server." << endl;
		return 1;
	}

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
	Sleep(20);

	string msgl;

	int answer;

	game();
	system("cls");
	show();

	while (true)
	{
		cout << "У вас осталось " << how_many_ships << " кораблей\n";
		cout << "Что вы хотите сделать?\n";
		cout << "вы можете отправить координаты только 1 раз. Сообщения - сколько угодно(вдруг вы захотите победить морально)\n";
		cout << "1)отправить сообщение в чат\n";
		cout << "2)отправить координаты\n";
		cin >> answer;
		cout << "\n";
		if (answer == 1)
		{
			cout << "you:";
			cin >> msgl;
			int msg_size = msgl.length();
			Packet msgtype = P_ChatMessage;
			send(Connection, (char*)&msgtype, sizeof(Packet), NULL);
			send(Connection, (char*)&msg_size, sizeof(int), NULL);
			send(Connection, msgl.c_str(), msg_size, NULL); //send может отправлять только char
			Sleep(10);
		}
		else if (answer == 2)
		{
			if (is_fire)
			{
				cout << "введите координаты цели.1-х,2-у" << endl;
				int x, y;
				cin >> x >> y;
				if ((x > 0 && x < 11) && (y > 0 && y < 11))//не от обратного 
				{
					enemy_sea[y - 1][x - 1] = 3;
					Packet msgtype = P_fire;
					send(Connection, (char*)&msgtype, sizeof(Packet), NULL);
					send(Connection, (char*)&x, sizeof(int), NULL);
					send(Connection, (char*)&y, sizeof(int), NULL);
					Sleep(300);
					is_fire = false;
					system("cls");
					show();
				}
				else
				{
					cout << "Ошибка в отправлении координат" << endl;
				}
			}
			else
			{
				cout << "Вы уже отправили координаты. Ждите ответную реакцию от второго игрока." << endl;
			}
		}
		else
		{
			cout << "ответ не правильный, введите ответ по новой" << endl;
		}
	}
	return 0;
}