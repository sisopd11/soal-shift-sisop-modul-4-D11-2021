# soal-shift-sisop-modul-4-D11-2021
Modul 4 
|Nama|NRP|
|----|-----|
|Afifah Nur Sabrina Syamsudin|05111940000022|
|Dewi Mardani Cristin|05111940000225|
|Avind Pramana Azhari|05111940000226|

## Soal No.1
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

## Soal No.3
## Soal No.4
## Kendala
- Gagalkan memasang FUSE.
- Kurang begitu paham dengan flow FUSE
