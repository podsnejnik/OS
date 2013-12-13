#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <cmath>
#include <stddef.h>//для нулевого указателя
#include <pthread.h>
#include <unistd.h>

using namespace std;
struct Cluster;

struct File_access {
    string name; 
    unsigned int size;
    struct Cluster *cluster_point;
    bool busy;
};

struct Cluster{
    Cluster *next;
   	unsigned char data[512];// один кластер - 512б
  
};

File_access array_file_access[204800];
bool interested[204800][2];
int turns[204800];

void read_posm_nbyte(long m, long n, FILE *f, unsigned char* data){
	fseek(f, m, SEEK_SET);//SEEK_SET - начало файла, m - смещение
	fread(data, 1, n, f);
}

void enterRegion(int i_size, int threadId){
	int other = 1 - threadId;                    // Идентификатор второго потока
    interested[i_size][threadId] = true;                 // Индикатор интереса текущего потока
    turns[i_size] = other;                                // Флаг очереди исполнения
    while (turns[i_size] == other && interested[i_size][other]);
}
 
void leaveRegion(int i_size, int threadId){
    interested[i_size][threadId] = false;
}

void * read_size(void *threadId){
	for(int i_size = 0; i_size < (sizeof(array_file_access) / sizeof(File_access)); i_size++){
		int *local_id = (int *) threadId;
		enterRegion(i_size,*local_id);//передать массив в функцию		
		if(array_file_access[i_size].name != ""){	 
			cout<<"№  Потока: "<<*local_id<< "\n"; 
			cout<<"Размер файла: "<< array_file_access[i_size].size << "\n";
		}	
		leaveRegion(i_size, *local_id);
	}
}

