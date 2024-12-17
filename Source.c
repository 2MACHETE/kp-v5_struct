#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
typedef struct {
	int ts;
	float mp;
	int minp;
} params;
double indicator(params params1);
void write_to_file(int K, double ts, double mp, double minp);
int manual_input(int minp, int* err) {
	while (1) {
		int K, ts, a; float mp; char pokaz[50];
		printf("Введите показатели (K_5#2,0mp#150ts): ");
		scanf("%18s", pokaz);
		int count = sscanf(pokaz, "%*[^0-9]%d%*[^0-9]%f%*[^0-9]%d", &K, &mp, &ts);
		if (count != 3) ++*err;
		printf("k = %d, mp = %f, ts = %d\n", K, mp, ts);
		params params1 = { ts, mp, minp };
		printf("Котёл %d: отклонение %.1lf процентов\n", K, indicator(params1));
		write_to_file(K, ts, mp, minp);
		puts("Ввести новые показатели? 1 - да, 2 - нет (меню)\n");
		scanf("%d", &a);
		if (a == 2) break;
	}
}
int monitoring(int minp, int* err)
{
	int K, ts, a; float mp, ps; char pokaz[50];
	FILE* file = fopen("data.txt", "r");
	if (file == NULL) {
		printf("error\n");
		return 1;
	}
	while (fgets(pokaz, sizeof(pokaz), file)) {
		int count = sscanf(pokaz, "%*[^0-9]%d%*[^0-9]%f%*[^0-9]%d", &K, &mp, &ts);
		params params1 = { ts, mp, minp };
		if (count == 3) {
			double tsf = (ts + 273.15) / 647.14;
			printf("Котёл %d: отклонение %.1lf процентов\n", K, indicator(params1));
		}
		else {
			printf("Ошибка в строке %s\n", pokaz);
			++*err;
		}
		write_to_file(K, ts, mp, minp);
	}
	fclose(file);
}
int chuv(int* minp) {
	printf("Введите чувствительность (100 - стандарт): ");
	scanf("%d", minp);
}
int put_error(int err) {
	printf("Количество ошибок = %d\n", err);
}
double indicator(params params1) {
	double tsf = (params1.ts + 273.15) / 647.14;
	double x = 1 - tsf;
	double drob = (-7.85823 + 1.83991 * pow(x, 1.5) - 11.7811 * pow(x, 3) + 22.6705 * pow(x, 3.5) - 15.9393 * pow(x, 4) + 1.77516 * pow(x, 7.5)) / params1.ts;
	double ps = 22.064 * exp(drob);
	double razl = fabs(ps - params1.ts) / fabs(params1.mp) * 100 * (params1.minp / 100.0);
	return razl;
}
void generate_data() {  // Функция случайной генерации параметров в файле data.txt
	FILE* file = fopen("data.txt", "w");
	if (file == NULL) {
		printf("error\n");
		return;
	}
	srand(time(NULL));  // Инициализация генератора сл. чисел
	setlocale(LC_ALL, "English");  // Переход обратно на Eng локаль, чтобы mp не записывались с запятой
	for (int i = 1; i <= 50; i++) { // Цикл for на 50 строк
		float mp = (rand() % 20001) / 1000.0f + 1.0f;  // Определение mp как сл. число от 1.0 до 20.0
		int ts = rand() % 651 + 50;  // Определение ts как сл. число от 50 до 700
		fprintf(file, "K_%d#%.1fmp#%dts\n", i, mp, ts);  // Запись значений в строку
	}
	fclose(file);
	setlocale(LC_ALL, "Russian");  // Обратный переход на Rus локаль
	printf("Данные сгенерированы в data.txt\n");
}
void write_to_file(int K, double ts, double mp, double minp) {
	FILE* file = fopen("result.txt", "a");
	if (file == NULL) {
		printf("error\n");
		return;
	}
	params params1 = { ts, mp, minp };
	fprintf(file, "Котёл %d: отклонение %.1lf процентов\n", K, indicator(params1));
	fclose(file);
}
void clear_result_file() {
	FILE* file = fopen("result.txt", "w");
	if (file == NULL) {
		printf("error\n");
		return;
	}
	fclose(file);
	printf("Содержимое очищено\n");
}
void main() {
	int minp; int err = 0; int a;
	setlocale(LC_ALL, "Russian");
	printf("[$ Программа отслеживания параметров тепловых котлов mk5 $]\n\t\tДобро пожаловать!\nДля начала введите чувствительность (100 - стандарт, 10 - малая и т.д.): ");
	scanf("%d", &minp);
	while (1) {
		printf("1) Ручной ввод\n2) Режим мониторинга\n3) Модификация параметров\n4) Вывести количество ошибок\n5) Генерация случайных значений\n6) Очистить файл результатов\n7) Выход\n");
		scanf("%d", &a);
		switch (a) {
		case 1:
			manual_input(minp, &err);
			break;
		case 2:
			monitoring(minp, &err);
			break;
		case 3:
			chuv(&minp);
			break;
		case 4:
			put_error(err);
			break;
		case 5:
			generate_data();
			break;
		case 6:
			clear_result_file();
			break;
		case 7:
			printf("mk5 is shutting down... goodbye!");
			exit(1);
		default:
			printf("error\n");
			break;
		}
	}
}