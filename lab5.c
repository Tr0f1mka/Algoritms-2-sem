#define _CRT_SECURE_NO_WARNINGS

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct a {
	int start, finish, length;
} Line;


typedef struct b {
	int* points;
	Line* lines;
	int len_points, len_lines;
} Graf;


void clearGraf(Graf* graf) {
	if (graf == NULL) return;
	free(graf->points);
	free(graf->lines);
	free(graf);
}


int FindIndElem(Graf* graf, int key, int cur_size) {

	if (graf == NULL) {
		return -1;
	}

	int left = 0,
		right = cur_size - 1,
		mid;

	while (left <= right) {
		mid = left + ((right - left) >> 1);
		if (key < graf->points[mid]) {
			right = mid - 1;
		}
		else {
			left = mid + 1;
		}
	}

	return left;
}


int BinaryInsertionSorting(Graf* graf) {
	if (graf == NULL) {
		return 1;
	}

	for (int i = 1; i < graf->len_points; i++) {
		int ind = FindIndElem(graf, graf->points[i], i);
		if (ind == -1) {
			return 1;
		}
		if (ind == i) {
			continue;
		}
		int moving = graf->points[i];
		for (int j = i; j > ind; j--) {
			graf->points[j] = graf->points[j - 1];
		}
		graf->points[ind] = moving;
	}

	return 0;
}


int keyExists(Graf* graf, int key, int cur_size) {
	if (graf == NULL || cur_size == 0) return 0;

	int left = 0, right = cur_size - 1;
	while (left <= right) {
		int mid = left + (right - left) / 2;
		if (graf->points[mid] == key)
			return 1;
		else if (graf->points[mid] < key)
			left = mid + 1;
		else
			right = mid - 1;
	}
	return 0;
}


Graf* ReadGraf(char* filename) {
	Graf* graf = (Graf*)malloc(sizeof(Graf));
	if (graf == NULL) return NULL;
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		clearGraf(graf);
		return NULL;
	}
	int n;
	fscanf(f, "%d\n", &n);

	graf->lines = (Line*)malloc(sizeof(Line) * n);
	if (graf->lines == NULL) {
		clearGraf(graf);
		fclose(f);
		return NULL;
	}
	graf->points = (int*)malloc(sizeof(int) * n);
	if (graf->points == NULL) {
		clearGraf(graf);
		fclose(f);
		return NULL;
	}

	graf->len_lines = n;
	graf->len_points = 0;
	char line[60], *elem;
	int buff;
	for (int i = 0; i < n; i++) {
		fgets(line, 59, f);

		elem = strtok(line, "\t\n");
		buff = atoi(elem);
		if (!keyExists(graf, buff, graf->len_points)) {
			graf->points[graf->len_points] = buff;
			graf->len_points++;
		}
		graf->lines[i].start = buff;

		elem = strtok(NULL, "\t\n");
		buff = atoi(elem);
		if (!keyExists(graf, buff, graf->len_points)) {
			graf->points[graf->len_points] = buff;
			graf->len_points++;
		}
		graf->lines[i].finish = buff;

		elem = strtok(NULL, "\t\n");
		graf->lines[i].length = atoi(elem);
	}
	if (BinaryInsertionSorting(graf)) {
		clearGraf(graf);
		fclose(f);
		return NULL;
	}
	return graf;
}


void Clear2Matrix(unsigned int** matrix, int len) {
	if (matrix == NULL) return;
	for (int i = 0; i < len; i++) {
		free(matrix[i]);
	}
	free(matrix);
}


int findVertexIndex(Graf* graf, int vertex) {
	int left = 0, right = graf->len_points - 1;
	while (left <= right) {
		int mid = left + (right - left) / 2;
		if (graf->points[mid] == vertex)
			return mid;
		else if (graf->points[mid] < vertex)
			left = mid + 1;
		else
			right = mid - 1;
	}
	return -1;
}

unsigned int** AlgorithmFloydWarshall(Graf* graf) {
	if (graf == NULL) return NULL;
	int n = graf->len_points;
	const unsigned int INF = 1e9;

	unsigned int** answer = (unsigned int**)malloc(sizeof(unsigned int*) * n);
	if (!answer) return NULL;

	for (int i = 0; i < n; i++) {
		answer[i] = (unsigned int*)malloc(sizeof(unsigned int) * n);
		if (!answer[i]) {
			Clear2Matrix(answer, i);
			return NULL;
		}
		for (int j = 0; j < n; j++) {
			answer[i][j] = (i == j) ? 0 : INF;
		}
	}

	for (int i = 0; i < graf->len_lines; i++) {
		int u = findVertexIndex(graf, graf->lines[i].start);
		int v = findVertexIndex(graf, graf->lines[i].finish);
		if (u != -1 && v != -1) {
			unsigned int w = graf->lines[i].length;
			if (w < answer[u][v])
				answer[u][v] = w;
		}
	}

	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {
			if (answer[i][k] == INF) continue;
			for (int j = 0; j < n; j++) {
				if (answer[k][j] == INF) continue;
				unsigned int new_dist = answer[i][k] + answer[k][j];
				if (new_dist < answer[i][j])
					answer[i][j] = new_dist;
			}
		}
	}

	return answer;
}

void Print2Matrix(unsigned int** matrix, int len) {
	if (matrix == NULL) {
		printf("ERROR\n");
		return;
	}
	const unsigned int INF = 1e9;
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < len; j++) {
			if (matrix[i][j] == INF)
				printf("|     --     ");
			else
				printf("|%12u", matrix[i][j]);
		}
		printf("|\n");
	}
}


int main() {
	setlocale(LC_ALL, "");

	Graf* graf = ReadGraf("test.txt");
	if (graf) {
		unsigned int** answer = AlgorithmFloydWarshall(graf);
		Print2Matrix(answer, graf->len_points);
		Clear2Matrix(answer, graf->len_points);
		clearGraf(graf);
	}
	else {
		printf("ERROR\n");
	}

	return 0;
}