int main(int argc, char* argv[]){ 			
	
	for (int i=0; i<204800; i++) {//объем памяти 100Мб, формула: 100*1024*1024/512 = 204800; инициализация
		File_access *file_access = new File_access; 
		file_access->cluster_point = new Cluster;		
		file_access->busy = 0;  
		array_file_access[i] = *file_access;//массив структур
	}
	
	string s;
	int count_cluster = 204800;
	while(1){
		cout<<">";
		getline(cin,s);
		if(s == "exit"){
			break;			
		}
		if(s ==  "cat >"){ //запись файла
			cout<<"Введите имя файла:\n";
			getline(cin, s);
			if(s.length() < 8){
				const char * s_char = s.c_str();
				FILE * f = fopen(s_char,"rb");
				if(f != NULL){
					fseek(f, 0, SEEK_END);
					long size = ftell(f);
					fseek(f, 0, SEEK_SET);

					int whole = size/512;
					float part = ((int)size)%512;
					if(part > 0){
						whole++;
					}
					
					if(count_cluster >= whole){//достаточно свободного места для записи
						int i;
						Cluster *next_cl = NULL;		 				
						for(i = 0; i < (sizeof(array_file_access) / sizeof(File_access)); i++){ //найти свободный кластер ДЛЯ НАЧАЛА ЗАПИСИ ФАЙЛА
							if(array_file_access[i].busy == 0){
								break;					
							}
						}
						
						bool flag_name = true;
						for(int c = 0; c < (sizeof(array_file_access) / sizeof(File_access)); c++){ 
							if(array_file_access[c].name == s){
								flag_name = false;
								break;					
							}
						}
						
						if(flag_name){
							array_file_access[i].busy = 1;
							array_file_access[i].name = s;
							array_file_access[i].size = size;

							int place = 0;
							int j;
							int count_write;
							int size_prev1 = 0;
							while(size > 0){
								if(size > 512){
									count_write = 512;
									size_prev1 = size;
									size = size - 512;
									for(j = 0; j < (sizeof(array_file_access) / sizeof(File_access)); j++){
										if(array_file_access[j].busy == 0){
											break;					
										}
									}
									next_cl = array_file_access[j].cluster_point;		
								}
								else{
									int count_write = size;
									size = 0;
									next_cl = NULL;
								}
								read_posm_nbyte(place, count_write, f, array_file_access[i].cluster_point->data);								
								array_file_access[i].cluster_point->next = next_cl;
								
								if(count_write == 512){
									place = place + 512;
								} 
								
								if(size_prev1 > 512){
									i = j;
									array_file_access[j].busy = 1;
								}
							}
							count_cluster = count_cluster - whole;
							cout<<"Файл записан!\n";	
						}
						else{
							cout<<"Файл с таким именем уже существует.\n";
						}
					}
					else{
						cout<<"Недостаточно места на диске.\n";
					}
				}
				else{
					cout<<"Ошибка записи.\n";
				}
			}
			else{
				cout<<"Имя файла не должно содержать больше 8 символов.\n";
			}
		}
		if(s ==  "cat >>"){ //дозапись или запись нового файла из командной строки
			cout<<"Введите имя файла:\n";
			getline(cin, s);
			cout<<"Введите данные:\n";
			string data, all_data;
			while(cin >> data){
				if (data == "END"){
					break;
				}
				all_data.append(data);
				all_data.append("\n");
			}
			
			if(s.length() < 8){
				long size = all_data.length();
				int whole = size/512;
				float part = ((int)size)%512;
				if(part > 0){
					whole++;
				}															
				if(count_cluster >= whole){//достаточно свободного места для записи
					int i;
					Cluster *next_cl = NULL;		 				
					for(i = 0; i < (sizeof(array_file_access) / sizeof(File_access)); i++){ //найти свободный кластер ДЛЯ НАЧАЛА ЗАПИСИ ФАЙЛА
						if(array_file_access[i].busy == 0){
							break;					
						}
					}
					
					bool flag_name = 1;//есть ли уже такой файл
					for(int c = 0; c < (sizeof(array_file_access) / sizeof(File_access)); c++){ 
						if(array_file_access[c].name == s){
							flag_name = 0;//уже есть такой файл - дозапись
							break;					
						}
					}
					
					if(flag_name){//создаем новый файл
						array_file_access[i].busy = 1;
						array_file_access[i].name = s;
						array_file_access[i].size = size;

						int j;
						int count_write;
						int count_record = 0;
						int size_prev = 0;
						while(size > 0){
							if(size > 512){
								count_write = 512;
								size_prev = size;
								size = size - 512;
								for(j = 0; j < (sizeof(array_file_access) / sizeof(File_access)); j++){
									if(array_file_access[j].busy == 0){
										break;					
									}
								}
								next_cl = array_file_access[j].cluster_point;		
							}
							else{
								count_write = size;
								size = 0;
								next_cl = NULL;
							}
							
							const char * all_data_char = all_data.c_str();
							for(int h = 0; h < count_write; h++){
								array_file_access[i].cluster_point->data[h] = all_data_char[count_record];
								count_record++;
							}								
							array_file_access[i].cluster_point->next = next_cl;
							if(size_prev > 512){
								i = j;
								array_file_access[j].busy = 1;
							}

						}
						count_cluster = count_cluster - whole;
						cout<<"Файл записан!\n";	
					}
					else{//дозапись в конец файла
						int i_read;
						for(i_read = 0; i_read < (sizeof(array_file_access) / sizeof(File_access)) + 1; i_read++){ 
							if(array_file_access[i_read].name == s){
								break;					
							}
						}
						Cluster * a = array_file_access[i_read].cluster_point;
						while( a->next != NULL){
							a = a->next;
						}	
						
						int ck;
						int j;
						int count_write;
						int count_record = 0;
						int size_prev = 0;
						bool count_while = 1;
						while(size > 0){
							if(count_while){
								count_while = 0;
								for(ck = 0; ck < (sizeof(array_file_access) / sizeof(File_access)); ck++){
										if(array_file_access[ck].busy == 0){
											break;					
										}
								}
								a->next = array_file_access[ck].cluster_point;//просто нашли следующий свободный кластер
								array_file_access[ck].busy = 1;
							}								
							if(size > 512){
								count_write = 512;
								size_prev = size;
								size = size - 512;
								for(j = 0; j < (sizeof(array_file_access) / sizeof(File_access)); j++){
									if(array_file_access[j].busy == 0){
										break;					
									}
								}
								next_cl = array_file_access[j].cluster_point;		
							}
							else{
								count_write = size;
								size = 0;
								next_cl = NULL;
							}
						
						
							const char * all_data_char = all_data.c_str();
							for(int h = 0; h < count_write; h++){
								array_file_access[ck].cluster_point->data[h] = all_data_char[count_record];
								count_record++;
							}									
							array_file_access[ck].cluster_point->next = next_cl;
							if(size_prev > 512){
								ck = j;
								array_file_access[j].busy = 1;
							}
						}

						count_cluster = count_cluster - whole;
						cout<<"Дозапись произведена!\n";
					}							
				}
				else{
					cout<<"Недостаточно места на диске.\n";
				}
			}					
			else{
				cout<<"Имя файла не должно содержать больше 8 символов.\n";
			}
		}

		if(s ==  "touch"){ //создание пустого файла
			cout<<"Введите имя файла:\n";
			getline(cin,s);
			if(s.length() < 8){
				if(count_cluster > 0){
					int i_touch;
					for(i_touch = 0; i_touch < (sizeof(array_file_access) / sizeof(File_access)); i_touch++){ 
						if(array_file_access[i_touch].busy == 0){
							break;					
						}
					}
					bool flag_name = 1;
					for(int c = 0; c < (sizeof(array_file_access) / sizeof(File_access)); c++){ 
						if(array_file_access[c].name == s){
							flag_name = 0;
							break;					
						}
					}
					if(flag_name){
						array_file_access[i_touch].busy = 1;
						array_file_access[i_touch].name = s;
						array_file_access[i_touch].size = 0;
						array_file_access[i_touch].cluster_point->next = NULL;
						count_cluster--;
					}
					else{
						cout<<"Файл с таким именем уже существует.\n";
					}
				}
				else{
					cout<<"Недостаточно места на диске.\n";
				}
			}
			else{
				cout<<"Имя файла не должно содержать больше 8 символов.\n";
			}

		}
		if(s ==  "cat"){ //чтение существующего файла
			cout<<"Введите имя файла:\n";
			getline(cin,s);
			int i_read;
			for(i_read = 0; i_read < (sizeof(array_file_access) / sizeof(File_access)) + 1; i_read++){ 
				if(i_read == (sizeof(array_file_access) / sizeof(File_access))){
					break;					
				}
				if(array_file_access[i_read].name == s){
					break;					
				}
			}
			if(i_read != (sizeof(array_file_access) / sizeof(File_access))){
				Cluster *temp = array_file_access[i_read].cluster_point;
				while(temp!= NULL){
					int size_read = 512;
					for (int k = 0; k < size_read; k++) {
						cout << temp->data[k];//прочитали биты из нужного кластера
					}
					 temp = temp->next;
				}				
			}
			else{
				cout<< "Неверное имя файла.Не могу прочитать.\n";
			}
		}		
		if(s ==  "rm -f"){ //удаление файла
			cout<<"Введите имя файла:\n";
			getline(cin,s);
			int i_del;
			for(i_del = 0;  i_del < (sizeof(array_file_access) / sizeof(File_access)) + 1;  i_del++){ 
				if(i_del == (sizeof(array_file_access) / sizeof(File_access))){
					break;					
				}
				if(array_file_access[i_del].name == s){
					break;					
				}
			}
			if(i_del != (sizeof(array_file_access) / sizeof(File_access))){
				Cluster *temp = array_file_access[i_del].cluster_point;
				array_file_access[i_del].busy = 0;
				array_file_access[i_del].name = "";
				int count_del = 1;

				while(temp!= NULL){
					int k_del;
					for(k_del = 0;  k_del < (sizeof(array_file_access) / sizeof(File_access)) + 1;  k_del++){ 
						if(array_file_access[k_del].cluster_point == temp){
							break;
						}
					}
					array_file_access[k_del].busy = 0;
					for (int k = 0; k < 512; k++) {
						temp->data[k] = 0;
					}

					temp = array_file_access[k_del].cluster_point->next;
					count_del++;
				}
				count_cluster = count_cluster + count_del; 
				cout<<"Удален файл "<<s<<"!\n";				
			}
			else{
				cout<< "Неверное имя файла.Не могу удалить.\n";
			}
					
		}
		if(s == "test"){//режим тестирования
			pthread_t t1, t2; 
			int i1 = 0;
 			int i2 = 1;			
			(void)pthread_create(&t1, NULL, read_size, (void *)&i1);
			(void)pthread_create(&t2, NULL, read_size, (void *)&i2);
			(void) pthread_join(t1, NULL);
			(void) pthread_join(t2, NULL);
		}	
	}
    return 0;
}