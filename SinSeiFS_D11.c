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
#include <sys/xattr.h>
#include<sys/types.h>
#include<sys/wait.h>

char *dirpath = "/home/dewi/Downloads";

void encode(char* str){
	int i=0;
	if(!strcmp(str,".") || !strcmp(str,"..")) return;
	while(str[i]!='\0'){
		if(!((str[i]>=0&&str[i]<65)||(str[i]>90&&str[i]<97)||(str[i]>122&&str[i]<=127))){
			if(str[i]>='A'&&str[i]<='Z' )
				str[i]='Z'+'A'-str[i];
			if(str[i]>='a'&&str[i]<='z' )
				str[i]='z'+'a'-str[i];
		}
	i++;	
	}
}

void Generator(char *desc, char *path, int check, int check2, char *path2){
	char result[1024];
	char level[10];
	FILE *demo;
	if(check==0) {
		strcpy(level,"WARNING");
	}
	else{
		strcpy(level,"INFO");
	}
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	if(check2==0) {
		sprintf(result,"%s::%02d%02d%02d-%02d:%02d:%02d::%s::%s", level,tm.tm_mday,(tm.tm_mon+1), (tm.tm_year+1900),tm.tm_hour, tm.tm_min, tm.tm_sec, desc, path);
	}
	else{
		sprintf(result,"%s::%02d%02d%02d-%02d:%02d:%02d::%s::%s::%s", level,tm.tm_mday,(tm.tm_mon+1), (tm.tm_year+1900), tm.tm_hour, tm.tm_min, tm.tm_sec, desc, path, path2);
	}
	demo=fopen("/home/dewi/fs.log", "a");
	fprintf(demo, "%s\n", result);
	fclose(demo);
}


void cekSubString(const char s[], char sub[],int p, int l){
	int i=0;
	while(i<l){
		sub[i]=s[p+i];
		i++;
	}

	sub[i]='\0';
}

int cekExt(char* file){
	char ext[1000]="/0";
	int id=0;	
	while(id<strlen(file) && file[id] != '.')id+=1;
	memset(ext,0,sizeof(ext));
	strcpy(ext,file+id);
	return id;
}

void listFilesRecursively(const char *basePath){
	char fpath[1000];
	if(strcmp(basePath,"/")==0)
	{
		basePath = dirpath;
		sprintf(fpath,"%s",basePath);
	}
	else
		sprintf(fpath,"%s%s",dirpath,basePath);

	char path[1024];
	struct dirent *dp;
	DIR *dir = opendir(fpath);

	if(!dir)
		return;

	while((dp=readdir(dir))!=NULL){
		if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0){
			char name[1024], newname[1024], foldername[1024];
			strcpy(name,dp->d_name);
			cekExt(name);
			char namaFile[10000];
			memset(namaFile,0,strlen(namaFile));
			strncpy(namaFile,name,id);
		encode(namaFile);
		sprintf(newname,"/log%s.%s",fpath,newname);
		memset(foldername,'\0',sizeof(foldername));
		sprintf(foldername,"%s%s",fpath,newname);
		
			pid_t cid;
			cid = fork();
			if(cid == 0){
				char *argv[]={"cp",fpath,foldername,NULL};
				execv("/bin/cp",argv);
				exit(1);
			}
			else wait(NULL);
		printf("%s\n",foldername);

		strcpy(path,fpath);
		strcat(path,"/");
		strcat(path,dp->d_name);
		listFilesRecursively(path);
		}	
	}
	closedir(dir);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];

	sprintf(fpath, "%s%s", dirpath, path);
	// sprintf(fpath, "%s/%s", dirpath, path);
	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];

	if (!strcmp(path, "/"))
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	Generator("LS",fpath,1,0,NULL);
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char substr[10];
	cekSubString(path,substr,0,6);
	printf("%s -- %s",path,substr);
	if(strcpy(substr,"AtoZ_")==0){
		listFilesRecursively(path);
	}
 	char fpath[1000];
	if (!strcmp(path, "/"))
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	Generator("MKDIR",fpath,1,0,NULL);
	int res;

	res = mkdir(fpath, mode);
	if (res == -1)return -errno;
 
	return 0;
}

static int xmp_unlink(const char *path)
{
	char fpath[1000];

	if (!strcmp(path, "/"))
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	Generator("RM",fpath,1,0,NULL);
	int res;

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	char fpath[1000];

	if (!strcmp(path, "/"))
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	int res;
	Generator("RMDIR",fpath,1,0,NULL);
	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char new_from[1000];
	char new_to[1000];
	char substr[10];
	cekSubString(from,substr,0,6);
	printf("%s -- %s",from,substr);
	if(strcpy(substr,"AtoZ_")==0){
		listFilesRecusively(to);
	}
	sprintf(new_from,"%s%s",dirpath,from);
	sprintf(new_to,"%s%s",dirpath,to);
	Generator("RENAME",new_from,1,1,new_to);
	res = rename(new_from, new_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];

	if (!strcmp(path, "/"))
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

	(void) fi;
	char fpath[1000];

	if (!strcmp(path, "/"))
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else
	{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	Generator("CREATE",fpath,1,0,NULL);
	int res;
	res = creat(fpath, mode);
	if(res == -1)
		return -errno;

	close(res);

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr        = xmp_getattr,
	.rmdir		= xmp_rmdir,
	.readdir        = xmp_readdir,
	.mkdir          = xmp_mkdir,
	.unlink         = xmp_unlink,
	.rename         = xmp_rename,
	//.link           = xmp_link,
	.read           = xmp_read,
	//.write          = xmp_write,
	.create         = xmp_create,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
