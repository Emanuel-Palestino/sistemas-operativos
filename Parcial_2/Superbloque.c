/*
 * 
 * Devuelve los mismos valores que se imprimen al ejecutar stat -f, recib
 * es para ejecutarlo es necesario escribir una ruta válida por linea de comandos.
 * CCBY: Cruz Villalba Roberto Carlos, Palestino Hernández Emanuel
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/statvfs.h>
#include <string.h>


//Definicion de los "Números mágicos" para la obtención del tipo de superbloque
// https://github.com/linux-test-project/ltp/tree/master/include/tst_fs.h
/* man 2 statfs or kernel-source/include/linux/magic.h */
#define TST_BTRFS_MAGIC    0x9123683E
#define TST_NFS_MAGIC      0x6969
#define TST_RAMFS_MAGIC    0x858458f6
#define TST_TMPFS_MAGIC    0x01021994
#define TST_V9FS_MAGIC     0x01021997
#define TST_XFS_MAGIC      0x58465342
#define TST_EXT2_OLD_MAGIC 0xEF51
/* ext2, ext3, ext4 have the same magic number */
#define TST_EXT234_MAGIC   0xEF53
#define TST_MINIX_MAGIC    0x137F
#define TST_MINIX_MAGIC2   0x138F
#define TST_MINIX2_MAGIC   0x2468
#define TST_MINIX2_MAGIC2  0x2478
#define TST_MINIX3_MAGIC   0x4D5A
#define TST_UDF_MAGIC      0x15013346
#define TST_SYSV2_MAGIC    0x012FF7B6
#define TST_SYSV4_MAGIC    0x012FF7B5
#define TST_UFS_MAGIC      0x00011954
#define TST_UFS2_MAGIC     0x19540119
#define TST_F2FS_MAGIC     0xF2F52010
#define TST_NILFS_MAGIC    0x3434
#define TST_EXOFS_MAGIC    0x5DF5
#define TST_OVERLAYFS_MAGIC 0x794c7630
#define TST_FUSE_MAGIC     0x65735546

//Prototipos de funciones
void print_filesystem(const char* path);
const char *tst_fs_type_name(long f_type);

int main(int argC, char *argV[]){

    char *ruta;


    if (argC < 2) {
        perror("Faltan argumentos. Ingresar ruta.");
        exit(EXIT_FAILURE);
    }
    ruta = argV[1];
    if(strlen(ruta)>255){
        perror("Tamaño de ruta excedido. Tamaño máximo de ruta = 255 caracteres");
        exit(EXIT_FAILURE);
    }


    struct statvfs vfs;
    if(statvfs(ruta,&vfs)!=0){
        perror("Llamado de statvfs");
        //perror(1);
    }


	printf("   Fichero: \"%s\"\n  ", ruta);

    printf("ID del Dispositivo: %lx ", (unsigned long) vfs.f_fsid);
	printf("Long. nombre: %ld    ", (long) vfs.f_namemax);
    //Buscar  tipo
    printf("Tipo:");print_filesystem(ruta);

	printf("Tam. bloque: %ld     ", (long) vfs.f_bsize);
	printf("Tam. bloque fundamental: %ld\n", (long) vfs.f_frsize);

    printf("Bloques: ");
	printf("Total: %lu\t", (unsigned long) vfs.f_blocks);
	printf("Libres: %lu\t", (unsigned long) vfs.f_bfree);
	printf("Disponibles: %lu\n", (unsigned long) vfs.f_bavail);

	printf("Inodos: ");
    printf("Total: %lu\t", (unsigned long) vfs.f_files);
	printf("Libres: %lu\n", (unsigned long) vfs.f_ffree);
	
	


    return 0;
}


/*Las dos siguientes funciones tienen como fuente común el siguiente link de GitHub
Fuente: https://stackoverflow.com/questions/48319246/how-can-i-determine-filesystem-type-name-with-linux-api-for-c
*/


/*función de obtención del nombre del tipo de archivo. 
Recibe como entrada el tipo de archivo (entero largo) y regresa 
una cadena de acuerdo al tipo de archivo que ingresó*/
const char *tst_fs_type_name(long f_type)
{
    switch (f_type) {
    case TST_TMPFS_MAGIC:
        return "tmpfs";
    case TST_NFS_MAGIC:
        return "nfs";
    case TST_V9FS_MAGIC:
        return "9p";
    case TST_RAMFS_MAGIC:
        return "ramfs";
    case TST_BTRFS_MAGIC:
        return "btrfs";
    case TST_XFS_MAGIC:
        return "xfs";
    case TST_EXT2_OLD_MAGIC:
        return "ext2";
    case TST_EXT234_MAGIC:
        return "ext2/ext3";
    case TST_MINIX_MAGIC:
    case TST_MINIX_MAGIC2:
    case TST_MINIX2_MAGIC:
    case TST_MINIX2_MAGIC2:
    case TST_MINIX3_MAGIC:
        return "minix";
    case TST_UDF_MAGIC:
        return "udf";
    case TST_SYSV2_MAGIC:
    case TST_SYSV4_MAGIC:
        return "sysv";
    case TST_UFS_MAGIC:
    case TST_UFS2_MAGIC:
        return "ufs";
    case TST_F2FS_MAGIC:
        return "f2fs";
    case TST_NILFS_MAGIC:
        return "nilfs";
    case TST_EXOFS_MAGIC:
        return "exofs";
    case TST_OVERLAYFS_MAGIC:
        return "overlayfs";
    case TST_FUSE_MAGIC:
        return "fuse";
    default:
        return "unknown";
    }
}
/*Función para imprimir el tipo del archivo como cadena
Recibe una ruta de tipo char*
tipo Void, no recibe nada.
Luego de las comprobaciones manda a llamar a la función que regresa el tipo de archivo convertido a cadena.
*/
void print_filesystem(const char* path)
{
    if (path == NULL)
        return;

    struct statfs s;

    if (statfs(path, &s)) {
        fprintf(stderr, "statfs(%s) failed: %s\n", path, strerror(errno));
        return;
    }

    printf("%s\n", tst_fs_type_name(s.f_type));
}