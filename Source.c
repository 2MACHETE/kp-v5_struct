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
		printf("������� ���������� (K_5#2,0mp#150ts): ");
		scanf("%18s", pokaz);
		int count = sscanf(pokaz, "%*[^0-9]%d%*[^0-9]%f%*[^0-9]%d", &K, &mp, &ts);
		if (count != 3) ++*err;
		printf("k = %d, mp = %f, ts = %d\n", K, mp, ts);
		params params1 = { ts, mp, minp };
		printf("���� %d: ���������� %.1lf ���������\n", K, indicator(params1));
		write_to_file(K, ts, mp, minp);
		puts("������ ����� ����������? 1 - ��, 2 - ��� (����)\n");
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
			printf("���� %d: ���������� %.1lf ���������\n", K, indicator(params1));
		}
		else {
			printf("������ � ������ %s\n", pokaz);
			++*err;
		}
		write_to_file(K, ts, mp, minp);
	}
	fclose(file);
}
int chuv(int* minp) {
	printf("������� ���������������� (100 - ��������): ");
	scanf("%d", minp);
}
int put_error(int err) {
	printf("���������� ������ = %d\n", err);
}
double indicator(params params1) {
	double tsf = (params1.ts + 273.15) / 647.14;
	double x = 1 - tsf;
	double drob = (-7.85823 + 1.83991 * pow(x, 1.5) - 11.7811 * pow(x, 3) + 22.6705 * pow(x, 3.5) - 15.9393 * pow(x, 4) + 1.77516 * pow(x, 7.5)) / params1.ts;
	double ps = 22.064 * exp(drob);
	double razl = fabs(ps - params1.ts) / fabs(params1.mp) * 100 * (params1.minp / 100.0);
	return razl;
}
void generate_data() {  // ������� ��������� ��������� ���������� � ����� data.txt
	FILE* file = fopen("data.txt", "w");
	if (file == NULL) {
		printf("error\n");
		return;
	}
	srand(time(NULL));  // ������������� ���������� ��. �����
	setlocale(LC_ALL, "English");  // ������� ������� �� Eng ������, ����� mp �� ������������ � �������
	for (int i = 1; i <= 50; i++) { // ���� for �� 50 �����
		float mp = (rand() % 20001) / 1000.0f + 1.0f;  // ����������� mp ��� ��. ����� �� 1.0 �� 20.0
		int ts = rand() % 651 + 50;  // ����������� ts ��� ��. ����� �� 50 �� 700
		fprintf(file, "K_%d#%.1fmp#%dts\n", i, mp, ts);  // ������ �������� � ������
	}
	fclose(file);
	setlocale(LC_ALL, "Russian");  // �������� ������� �� Rus ������
	printf("������ ������������� � data.txt\n");
}
void write_to_file(int K, double ts, double mp, double minp) {
	FILE* file = fopen("result.txt", "a");
	if (file == NULL) {
		printf("error\n");
		return;
	}
	params params1 = { ts, mp, minp };
	fprintf(file, "���� %d: ���������� %.1lf ���������\n", K, indicator(params1));
	fclose(file);
}
void clear_result_file() {
	FILE* file = fopen("result.txt", "w");
	if (file == NULL) {
		printf("error\n");
		return;
	}
	fclose(file);
	printf("���������� �������\n");
}
void main() {
	int minp; int err = 0; int a;
	setlocale(LC_ALL, "Russian");
	printf("[$ ��������� ������������ ���������� �������� ������ mk5 $]\n\t\t����� ����������!\n��� ������ ������� ���������������� (100 - ��������, 10 - ����� � �.�.): ");
	scanf("%d", &minp);
	while (1) {
		printf("1) ������ ����\n2) ����� �����������\n3) ����������� ����������\n4) ������� ���������� ������\n5) ��������� ��������� ��������\n6) �������� ���� �����������\n7) �����\n");
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