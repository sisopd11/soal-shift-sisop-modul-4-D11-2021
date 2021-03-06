# soal-shift-sisop-modul-4-D11-2021
Modul 4 
|Nama|NRP|
|----|-----|
|Afifah Nur Sabrina Syamsudin|05111940000022|
|Dewi Mardani Cristin|05111940000225|
|Avind Pramana Azhari|05111940000226|

## Soal No.1
a. Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.

b. Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.

c. Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.

d. Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]

e. Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

Untuk melakukan encode dan decode menggunakan Atbash cipher

folder akan terencode apabila dibuat atau direname dengan awalan AtoZ dan apabila direktori yang ternkripsi di rename maka isi direktori akan terdecode. Fungsi ini nantinya akan dipanggil disetiap utlity function yang akan digunakan.

```

void encode1(char *str)
{
	if (!strcmp(str, ".") || !strcmp(str, "..")) return;
	int strLength = strlen(str);
    	for(int i = 0; i < strLength; i++){
		if(str[i] == '/') 
            		continue;
		if(str[i]=='.')
            		break;
        
		if(str[i]>='A'&&str[i]<='Z')
            		str[i] = 'Z'+'A'-str[i];
       		if(str[i]>='a'&&str[i]<='z')
            		str[i] = 'z'+'a'-str[i];
	}
}

void decode1(char *str)
{
	if (!strcmp(str, ".") || !strcmp(str, "..")) return;
	int strLength = strlen(str), s=0;
	for(int i = strLength; i >= 0; i--){
		if(str[i]=='/')break;

		if(str[i]=='.'){
			strLength = i;
			break;
		}
	}
	for(int i = 0; i < strLength; i++){
		if(str[i]== '/'){
			s = i+1;
			break;
		}
	}
    for(int i = s; i < strLength; i++) {
		if(str[i] =='/'){
            continue;
        }
        if(str[i]>='A'&&str[i]<='Z'){
            str[i] = 'Z'+'A'-str[i];
        }
        if(str[i]>='a'&&str[i]<='z'){
            str[i] = 'z'+'a'-str[i];
        }
    }
	
}
```
Kendala :
1. Sempat bingung dengan utility function yang akan digunakan
  
Output :

