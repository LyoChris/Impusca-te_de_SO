#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>    // necesar pentru functia getpwuid
#include <grp.h>    // necesar pentru functia getgrgid
#include <limits.h> // necesar pentru PATH_MAX

int nr=0;

void parcurgere_director(char *cale, char* fisier_cautat) {
    DIR *dir;
    struct dirent *de;

    char nume[PATH_MAX];
    int isFolder;

    isFolder = afisare_fileinfo(cale);

    if(isFolder != 1){
        return;
    }
    else
    {
        if(NULL == (dir = opendir(cale)) ){
            //fprintf(stderr,"Eroare deschidere director %s .\t",cale);  perror("Cauza este");
            return;
        }

        /* parcurgerea pe orizontală a primului nivel, i.e. descendenții direcți ai argumentului primit de această funcție */
        while(NULL != (de = readdir(dir)) )
        {
            if(de->d_name[0] != '.' && strcmp(de->d_name,".") && strcmp(de->d_name,"..") ) {  /* ignorăm intrările implicite "." și ".." din orice director */
                sprintf(nume,"%s/%s",cale,de->d_name);  /* construim calea nouă, ca și concatenare de string-uri */
                /* Notă: alternativ, puteam gestiona calea cu apeluri chdir(), similar ideii de la exercițiile de parcurgere recursivă cu scripuri, din lab.5 */
                //printf("%s", de->d_name);
                if (strcmp(de->d_name, fisier_cautat) == 0) {
                    printf("Fișier găsit: %s\n", nume);
                }
                parcurgere_director(nume, fisier_cautat);              /* apelul recursiv pentru parcurgerea intrării curente */
            }
        }

        closedir(dir);
    }
}


int afisare_fileinfo(char* cale) {
    struct stat st;             /* această structură o vom folosi pentru a afla, cu apelul stat, metadatele asociate unui fișier */
    struct passwd *pwd;         /* această structură o vom folosi pentru a afla username-ul asociat unui UID */
    struct group *grp;          /* această structură o vom folosi pentru a afla groupname-ul asociat unui GID */
    char perm[10]="---------";  /* aici vom construi forma simbolică pentru permisiunile fișierului */
    int result=0;

    if(0 != stat(cale,&st) ) {
        //fprintf(stderr,"Eroare la stat pentru %s .\t", cale);  perror("Cauza este");
        return 2;
    }

    switch(st.st_mode & S_IFMT) {
        case S_IFDIR : result=1; break;
        case S_IFREG : break;
        case S_IFLNK : break;
        case S_IFIFO : break;
        case S_IFSOCK:  break;
        case S_IFBLK : break;
        case S_IFCHR : break;
        default: result = -1;
    }

    //printf("\tNumarul de nume alternative (i.e., link-uri hard) ale fisierului: %ld octeti\n", st.st_nlink );

    //printf("\tDimensiunea acestuia: %lld octeti\n", (long long)st.st_size );
    //printf("\tSpatiul ocupat de disc pentru stocarea acestuia: %lld sectoare (blocuri de 512 octeti)\n", (long long)st.st_blocks );


    //printf("\tPermisiunile acestuia, in notatie octala: %o\n", st.st_mode & 0777 );

    //if( S_IRUSR & st.st_mode )  perm[0]='r';
    //if( S_IWUSR & st.st_mode )  perm[1]='w';
    //if( S_IXUSR & st.st_mode )  perm[2]='x';
    //if( S_IRGRP & st.st_mode )  perm[3]='r';
    //if( S_IWGRP & st.st_mode )  perm[4]='w';
    //if( S_IXGRP & st.st_mode )  perm[5]='x';
    //if( S_IROTH & st.st_mode )  perm[6]='r';
    //if( S_IWOTH & st.st_mode )  perm[7]='w';
    //if( S_IXOTH & st.st_mode )  perm[8]='x';

    //printf("\tPermisiunile acestuia, in notatie simbolica: %s\n", perm );

    //if(NULL != (pwd = getpwuid(st.st_uid)) )
        //printf("\tProprietarul acestuia: %s (cu UID-ul: %ld)\n", pwd->pw_name, (long)st.st_uid );
    //else
       // printf("\tProprietarul acestuia are UID-ul: %ld\n", (long)st.st_uid );

    //if(NULL != (grp = getgrgid(st.st_gid)) )
        //printf("\tGrupul proprietar al acestuia: %s (cu GID-ul: %ld)\n", grp->gr_name, (long)st.st_gid );
    //else
        //printf("\tGrupul proprietar al acestuia are GID-ul: %ld\n", (long)st.st_gid );

    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2){
        fprintf(stderr, "Utilizare: %s <nume_fisier>\n", argv[0]);
        exit (1);
    }

    char* home_dir = getenv("HOME");
    if(home_dir == NULL) {
        fprintf(stderr, "Eroare la obtinerea directorului home\n");
        exit (1);
    }

    parcurgere_director(home_dir, argv[1]);

    return 0;
}