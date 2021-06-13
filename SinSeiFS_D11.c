#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#define SIZE 1024
#define ARR_SIZE 100
#define ROT13 1
#define VIGENERE 

static const char *dirPath = "/home/dewi/Downloads";
char *atoz = "AtoZ_";
char *rx = "RX_";
int id = 0;

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


int split_ext_id(char *path)
{
	int ada = 0;
	for(int i=strlen(path)-1; i>=0; i--){
		if (path[i] == '.'){
			if(ada == 1) return i;
			else ada = 1;
		}
	}
	return strlen(path);
}

int ext_id(char *path)
{
	for(int i=strlen(path)-1; i>=0; i--){
		if (path[i] == '.') 
			return i;
	}
	return strlen(path);
}

int slash_id(char *path, int end)
{
	for(int i=0; i<strlen(path); i++){
		if (path[i] == '/')
			return i + 1;
	}
	return end;
}


void encryptRot13(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) 
		return;
	
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

void decryptRot13(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, "..")) 
		return;
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

void encryptVigenere(char *path)
{
	char *key = "SISOP";
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, 0);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp1 = path[i];
			char tmp2 = key[(i-startid) % strlen(key)];
			if(isupper(path[i])){
				tmp1 -= 'A';
				tmp2 -= 'A';
				tmp1 = (tmp1 + tmp2) % 26; //Vigenere cipher
				tmp1 += 'A';
			}
			else{
				tmp1 -= 'a';
				tmp2 = tolower(tmp2) - 'a';
				tmp1 = (tmp1 + tmp2) % 26; //Vigenere cipher
				tmp1 += 'a';
			}
			path[i] = tmp1;
		}
	}
}

void decryptVigenere(char *path)
{
	char *key = "SISOP";
	if (!strcmp(path, ".") || !strcmp(path, "..")) return;
	
	printf("decrypt path Vigenere: %s\n", path);
	
	int endid = split_ext_id(path);
	int startid = slash_id(path, endid);
	
	for (int i = startid; i < endid; i++){
		if (path[i] != '/' && isalpha(path[i])){
			char tmp1 = path[i];
			char tmp2 = key[(i-startid) % strlen(key)];
			if(isupper(path[i])){
				tmp1 -= 'A';
				tmp2 -= 'A';
				tmp1 = (tmp1 - tmp2 + 26) % 26; //Vigenere cipher
				tmp1 += 'A';
			}
			else{
				tmp1 -= 'a';
				tmp2 = tolower(tmp2) - 'a';
				tmp1 = (tmp1 - tmp2 + 26) % 26; //Vigenere cipher
				tmp1 += 'a';
			}
			path[i] = tmp1;
		}
	}
}

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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = lstat(fpath, stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL)
	{
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	if (id != 24) 
		id++;
	else 
		generator1("READDIR", fpath);

	int res = 0;
	DIR *dp;
	struct dirent *de;

	(void)offset;
	(void)fi;

	dp = opendir(fpath);
	if (dp == NULL) return -errno;

	while ((de = readdir(dp)) != NULL){
		if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		if (a != NULL) encode1(de->d_name);
		if (b != NULL){
			encode1(de->d_name);
			encryptRot13(de->d_name);
		}
		res = (filler(buf, de->d_name, &st, 0));
		if (res != 0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	int res = 0;
	int fd = 0;

	(void)fi;
	generator1("READ", fpath);

	fd = open(fpath, O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = mkdir(fpath, mode);
	generator1("MKDIR", fpath);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = rmdir(fpath);
	generator1("RMDIR", fpath);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	char *a = strstr(to, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(from, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}
	
	char *c = strstr(to, rx);
	if (c != NULL){
		decryptRot13(c);
		decode1(c);
	}

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1) return -errno;

	generator2("RENAME", frompath, topath);
	
	if (c != NULL){
		enkripsi2(topath);
		generator2("ENCRYPT2", from, to);
	}

	if (b != NULL && c == NULL){
		dekripsi2(topath);
		generator2("DECRYPT2", from, to);
	}

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	(void)fi;
	generator1("WRITE", fpath);

	fd = open(fpath, O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	(void)fi;
	generator1("CREATE", fpath);
	
	res = creat(fpath, mode);	
	if (res == -1) return -errno;

	close(res);
	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, atoz);
	if (a != NULL) decode1(a);
	
	char *b = strstr(path, rx);
	if (b != NULL){
		decryptRot13(b);
		decode1(b);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = unlink(fpath);
	generator1("UNLINK", fpath);

	if (res == -1) return -errno;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.write = xmp_write,
	.create = xmp_create,
	.unlink = xmp_unlink,

};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