1. Kondisi Awal
2. 
![output modul 4 1](https://user-images.githubusercontent.com/80894892/121809584-f3f15080-cc8f-11eb-9313-57452d408efe.png)

![output modul 4 2](https://user-images.githubusercontent.com/80894892/121809648-36b32880-cc90-11eb-8c69-94942bb1c3fe.png)

Setelah di Rename

![output modul 4 3](https://user-images.githubusercontent.com/80894892/121809708-6f530200-cc90-11eb-8915-76c7e8980ffc.png)

## Soal No.2
Fungsi `rename` digunakan untuk memeriksa apakah direktori direname dengan menambahkan RX_ atau menghilangkan RX_ dengan fungsi strstr().
```
static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	char *a = strstr(to, atoz);
	if (a != NULL) decryptAtbash(a);
	
	char *b = strstr(from, rx);
	if (b != NULL){
		decryptRot13(b);
		decryptAtbash(b);
	}
	
	char *c = strstr(to, rx);
	if (c != NULL){
		decryptRot13(c);
		decryptAtbash(c);
	}

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1) return -errno;

	tulisLog2("RENAME", frompath, topath);
	
	if (c != NULL){
		enkripsi2(topath);
		tulisLog2("ENCRYPT2", from, to);
	}

	if (b != NULL && c == NULL){
		dekripsi2(topath);
		tulisLog2("DECRYPT2", from, to);
	}
	
	...

	return 0;
}
```
Jika terdeteksi RX_ pada path yang dituju maka direktori direname dengan menambahkan RX_ dan selanjutnya memecahkan file pada fungsi enkripsi2.
```
void enkripsi2(char *fpath)
{
	chdir(fpath);
	DIR *dp;
	struct dirent *dir;
	
	dp = opendir(".");
	if(dp == NULL) return;
	struct stat lol;
	char dirPath[1000];
	char filePath[1000];
	
	while ((dir = readdir(dp)) != NULL){
		printf("dirname %s\n", dir->d_name);
		printf("%s/%s\n", fpath, dir->d_name);
		if (stat(dir->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0) continue;
			sprintf(dirPath,"%s/%s",fpath, dir->d_name);
			enkripsi2(dirPath);
			printf("dirpath %s\n", dirPath);
		}
		else{
			sprintf(filePath,"%s/%s",fpath,dir->d_name);
			FILE *input = fopen(filePath, "r");
			if (input == NULL) return;
			int bytes_read, count = 0;
			void *buffer = malloc(1024);
			while((bytes_read = fread(buffer, 1, 1024, input)) > 0){
				char newFilePath[1000];
				sprintf(newFilePath, "%s.%04d", filePath, count);
				count++;
				FILE *output = fopen(newFilePath, "w+");
				if(output == NULL) return;
				fwrite(buffer, 1, bytes_read, output);
				fclose(output);
				memset(buffer, 0, 1024);
			}
			fclose(input);
			printf("filepath %s\n", filePath);
			remove(filePath);
		}
	}
	closedir(dp);
}
```
Jika terdeteksi RX_ pada path asal dan tidak terdeteksi RX_ pada path tujuan maka direktori akan direname dengan menghilangkan RX_ dan selanjutnya menggabungkan file pada fungsi dekripsi2.
```
void dekripsi2(char *dir)
{
	chdir(dir);
	DIR *dp;
	struct dirent *de;
	struct stat lol;
	dp = opendir(".");
	if (dp == NULL) return;
	
	char dirPath[1000];
	char filePath[1000];
	
	while ((de = readdir(dp)) != NULL){
		if (stat(de->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
			sprintf(dirPath, "%s/%s", dir, de->d_name);
			dekripsi2(dirPath);
		}
		else{
			sprintf(filePath, "%s/%s", dir, de->d_name);
			filePath[strlen(filePath) - 5] = '\0';
			FILE *check = fopen(filePath, "r");
			if (check != NULL) return;
			FILE *file = fopen(filePath, "w");
			int count = 0;
			char topath[1000];
			sprintf(topath, "%s.%04d", filePath, count);
			void *buffer = malloc(1024);
			while (1){
				FILE *op = fopen(topath, "rb");
				if (op == NULL) break;
				size_t readSize = fread(buffer, 1, 1024, op);
				fwrite(buffer, 1, readSize, file);
				fclose(op);
				remove(topath);
				count++;
				sprintf(topath, "%s.%04d", filePath, count);
			}
			free(buffer);
			fclose(file);
		}
	}
	closedir(dp);
}
```
Untuk enkripsi dan dekripsi menggunakan ROT13 cipher.
```
void encryptRot13(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("encrypt path ROT13: %s\n", path);
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, 0);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp = path[i];
			if(isupper(path[i])) tmp -= 'A';
			else tmp -= 'a';
			tmp = (tmp + 13) % 26; //ROT13 cipher
			if(isupper(path[i])) tmp += 'A';
			else tmp += 'a';
			path[i] = tmp;
		}
	}
}
```
```
void decryptRot13(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("decrypt path ROT13: %s\n", path);
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, endid);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp = path[i];
			if(isupper(path[i])) tmp -= 'A';
			else tmp -= 'a';
			tmp = (tmp + 13) % 26; //ROT13 cipher
			if(isupper(path[i])) tmp += 'A';
			else tmp += 'a';
			path[i] = tmp;
		}
	}
}
```
Fungsi decrypt dipanggil pada function getattr, rename, rmdir, create, mkdir dan lain lain dalam proses sinkronisasi FUSE. Fungsi decrypt dan encryt dilakukan pada function readdir melalui pengecekan string RX_ terdapat di string path di setiap function atau tidak dengan menggunakan fungsi strstr().

Output :
![output modul 4 4](https://user-images.githubusercontent.com/80894892/121809742-9c071980-cc90-11eb-85ea-e85a7d4a202d.png)

## Soal No.3
## Soal No.4
a. Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.

b. Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.

c. Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.

d. Sisanya, akan dicatat pada level INFO.

e. Format untuk logging yaitu:


[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]

Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan

INFO::28052021-10:00:00:CREATE::/test.txt

INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt


```
void generator1(char *nama, char *path)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char result[1000];

	FILE *file;
	file = fopen("/home/dewi/SinSeiFS.log", "a");

	if (strcmp(nama, "RMDIR") == 0 || strcmp(nama, "UNLINK") == 0)
		sprintf(result, "WARNING::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, path);
	else
		sprintf(result, "INFO::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, path);

	fputs(result, file);
	fclose(file);
	return;
}

void generator2(char *nama, const char *from, const char *to)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char result[1000];

	FILE *file;
	file = fopen("/home/dewi/SinSeiFS.log", "a");

	if (strcmp(nama, "UNLINK") == 0 || strcmp(nama, "RMDIR") == 0)
		sprintf(result, "WARNING::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, from, to);
	else
		sprintf(result, "INFO::%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, from, to);

	fputs(result, file);
	fclose(file);
	return;
}
```
Untuk membuka log file maka menggunakan FILE *fl = fopen("/home/natih/fs.log", "a") yang nantinya akan berisi perintah system call yang dijalankan. Mode "a" menandakan bahwa file SinSeiFS.log hanya untuk ditulis.

Format untuk logging ini berisi time (tahun, bulan, hari, jam, menit, detik) diketahui menggunakan t_time t.

fprintf(fl, "%s\n", log) untuk menuliskan format logging WARNING ataupun INFO ke log file.

Kemudian, kita akan memanggil kedua fungsi ini, di setiap fungsi yang ada dalam fuse sesuai dengan perintah. Contohnya adalah pada utility function misalnya mkdir().

Output

![output modul 4 5](https://user-images.githubusercontent.com/80894892/121810183-606d4f00-cc92-11eb-90fe-20075d4cf208.png)

## Kendala
- Sempat gagal memasang FUSE.
- Kurang begitu paham dengan flow FUSE
