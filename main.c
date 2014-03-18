#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>

#include <unistd.h>
#include <sys/mman.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int shmid;
	int key;
	char *addr;
	struct shmid_ds seg_info;

	key = 75;

	if (fork())
		sleep(10);
	// 1 page == PAGE_SIZE
	if ((shmid = shmget(key , 1024, IPC_CREAT | IPC_EXCL)) == -1)//creation segment memoire et retourd d'un id segment ; IPC_PRIVATE ne lit que sauf les 9 premiers bits du flag et créer un nouveau segment
		perror("error -> shmget : ");

	addr = shmat(shmid, NULL, NULL);//attachement segment -> processus 
//(si addr(arg2) != 0 && arg3(flag) == SHM_RMD ? attachement a l'addresse (SHMLBA * ((addr / SHMLBA) + 1)) : addr -> ? -> system => ! => addr
	if (-1 == (long unsigned int)addr)
		perror("error -> shmat : ");

	if (errno == EACCES)
		printf("EACCES\n");
	else if (errno == EINVAL)
		printf("EINVAL\n");
	else if (errno == ENOMEM)
		printf("ENOMEM\n");

	if ((shmctl(shmid, IPC_STAT, &seg_info)) == -1)//recuperation des info_sig 
		perror("error -> shmctl : ");

	system("ipcs");

	if ((shmdt(addr)) == -1)//detachement segment -> processus
		perror("error -> shmdt : ");

	if ((shmctl(shmid, IPC_RMID, &seg_info)) == -1)//marquage du segment pour destruction (quand file vide) shm_nattch shmid_ds /!\ uniquement createur ou proprietaire (ou setuid)
		perror("error -> shmctl : ");

	seg_info.shm_nattch = 0;//fille mise a 0 pour destruction effectif du segment memoire
	if ((shmctl(shmid, IPC_SET, &seg_info)) == -1)//application des modification au segment
		perror("error -> shmctl : ");

	return (errno);
}

//	mem = mmap(NULL, 60, PROT_WRITE | PROT_READ | SHM_NORESERVE, fd, 0);
