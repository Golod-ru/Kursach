
// так как мы делаем "сервер" будет использовать сокеты. Для сокетов нужно подключить кучу всего что ниже. БУДУ ПОДРОБНО ПИСАТЬ, потому что дурак на Кирилле
// может все забыть как всегда. 
#pragma comment(lib,"ws2_32.lib")//короче, оно ищет библиотеку, за это отвечает lib.
#include <WinSock2.h>//Библиотека для работы с сетями. есть две версии (заголовочный файл, содержащий актуальные реализации функций для работы с сокетами.)
#include <iostream>
#include <stdio.h>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma warning(disable: 4996)

using namespace std;

SOCKET Connections[2];
int Counter = 0;

enum Packet
{
	P_ChatMessage,
	P_fire,
	P_hit,
	P_victory
};

bool ProcessPacket(int index, Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		setlocale(LC_ALL, "");
		int msg_size;
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connections[index], msg, msg_size, NULL);
		for (int i = 0; i < Counter; i++)
		{
			if (i == index)
			{
				continue;
			}

			Packet msgtype = P_ChatMessage;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
			send(Connections[i], msg, msg_size, NULL);
		}
		delete[] msg;
		break;
	}
	case P_fire:
	{
		int x;
		int y;
		recv(Connections[index], (char*)&x, sizeof(int), NULL);
		recv(Connections[index], (char*)&y, sizeof(int), NULL);
		cout << "Игрок " << index << "Выстрелил по координатам " << x << " " << y << endl;
		for (int i = 0; i < Counter; i++)
		{
			if (i == index)
			{
				continue;
			}
			Packet msgtype = P_fire;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&x, sizeof(int), NULL);
			send(Connections[i], (char*)&y, sizeof(int), NULL);
		}
		break;
	}
	case P_hit:
	{
		int x;
		int y;
		recv(Connections[index], (char*)&x, sizeof(int), NULL);
		recv(Connections[index], (char*)&y, sizeof(int), NULL);
		for (int i = 0; i < Counter; i++)
		{
			if (i == index)
			{
				continue;
			}
			Packet msgtype = P_hit;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&x, sizeof(int), NULL);
			send(Connections[i], (char*)&y, sizeof(int), NULL);
		}
		break;
	}
	case P_victory:
	{
		for (int i = 0; i < Counter; i++)
		{
			if (i==index)
			{
				continue;
			}
			Packet msgtype = P_victory;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
		}
		cout << "на этом мои полномочья все, закончились" << endl;
		cout << "выключусь, если вы нажмете на любую кнопку" << endl;
		exit(0);
		break;
	}
	default:
		cout << "Unrecognized packet: " << packettype << endl;
		break;
	}
	return true;
}

void ClientHandler(int index) {
	Packet packettype;
	while (true)
	{
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
		if (!ProcessPacket(index,packettype))
		{
			break;
		}
	}
	closesocket(Connections[index]);
}



int main(int argc,char* arhv[])
{
	setlocale(LC_ALL, "");
	WSAData wsaData;
	WORD DLLVers = MAKEWORD(2, 1); //получаем версию для библиотеки ВиндоусНосок(WinSock)
	if (WSAStartup(DLLVers, &wsaData) != 0)
	{
		cout << "Ошибка1 - версия WinSock не получена" << endl;
		exit(1);
	} //ну тут вроде и так понятно
	//заполняет информацию об адресе сокита
	SOCKADDR_IN addr; //SOCKADDR хранит адресса. Для интеренет протоколов с приставкой _IN
	int sizeofaddr = sizeof(addr);
	string ip_adress;
	cout << "Введите свой IP4(В командной строке введите ipconfig), но если вы ведете !, то IP адрес будет по умолчанию" << endl;
	cin >> ip_adress;
	if (ip_adress == "!")
	{
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(ip_adress.c_str());
	}
	addr.sin_port = htons(1111);//port
	addr.sin_family = AF_INET;//семейство протоколов

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //создаем сокет
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));//привязвывем сокет к адрессу.
	listen(sListen, SOMAXCONN);//прослушивем(лучше почитать об этом)
	
	SOCKET newConnection;
	for (int i = 0; i < 2; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0)
		{
			cout << "Ошибка2 - соединение с клиентом не получено" << endl;
		}
		else
		{
			cout << "Соединение с клиентом прошло успешно" << endl;
			string msg = "Подкление с серверу прошло успешно";
			int msg_size = msg.length();
			Packet msgtype = P_ChatMessage;
			send(newConnection, (char*)&msgtype, sizeof(Packet), NULL);
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msg_size, NULL);


			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);//созадется второй поток, в котром будет выполн. функция чат, с переданным параметром i
		}
	}
	
	system("pause");
	return 0;
}