#include "trainer.hpp"


using namespace std;

void exit_with_error(char* err, HANDLE* hProcess)
{
	cout << "[!] " << err << endl;
	CloseHandle(hProcess);
	cin.get();
	exit(1);
}

CHAR getch()
{
	DWORD mode, cc;
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

	if (h == NULL) return 0;

	GetConsoleMode(h, &mode);
	SetConsoleMode(h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
	TCHAR c = 0;
	ReadConsole(h, &c, 1, &cc, NULL);
	SetConsoleMode(h, mode);

	return c;
}

#pragma comment(lib, "user32.lib")
int main()
{
	HWND dWin = FindWindow(0, _T(winStr));
	HANDLE hProcess;
	DWORD pid;
	int canRead;
	int canWrite;

	cout << ">> Searching for " << winStr << " window\n";
	if (!dWin)
		exit_with_error(ERROR_NOT_OPEN, &hProcess);

	GetWindowThreadProcessId(dWin, &pid);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess)
		exit_with_error(ERROR_PROCESS, &hProcess);

	DWORD BaseAddr;
	DWORD mana;
	DWORD gold;
	DWORD health;
	TCHAR key;
	unsigned int addGold = 999999;
	unsigned int addMana = 100;
	unsigned int addHealth = 100;

	canRead = ReadProcessMemory(hProcess, (void*)BASEADDR, &BaseAddr, 4, NULL);
	if (!canRead)
		exit_with_error(ERROR_MEMORY_READ, &hProcess);

	canRead = ReadProcessMemory(hProcess, (void*)(BaseAddr + OFFSET_GOLD), &gold, sizeof(gold), NULL);
	if (!canRead) exit_with_error(ERROR_MEMORY_READ, &hProcess);
	cout << "You have " << gold << " gold\n";

	while (1) {
		canRead = ReadProcessMemory(hProcess, (void*)(BaseAddr + OFFSET_MANA), &mana, sizeof(mana), NULL);
		if (!canRead)
			exit_with_error(ERROR_MEMORY_READ, &hProcess);

		if (mana < 100) {
			canWrite = WriteProcessMemory(hProcess, (void*)(BaseAddr + OFFSET_MANA), &addMana, sizeof(addMana), NULL);
			if (!canWrite)
				exit_with_error(ERROR_MEMORY_WRITE, &hProcess);
		}

		canRead = ReadProcessMemory(hProcess, (void*)(BaseAddr + OFFSET_HEALTH), &health, sizeof(health), NULL);
		if (!canRead) exit_with_error(ERROR_MEMORY_READ, &hProcess);

		if (health < 100) {
			canWrite = WriteProcessMemory(hProcess, (void*)(BaseAddr + OFFSET_HEALTH), &addHealth, sizeof(addHealth), NULL);
			if (!canWrite) exit_with_error(ERROR_MEMORY_WRITE, &hProcess);
		}

		Sleep(500);
	}

	CloseHandle(hProcess);
	return 0;
}